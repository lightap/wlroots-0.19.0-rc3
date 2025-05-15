
#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/backend/RDP.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/log.h>
#include <xkbcommon/xkbcommon.h>
#include <wlr/render/gles2.h>
#include <wlr/render/drm_format_set.h>
#include <wayland-server-protocol.h>
#include <drm_fourcc.h>
#include <string.h>
#include <EGL/egl.h>
#include <wlr/render/egl.h>
#include <wlr/render/gles2.h>
#include <wlr/render/egl.h>

#include <render/allocator/RDP_allocator.h>
#include <wlr/backend/RDP.h>



/* RDP backend access */
struct wlr_rdp_backend {
    struct wlr_backend backend;
    freerdp_peer *global_rdp_peer;
    bool started;
    /* Other fields may exist, but we only need these */
};

/* Global backend for get_global_rdp_peer */
//static struct wlr_backend *backend = NULL;

/* Implementation of get_global_rdp_peer 
freerdp_peer *get_global_rdp_peer(void) {
    if (!backend) {
        wlr_log(WLR_ERROR, "get_global_rdp_peer: Backend not set");
        return NULL;
    }
    struct wlr_rdp_backend *rdp_backend = wl_container_of(backend, rdp_backend, backend);
    wlr_log(WLR_DEBUG, "get_global_rdp_peer: backend=%p, rdp_backend=%p, global_rdp_peer=%p",
            (void*)backend, (void*)rdp_backend, (void*)rdp_backend->global_rdp_peer);
    if (!rdp_backend->global_rdp_peer) {
        wlr_log(WLR_DEBUG, "No global RDP peer available");
        return NULL;
    }
    wlr_log(WLR_DEBUG, "Retrieved global RDP peer: %p", (void*)rdp_backend->global_rdp_peer);
    return rdp_backend->global_rdp_peer;
}*/


/* Implementation of rdp_transmit_surface 
void rdp_transmit_surface(struct wlr_buffer *buffer) {
    if (!buffer) {
        wlr_log(WLR_ERROR, "rdp_transmit_surface: NULL buffer");
        return;
    }

    wlr_log(WLR_DEBUG, "rdp_transmit_surface: buffer=%p, width=%d, height=%d",
            (void*)buffer, buffer->width, buffer->height);

    freerdp_peer *peer = get_global_rdp_peer();
    if (!peer) {
        wlr_log(WLR_DEBUG, "rdp_transmit_surface: No peer available");
        return;
    }

    if (!peer->context) {
        wlr_log(WLR_ERROR, "rdp_transmit_surface: NULL peer context for peer %p", (void*)peer);
        return;
    }
    if (!peer->update || !peer->update->BeginPaint) {
        wlr_log(WLR_ERROR, "rdp_transmit_surface: Invalid update interface for peer %p", (void*)peer);
        return;
    }

    struct wlr_buffer *locked_buffer = wlr_buffer_lock(buffer);
    if (!locked_buffer) {
        wlr_log(WLR_ERROR, "Failed to lock buffer");
        return;
    }

    void *data;
    uint32_t format;
    size_t stride;
    if (!wlr_buffer_begin_data_ptr_access(locked_buffer, WLR_BUFFER_DATA_PTR_ACCESS_READ, &data, &format, &stride)) {
        wlr_log(WLR_ERROR, "Failed to access buffer data, format=%u", format);
        wlr_buffer_unlock(locked_buffer);
        return;
    }

    if (format != DRM_FORMAT_ARGB8888 && format != DRM_FORMAT_XRGB8888) {
        wlr_log(WLR_ERROR, "Unsupported buffer format: 0x%x", format);
        wlr_buffer_end_data_ptr_access(locked_buffer);
        wlr_buffer_unlock(locked_buffer);
        return;
    }

    rdpContext *context = peer->context;
    rdpUpdate *update = peer->update;

    update->BeginPaint(context);

    BITMAP_UPDATE bitmap_update = {0};
    BITMAP_DATA bitmap_data = {0};
    bitmap_update.number = 1;
    bitmap_update.rectangles = &bitmap_data;

    bitmap_data.destLeft = 0;
    bitmap_data.destTop = 0;
    bitmap_data.destRight = buffer->width - 1;
    bitmap_data.destBottom = buffer->height - 1;
    bitmap_data.width = buffer->width;
    bitmap_data.height = buffer->height;
    bitmap_data.bitsPerPixel = 32;
    bitmap_data.compressed = FALSE;
    bitmap_data.bitmapDataStream = data;
    bitmap_data.bitmapLength = stride * buffer->height;

    update->BitmapUpdate(context, &bitmap_update);
    update->EndPaint(context);

    wlr_buffer_end_data_ptr_access(locked_buffer);
    wlr_buffer_unlock(locked_buffer);
    wlr_log(WLR_DEBUG, "Transmitted bitmap update: %dx%d, stride=%zu, format=0x%x",
            buffer->width, buffer->height, stride, format);
}*/


/* Structures */
enum tinywl_cursor_mode {
    TINYWL_CURSOR_PASSTHROUGH,
    TINYWL_CURSOR_MOVE,
    TINYWL_CURSOR_RESIZE,
};



struct tinywl_server {
    struct wl_display *wl_display;
    struct wlr_backend *backend;
    struct wlr_renderer *renderer;
    struct wlr_allocator *allocator;
    struct wlr_scene *scene;
    struct wlr_scene_output_layout *scene_layout;

    struct wlr_xdg_shell *xdg_shell;
    struct wl_listener new_xdg_toplevel;
    struct wl_listener new_xdg_popup;
    struct wl_list toplevels;

    struct wlr_cursor *cursor;
    struct wlr_xcursor_manager *cursor_mgr;
    struct wl_listener cursor_motion;
    struct wl_listener cursor_motion_absolute;
    struct wl_listener cursor_button;
    struct wl_listener cursor_axis;
    struct wl_listener cursor_frame;

    struct wlr_seat *seat;
    struct wl_listener new_input;
    struct wl_listener request_cursor;
    struct wl_listener request_set_selection;
    struct wl_list keyboards;
    enum tinywl_cursor_mode cursor_mode;
    struct tinywl_toplevel *grabbed_toplevel;
    double grab_x, grab_y;
    struct wlr_box grab_geobox;
    uint32_t resize_edges;

    struct wlr_output_layout *output_layout;
    struct wl_list outputs;
    struct wl_listener new_output;
      EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
   EGLSurface egl_surface;
};

/* Updated tinywl_output struct to store timer */
struct tinywl_output {
    struct wl_list link;
    struct tinywl_server *server;
    struct wlr_output *wlr_output;
    struct wl_listener frame;
    struct wl_listener request_state;
    struct wl_listener destroy;
    struct wl_event_source *timer; /* Added for timer access */
};

struct tinywl_toplevel {
    struct wl_list link;
    struct tinywl_server *server;
    struct wlr_xdg_toplevel *xdg_toplevel;
    struct wlr_scene_tree *scene_tree;
    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener commit;
    struct wl_listener destroy;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    struct wl_listener request_maximize;
    struct wl_listener request_fullscreen;
};

struct tinywl_popup {
    struct wlr_xdg_popup *xdg_popup;
    struct wl_listener commit;
    struct wl_listener destroy;
};

struct tinywl_keyboard {
    struct wl_list link;
    struct tinywl_server *server;
    struct wlr_keyboard *wlr_keyboard;
    struct wl_listener modifiers;
    struct wl_listener key;
    struct wl_listener destroy;
};



/* Function declarations */
//struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display, struct wlr_egl *egl);
//struct wlr_renderer *wlr_gles2_renderer_create_surfaceless(void);
struct wlr_allocator *wlr_rdp_allocator_create(struct wlr_renderer *renderer);
void rdp_transmit_surface(struct wlr_buffer *buffer);
//freerdp_peer *get_global_rdp_peer(void);
struct wlr_egl *setup_surfaceless_egl(struct tinywl_server *server) ;
void cleanup_egl(struct tinywl_server *server);

/* Function implementations */
static void server_destroy(struct tinywl_server *server) {
    /* Remove all listeners */
    wl_list_remove(&server->new_xdg_toplevel.link);
    wl_list_remove(&server->new_xdg_popup.link);
    wl_list_remove(&server->cursor_motion.link);
    wl_list_remove(&server->cursor_motion_absolute.link);
    wl_list_remove(&server->cursor_button.link);
    wl_list_remove(&server->cursor_axis.link);
    wl_list_remove(&server->cursor_frame.link);
    wl_list_remove(&server->new_input.link);
    wl_list_remove(&server->request_cursor.link);
    wl_list_remove(&server->request_set_selection.link);
    wl_list_remove(&server->new_output.link);

    /* Clean up toplevels */
    struct tinywl_toplevel *toplevel, *toplevel_tmp;
    wl_list_for_each_safe(toplevel, toplevel_tmp, &server->toplevels, link) {
        wl_list_remove(&toplevel->link);
        wl_list_remove(&toplevel->map.link);
        wl_list_remove(&toplevel->unmap.link);
        wl_list_remove(&toplevel->commit.link);
        wl_list_remove(&toplevel->destroy.link);
        wl_list_remove(&toplevel->request_move.link);
        wl_list_remove(&toplevel->request_resize.link);
        wl_list_remove(&toplevel->request_maximize.link);
        wl_list_remove(&toplevel->request_fullscreen.link);
        free(toplevel);
    }

    /* Clean up outputs */
    struct tinywl_output *output, *output_tmp;
    wl_list_for_each_safe(output, output_tmp, &server->outputs, link) {
        wl_list_remove(&output->link);
        wl_list_remove(&output->frame.link);
        wl_list_remove(&output->request_state.link);
        wl_list_remove(&output->destroy.link);
        free(output);
    }

    /* Clean up keyboards */
    struct tinywl_keyboard *kb, *kb_tmp;
    wl_list_for_each_safe(kb, kb_tmp, &server->keyboards, link) {
        wl_list_remove(&kb->link);
        wl_list_remove(&kb->modifiers.link);
        wl_list_remove(&kb->key.link);
        wl_list_remove(&kb->destroy.link);
        free(kb);
    }

    /* Destroy resources */
    if (server->scene) {
        wlr_scene_node_destroy(&server->scene->tree.node);
    }
    if (server->cursor_mgr) {
        wlr_xcursor_manager_destroy(server->cursor_mgr);
    }
    if (server->cursor) {
        wlr_cursor_destroy(server->cursor);
    }
    if (server->allocator) {
        wlr_allocator_destroy(server->allocator);
    }
    /* Temporary workaround for renderer buffer list corruption */
    if (server->renderer) {
        wlr_log(WLR_INFO, "Skipping renderer destruction due to potential buffer list corruption");
        // wlr_renderer_destroy(server.renderer); // Commented out to avoid segfault
    }
    if (server->backend) {
        wlr_backend_destroy(server->backend);
    }
    if (server->seat) {
        wlr_seat_destroy(server->seat);
    }
    if (server->wl_display) {
        wl_display_destroy(server->wl_display);
    }
}




static void focus_toplevel(struct tinywl_toplevel *toplevel) {
    if (toplevel == NULL) {
        return;
    }
    struct tinywl_server *server = toplevel->server;
    struct wlr_seat *seat = server->seat;
    struct wlr_surface *prev_surface = seat->keyboard_state.focused_surface;
    struct wlr_surface *surface = toplevel->xdg_toplevel->base->surface;
    if (prev_surface == surface) {
        return;
    }
    if (prev_surface) {
        struct wlr_xdg_toplevel *prev_toplevel =
            wlr_xdg_toplevel_try_from_wlr_surface(prev_surface);
        if (prev_toplevel != NULL) {
            wlr_xdg_toplevel_set_activated(prev_toplevel, false);
        }
    }
    struct wlr_keyboard *keyboard = wlr_seat_get_keyboard(seat);
    wlr_scene_node_raise_to_top(&toplevel->scene_tree->node);
    wl_list_remove(&toplevel->link);
    wl_list_insert(&server->toplevels, &toplevel->link);
    wlr_xdg_toplevel_set_activated(toplevel->xdg_toplevel, true);
    if (keyboard != NULL) {
        wlr_seat_keyboard_notify_enter(seat, surface,
            keyboard->keycodes, keyboard->num_keycodes, &keyboard->modifiers);
    }
}

static void keyboard_handle_modifiers(struct wl_listener *listener, void *data) {
    struct tinywl_keyboard *keyboard = wl_container_of(listener, keyboard, modifiers);
    wlr_seat_set_keyboard(keyboard->server->seat, keyboard->wlr_keyboard);
    wlr_seat_keyboard_notify_modifiers(keyboard->server->seat,
        &keyboard->wlr_keyboard->modifiers);
}

static bool handle_keybinding(struct tinywl_server *server, xkb_keysym_t sym) {
    switch (sym) {
    case XKB_KEY_Escape:
        wl_display_terminate(server->wl_display);
        break;
    case XKB_KEY_F1:
        if (wl_list_length(&server->toplevels) < 2) {
            break;
        }
        struct tinywl_toplevel *next_toplevel =
            wl_container_of(server->toplevels.prev, next_toplevel, link);
        focus_toplevel(next_toplevel);
        break;
    default:
        return false;
    }
    return true;
}

static void keyboard_handle_key(struct wl_listener *listener, void *data) {
    struct tinywl_keyboard *keyboard = wl_container_of(listener, keyboard, key);
    struct tinywl_server *server = keyboard->server;
    struct wlr_keyboard_key_event *event = data;
    struct wlr_seat *seat = server->seat;

    uint32_t keycode = event->keycode + 8;
    const xkb_keysym_t *syms;
    int nsyms = xkb_state_key_get_syms(keyboard->wlr_keyboard->xkb_state, keycode, &syms);

    bool handled = false;
    uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard->wlr_keyboard);
    if ((modifiers & WLR_MODIFIER_ALT) && event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
        for (int i = 0; i < nsyms; i++) {
            handled = handle_keybinding(server, syms[i]);
        }
    }

    if (!handled) {
        wlr_seat_set_keyboard(seat, keyboard->wlr_keyboard);
        wlr_seat_keyboard_notify_key(seat, event->time_msec, event->keycode, event->state);
    }
}

static void keyboard_handle_destroy(struct wl_listener *listener, void *data) {
    struct tinywl_keyboard *keyboard = wl_container_of(listener, keyboard, destroy);
    wl_list_remove(&keyboard->modifiers.link);
    wl_list_remove(&keyboard->key.link);
    wl_list_remove(&keyboard->destroy.link);
    wl_list_remove(&keyboard->link);
    free(keyboard);
}

static void server_new_keyboard(struct tinywl_server *server, struct wlr_input_device *device) {
    struct wlr_keyboard *wlr_keyboard = wlr_keyboard_from_input_device(device);
    struct tinywl_keyboard *keyboard = calloc(1, sizeof(*keyboard));
    keyboard->server = server;
    keyboard->wlr_keyboard = wlr_keyboard;

    struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    struct xkb_keymap *keymap = xkb_keymap_new_from_names(context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);
    wlr_keyboard_set_keymap(wlr_keyboard, keymap);
    xkb_keymap_unref(keymap);
    xkb_context_unref(context);
    wlr_keyboard_set_repeat_info(wlr_keyboard, 25, 600);

    keyboard->modifiers.notify = keyboard_handle_modifiers;
    wl_signal_add(&wlr_keyboard->events.modifiers, &keyboard->modifiers);
    keyboard->key.notify = keyboard_handle_key;
    wl_signal_add(&wlr_keyboard->events.key, &keyboard->key);
    keyboard->destroy.notify = keyboard_handle_destroy;
    wl_signal_add(&device->events.destroy, &keyboard->destroy);

    wlr_seat_set_keyboard(server->seat, keyboard->wlr_keyboard);
    wl_list_insert(&server->keyboards, &keyboard->link);
}

static void server_new_pointer(struct tinywl_server *server, struct wlr_input_device *device) {
    wlr_cursor_attach_input_device(server->cursor, device);
}

static void server_new_input(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, new_input);
    struct wlr_input_device *device = data;
    switch (device->type) {
    case WLR_INPUT_DEVICE_KEYBOARD:
        server_new_keyboard(server, device);
        break;
    case WLR_INPUT_DEVICE_POINTER:
        server_new_pointer(server, device);
        break;
    default:
        break;
    }
    uint32_t caps = WL_SEAT_CAPABILITY_POINTER;
    if (!wl_list_empty(&server->keyboards)) {
        caps |= WL_SEAT_CAPABILITY_KEYBOARD;
    }
    wlr_seat_set_capabilities(server->seat, caps);
}

static void seat_request_cursor(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, request_cursor);
    struct wlr_seat_pointer_request_set_cursor_event *event = data;
    struct wlr_seat_client *focused_client = server->seat->pointer_state.focused_client;
    if (focused_client == event->seat_client) {
        wlr_cursor_set_surface(server->cursor, event->surface, event->hotspot_x, event->hotspot_y);
    }
}

static void seat_request_set_selection(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, request_set_selection);
    struct wlr_seat_request_set_selection_event *event = data;
    wlr_seat_set_selection(server->seat, event->source, event->serial);
}

static struct tinywl_toplevel *desktop_toplevel_at(struct tinywl_server *server, double lx, double ly,
        struct wlr_surface **surface, double *sx, double *sy) {
    struct wlr_scene_node *node = wlr_scene_node_at(&server->scene->tree.node, lx, ly, sx, sy);
    if (node == NULL || node->type != WLR_SCENE_NODE_BUFFER) {
        return NULL;
    }
    struct wlr_scene_buffer *scene_buffer = wlr_scene_buffer_from_node(node);
    struct wlr_scene_surface *scene_surface = wlr_scene_surface_try_from_buffer(scene_buffer);
    if (!scene_surface) {
        return NULL;
    }
    *surface = scene_surface->surface;
    struct wlr_scene_tree *tree = node->parent;
    while (tree != NULL && tree->node.data == NULL) {
        tree = tree->node.parent;
    }
    return tree->node.data;
}

static void reset_cursor_mode(struct tinywl_server *server) {
    server->cursor_mode = TINYWL_CURSOR_PASSTHROUGH;
    server->grabbed_toplevel = NULL;
}

static void process_cursor_move(struct tinywl_server *server) {
    struct tinywl_toplevel *toplevel = server->grabbed_toplevel;
    wlr_scene_node_set_position(&toplevel->scene_tree->node,
        server->cursor->x - server->grab_x, server->cursor->y - server->grab_y);
}

static void process_cursor_resize(struct tinywl_server *server) {
    struct tinywl_toplevel *toplevel = server->grabbed_toplevel;
    double border_x = server->cursor->x - server->grab_x;
    double border_y = server->cursor->y - server->grab_y;
    int new_left = server->grab_geobox.x;
    int new_right = server->grab_geobox.x + server->grab_geobox.width;
    int new_top = server->grab_geobox.y;
    int new_bottom = server->grab_geobox.y + server->grab_geobox.height;

    if (server->resize_edges & WLR_EDGE_TOP) {
        new_top = border_y;
        if (new_top >= new_bottom) {
            new_top = new_bottom - 1;
        }
    } else if (server->resize_edges & WLR_EDGE_BOTTOM) {
        new_bottom = border_y;
        if (new_bottom <= new_top) {
            new_bottom = new_top + 1;
        }
    }
    if (server->resize_edges & WLR_EDGE_LEFT) {
        new_left = border_x;
        if (new_left >= new_right) {
            new_left = new_right - 1;
        }
    } else if (server->resize_edges & WLR_EDGE_RIGHT) {
        new_right = border_x;
        if (new_right <= new_left) {
            new_right = new_left + 1;
        }
    }

    struct wlr_box *geo_box = &toplevel->xdg_toplevel->base->geometry;
    wlr_scene_node_set_position(&toplevel->scene_tree->node,
        new_left - geo_box->x, new_top - geo_box->y);

    int new_width = new_right - new_left;
    int new_height = new_bottom - new_top;
    wlr_xdg_toplevel_set_size(toplevel->xdg_toplevel, new_width, new_height);
}

static void process_cursor_motion(struct tinywl_server *server, uint32_t time) {
    if (server->cursor_mode == TINYWL_CURSOR_MOVE) {
        process_cursor_move(server);
        return;
    } else if (server->cursor_mode == TINYWL_CURSOR_RESIZE) {
        process_cursor_resize(server);
        return;
    }

    double sx, sy;
    struct wlr_seat *seat = server->seat;
    struct wlr_surface *surface = NULL;
    struct tinywl_toplevel *toplevel = desktop_toplevel_at(server,
            server->cursor->x, server->cursor->y, &surface, &sx, &sy);
    if (!toplevel) {
        wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
    }
    if (surface) {
        wlr_seat_pointer_notify_enter(seat, surface, sx, sy);
        wlr_seat_pointer_notify_motion(seat, time, sx, sy);
    } else {
        wlr_seat_pointer_clear_focus(seat);
    }
}

static void server_cursor_motion(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_motion);
    struct wlr_pointer_motion_event *event = data;
    wlr_cursor_move(server->cursor, &event->pointer->base, event->delta_x, event->delta_y);
    process_cursor_motion(server, event->time_msec);
}

static void server_cursor_motion_absolute(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_motion_absolute);
    struct wlr_pointer_motion_absolute_event *event = data;
    wlr_cursor_warp_absolute(server->cursor, &event->pointer->base, event->x, event->y);
    process_cursor_motion(server, event->time_msec);
}

static void server_cursor_button(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_button);
    struct wlr_pointer_button_event *event = data;
    wlr_seat_pointer_notify_button(server->seat, event->time_msec, event->button, event->state);
    if (event->state == WL_POINTER_BUTTON_STATE_RELEASED) {
        reset_cursor_mode(server);
    } else {
        double sx, sy;
        struct wlr_surface *surface = NULL;
        struct tinywl_toplevel *toplevel = desktop_toplevel_at(server,
                server->cursor->x, server->cursor->y, &surface, &sx, &sy);
        focus_toplevel(toplevel);
    }
}

static void server_cursor_axis(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_axis);
    struct wlr_pointer_axis_event *event = data;
    wlr_seat_pointer_notify_axis(server->seat, event->time_msec, event->orientation,
        event->delta, event->delta_discrete, event->source, event->relative_direction);
}

static void server_cursor_frame(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_frame);
    wlr_seat_pointer_notify_frame(server->seat);
}





/* Updated timer callback without damage */
static int schedule_frame_timer(void *data) {
    struct tinywl_output *output = data;
    struct wlr_output *wlr_output = output->wlr_output;
    struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(output->server->scene, wlr_output);

    wlr_output_schedule_frame(wlr_output);
    wlr_log(WLR_DEBUG, "Scheduled frame for output %s via timer", wlr_output->name);

    /* Attempt to force rendering by committing the scene output */
    if (scene_output) {
        wlr_scene_output_commit(scene_output, NULL);
        wlr_log(WLR_DEBUG, "Committed scene output for %s to force rendering", wlr_output->name);
    } else {
        wlr_log(WLR_DEBUG, "No scene output found for %s during timer", wlr_output->name);
    }

    /* Rearm the timer */
    if (output->timer) {
        wl_event_source_timer_update(output->timer, 100); /* 100ms = 10 FPS */
    }

    return 1; /* Keep timer alive */
}

/* Unchanged output_frame with debug logging */
static void output_frame(struct wl_listener *listener, void *data) {
    wlr_log(WLR_DEBUG, "output_frame called for output");
    struct tinywl_output *output = wl_container_of(listener, output, frame);
    struct tinywl_server *server = output->server;
    struct wlr_scene *scene = server->scene;
    struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(scene, output->wlr_output);

    if (!scene_output) {
        wlr_log(WLR_ERROR, "No scene_output found for this output");
        return;
    }
    wlr_log(WLR_DEBUG, "Scene output found for output %s", output->wlr_output->name);

    /* Lock the output for rendering */
    wlr_output_lock_attach_render(output->wlr_output, true);

    /* Commit the scene output to render the frame */
    struct wlr_scene_output_state_options opts = {0};
    if (!wlr_scene_output_commit(scene_output, &opts)) {
        wlr_log(WLR_ERROR, "wlr_scene_output_commit failed");
        wlr_output_lock_attach_render(output->wlr_output, false);
        return;
    }
    wlr_log(WLR_DEBUG, "Scene output committed successfully");

    /* Take screenshot automatically */
    int width = output->wlr_output->width;
    int height = output->wlr_output->height;

    /* Read pixels for screenshot */
    size_t stride = width * 4; /* 4 bytes per pixel (RGBA) */
    size_t size = stride * height;
    uint8_t *pixels = malloc(size);
    if (!pixels) {
        wlr_log(WLR_ERROR, "Failed to allocate memory for pixels");
        wlr_output_lock_attach_render(output->wlr_output, false);
        return;
    }

    /* Capture the rendered frame */
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);



    wlr_output_lock_attach_render(output->wlr_output, false);

    /* Send frame done event */
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    wlr_scene_output_send_frame_done(scene_output, &now);
}


static void output_request_state(struct wl_listener *listener, void *data) {
    struct tinywl_output *output = wl_container_of(listener, output, request_state);
    const struct wlr_output_event_request_state *event = data;
    wlr_output_commit_state(output->wlr_output, event->state);
}

static void output_destroy(struct wl_listener *listener, void *data) {
    struct tinywl_output *output = wl_container_of(listener, output, destroy);
    wl_list_remove(&output->frame.link);
    wl_list_remove(&output->request_state.link);
    wl_list_remove(&output->destroy.link);
    wl_list_remove(&output->link);
    free(output);
}



/* Updated server_new_output without damage */
static void server_new_output(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, new_output);
    struct wlr_output *wlr_output = data;

    wlr_log(WLR_INFO, "Initializing RDP output with surfaceless EGL/Zink");

    if (!server->renderer || !server->allocator) {
        wlr_log(WLR_ERROR, "No renderer or allocator available");
        return;
    }

    struct wlr_allocator *new_allocator = wlr_rdp_allocator_create(server->renderer);
    if (!new_allocator) {
        wlr_log(WLR_ERROR, "Failed to create RDP allocator");
        return;
    }

    if (!wlr_output_init_render(wlr_output, new_allocator, server->renderer)) {
        wlr_log(WLR_ERROR, "Failed to initialize output rendering");
        wlr_allocator_destroy(new_allocator);
        return;
    }

    if (server->allocator) {
        wlr_allocator_destroy(server->allocator);
    }
    server->allocator = new_allocator;

    const uint32_t formats[] = {
        DRM_FORMAT_XRGB8888,
        DRM_FORMAT_ARGB8888,
        DRM_FORMAT_XBGR8888,
        WL_SHM_FORMAT_XRGB8888,
        WL_SHM_FORMAT_ARGB8888
    };

    bool output_initialized = false;
    for (size_t i = 0; i < sizeof(formats)/sizeof(formats[0]); i++) {
        wlr_log(WLR_DEBUG, "Attempting format: 0x%x", formats[i]);
        struct wlr_output_state state;
        wlr_output_state_init(&state);
        wlr_output_state_set_enabled(&state, true);
        wlr_output_state_set_custom_mode(&state, 1024, 768, 60000);
        wlr_output_state_set_render_format(&state, formats[i]);
        if (wlr_output_test_state(wlr_output, &state)) {
            if (wlr_output_commit_state(wlr_output, &state)) {
                wlr_log(WLR_INFO, "Successfully committed output with format 0x%x", formats[i]);
                output_initialized = true;
            }
        }
        wlr_output_state_finish(&state);
        if (output_initialized) break;
    }

    if (!output_initialized) {
        wlr_log(WLR_ERROR, "Failed to initialize output with any format");
        return;
    }

    struct tinywl_output *output = calloc(1, sizeof(*output));
    if (!output) {
        wlr_log(WLR_ERROR, "Failed to allocate output structure");
        return;
    }

    output->wlr_output = wlr_output;
    output->server = server;

    output->frame.notify = output_frame;
    wl_signal_add(&wlr_output->events.frame, &output->frame);
    output->request_state.notify = output_request_state;
    wl_signal_add(&wlr_output->events.request_state, &output->request_state);
    output->destroy.notify = output_destroy;
    wl_signal_add(&wlr_output->events.destroy, &output->destroy);

    wl_list_insert(&server->outputs, &output->link);

    struct wlr_output_layout_output *l_output = wlr_output_layout_add_auto(server->output_layout, wlr_output);
    struct wlr_scene_output *scene_output = wlr_scene_output_create(server->scene, wlr_output);
    wlr_scene_output_layout_add_output(server->scene_layout, l_output, scene_output);

    /* Attempt initial rendering by committing the scene output */
    if (scene_output) {
        wlr_scene_output_commit(scene_output, NULL);
        wlr_log(WLR_INFO, "Committed scene output for %s to force initial rendering", wlr_output->name);
    } else {
        wlr_log(WLR_ERROR, "Failed to create scene output for initial rendering");
    }

    /* Schedule an initial frame */
    wlr_output_schedule_frame(wlr_output);
    wlr_log(WLR_INFO, "Scheduled initial frame for output %s", wlr_output->name);

    /* Set up a timer to continuously schedule frames */
    struct wl_event_loop *loop = wl_display_get_event_loop(server->wl_display);
    struct wl_event_source *timer = wl_event_loop_add_timer(loop, schedule_frame_timer, output);
    if (!timer) {
        wlr_log(WLR_ERROR, "Failed to create frame scheduling timer");
    } else {
        output->timer = timer;
        wl_event_source_timer_update(timer, 100); /* 100ms = 10 FPS */
        wlr_log(WLR_INFO, "Frame scheduling timer set up for output %s", wlr_output->name);
    }

    wlr_log(WLR_INFO, "RDP output initialized successfully");
}

static void xdg_toplevel_map(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, map);
    wl_list_insert(&toplevel->server->toplevels, &toplevel->link);
    focus_toplevel(toplevel);

    struct wlr_surface *surface = toplevel->xdg_toplevel->base->surface;
    if (surface) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        wlr_surface_send_frame_done(surface, &now);
    }
}

static void xdg_toplevel_unmap(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, unmap);
    if (toplevel == toplevel->server->grabbed_toplevel) {
        reset_cursor_mode(toplevel->server);
    }
    wl_list_remove(&toplevel->link);
}

static void xdg_toplevel_commit(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, commit);
    if (toplevel->xdg_toplevel->base->initial_commit) {
        wlr_xdg_toplevel_set_size(toplevel->xdg_toplevel, 0, 0);
    }

    struct wlr_surface *surface = toplevel->xdg_toplevel->base->surface;
    wlr_log(WLR_DEBUG, "xdg_toplevel_commit: surface=%p, has_buffer=%d, mapped=%d, buffer=%p",
            (void*)surface, wlr_surface_has_buffer(surface), surface->mapped,
            surface->buffer ? (void*)surface->buffer : NULL);
    if (surface && wlr_surface_has_buffer(surface)) {
        rdp_transmit_surface(&surface->buffer->base);
        wlr_log(WLR_DEBUG, "Transmitted surface buffer to RDP peer for surface %p", (void*)surface);
    } else {
        wlr_log(WLR_DEBUG, "Surface %p has no buffer to transmit", (void*)surface);
    }

    /* Schedule a frame to ensure rendering updates */
    struct tinywl_output *output;
    wl_list_for_each(output, &toplevel->server->outputs, link) {
        if (wlr_scene_get_scene_output(toplevel->server->scene, output->wlr_output)) {
            wlr_output_schedule_frame(output->wlr_output);
            wlr_log(WLR_DEBUG, "Scheduled frame for output %s after surface commit", output->wlr_output->name);
        }
    }
}

static void xdg_toplevel_destroy(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, destroy);
    wl_list_remove(&toplevel->map.link);
    wl_list_remove(&toplevel->unmap.link);
    wl_list_remove(&toplevel->commit.link);
    wl_list_remove(&toplevel->destroy.link);
    wl_list_remove(&toplevel->request_move.link);
    wl_list_remove(&toplevel->request_resize.link);
    wl_list_remove(&toplevel->request_maximize.link);
    wl_list_remove(&toplevel->request_fullscreen.link);
    free(toplevel);
}

static void begin_interactive(struct tinywl_toplevel *toplevel, enum tinywl_cursor_mode mode, uint32_t edges) {
    struct tinywl_server *server = toplevel->server;
    server->grabbed_toplevel = toplevel;
    server->cursor_mode = mode;

    if (mode == TINYWL_CURSOR_MOVE) {
        server->grab_x = server->cursor->x - toplevel->scene_tree->node.x;
        server->grab_y = server->cursor->y - toplevel->scene_tree->node.y;
    } else {
        struct wlr_box *geo_box = &toplevel->xdg_toplevel->base->geometry;
        double border_x = (toplevel->scene_tree->node.x + geo_box->x) +
            ((edges & WLR_EDGE_RIGHT) ? geo_box->width : 0);
        double border_y = (toplevel->scene_tree->node.y + geo_box->y) +
            ((edges & WLR_EDGE_BOTTOM) ? geo_box->height : 0);
        server->grab_x = server->cursor->x - border_x;
        server->grab_y = server->cursor->y - border_y;
        server->grab_geobox = *geo_box;
        server->grab_geobox.x += toplevel->scene_tree->node.x;
        server->grab_geobox.y += toplevel->scene_tree->node.y;
        server->resize_edges = edges;
    }
}

static void xdg_toplevel_request_move(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, request_move);
    begin_interactive(toplevel, TINYWL_CURSOR_MOVE, 0);
}

static void xdg_toplevel_request_resize(struct wl_listener *listener, void *data) {
    struct wlr_xdg_toplevel_resize_event *event = data;
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, request_resize);
    begin_interactive(toplevel, TINYWL_CURSOR_RESIZE, event->edges);
}

static void xdg_toplevel_request_maximize(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, request_maximize);
    if (toplevel->xdg_toplevel->base->initialized) {
        wlr_xdg_surface_schedule_configure(toplevel->xdg_toplevel->base);
    }
}

static void xdg_toplevel_request_fullscreen(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, request_fullscreen);
    if (toplevel->xdg_toplevel->base->initialized) {
        wlr_xdg_surface_schedule_configure(toplevel->xdg_toplevel->base);
    }
}

static void server_new_xdg_toplevel(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, new_xdg_toplevel);
    struct wlr_xdg_toplevel *xdg_toplevel = data;
    struct tinywl_toplevel *toplevel = calloc(1, sizeof(*toplevel));
    toplevel->server = server;
    toplevel->xdg_toplevel = xdg_toplevel;
    toplevel->scene_tree = wlr_scene_xdg_surface_create(&toplevel->server->scene->tree, xdg_toplevel->base);
    toplevel->scene_tree->node.data = toplevel;
    xdg_toplevel->base->data = toplevel->scene_tree;

    toplevel->map.notify = xdg_toplevel_map;
    wl_signal_add(&xdg_toplevel->base->surface->events.map, &toplevel->map);
    toplevel->unmap.notify = xdg_toplevel_unmap;
    wl_signal_add(&xdg_toplevel->base->surface->events.unmap, &toplevel->unmap);
    toplevel->commit.notify = xdg_toplevel_commit;
    wl_signal_add(&xdg_toplevel->base->surface->events.commit, &toplevel->commit);
    toplevel->destroy.notify = xdg_toplevel_destroy;
    wl_signal_add(&xdg_toplevel->events.destroy, &toplevel->destroy);
    toplevel->request_move.notify = xdg_toplevel_request_move;
    wl_signal_add(&xdg_toplevel->events.request_move, &toplevel->request_move);
    toplevel->request_resize.notify = xdg_toplevel_request_resize;
    wl_signal_add(&xdg_toplevel->events.request_resize, &toplevel->request_resize);
    toplevel->request_maximize.notify = xdg_toplevel_request_maximize;
    wl_signal_add(&xdg_toplevel->events.request_maximize, &toplevel->request_maximize);
    toplevel->request_fullscreen.notify = xdg_toplevel_request_fullscreen;
    wl_signal_add(&xdg_toplevel->events.request_fullscreen, &toplevel->request_fullscreen);
}

static void xdg_popup_commit(struct wl_listener *listener, void *data) {
    struct tinywl_popup *popup = wl_container_of(listener, popup, commit);
    if (popup->xdg_popup->base->initial_commit) {
        wlr_xdg_surface_schedule_configure(popup->xdg_popup->base);
    }
}

static void xdg_popup_destroy(struct wl_listener *listener, void *data) {
    struct tinywl_popup *popup = wl_container_of(listener, popup, destroy);
    wl_list_remove(&popup->commit.link);
    wl_list_remove(&popup->destroy.link);
    free(popup);
}

static void server_new_xdg_popup(struct wl_listener *listener, void *data) {
    struct wlr_xdg_popup *xdg_popup = data;
    struct tinywl_popup *popup = calloc(1, sizeof(*popup));
    popup->xdg_popup = xdg_popup;
    struct wlr_xdg_surface *parent = wlr_xdg_surface_try_from_wlr_surface(xdg_popup->parent);
    assert(parent != NULL);
    struct wlr_scene_tree *parent_tree = parent->data;
    xdg_popup->base->data = wlr_scene_xdg_surface_create(parent_tree, xdg_popup->base);
    popup->commit.notify = xdg_popup_commit;
    wl_signal_add(&xdg_popup->base->surface->events.commit, &popup->commit);
    popup->destroy.notify = xdg_popup_destroy;
    wl_signal_add(&xdg_popup->events.destroy, &popup->destroy);
}


static bool is_zink_renderer(struct wlr_backend *backend, struct wlr_renderer *renderer, struct wlr_allocator *allocator) {
    if (!renderer) {
        wlr_log(WLR_ERROR, "No renderer available to check type");
        return false;
    }

    struct wlr_egl *egl = wlr_gles2_renderer_get_egl(renderer);
    if (!egl) {
        wlr_log(WLR_ERROR, "No EGL context available to check renderer");
        return false;
    }

    // Get the EGL display and context from wlr_egl
    EGLDisplay egl_display = wlr_egl_get_display(egl);
    EGLContext egl_context = wlr_egl_get_context(egl);
    if (!egl_display || !egl_context) {
        wlr_log(WLR_ERROR, "Invalid EGL display or context");
        return false;
    }

    // Make the EGL context current
    if (!eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_context)) {
        wlr_log(WLR_ERROR, "Failed to make EGL context current: 0x%x", eglGetError());
        return false;
    }

    // Query the GL_RENDERER string
    const char *gl_renderer = (const char *)glGetString(GL_RENDERER);
    if (!gl_renderer) {
        wlr_log(WLR_ERROR, "Failed to query GL_RENDERER");
        eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        return false;
    }

    // Check if the renderer is Zink
    bool is_zink = strstr(gl_renderer, "Zink") != NULL;
    wlr_log(WLR_DEBUG, "Renderer check: is_zink=%d, GL_RENDERER=%s", is_zink, gl_renderer);
    if (!is_zink) {
        wlr_log(WLR_DEBUG, "Renderer is not Zink: %s", gl_renderer);
    }

    // Unset the current EGL context
    eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    return is_zink;
}

#ifndef EGL_PLATFORM_SURFACELESS_MESA
#define EGL_PLATFORM_SURFACELESS_MESA 0x31DD
#endif
#ifndef EGL_PLATFORM_SURFACELESS_MESA
#define EGL_PLATFORM_SURFACELESS_MESA 0x31DD
#endif

#ifndef EGL_PLATFORM_SURFACELESS_MESA
#define EGL_PLATFORM_SURFACELESS_MESA 0x31DD
#endif

struct wlr_egl *setup_surfaceless_egl(struct tinywl_server *server) {
wlr_log(WLR_INFO, "Starting surfaceless EGL setup");

    // 1. Check for client extensions (can use EGL_NO_DISPLAY)
    const char *client_extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    if (!client_extensions) {
        wlr_log(WLR_ERROR, "Could not query EGL client extensions");
        return NULL;
    }
    wlr_log(WLR_INFO, "EGL client extensions: %s", client_extensions);

    bool has_surfaceless = strstr(client_extensions, "EGL_MESA_platform_surfaceless") != NULL;
    bool has_platform_base = strstr(client_extensions, "EGL_EXT_platform_base") != NULL;
    wlr_log(WLR_INFO, "Surfaceless platform support: %s", has_surfaceless ? "YES" : "NO");
    wlr_log(WLR_INFO, "Platform base extension: %s", has_platform_base ? "YES" : "NO");

    if (!has_surfaceless || !has_platform_base) {
        wlr_log(WLR_ERROR, "Required EGL client extensions not available");
        return NULL;
    }

    // 2. Get platform display function
    PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform_display =
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    if (!get_platform_display) {
        wlr_log(WLR_ERROR, "Platform display function not available");
        return NULL;
    }
    wlr_log(WLR_INFO, "Retrieved eglGetPlatformDisplayEXT");

    // 3. Create surfaceless display
    EGLDisplay display = get_platform_display(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, NULL);
    if (display == EGL_NO_DISPLAY) {
        wlr_log(WLR_ERROR, "Failed to create surfaceless display. Error: 0x%x", eglGetError());
        return NULL;
    }
    wlr_log(WLR_INFO, "Created EGL display: %p", (void*)display);

    // 4. Initialize EGL
    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        wlr_log(WLR_ERROR, "EGL initialization failed. Error: 0x%x", eglGetError());
        eglTerminate(display);
        return NULL;
    }
    wlr_log(WLR_INFO, "EGL initialized, version: %d.%d", major, minor);

    // 5. Query display extensions (must use initialized display)
    const char *display_extensions = eglQueryString(display, EGL_EXTENSIONS);
    if (!display_extensions) {
        wlr_log(WLR_ERROR, "Failed to query EGL display extensions. Error: 0x%x", eglGetError());
        eglTerminate(display);
        return NULL;
    }
    wlr_log(WLR_INFO, "EGL display extensions: %s", display_extensions);

    // 6. Set bind API first (before choosing config)
    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        wlr_log(WLR_ERROR, "Failed to bind OpenGL ES API. Error: 0x%x", eglGetError());
        eglTerminate(display);
        return NULL;
    }
    wlr_log(WLR_INFO, "Successfully bound OpenGL ES API");

    // 7. Choose a config with explicit GLES2
    const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    EGLConfig config;
    EGLint num_config;
    if (!eglChooseConfig(display, config_attribs, &config, 1, &num_config) || num_config < 1) {
        wlr_log(WLR_ERROR, "Failed to choose EGL config. Error: 0x%x", eglGetError());
        eglTerminate(display);
        return NULL;
    }
    wlr_log(WLR_INFO, "Found suitable EGL configuration");

    // 8. Create context with explicit version
    const EGLint ctx_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE // Remove profile mask to avoid compatibility issues
    };

    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attribs);
    if (context == EGL_NO_CONTEXT) {
        wlr_log(WLR_ERROR, "Failed to create EGL context. Error: 0x%x", eglGetError());
        eglTerminate(display);
        return NULL;
    }
    wlr_log(WLR_INFO, "Created EGL context");

    // 9. Query EGL_CONTEXT_CLIENT_TYPE
    EGLint client_type;
    if (!eglQueryContext(display, context, EGL_CONTEXT_CLIENT_TYPE, &client_type)) {
        wlr_log(WLR_ERROR, "Failed to query EGL_CONTEXT_CLIENT_TYPE. Error: 0x%x", eglGetError());
    } else {
        wlr_log(WLR_INFO, "EGL_CONTEXT_CLIENT_TYPE: 0x%x", client_type);
        if (client_type == EGL_OPENGL_ES_API) {
            wlr_log(WLR_INFO, "Context client type is EGL_OPENGL_ES_API");
        } else {
            wlr_log(WLR_ERROR, "Unexpected context client type: 0x%x", client_type);
            eglDestroyContext(display, context);
            eglTerminate(display);
            return NULL;
        }
    }

    // 10. Create pbuffer surface
    const EGLint pbuffer_attribs[] = {
        EGL_WIDTH, 16,
        EGL_HEIGHT, 16,
        EGL_NONE
    };

    EGLSurface surface = eglCreatePbufferSurface(display, config, pbuffer_attribs);
    if (surface == EGL_NO_SURFACE) {
        wlr_log(WLR_ERROR, "Failed to create pbuffer surface. Error: 0x%x", eglGetError());
        eglDestroyContext(display, context);
        eglTerminate(display);
        return NULL;
    }
    wlr_log(WLR_INFO, "Created EGL pbuffer surface");

    // 11. Make context current to verify it works
    if (!eglMakeCurrent(display, surface, surface, context)) {
        wlr_log(WLR_ERROR, "Failed to make context current. Error: 0x%x", eglGetError());
        eglDestroySurface(display, surface);
        eglDestroyContext(display, context);
        eglTerminate(display);
        return NULL;
    }
    wlr_log(WLR_INFO, "Made EGL context current");

    // 12. Verify the context is GLES2 as expected
    const char *gl_vendor = (const char *)glGetString(GL_VENDOR);
    const char *gl_renderer = (const char *)glGetString(GL_RENDERER);
    const char *gl_version = (const char *)glGetString(GL_VERSION);
    wlr_log(WLR_INFO, "OpenGL ES Vendor: %s", gl_vendor ? gl_vendor : "Unknown");
    wlr_log(WLR_INFO, "OpenGL ES Renderer: %s", gl_renderer ? gl_renderer : "Unknown");
    wlr_log(WLR_INFO, "OpenGL ES Version: %s", gl_version ? gl_version : "Unknown");

    // 13. Store EGL resources in server structure
    server->egl_display = display;
    server->egl_config = config;
    server->egl_context = context;
    server->egl_surface = surface;

    // 14. Create wlr_egl with the valid context
    wlr_log(WLR_INFO, "Creating wlr_egl with context");
    struct wlr_egl *wlr_egl = wlr_egl_create_with_context(display, context);
    if (!wlr_egl) {
        wlr_log(WLR_ERROR, "Failed to create wlr_egl with context");
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(display, surface);
        eglDestroyContext(display, context);
        eglTerminate(display);
        return NULL;
    }

    wlr_log(WLR_INFO, "Successfully created wlr_egl");
    return wlr_egl;
}
void cleanup_egl(struct tinywl_server *server) {
    printf("Cleaning up EGL resources\n");
    if (server->egl_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(server->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (server->egl_surface != EGL_NO_SURFACE) {
            eglDestroySurface(server->egl_display, server->egl_surface);
            server->egl_surface = EGL_NO_SURFACE;
        }
        if (server->egl_context != EGL_NO_CONTEXT) {
            eglDestroyContext(server->egl_display, server->egl_context);
            server->egl_context = EGL_NO_CONTEXT;
        }
        eglTerminate(server->egl_display);
        server->egl_display = EGL_NO_DISPLAY;
    }
}


/* Updated main function */
int main(int argc, char *argv[]) {
    


    printf("Starting compositor with surfaceless EGL display\n");

    // Print environment variables
    printf("VK_ICD_FILENAMES=%s\n", getenv("VK_ICD_FILENAMES"));
    printf("MESA_LOADER_DRIVER_OVERRIDE=%s\n", getenv("MESA_LOADER_DRIVER_OVERRIDE"));
    printf("GALLIUM_DRIVER=%s\n", getenv("GALLIUM_DRIVER"));
    wlr_log_init(WLR_DEBUG, NULL);
    char *startup_cmd = NULL;

    int c;
    while ((c = getopt(argc, argv, "s:h")) != -1) {
        switch (c) {
        case 's':
            startup_cmd = optarg;
            break;
        default:
            printf("Usage: %s [-s startup command]\n", argv[0]);
            return 0;
        }
    }
    if (optind < argc) {
        printf("Usage: %s [-s startup command]\n", argv[0]);
        return 0;
    }

    struct tinywl_server server = {0};
 struct wlr_egl *wlr_egl = NULL;

    // Setup EGL surfaceless display
    wlr_egl = setup_surfaceless_egl(&server);
    if (!wlr_egl) {
        fprintf(stderr, "Failed to setup surfaceless EGL display\n");
        cleanup_egl(&server);
        return 1;
    }


    server.wl_display = wl_display_create();
    if (!server.wl_display) {
        wlr_log(WLR_ERROR, "Cannot create Wayland display");
        return 1;
    }



wlr_log(WLR_INFO, "Setting compositor display");
wlr_backend_set_compositor_display(server.wl_display);
    const char *backends_env = getenv("WLR_BACKENDS");
    if (backends_env && strcmp(backends_env, "RDP") == 0) {
        wlr_log(WLR_INFO, "Creating RDP backend");
//        server.backend = wlr_RDP_backend_create(server.wl_display);


 setenv("WLR_BACKENDS", "RDP",1);

server.backend = wlr_RDP_backend_create(server.wl_display, wlr_egl, NULL);
if (!server.backend) {
    wlr_log(WLR_ERROR, "Failed to create RDP backend");
 //   wlr_egl_free(wlr_egl);
    wl_display_destroy(server.wl_display);
    return 1;
}
    } else {
        server.backend = wlr_backend_autocreate(wl_display_get_event_loop(server.wl_display), NULL);
    }
    if (!server.backend) {
        wlr_log(WLR_ERROR, "Failed to create backend");
        server_destroy(&server);
        return 1;
    }

    server.renderer = wlr_renderer_autocreate(server.backend);
    if (server.renderer == NULL) {
        wlr_log(WLR_ERROR, "failed to create wlr_renderer");
        return 1;
    }


// Check if the renderer is Zink
    if (!is_zink_renderer(server.backend, server.renderer, NULL)) {
        wlr_log(WLR_ERROR, "Renderer is not using Zink (Vulkan). Performance may be suboptimal.");
        // Optionally, you can exit here if Zink is required:
        // wlr_egl_free(wlr_egl);
        // cleanup_egl(&server);
        // wlr_renderer_destroy(server.renderer);
        // wlr_backend_destroy(server.backend);
        // wl_display_destroy(server.wl_display);
        // return 1;
    } else {
        wlr_log(WLR_INFO, "Confirmed Zink (Vulkan) renderer in use.");
    }

    wlr_renderer_init_wl_display(server.renderer, server.wl_display);

    server.allocator = wlr_allocator_autocreate(server.backend, server.renderer);
    if (!server.allocator) {
        wlr_log(WLR_ERROR, "Failed to create allocator");
        server_destroy(&server);
        return 1;
    }

    wlr_compositor_create(server.wl_display, 5, server.renderer);
    wlr_subcompositor_create(server.wl_display);
    wlr_data_device_manager_create(server.wl_display);

    server.output_layout = wlr_output_layout_create(server.wl_display);
    wl_list_init(&server.outputs);
    server.new_output.notify = server_new_output;
    wl_signal_add(&server.backend->events.new_output, &server.new_output);

    server.scene = wlr_scene_create();
    server.scene_layout = wlr_scene_attach_output_layout(server.scene, server.output_layout);

    wl_list_init(&server.toplevels);
    server.xdg_shell = wlr_xdg_shell_create(server.wl_display, 3);
    server.new_xdg_toplevel.notify = server_new_xdg_toplevel;
    wl_signal_add(&server.xdg_shell->events.new_toplevel, &server.new_xdg_toplevel);
    server.new_xdg_popup.notify = server_new_xdg_popup;
    wl_signal_add(&server.xdg_shell->events.new_popup, &server.new_xdg_popup);

    server.cursor = wlr_cursor_create();
    wlr_cursor_attach_output_layout(server.cursor, server.output_layout);
    server.cursor_mgr = wlr_xcursor_manager_create(NULL, 24);
    wlr_xcursor_manager_load(server.cursor_mgr, 1);

    server.cursor_mode = TINYWL_CURSOR_PASSTHROUGH;
    server.cursor_motion.notify = server_cursor_motion;
    wl_signal_add(&server.cursor->events.motion, &server.cursor_motion);
    server.cursor_motion_absolute.notify = server_cursor_motion_absolute;
    wl_signal_add(&server.cursor->events.motion_absolute, &server.cursor_motion_absolute);
    server.cursor_button.notify = server_cursor_button;
    wl_signal_add(&server.cursor->events.button, &server.cursor_button);
    server.cursor_axis.notify = server_cursor_axis;
    wl_signal_add(&server.cursor->events.axis, &server.cursor_axis);
    server.cursor_frame.notify = server_cursor_frame;
    wl_signal_add(&server.cursor->events.frame, &server.cursor_frame);

    wl_list_init(&server.keyboards);
    server.new_input.notify = server_new_input;
    wl_signal_add(&server.backend->events.new_input, &server.new_input);
    server.seat = wlr_seat_create(server.wl_display, "seat0");
    server.request_cursor.notify = seat_request_cursor;
    wl_signal_add(&server.seat->events.request_set_cursor, &server.request_cursor);
    server.request_set_selection.notify = seat_request_set_selection;
    wl_signal_add(&server.seat->events.request_set_selection, &server.request_set_selection);

    const char *socket = wl_display_add_socket_auto(server.wl_display);
    if (!socket) {
        wlr_log(WLR_ERROR, "Unable to create wayland socket");
        wlr_seat_destroy(server.seat);
        wlr_xcursor_manager_destroy(server.cursor_mgr);
        wlr_cursor_destroy(server.cursor);
        wlr_output_layout_destroy(server.output_layout);
        wlr_allocator_destroy(server.allocator);
        wlr_renderer_destroy(server.renderer);
        wlr_backend_destroy(server.backend);
        wl_display_destroy(server.wl_display);
        return 1;
    }

    if (!wlr_backend_start(server.backend)) {
        wlr_log(WLR_ERROR, "Failed to start backend");
        server_destroy(&server);
        return 1;
    }

    setenv("WAYLAND_DISPLAY", socket, true);
    if (startup_cmd) {
        if (fork() == 0) {
            execl("/bin/sh", "/bin/sh", "-c", startup_cmd, (void *)NULL);
        }
    }

    wlr_log(WLR_INFO, "Running Wayland compositor on WAYLAND_DISPLAY=%s", socket);
    wl_display_run(server.wl_display);

    wl_display_destroy_clients(server.wl_display);
    server_destroy(&server);
    return 0;
}