#include <wlr/types/wlr_xdg_shell.h>
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
#include <wlr/types/wlr_xdg_shell.h>

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

#include <wlr/backend/RDP.h>
#include <wlr/render/gles2.h>     // For wlr_gles2_renderer_create_surfaceless, wlr_gles2_renderer_create_with_egl
#include <wlr/render/wlr_renderer.h> // For wlr_renderer_begin, wlr_renderer_end etc.
#include <wlr/render/egl.h>    
#include <GLES3/gl31.h> // For OpenGL ES 3.1 constants
#include <wlr/render/gles2.h>

#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/box.h> // For wlr_matrix_project_box, also includes matrix.h
#include <wlr/util/log.h> // For WLR_ERRORARNING
#include <freerdp/freerdp.h> // For freerdp_peer and rdpContext
 // For wlr_rdp_backend
#include <wlr/types/wlr_seat.h> // For wlr_seat_pointer_notify_*
#include <linux/input-event-codes.h> // For BTN_LEFT, BTN_RIGHT, BTN_MIDDLE

#ifndef RENDER_EGL_H
#define RENDER_EGL_H

#include <wlr/render/egl.h>

struct wlr_egl {
    EGLDisplay display;
    EGLContext context;
    EGLDeviceEXT device; // may be EGL_NO_DEVICE_EXT
    struct gbm_device *gbm_device;

    struct {
        // Display extensions
        bool KHR_image_base;
        bool EXT_image_dma_buf_import;
        bool EXT_image_dma_buf_import_modifiers;
        bool IMG_context_priority;
        bool EXT_create_context_robustness;

        // Device extensions
        bool EXT_device_drm;
        bool EXT_device_drm_render_node;

        // Client extensions
        bool EXT_device_query;
        bool KHR_platform_gbm;
        bool EXT_platform_device;
        bool KHR_display_reference;
    } exts;

    struct {
        PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT;
        PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
        PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR;
        PFNEGLQUERYDMABUFFORMATSEXTPROC eglQueryDmaBufFormatsEXT;
        PFNEGLQUERYDMABUFMODIFIERSEXTPROC eglQueryDmaBufModifiersEXT;
        PFNEGLDEBUGMESSAGECONTROLKHRPROC eglDebugMessageControlKHR;
        PFNEGLQUERYDISPLAYATTRIBEXTPROC eglQueryDisplayAttribEXT;
        PFNEGLQUERYDEVICESTRINGEXTPROC eglQueryDeviceStringEXT;
        PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT;
        PFNEGLCREATESYNCKHRPROC eglCreateSyncKHR;
        PFNEGLDESTROYSYNCKHRPROC eglDestroySyncKHR;
        PFNEGLDUPNATIVEFENCEFDANDROIDPROC eglDupNativeFenceFDANDROID;
        PFNEGLWAITSYNCKHRPROC eglWaitSyncKHR;
    } procs;

    bool has_modifiers;
    struct wlr_drm_format_set dmabuf_texture_formats;
    struct wlr_drm_format_set dmabuf_render_formats;
};

struct wlr_egl_context {
    EGLDisplay display;
    EGLContext context;
    EGLSurface draw_surface;
    EGLSurface read_surface;
};

/**
 * Initializes an EGL context for the given DRM FD.
 *
 * Will attempt to load all possibly required API functions.
 */
struct wlr_egl *wlr_egl_create_with_drm_fd(int drm_fd);

/**
 * Frees all related EGL resources, makes the context not-current and
 * unbinds a bound wayland display.
 */
void wlr_egl_destroy(struct wlr_egl *egl);

/**
 * Creates an EGL image from the given dmabuf attributes. Check usability
 * of the dmabuf with wlr_egl_check_import_dmabuf once first.
 */
EGLImageKHR wlr_egl_create_image_from_dmabuf(struct wlr_egl *egl,
    struct wlr_dmabuf_attributes *attributes, bool *external_only);

/**
 * Get DMA-BUF formats suitable for sampling usage.
 */
const struct wlr_drm_format_set *wlr_egl_get_dmabuf_texture_formats(
    struct wlr_egl *egl);
/**
 * Get DMA-BUF formats suitable for rendering usage.
 */
const struct wlr_drm_format_set *wlr_egl_get_dmabuf_render_formats(
    struct wlr_egl *egl);

/**
 * Destroys an EGL image created with the given wlr_egl.
 */
bool wlr_egl_destroy_image(struct wlr_egl *egl, EGLImageKHR image);

int wlr_egl_dup_drm_fd(struct wlr_egl *egl);

/**
 * Restore EGL context that was previously saved using wlr_egl_save_current().
 */
bool wlr_egl_restore_context(struct wlr_egl_context *context);

/**
 * Make the EGL context current.
 *
 * The old EGL context is saved. Callers are expected to clear the current
 * context when they are done by calling wlr_egl_restore_context().
 */
bool wlr_egl_make_current(struct wlr_egl *egl, struct wlr_egl_context *save_context);

bool wlr_egl_unset_current(struct wlr_egl *egl);

EGLSyncKHR wlr_egl_create_sync(struct wlr_egl *egl, int fence_fd);

void wlr_egl_destroy_sync(struct wlr_egl *egl, EGLSyncKHR sync);

int wlr_egl_dup_fence_fd(struct wlr_egl *egl, EGLSyncKHR sync);

bool wlr_egl_wait_sync(struct wlr_egl *egl, EGLSyncKHR sync);

#endif

// Add at the top of your tinywl.c file
struct wlr_renderer *wlr_gles2_renderer_create_surfaceless(void);

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
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_xdg_shell.h> // THIS IS KEY for wlr_xdg_surface_get_geometry
// Add other wlr/types/ as needed: scene, output_layout, cursor, seat, etc.
#include <wlr/types/wlr_scene.h>


#include <wlr/backend.h>
#include <wlr/render/wlr_renderer.h> // THIS IS KEY for struct wlr_render_texture_options
#include <wlr/render/allocator.h>
#include <wlr/render/egl.h>
#include <wlr/render/gles2.h>

#include <drm_fourcc.h>

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

struct wl_listener pointer_motion;
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
 // For XDG clients
 // list of tinywl_output.link
    struct wl_list views;   // <<< THIS IS CRUCIAL: list of tinywl_view.link

  
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
    struct wlr_scene_output *scene_output;
    bool rendering; 
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

struct tinywl_view {
    struct tinywl_server *server;
    struct wlr_xdg_surface *xdg_surface; // Or wlr_layer_surface_v1, etc.
    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener destroy;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    // You also need a commit listener on xdg_surface->surface->events.commit
    // to know when to ask for a frame render.
    struct wl_listener surface_commit;

    bool mapped;
    int x, y; // Position on the output
    struct wl_list link; // For server.views
};

/* Function declarations */
//struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display, struct wlr_egl *egl);
//struct wlr_renderer *wlr_gles2_renderer_create_surfaceless(void);
struct wlr_allocator *wlr_rdp_allocator_create(struct wlr_renderer *renderer);
void rdp_transmit_surface(struct wlr_buffer *buffer);
//freerdp_peer *get_global_rdp_peer(void);
struct wlr_egl *setup_surfaceless_egl(struct tinywl_server *server) ;
void cleanup_egl(struct tinywl_server *server);
static void process_cursor_motion(struct tinywl_server *server, uint32_t time);

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
        wlr_log(WLR_ERROR, "focus_toplevel: No toplevel to focus");
        return;
    }
    struct tinywl_server *server = toplevel->server;
    struct wlr_seat *seat = server->seat;
    struct wlr_surface *surface = toplevel->xdg_toplevel->base->surface;
    struct wlr_surface *prev_surface = seat->keyboard_state.focused_surface;

    if (prev_surface == surface) {
        wlr_log(WLR_ERROR, "Surface %p already focused, skipping", surface);
        return;
    }

    // Deactivate previous toplevel
    if (prev_surface) {
        struct wlr_xdg_toplevel *prev_toplevel =
            wlr_xdg_toplevel_try_from_wlr_surface(prev_surface);
        if (prev_toplevel != NULL) {
            wlr_xdg_toplevel_set_activated(prev_toplevel, false);
            wlr_log(WLR_ERROR, "Deactivated previous toplevel %p", prev_toplevel);
        }
    }

    // Raise and focus new toplevel
    wlr_scene_node_raise_to_top(&toplevel->scene_tree->node);
    wl_list_remove(&toplevel->link);
    wl_list_insert(&server->toplevels, &toplevel->link);
    wlr_xdg_toplevel_set_activated(toplevel->xdg_toplevel, true);
    wlr_log(WLR_ERROR, "Focused toplevel %p, title='%s'", toplevel,
            toplevel->xdg_toplevel->title ? toplevel->xdg_toplevel->title : "(null)");

    // Notify seat of new focus
    struct wlr_keyboard *keyboard = wlr_seat_get_keyboard(seat);
    if (keyboard && surface) {
        wlr_seat_keyboard_notify_enter(seat, surface,
            keyboard->keycodes, keyboard->num_keycodes, &keyboard->modifiers);
        wlr_log(WLR_ERROR, "Notified seat of keyboard focus on surface %p", surface);
        wl_display_flush_clients(server->wl_display);
    } else {
        wlr_log(WLR_ERROR, "No keyboard or surface for focus: keyboard=%p, surface=%p",
                keyboard, surface);
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
    
    // Log key event details with timestamp to track potential duplicates
    wlr_log(WLR_DEBUG, "Key event: keycode=%u, state=%s, time=%u",
            event->keycode, event->state == WL_KEYBOARD_KEY_STATE_PRESSED ? "pressed" : "released",
            event->time_msec);
    
    // Track duplicate events - if we see same keycode and state with similar timestamp, ignore it
    static uint32_t last_keycode = 0;
    static uint32_t last_state = 0;
    static uint32_t last_time = 0;
    
    // Check for duplicate events (same key, same state, within 5ms)
    if (last_keycode == event->keycode && 
        last_state == event->state &&
        event->time_msec > 0 && 
        last_time > 0 &&
        (event->time_msec - last_time < 5 || last_time - event->time_msec < 5)) {
        wlr_log(WLR_ERROR, "Ignoring duplicate key event: keycode=%u, state=%u, time=%u (prev=%u)",
                event->keycode, event->state, event->time_msec, last_time);
        return;
    }
    
    // Store current event details for duplicate detection
    last_keycode = event->keycode;
    last_state = event->state;
    last_time = event->time_msec;

    // Ignore release events at startup with no focused surface and no prior press
    if (event->state == WL_KEYBOARD_KEY_STATE_RELEASED && !seat->keyboard_state.focused_surface) {
        // Note: XKB keycodes are offset by 8 from evdev keycodes
        xkb_keycode_t xkb_keycode = event->keycode + 8;
        struct xkb_state *state = keyboard->wlr_keyboard->xkb_state;
        if (xkb_state_key_get_level(state, xkb_keycode, 0) == 0) {
            wlr_log(WLR_ERROR, "Ignoring phantom release for keycode=%u (no prior press, no focused surface)",
                    event->keycode);
            return;
        }
    }
    
    // Convert keycode to XKB keycode (Wayland keycodes are offset by 8)
    uint32_t keycode = event->keycode + 8;
    const xkb_keysym_t *syms;
    int nsyms = xkb_state_key_get_syms(keyboard->wlr_keyboard->xkb_state, keycode, &syms);
    
    if (nsyms <= 0) {
        wlr_log(WLR_ERROR, "No keysyms found for keycode %u (raw=%u)", keycode, event->keycode);
    } else {
        // Only log detailed keysym info at debug level to reduce noise
        if (wlr_log_get_verbosity() >= WLR_DEBUG) {
            for (int i = 0; i < nsyms; i++) {
                char buf[32];
                xkb_keysym_get_name(syms[i], buf, sizeof(buf));
                wlr_log(WLR_DEBUG, "Keysym %d: %s (0x%x)", i, buf, syms[i]);
            }
        }
    }
    
    // Handle keybindings for Alt+key combinations
    bool handled = false;
    uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard->wlr_keyboard);
    if ((modifiers & WLR_MODIFIER_ALT) && event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
        for (int i = 0; i < nsyms; i++) {
            handled = handle_keybinding(server, syms[i]);
            if (handled) break;
        }
    }
    
    // Forward key event to the focused client if not handled
    if (!handled) {
        // Set the keyboard once at initialization or focus change, not on every key event
        static struct wlr_keyboard *last_keyboard = NULL;
        if (last_keyboard != keyboard->wlr_keyboard) {
            wlr_seat_set_keyboard(seat, keyboard->wlr_keyboard);
            last_keyboard = keyboard->wlr_keyboard;
        }
        
        // Only notify if there's a focused surface
        if (seat->keyboard_state.focused_surface) {
            wlr_seat_keyboard_notify_key(seat, event->time_msec, event->keycode, event->state);
            wlr_log(WLR_DEBUG, "Key sent to focused surface %p", seat->keyboard_state.focused_surface);
        } else {
            wlr_log(WLR_DEBUG, "No focused surface for key event");
        }
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
    if (!wlr_keyboard) {
        wlr_log(WLR_ERROR, "Failed to get wlr_keyboard from input device %p", device);
        return;
    }

    struct tinywl_keyboard *keyboard = calloc(1, sizeof(*keyboard));
    if (!keyboard) {
        wlr_log(WLR_ERROR, "Failed to allocate tinywl_keyboard");
        return;
    }
    keyboard->server = server;
    keyboard->wlr_keyboard = wlr_keyboard;

    // Initialize XKB context and keymap
    struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    if (!context) {
        wlr_log(WLR_ERROR, "Failed to create XKB context");
        free(keyboard);
        return;
    }

    // Use a more specific keymap to align with RDP client expectations
    struct xkb_rule_names rules = {0};
    rules.rules = "evdev";
    rules.model = "pc105";
    rules.layout = "us";
    rules.variant = "";
    rules.options = "";
    struct xkb_keymap *keymap = xkb_keymap_new_from_names(context, &rules, XKB_KEYMAP_COMPILE_NO_FLAGS);
    if (!keymap) {
        wlr_log(WLR_ERROR, "Failed to create XKB keymap");
        xkb_context_unref(context);
        free(keyboard);
        return;
    }

    if (!wlr_keyboard_set_keymap(wlr_keyboard, keymap)) {
        wlr_log(WLR_ERROR, "Failed to set keymap on wlr_keyboard");
        xkb_keymap_unref(keymap);
        xkb_context_unref(context);
        free(keyboard);
        return;
    }
    xkb_keymap_unref(keymap);
    xkb_context_unref(context);

    // Set repeat info
    wlr_keyboard_set_repeat_info(wlr_keyboard, 25, 600);
    wlr_log(WLR_ERROR, "Keyboard %p initialized with keymap and repeat info", wlr_keyboard);

    // Add event listeners
    keyboard->modifiers.notify = keyboard_handle_modifiers;
    wl_signal_add(&wlr_keyboard->events.modifiers, &keyboard->modifiers);
    keyboard->key.notify = keyboard_handle_key;
    wl_signal_add(&wlr_keyboard->events.key, &keyboard->key);
    keyboard->destroy.notify = keyboard_handle_destroy;
    wl_signal_add(&device->events.destroy, &keyboard->destroy);

    // Set keyboard on seat
    wlr_seat_set_keyboard(server->seat, wlr_keyboard);
    wlr_log(WLR_ERROR, "Keyboard %p set on seat %s", wlr_keyboard, server->seat->name);

    wl_list_insert(&server->keyboards, &keyboard->link);
    wlr_log(WLR_ERROR, "Keyboard %p added to server keyboards list", wlr_keyboard);

    // Update seat capabilities
    wlr_seat_set_capabilities(server->seat, WL_SEAT_CAPABILITY_KEYBOARD);
    wlr_log(WLR_ERROR, "Set seat capabilities: WL_SEAT_CAPABILITY_KEYBOARD");
}

static void server_pointer_motion(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, pointer_motion);
    struct wlr_pointer_motion_event *event = data;

    if (!server->cursor || !server->output_layout) {
        wlr_log(WLR_ERROR, "Cursor or output layout is null during motion event");
        return;
    }

    wlr_cursor_move(server->cursor, &event->pointer->base, event->delta_x, event->delta_y);
    wlr_log(WLR_DEBUG, "Pointer motion: cursor moved to (%f, %f)", server->cursor->x, server->cursor->y);
    process_cursor_motion(server, event->time_msec);
}

static void server_new_pointer(struct tinywl_server *server, struct wlr_input_device *device) {
    if (device->type != WLR_INPUT_DEVICE_POINTER) {
        wlr_log(WLR_ERROR, "Device %p is not a pointer", device);
        return;
    }
    wlr_cursor_attach_input_device(server->cursor, device);
    struct wlr_pointer *pointer = wlr_pointer_from_input_device(device);
    if (pointer) {
        static struct wl_listener pointer_motion_listener;
        pointer_motion_listener.notify = server_pointer_motion;
        wl_signal_add(&pointer->events.motion, &pointer_motion_listener);
        wlr_log(WLR_DEBUG, "Attached motion listener for pointer %p", pointer);
        // Set default cursor image
        wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
        wlr_log(WLR_INFO, "Set default cursor image for device %p", device);
    } else {
        wlr_log(WLR_ERROR, "Failed to get wlr_pointer from device %p", device);
    }
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
    // Preserve existing cursor mode handling
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
    
    // Preserve cursor icon setting
    if (!toplevel) {
        wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
    } else {
        wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
    }
    
    // GTK fix: Handle surface focus changes more explicitly
    if (surface) {
        // Check if this is a focus change
        bool is_new_focus = seat->pointer_state.focused_surface != surface;
        
        // For GTK applications, we need to be more explicit with focus changes
        if (is_new_focus && seat->pointer_state.focused_surface != NULL) {
            // Clear existing focus first
            wlr_seat_pointer_clear_focus(seat);
            // Add frame notification to help GTK register the focus change
            wlr_seat_pointer_notify_frame(seat);
        }
        
        // Set focus and send motion
        wlr_seat_pointer_notify_enter(seat, surface, sx, sy);
        wlr_seat_pointer_notify_motion(seat, time, sx, sy);
        
        // Important for GTK: Always send frame after pointer events
        wlr_seat_pointer_notify_frame(seat);
        
        // Flush events to ensure they're processed in order
        wl_display_flush_clients(server->wl_display);
    } else {
        // No surface under cursor
        wlr_seat_pointer_clear_focus(seat);
        // Add frame notification for consistency
        wlr_seat_pointer_notify_frame(seat);
    }
    
    // Schedule frame to ensure cursor is rendered (preserve existing code)
    struct tinywl_output *output;
    wl_list_for_each(output, &server->outputs, link) {
        if (output->wlr_output && output->wlr_output->enabled) {
            wlr_output_schedule_frame(output->wlr_output);
        }
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
/*
static void server_cursor_button(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_button);
    struct wlr_pointer_button_event *event = data;

    // Log the event for debugging
    wlr_log(WLR_DEBUG, "Cursor button event: button=%u, state=%s, time=%u",
            event->button, event->state == WL_POINTER_BUTTON_STATE_PRESSED ? "pressed" : "released",
            event->time_msec);

    // Find the surface under the cursor
    double sx, sy;
    struct wlr_surface *surface = NULL;
    struct tinywl_toplevel *toplevel = desktop_toplevel_at(server,
            server->cursor->x, server->cursor->y, &surface, &sx, &sy);

    // Notify the seat of the button event
    wlr_seat_pointer_notify_button(server->seat, event->time_msec, event->button, event->state);

    if (event->state == WL_POINTER_BUTTON_STATE_PRESSED) {
        // On press, focus the toplevel and notify the surface
        focus_toplevel(toplevel);
        if (surface) {
            wlr_seat_pointer_notify_enter(server->seat, surface, sx, sy);
            wlr_seat_pointer_notify_motion(server->seat, event->time_msec, sx, sy);
            wlr_log(WLR_DEBUG, "Notified surface %p at (%f, %f) of button press", surface, sx, sy);
        }
    } else if (event->state == WL_POINTER_BUTTON_STATE_RELEASED) {
        // On release, reset cursor mode and schedule a frame
        reset_cursor_mode(server);
        // Schedule a frame to update the cursor visually
        struct tinywl_output *output;
        wl_list_for_each(output, &server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
            }
        }
        wlr_log(WLR_DEBUG, "Processed button release, cursor mode reset");
    }

    // Notify the seat of the frame to finalize the event
    wlr_seat_pointer_notify_frame(server->seat);
}*/

// Enhanced button handling with special GTK fixes
static void server_cursor_button(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_button);
    struct wlr_pointer_button_event *event = data;
    
    wlr_log(WLR_DEBUG, "Button %d %s", event->button,
            event->state == WL_POINTER_BUTTON_STATE_PRESSED ? "pressed" : "released");
    
    double sx, sy;
    struct wlr_surface *surface = NULL;
    struct tinywl_toplevel *toplevel = desktop_toplevel_at(server,
            server->cursor->x, server->cursor->y, &surface, &sx, &sy);
    
    struct wlr_seat *seat = server->seat;
    
    if (event->state == WL_POINTER_BUTTON_STATE_PRESSED) {
        if (surface) {
            // Only clear focus if the surface is different from the current focus
            if (seat->pointer_state.focused_surface != surface) {
                wlr_seat_pointer_clear_focus(seat);
                wlr_seat_pointer_notify_frame(seat);
                wlr_seat_pointer_notify_enter(seat, surface, sx, sy);
                wlr_seat_pointer_notify_motion(seat, event->time_msec, sx, sy);
                wlr_seat_pointer_notify_frame(seat);
                wl_display_flush_clients(server->wl_display);
            } else {
                // Surface already focused; just send motion
                wlr_seat_pointer_notify_motion(seat, event->time_msec, sx, sy);
                wlr_seat_pointer_notify_frame(seat);
            }
            
            // Send the button event
            wlr_seat_pointer_notify_button(seat, event->time_msec, event->button, event->state);
            wlr_seat_pointer_notify_frame(seat);
            wl_display_flush_clients(server->wl_display);
            
            // Focus the toplevel after sending button events
            if (toplevel) {
                focus_toplevel(toplevel);
            }
        } else {
            // No surface under cursor; send button event and clear focus
            if (seat->pointer_state.focused_surface) {
                wlr_seat_pointer_clear_focus(seat);
                wlr_seat_pointer_notify_frame(seat);
            }
            wlr_seat_pointer_notify_button(seat, event->time_msec, event->button, event->state);
            wlr_seat_pointer_notify_frame(seat);
            wl_display_flush_clients(server->wl_display);
        }
    } else {
        // Button release
        wlr_seat_pointer_notify_button(seat, event->time_msec, event->button, event->state);
        wlr_seat_pointer_notify_frame(seat);
        reset_cursor_mode(server);
        wl_display_flush_clients(server->wl_display);
    }
    
    // Schedule frame for all outputs
    struct tinywl_output *output;
    wl_list_for_each(output, &server->outputs, link) {
        if (output->wlr_output && output->wlr_output->enabled) {
            wlr_output_schedule_frame(output->wlr_output);
        }
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





// In tinywl.c (or your main server file)

// Ensure you have these includes (or equivalent for your project structure)
#include <time.h>
#include <wlr/types/wlr_compositor.h> // For wlr_surface_send_frame_done
#include <wlr/types/wlr_output_damage.h> // For wlr_output_damage_add_whole (optional but good)

// You'll need to define or include your tinywl_server and tinywl_view structs
// For example:
// struct tinywl_server {
//     // ... other members ...
//     struct wl_list views; // list of struct tinywl_view
//     // ...
// };
//
// struct tinywl_view {
//     struct tinywl_server *server;
//     struct wlr_xdg_surface *xdg_surface;
//     struct wl_listener map;
//     struct wl_listener unmap;
//     struct wl_listener destroy;
//     struct wl_listener request_move;
//     struct wl_listener request_resize;
//     struct wl_listener commit;
//     bool mapped;
//     int x, y;
//     struct wl_list link;
// };

// Assume 'server' is a global or accessible pointer to your tinywl_server instance
// extern struct tinywl_server server; // If global, or pass it in somehow

// In tinywl.c

// Ensure these are included at the top of your tinywl.c:
// #include <time.h>
// #include <wlr/types/wlr_compositor.h>
// #include <wlr/types/wlr_xdg_shell.h>
// #include <wlr/util/box.h>
// #include <wlr/util/log.h>
// #include <wlr/render/wlr_renderer.h> // For wlr_renderer_is_gles2
// #include <wlr/render/egl.h>         // For wlr_gles2_renderer_get_egl, wlr_egl_make_current etc.
// In tinywl.c

#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/box.h>
#include <wlr/util/transform.h>
#include <wlr/util/log.h>
#include <wlr/render/wlr_renderer.h>

static void output_frame(struct wl_listener *listener, void *data) {
    struct tinywl_output *output = wl_container_of(listener, output, frame);
    struct wlr_output *wlr_output = output->wlr_output;
    struct tinywl_server *server = output->server;
    struct wlr_scene *scene = server->scene;

    struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(scene, wlr_output);
    if (!scene_output) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] No scene output for %s", wlr_output->name);
        return;
    }

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    if (!wlr_output || !wlr_output->enabled) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Output '%s' not valid or not enabled.", wlr_output->name);
        return;
    }
    if (!wlr_output->renderer || !wlr_output->allocator) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Output '%s' has no renderer or allocator.", wlr_output->name);
        return;
    }
    if (!wlr_renderer_is_gles2(wlr_output->renderer)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Renderer is not GLES2");
        return;
    }

    // Clear any existing OpenGL errors
    GLenum err_clear_loop;
    int clear_loop_count = 0;
    while ((err_clear_loop = glGetError()) != GL_NO_ERROR && clear_loop_count < 10) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Clearing OpenGL error: 0x%x", err_clear_loop);
        clear_loop_count++;
    }

    // Begin rendering
    struct wlr_egl_context prev_ctx = {0};
    struct wlr_egl *egl = wlr_gles2_renderer_get_egl(wlr_output->renderer);
    if (!egl || !wlr_egl_make_current(egl, &prev_ctx)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Failed to make EGL context current. EGL error: 0x%x", eglGetError());
        wlr_egl_restore_context(&prev_ctx);
        return;
    }

    struct wlr_output_state state;
    wlr_output_state_init(&state);

    struct wlr_buffer_pass_options pass_options = {0};
    struct wlr_render_pass *pass = wlr_output_begin_render_pass(wlr_output, &state, &pass_options);
    if (!pass) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] wlr_output_begin_render_pass failed for '%s'", wlr_output->name);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }

    GLenum fbo_status_check = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fbo_status_check != GL_FRAMEBUFFER_COMPLETE) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] FBO incomplete after begin_render_pass: status=0x%x", fbo_status_check);
        wlr_render_pass_submit(pass);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }

    struct wlr_buffer *rendered_buffer = state.buffer;
    if (!rendered_buffer) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] No buffer available for rendering in wlr_output_state");
        wlr_render_pass_submit(pass);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }

    // Render background
    float background_color[] = {0.1f, 0.1f, 0.15f, 1.0f};
    struct wlr_render_rect_options background_opts = {
        .box = {0, 0, wlr_output->width, wlr_output->height},
        .color = {background_color[0], background_color[1], background_color[2], background_color[3]},
        .blend_mode = WLR_RENDER_BLEND_MODE_NONE,
    };
    wlr_render_pass_add_rect(pass, &background_opts);
    wlr_log(WLR_INFO, "[RENDER_FRAME] Rendered background for output %s (%dx%d)", 
            wlr_output->name, wlr_output->width, wlr_output->height);

    // Render all toplevel views in reverse order (newest on top)
    struct tinywl_toplevel *toplevel_view;
    int window_count = 0;
    bool content_rendered = false;
    wl_list_for_each_reverse(toplevel_view, &server->toplevels, link) {
        if (!toplevel_view->xdg_toplevel || !toplevel_view->xdg_toplevel->base ||
            !toplevel_view->xdg_toplevel->base->surface->mapped) {
            continue;
        }

        struct wlr_xdg_surface *xdg_surface = toplevel_view->xdg_toplevel->base;
        struct wlr_surface *surface = xdg_surface->surface;

        if (!wlr_surface_has_buffer(surface)) {
            continue;
        }

        struct wlr_texture *texture = wlr_surface_get_texture(surface);
        if (!texture) {
            wlr_log(WLR_ERROR, "[RENDER_FRAME] Failed to get texture for surface");
            continue;
        }

        window_count++;
        content_rendered = true;

        double view_x = toplevel_view->scene_tree->node.x;
        double view_y = toplevel_view->scene_tree->node.y;
        int width = surface->current.width;
        int height = surface->current.height;

        // Flip texture vertically to match RDP coordinate system
        struct wlr_fbox src_box = { .x = 0, .y = 0, .width = width, .height = height };
        struct wlr_box dst_box = { 
            .x = (int)view_x, 
            .y = wlr_output->height - (int)view_y - height,
            .width = width, 
            .height = height 
        };

        wlr_log(WLR_INFO, "[RENDER_FRAME] Window %d: pos=(%d,%d) size=(%dx%d)", 
                window_count, dst_box.x, dst_box.y, dst_box.width, dst_box.height);

        struct wlr_render_texture_options tex_opts = {
            .texture = texture,
            .src_box = src_box,
            .dst_box = dst_box,
            .blend_mode = WLR_RENDER_BLEND_MODE_PREMULTIPLIED,
            .transform = WL_OUTPUT_TRANSFORM_FLIPPED_180,
        };
        wlr_render_pass_add_texture(pass, &tex_opts);
    }

    // Render the cursor
    if (server->cursor && server->cursor_mgr) {
        struct wlr_xcursor *xcursor = wlr_xcursor_manager_get_xcursor(server->cursor_mgr, "default", 1.0);
        if (xcursor && xcursor->images[0]) {
            struct wlr_xcursor_image *image = xcursor->images[0];
            struct wlr_texture *cursor_texture = wlr_texture_from_pixels(
                server->renderer, DRM_FORMAT_ARGB8888, image->width * 4,
                image->width, image->height, image->buffer
            );
            if (cursor_texture) {
                // Adjust cursor position for RDP coordinate system (origin at top-left)
                int cursor_x = (int)server->cursor->x;
                int cursor_y = wlr_output->height - (int)server->cursor->y - (int)image->height;

                struct wlr_fbox cursor_src_box = {
                    .x = 0, .y = 0,
                    .width = image->width, .height = image->height
                };
                struct wlr_box cursor_dst_box = {
                    .x = cursor_x - image->hotspot_x,
                    .y = cursor_y - image->hotspot_y,
                    .width = (int)image->width,
                    .height = (int)image->height
                };

                struct wlr_render_texture_options cursor_tex_opts = {
                    .texture = cursor_texture,
                    .src_box = cursor_src_box,
                    .dst_box = cursor_dst_box,
                    .blend_mode = WLR_RENDER_BLEND_MODE_PREMULTIPLIED,
                };
                wlr_render_pass_add_texture(pass, &cursor_tex_opts);
                wlr_log(WLR_INFO, "[RENDER_FRAME] Rendered cursor at (%d,%d) size=(%dx%d)",
                        cursor_dst_box.x, cursor_dst_box.y, cursor_dst_box.width, cursor_dst_box.height);
                wlr_texture_destroy(cursor_texture);
            } else {
                wlr_log(WLR_ERROR, "[RENDER_FRAME] Failed to create cursor texture");
            }
        } else {
            wlr_log(WLR_ERROR, "[RENDER_FRAME] No cursor image available");
        }
    } else {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Cursor or cursor manager not initialized");
    }

    wlr_log(WLR_INFO, "[RENDER_FRAME] Rendered %d windows", window_count);

    if (!wlr_render_pass_submit(pass)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] wlr_render_pass_submit failed for '%s'", wlr_output->name);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }

    // Transmit buffer if content was rendered
    if (content_rendered || server->cursor) {
        void *bdata;
        uint32_t bfmt;
        size_t bstride;
        if (wlr_buffer_begin_data_ptr_access(rendered_buffer, WLR_BUFFER_DATA_PTR_ACCESS_READ | WLR_BUFFER_DATA_PTR_ACCESS_WRITE, &bdata, &bfmt, &bstride)) {
            if (bdata && rendered_buffer->width > 5 && rendered_buffer->height > 5) {
                unsigned char *pixels = (unsigned char *)bdata;
                size_t offset = 5 * bstride + 5 * 4;

                // Log pixel data for debugging
                if (offset + 3 < bstride * rendered_buffer->height) {
                    wlr_log(WLR_INFO, "[RENDER_FRAME] Raw pixel data at (5,5): %02X %02X %02X %02X (Format: 0x%X, Stride: %zu)",
                            pixels[offset + 0], pixels[offset + 1], pixels[offset + 2], pixels[offset + 3], bfmt, bstride);
                }

                // Convert buffer to BGRA8888 if needed
                if (bfmt != DRM_FORMAT_BGRA8888) {
                    wlr_log(WLR_INFO, "[RENDER_FRAME] Converting buffer from format 0x%X to BGRA8888", bfmt);
                    for (int y = 0; y < rendered_buffer->height; y++) {
                        for (int x = 0; x < rendered_buffer->width; x++) {
                            size_t src_offset = y * bstride + x * 4;
                            unsigned char r, g, b, a;

                            if (bfmt == DRM_FORMAT_XBGR8888 || bfmt == DRM_FORMAT_ABGR8888) {
                                b = pixels[src_offset + 0];
                                g = pixels[src_offset + 1];
                                r = pixels[src_offset + 2];
                                a = (bfmt == DRM_FORMAT_ABGR8888) ? pixels[src_offset + 3] : 0xFF;
                            } else if (bfmt == DRM_FORMAT_XRGB8888) {
                                r = pixels[src_offset + 0];
                                g = pixels[src_offset + 1];
                                b = pixels[src_offset + 2];
                                a = 0xFF;
                            } else if (bfmt == DRM_FORMAT_ARGB8888) {
                                a = pixels[src_offset + 0];
                                r = pixels[src_offset + 1];
                                g = pixels[src_offset + 2];
                                b = pixels[src_offset + 3];
                            } else {
                                r = pixels[src_offset + 0];
                                g = pixels[src_offset + 1];
                                b = pixels[src_offset + 2];
                                a = (bfmt == DRM_FORMAT_BGRA8888) ? pixels[src_offset + 3] : 0xFF;
                                wlr_log(WLR_ERROR, "[RENDER_FRAME] Unknown format 0x%X, assuming RGBA order", bfmt);
                            }

                            pixels[src_offset + 0] = b;
                            pixels[src_offset + 1] = g;
                            pixels[src_offset + 2] = r;
                            pixels[src_offset + 3] = a;
                        }
                    }
                    bfmt = DRM_FORMAT_BGRA8888;
                    bstride = rendered_buffer->width * 4;
                }

                // Verify pixel at cursor position
                if (server->cursor) {
                    int cursor_pixel_x = (int)server->cursor->x;
                    int cursor_pixel_y = wlr_output->height - (int)server->cursor->y;
                    size_t cursor_offset = cursor_pixel_y * bstride + cursor_pixel_x * 4;
                    if (cursor_offset + 3 < bstride * rendered_buffer->height) {
                        wlr_log(WLR_INFO, "[RENDER_FRAME] Cursor pixel at (%d,%d): %02X %02X %02X %02X",
                                cursor_pixel_x, cursor_pixel_y,
                                pixels[cursor_offset + 0], pixels[cursor_offset + 1],
                                pixels[cursor_offset + 2], pixels[cursor_offset + 3]);
                    }
                }

                wlr_buffer_end_data_ptr_access(rendered_buffer);
                wlr_log(WLR_INFO, "[RENDER_FRAME] Transmitting buffer with format 0x%X", bfmt);
                rdp_transmit_surface(rendered_buffer);
            } else {
                wlr_log(WLR_ERROR, "[RENDER_FRAME] Invalid buffer dimensions for readback");
                wlr_buffer_end_data_ptr_access(rendered_buffer);
            }
        } else {
            wlr_log(WLR_ERROR, "[RENDER_FRAME] Failed RDP buffer pixel readback");
            rdp_transmit_surface(rendered_buffer);
        }
    } else {
        wlr_log(WLR_DEBUG, "[RENDER_FRAME] Skipping RDP transmission: no content or cursor");
    }

    wlr_egl_restore_context(&prev_ctx);
    wlr_output_state_finish(&state);
    wlr_scene_output_send_frame_done(scene_output, &now);
}
/*
static void output_frame(struct wl_listener *listener, void *data) {
    struct tinywl_output *output_wrapper = wl_container_of(listener, output_wrapper, frame);
    struct wlr_output *wlr_output = output_wrapper->wlr_output;
    struct tinywl_server *server = output_wrapper->server;

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    if (!wlr_output || !wlr_output->enabled) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Output '%s' not valid or not enabled.", wlr_output ? wlr_output->name : "null");
        return;
    }
    if (!wlr_output->renderer || !wlr_output->allocator) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Output '%s' has no renderer or allocator.", wlr_output->name);
        return;
    }
    if (!wlr_renderer_is_gles2(wlr_output->renderer)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Renderer is not GLES2");
        return;
    }

    // Clear any existing OpenGL errors
    GLenum err_clear_loop;
    int clear_loop_count = 0;
    while ((err_clear_loop = glGetError()) != GL_NO_ERROR && clear_loop_count < 10) {
        clear_loop_count++;
    }

    // Begin rendering
    struct wlr_egl_context prev_ctx = {0};
    struct wlr_egl *egl = wlr_gles2_renderer_get_egl(wlr_output->renderer);
    if (!egl || !wlr_egl_make_current(egl, &prev_ctx)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Failed to make EGL context current. EGL error: 0x%x", eglGetError());
        wlr_egl_restore_context(&prev_ctx);
        return;
    }

    struct wlr_output_state state;
    wlr_output_state_init(&state);

    struct wlr_buffer_pass_options pass_options = {0};
    struct wlr_render_pass *pass = wlr_output_begin_render_pass(wlr_output, &state, &pass_options);
    if (!pass) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] wlr_output_begin_render_pass failed for '%s'", wlr_output->name);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }

    GLenum fbo_status_check = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fbo_status_check != GL_FRAMEBUFFER_COMPLETE) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] FBO incomplete after begin_render_pass: status=0x%x", fbo_status_check);
        wlr_render_pass_submit(pass);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }

    struct wlr_buffer *rendered_buffer = state.buffer;
    if (!rendered_buffer) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] No buffer available for rendering in wlr_output_state");
        wlr_render_pass_submit(pass);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }

    // Render background
    float background_color[] = {0.1f, 0.1f, 0.15f, 1.0f};
    struct wlr_render_rect_options background_opts = {
        .box = {0, 0, wlr_output->width, wlr_output->height},
        .color = {background_color[0], background_color[1], background_color[2], background_color[3]},
        .blend_mode = WLR_RENDER_BLEND_MODE_NONE,
    };
    wlr_render_pass_add_rect(pass, &background_opts);
    wlr_log(WLR_INFO, "[RENDER_FRAME] Rendered background for output %s (%dx%d)", 
            wlr_output->name, wlr_output->width, wlr_output->height);

    // Render all toplevel views
    struct tinywl_toplevel *toplevel_view;
    int window_count = 0;
    bool content_rendered = false;
    wl_list_for_each(toplevel_view, &server->toplevels, link) {
        if (!toplevel_view->xdg_toplevel || !toplevel_view->xdg_toplevel->base ||
            !toplevel_view->xdg_toplevel->base->surface->mapped) {
            continue;
        }

        struct wlr_xdg_surface *xdg_surface = toplevel_view->xdg_toplevel->base;
        struct wlr_surface *surface = xdg_surface->surface;

        if (!wlr_surface_has_buffer(surface)) {
            continue;
        }

        struct wlr_texture *texture = wlr_surface_get_texture(surface);
        if (!texture) {
            wlr_log(WLR_ERROR, "[RENDER_FRAME] Failed to get texture for surface");
            continue;
        }

        window_count++;
        content_rendered = true;

        double view_x = toplevel_view->scene_tree->node.x;
        double view_y = toplevel_view->scene_tree->node.y;
        int width = surface->current.width;
        int height = surface->current.height;

        struct wlr_fbox src_box = { .x = 0, .y = 0, .width = width, .height = height };
        struct wlr_box dst_box = { .x = (int)view_x, .y = (int)view_y, .width = width, .height = height };

        wlr_log(WLR_INFO, "[RENDER_FRAME] Window %d: pos=(%d,%d) size=(%dx%d)", 
                window_count, dst_box.x, dst_box.y, dst_box.width, dst_box.height);

        struct wlr_render_texture_options tex_opts = {
            .texture = texture,
            .src_box = src_box,
            .dst_box = dst_box,
            .blend_mode = WLR_RENDER_BLEND_MODE_PREMULTIPLIED,
        };
        wlr_render_pass_add_texture(pass, &tex_opts);
    }

    wlr_log(WLR_INFO, "[RENDER_FRAME] Rendered %d windows", window_count);

    if (!wlr_render_pass_submit(pass)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] wlr_render_pass_submit failed for '%s'", wlr_output->name);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }

    // Only transmit if content was rendered
    if (content_rendered) {
        void *bdata;
        uint32_t bfmt;
        size_t bstride;
        if (wlr_buffer_begin_data_ptr_access(rendered_buffer, WLR_BUFFER_DATA_PTR_ACCESS_READ, &bdata, &bfmt, &bstride)) {
            if (bdata && rendered_buffer->width > 5 && rendered_buffer->height > 5) {
                unsigned char *pixels_final = (unsigned char *)bdata;
                size_t offset = 5 * bstride + 5 * 4;
                if (bfmt != DRM_FORMAT_XRGB8888 && bfmt != DRM_FORMAT_ARGB8888 && bfmt != 0x34325258) {
                    wlr_log(WLR_INFO, "[RENDER_FRAME] Readback format 0x%X might not be 4bpp as assumed for offset calculation.", bfmt);
                }
                if (offset + 3 < bstride * rendered_buffer->height) {
                    unsigned char r_val = pixels_final[offset + 0];
                    unsigned char g_val = pixels_final[offset + 1];
                    unsigned char b_val = pixels_final[offset + 2];
                    unsigned char a_val = pixels_final[offset + 3];
                    printf("[RENDER_FRAME] RDP Buffer Readback Pixel (5,5) RGBA-order: %02X %02X %02X %02X (Format: 0x%X, Stride: %zu)\n",
                           r_val, g_val, b_val, a_val, bfmt, bstride);
                    if (r_val == (unsigned char)(background_color[0]*255.0f) &&
                        g_val == (unsigned char)(background_color[1]*255.0f) &&
                        b_val == (unsigned char)(background_color[2]*255.0f) ) {
                        printf("[RENDER_FRAME] Pixel (5,5) IS BACKGROUND COLOR!\n");
                    } else {
                        printf("[RENDER_FRAME] Pixel (5,5) IS NOT PLAIN BACKGROUND (might be client window or other content).\n");
                    }
                } else {
                    wlr_log(WLR_ERROR, "[RENDER_FRAME] Pixel readback offset out of bounds.");
                }
            }
            wlr_buffer_end_data_ptr_access(rendered_buffer);
        } else {
            wlr_log(WLR_ERROR, "[RENDER_FRAME] Failed RDP buffer pixel readback: wlr_buffer_begin_data_ptr_access failed.");
        }

        rdp_transmit_surface(rendered_buffer);
    } else {
        wlr_log(WLR_DEBUG, "[RENDER_FRAME] Skipping RDP transmission: no windows rendered");
    }

    if (!wlr_output_commit_state(wlr_output, &state)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] wlr_output_commit_state failed for '%s'", wlr_output->name);
    } else {
        wl_list_for_each(toplevel_view, &server->toplevels, link) {
            if (!toplevel_view->xdg_toplevel || !toplevel_view->xdg_toplevel->base ||
                !toplevel_view->xdg_toplevel->base->surface->mapped) {
                continue;
            }
            struct wlr_surface *surface = toplevel_view->xdg_toplevel->base->surface;
            if (wlr_surface_has_buffer(surface)) {
                wlr_surface_send_frame_done(surface, &now);
            }
        }
    }

    wlr_egl_restore_context(&prev_ctx);
    wlr_output_state_finish(&state);
}*/
/*
static void output_frame(struct wl_listener *listener, void *data) {
    struct tinywl_output *output_wrapper = wl_container_of(listener, output_wrapper, frame);
    struct wlr_output *wlr_output = output_wrapper->wlr_output;
    struct tinywl_server *server = output_wrapper->server;

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    if (!wlr_output || !wlr_output->enabled) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Output '%s' not valid or not enabled.", wlr_output ? wlr_output->name : "null");
        return;
    }
    if (!wlr_output->renderer || !wlr_output->allocator) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Output '%s' has no renderer or allocator.", wlr_output->name);
        return;
    }
    if (!wlr_renderer_is_gles2(wlr_output->renderer)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Renderer is not GLES2");
        return;
    }

    // Clear any existing OpenGL errors
    GLenum err_clear_loop;
    int clear_loop_count = 0;
    while ((err_clear_loop = glGetError()) != GL_NO_ERROR && clear_loop_count < 10) {
        clear_loop_count++;
    }

    // Begin rendering
    struct wlr_egl_context prev_ctx = {0};
    struct wlr_egl *egl = wlr_gles2_renderer_get_egl(wlr_output->renderer);
    if (!egl || !wlr_egl_make_current(egl, &prev_ctx)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Failed to make EGL context current. EGL error: 0x%x", eglGetError());
        wlr_egl_restore_context(&prev_ctx);
        return;
    }

    // Initialize output state
    struct wlr_output_state state;
    wlr_output_state_init(&state);

    // Begin the render pass
    struct wlr_buffer_pass_options pass_options = {0};
    struct wlr_render_pass *pass = wlr_output_begin_render_pass(wlr_output, &state, &pass_options);
    if (!pass) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] wlr_output_begin_render_pass failed for '%s'", wlr_output->name);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            wlr_log(WLR_ERROR, "[RENDER_FRAME] OpenGL error: 0x%x", err);
        }
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }

    // Check framebuffer status
    GLenum fbo_status_check = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fbo_status_check != GL_FRAMEBUFFER_COMPLETE) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] FBO incomplete after begin_render_pass: status=0x%x", fbo_status_check);
        wlr_render_pass_submit(pass);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }

    // Get the buffer we're rendering to
    struct wlr_buffer *rendered_buffer = state.buffer;
    if (!rendered_buffer) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] No buffer available for rendering in wlr_output_state");
        wlr_render_pass_submit(pass);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }

    // --- BEGIN RENDERING LOGIC ---
    
    // Render background
    float background_color[] = {0.1f, 0.1f, 0.15f, 1.0f};
    struct wlr_render_rect_options background_opts = {
        .box = {0, 0, wlr_output->width, wlr_output->height},
        .color = {background_color[0], background_color[1], background_color[2], background_color[3]},
        .blend_mode = WLR_RENDER_BLEND_MODE_NONE,
    };
    wlr_render_pass_add_rect(pass, &background_opts);
    
    // Debug
    wlr_log(WLR_INFO, "[RENDER_FRAME] Rendered background for output %s (%dx%d)", 
            wlr_output->name, wlr_output->width, wlr_output->height);
    
    // Render all toplevel views
    struct tinywl_toplevel *toplevel_view;
    int window_count = 0;
    wl_list_for_each(toplevel_view, &server->toplevels, link) {
        // Skip if no toplevel or not mapped
        if (!toplevel_view->xdg_toplevel || !toplevel_view->xdg_toplevel->base ||
            !toplevel_view->xdg_toplevel->base->surface->mapped) {
            continue;
        }

        struct wlr_xdg_surface *xdg_surface = toplevel_view->xdg_toplevel->base;
        struct wlr_surface *surface = xdg_surface->surface;

        // Skip if no buffer
        if (!wlr_surface_has_buffer(surface)) {
            continue;
        }



        // Get the texture
        struct wlr_texture *texture = wlr_surface_get_texture(surface);
        if (!texture) {
            wlr_log(WLR_ERROR, "[RENDER_FRAME] Failed to get texture for surface");
            continue;
        }

        window_count++;

        // Get the view's position
        double view_x = toplevel_view->scene_tree->node.x;
        double view_y = toplevel_view->scene_tree->node.y;
        
        // Get the surface dimensions
        int width = surface->current.width;
        int height = surface->current.height;
        
        // Set up source box (entire texture)
        struct wlr_fbox src_box = {
            .x = 0, .y = 0,
            .width = width, .height = height
        };
        
        // Set up destination box (where to render on screen)
        struct wlr_box dst_box = {
            .x = (int)view_x,
            .y = (int)view_y,
            .width = width,
            .height = height
        };
        
        // Log window position and size
        wlr_log(WLR_INFO, "[RENDER_FRAME] Window %d: pos=(%d,%d) size=(%dx%d)", 
                window_count, dst_box.x, dst_box.y, dst_box.width, dst_box.height);
        
        // Create texture rendering options
        struct wlr_render_texture_options tex_opts = {
            .texture = texture,
            .src_box = src_box,
            .dst_box = dst_box,
            .blend_mode = WLR_RENDER_BLEND_MODE_PREMULTIPLIED,
        };
        
        // Add the texture to the render pass
        wlr_render_pass_add_texture(pass, &tex_opts);
    }
    
    wlr_log(WLR_INFO, "[RENDER_FRAME] Rendered %d windows", window_count);
    
    // --- END RENDERING LOGIC ---

    // Submit the render pass
    if (!wlr_render_pass_submit(pass)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] wlr_render_pass_submit failed for '%s'", wlr_output->name);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }

    // CPU Readback Test (from the RDP buffer)
    void *bdata;
    uint32_t bfmt;
    size_t bstride;
    if (wlr_buffer_begin_data_ptr_access(rendered_buffer, WLR_BUFFER_DATA_PTR_ACCESS_READ, &bdata, &bfmt, &bstride)) {
        if (bdata && rendered_buffer->width > 5 && rendered_buffer->height > 5) {
            unsigned char *pixels_final = (unsigned char *)bdata;
            size_t offset = 5 * bstride + 5 * 4;
            if (bfmt != DRM_FORMAT_XRGB8888 && bfmt != DRM_FORMAT_ARGB8888 && bfmt != 0x34325258) {
                wlr_log(WLR_INFO, "[RENDER_FRAME] Readback format 0x%X might not be 4bpp as assumed for offset calculation.", bfmt);
            }
            if (offset + 3 < bstride * rendered_buffer->height) {
                unsigned char r_val = pixels_final[offset + 0];
                unsigned char g_val = pixels_final[offset + 1];
                unsigned char b_val = pixels_final[offset + 2];
                unsigned char a_val = pixels_final[offset + 3];
                printf("[RENDER_FRAME] RDP Buffer Readback Pixel (5,5) RGBA-order: %02X %02X %02X %02X (Format: 0x%X, Stride: %zu)\n",
                       r_val, g_val, b_val, a_val, bfmt, bstride);
                if (r_val == (unsigned char)(background_color[0]*255.0f) &&
                    g_val == (unsigned char)(background_color[1]*255.0f) &&
                    b_val == (unsigned char)(background_color[2]*255.0f) ) {
                    printf("[RENDER_FRAME] Pixel (5,5) IS BACKGROUND COLOR!\n");
                } else {
                    printf("[RENDER_FRAME] Pixel (5,5) IS NOT PLAIN BACKGROUND (might be client window or other content).\n");
                }
            } else {
                wlr_log(WLR_ERROR, "[RENDER_FRAME] Pixel readback offset out of bounds.");
            }
        }
        wlr_buffer_end_data_ptr_access(rendered_buffer);
    } else {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Failed RDP buffer pixel readback: wlr_buffer_begin_data_ptr_access failed.");
    }

    // Transmit the rendered buffer
    rdp_transmit_surface(rendered_buffer);

    // Commit the output state
    if (!wlr_output_commit_state(wlr_output, &state)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] wlr_output_commit_state failed for '%s'", wlr_output->name);
    } else {
        // Send frame done events to client surfaces
        wl_list_for_each(toplevel_view, &server->toplevels, link) {
            if (!toplevel_view->xdg_toplevel || !toplevel_view->xdg_toplevel->base ||
                !toplevel_view->xdg_toplevel->base->surface->mapped) {
                continue;
            }
            struct wlr_surface *surface = toplevel_view->xdg_toplevel->base->surface;
            if (wlr_surface_has_buffer(surface)) {
                wlr_surface_send_frame_done(surface, &now);
            }
        }
    }

    // Restore the EGL context and clean up
    wlr_egl_restore_context(&prev_ctx);
    wlr_output_state_finish(&state);
}*/

/*

static void output_frame(struct wl_listener *listener, void *data) {
    struct tinywl_output *output_wrapper = wl_container_of(listener, output_wrapper, frame);
    struct wlr_output *wlr_output = output_wrapper->wlr_output;

    printf("[DIRECT_CLEAR_TEST] Frame for '%s'\n", wlr_output->name);
    fflush(stdout);

    if (!wlr_output || !wlr_output->enabled) {
        wlr_log(WLR_ERROR, "[DIRECT_CLEAR_TEST] Output '%s' not valid or not enabled.", wlr_output ? wlr_output->name : "null");
        return;
    }
    if (!wlr_output->renderer || !wlr_output->allocator) {
        wlr_log(WLR_ERROR, "[DIRECT_CLEAR_TEST] Output '%s' has no renderer or allocator.", wlr_output->name);
        return;
    }

    if (!wlr_renderer_is_gles2(wlr_output->renderer)) {
        wlr_log(WLR_ERROR, "[DIRECT_CLEAR_TEST] Renderer is not GLES2");
        return;
    }

    GLenum err_clear_loop;
    int clear_loop_count = 0;
    while ((err_clear_loop = glGetError()) != GL_NO_ERROR && clear_loop_count < 10) {
        wlr_log(WLR_DEBUG, "[DIRECT_CLEAR_TEST] Cleared GL error: 0x%x", err_clear_loop);
        clear_loop_count++;
    }

    struct wlr_egl_context prev_ctx = {0};
    struct wlr_egl *egl = wlr_gles2_renderer_get_egl(wlr_output->renderer);
    if (!egl || !wlr_egl_make_current(egl, &prev_ctx)) {
        wlr_log(WLR_ERROR, "[DIRECT_CLEAR_TEST] Failed to make EGL context current");
        wlr_egl_restore_context(&prev_ctx);
        return;
    }

    struct wlr_output_state state;
    wlr_output_state_init(&state);

    struct wlr_buffer_pass_options pass_options = {0};
    printf("[DIRECT_CLEAR_TEST] Calling wlr_output_begin_render_pass for '%s'\n", wlr_output->name);
    fflush(stdout);
    struct wlr_render_pass *pass = wlr_output_begin_render_pass(wlr_output, &state, &pass_options);

    GLenum err;
    if (!pass) {
        wlr_log(WLR_ERROR, "[DIRECT_CLEAR_TEST] wlr_output_begin_render_pass failed for '%s'", wlr_output->name);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            printf("[DIRECT_CLEAR_TEST] GL Error after FAILED wlr_output_begin_render_pass: 0x%x\n", err);
            fflush(stdout);
        }
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }
    printf("[DIRECT_CLEAR_TEST] wlr_output_begin_render_pass succeeded for '%s'\n", wlr_output->name);
    fflush(stdout);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        wlr_log(WLR_ERROR, "[DIRECT_CLEAR_TEST] FBO incomplete after begin_render_pass: status=0x%x", status);
        wlr_render_pass_submit(pass);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }
    wlr_log(WLR_DEBUG, "[DIRECT_CLEAR_TEST] FBO complete: status=0x%x", status);

    struct wlr_buffer *rendered_buffer = state.buffer;
    if (!rendered_buffer) {
        wlr_log(WLR_ERROR, "[DIRECT_CLEAR_TEST] No buffer available for rendering");
        wlr_render_pass_submit(pass);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }

    bool rendered_surface = false;
    struct wlr_xdg_surface *xdg_surface = NULL;
    struct wlr_surface *surface = xdg_surface ? xdg_surface->surface : NULL;
    if (surface && wlr_surface_has_buffer(surface)) {
        struct wlr_texture *texture = wlr_surface_get_texture(surface);
        if (texture) {
            struct wlr_render_texture_options tex_opts = {
                .texture = texture,
                .dst_box = {0, 0, surface->current.width, surface->current.height},
                .blend_mode = WLR_RENDER_BLEND_MODE_PREMULTIPLIED,
            };
            wlr_render_pass_add_texture(pass, &tex_opts);
            wlr_log(WLR_DEBUG, "[DIRECT_CLEAR_TEST] Rendered XDG surface 0x%p", surface);
            rendered_surface = true;
        }
    }

    if (!rendered_surface) {
        wlr_log(WLR_DEBUG, "[DIRECT_CLEAR_TEST] Using GLES2 renderer for clear");
        struct wlr_render_rect_options clear_opts = {
            .box = {0, 0, wlr_output->width, wlr_output->height},
            .color = {0.0f, 1.0f, 0.0f, 1.0f},
            .blend_mode = WLR_RENDER_BLEND_MODE_NONE,
        };
        wlr_render_pass_add_rect(pass, &clear_opts);
        printf("[DIRECT_CLEAR_TEST] Cleared buffer to green for '%s' (%dx%d)\n",
               wlr_output->name, wlr_output->width, wlr_output->height);
    }

    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    wlr_log(WLR_DEBUG, "[DIRECT_CLEAR_TEST] FBO status before submit: 0x%x", status);

    uint8_t pixel[4];
    glReadPixels(5, 5, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    err = glGetError();
    wlr_log(WLR_DEBUG, "[DIRECT_CLEAR_TEST] glReadPixels before submit, error: 0x%x", err);
    wlr_log(WLR_DEBUG, "[DIRECT_CLEAR_TEST] glReadPixels Pixel (5,5) RGBA: %02x %02x %02x %02x",
            pixel[0], pixel[1], pixel[2], pixel[3]);

    printf("[DIRECT_CLEAR_TEST] Calling wlr_render_pass_submit for '%s'\n", wlr_output->name);
    fflush(stdout);
    if (!wlr_render_pass_submit(pass)) {
        wlr_log(WLR_ERROR, "[DIRECT_CLEAR_TEST] wlr_render_pass_submit failed for '%s'", wlr_output->name);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        return;
    }
    glFinish(); 

    glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);
    printf("[DIRECT_CLEAR_TEST] Render pass submitted for '%s'\n", wlr_output->name);
    fflush(stdout);

    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    wlr_log(WLR_DEBUG, "[DIRECT_CLEAR_TEST] FBO status after submit: 0x%x", status);

    glFlush();
    glFinish();

    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    wlr_log(WLR_DEBUG, "[DIRECT_CLEAR_TEST] FBO status after sync: 0x%x", status);

    void *bdata;
    uint32_t bfmt;
    size_t bstride;
    printf("[DIRECT_CLEAR_TEST] Attempting pixel readback for '%s'\n", wlr_output->name);
    fflush(stdout);
    if (wlr_buffer_begin_data_ptr_access(rendered_buffer, WLR_BUFFER_DATA_PTR_ACCESS_READ, &bdata, &bfmt, &bstride)) {
        if (bdata && rendered_buffer->width > 5 && rendered_buffer->height > 5) {
            unsigned char *pixels = (unsigned char *)bdata;
            size_t offset = 5 * bstride + 5 * 4;
            if (offset + 3 < bstride * rendered_buffer->height) {
                unsigned char r_val = pixels[offset + 0];
                unsigned char g_val = pixels[offset + 1];
                unsigned char b_val = pixels[offset + 2];
                unsigned char a_val = pixels[offset + 3];
                printf("[DIRECT_CLEAR_TEST] Readback Pixel (5,5) RGBA: %02X %02X %02X %02X (Format: 0x%X, Stride: %zu)\n",
                       r_val, g_val, b_val, a_val, bfmt, bstride);
                if (g_val == 0xFF && r_val == 0x00 && b_val == 0x00) {
                    printf("[DIRECT_CLEAR_TEST] Pixel (5,5) IS GREEN!\n");
                } else {
                    printf("[DIRECT_CLEAR_TEST] Pixel (5,5) IS NOT GREEN as expected.\n");
                }
                wlr_log(WLR_DEBUG, "[DIRECT_CLEAR_TEST] Buffer sample (0,0): %02x %02x %02x %02x",
                        pixels[0], pixels[1], pixels[2], pixels[3]);
            } else {
                wlr_log(WLR_ERROR, "[DIRECT_CLEAR_TEST] Pixel readback offset out of bounds.");
            }
        } else {
            wlr_log(WLR_INFO, "[DIRECT_CLEAR_TEST] Buffer too small or bdata NULL for pixel check.");
        }
        wlr_buffer_end_data_ptr_access(rendered_buffer);
    } else {
        wlr_log(WLR_ERROR, "[DIRECT_CLEAR_TEST] Failed pixel readback: begin_data_ptr_access failed.");
    }

    printf("[DIRECT_CLEAR_TEST] Transmitting buffer %p via RDP for '%s'\n", (void *)rendered_buffer, wlr_output->name);
    fflush(stdout);
    rdp_transmit_surface(rendered_buffer);
    wlr_buffer_unlock(rendered_buffer);

    printf("[DIRECT_CLEAR_TEST] Committing state to output '%s'\n", wlr_output->name);
    fflush(stdout);
    if (!wlr_output_commit_state(wlr_output, &state)) {
        wlr_log(WLR_ERROR, "[DIRECT_CLEAR_TEST] wlr_output_commit_state failed for '%s'", wlr_output->name);
    } else {
        printf("[DIRECT_CLEAR_TEST] wlr_output_commit_state succeeded for '%s'\n", wlr_output->name);
    }
    wlr_egl_restore_context(&prev_ctx);
    wlr_output_state_finish(&state);

    printf("[DIRECT_CLEAR_TEST] Frame complete for '%s'\n", wlr_output->name);
    fflush(stdout);
}*/


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
    /* This event is raised by the backend when a new output (aka a display or
     * monitor) becomes available. */
struct tinywl_server *server =
        wl_container_of(listener, server, new_output);
    struct wlr_output *wlr_output = data;
     struct tinywl_output *output_wrapper_iter;
    wl_list_for_each(output_wrapper_iter, &server->outputs, link) {
        if (output_wrapper_iter->wlr_output == wlr_output) {
            wlr_log(WLR_INFO, "server_new_output: Output %s (ptr %p) already processed, skipping.",
                    wlr_output->name ? wlr_output->name : "(null)", (void*)wlr_output);
            return; // Already handled this specific wlr_output instance
        }
    }
    

    /* Configures the output created by the backend to use our allocator
     * and our renderer. Must be done once, before commiting the output */
    wlr_output_init_render(wlr_output, server->allocator, server->renderer);

    /* The output may be disabled, switch it on. */
    struct wlr_output_state state;
    wlr_output_state_init(&state);
    wlr_output_state_set_enabled(&state, true);

    /* Some backends don't have modes. DRM+KMS does, and we need to set a mode
     * before we can use the output. The mode is a tuple of (width, height,
     * refresh rate), and each monitor supports only a specific set of modes. We
     * just pick the monitor's preferred mode, a more sophisticated compositor
     * would let the user configure it. */
    struct wlr_output_mode *mode = wlr_output_preferred_mode(wlr_output);
    if (mode != NULL) {
        wlr_output_state_set_mode(&state, mode);
    }

    /* Atomically applies the new output state. */
    wlr_output_commit_state(wlr_output, &state);
    wlr_output_state_finish(&state);

    /* Allocates and configures our state for this output */
    struct tinywl_output *output = calloc(1, sizeof(*output));
    output->wlr_output = wlr_output;
    output->server = server;

    /* Sets up a listener for the frame event. */
    output->frame.notify = output_frame;
    wl_signal_add(&wlr_output->events.frame, &output->frame);

    /* Sets up a listener for the state request event. */
    output->request_state.notify = output_request_state;
    wl_signal_add(&wlr_output->events.request_state, &output->request_state);

    /* Sets up a listener for the destroy event. */
    output->destroy.notify = output_destroy;
    wl_signal_add(&wlr_output->events.destroy, &output->destroy);

    wl_list_insert(&server->outputs, &output->link);

    /* Adds this to the output layout. The add_auto function arranges outputs
     * from left-to-right in the order they appear. A more sophisticated
     * compositor would let the user configure the arrangement of outputs in the
     * layout.
     *
     * The output layout utility automatically adds a wl_output global to the
     * display, which Wayland clients can see to find out information about the
     * output (such as DPI, scale factor, manufacturer, etc).
     */
    struct wlr_output_layout_output *l_output = wlr_output_layout_add_auto(server->output_layout,
        wlr_output);
    struct wlr_scene_output *scene_output = wlr_scene_output_create(server->scene, wlr_output);
    wlr_scene_output_layout_add_output(server->scene_layout, l_output, scene_output);
    wlr_output_schedule_frame(output->wlr_output);

}

static void xdg_toplevel_map(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, map);
    wlr_log(WLR_INFO, "Mapping XDG toplevel: %p, title='%s', mapped=%d",
            toplevel->xdg_toplevel,
            toplevel->xdg_toplevel->title ? toplevel->xdg_toplevel->title : "(null)",
            toplevel->xdg_toplevel->base->surface->mapped);

    wl_list_insert(&toplevel->server->toplevels, &toplevel->link);
    focus_toplevel(toplevel);

    struct wlr_surface *surface = toplevel->xdg_toplevel->base->surface;
    if (surface && surface->mapped) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        wlr_surface_send_frame_done(surface, &now);

        // Force a frame schedule for all outputs
        struct tinywl_output *output;
        wl_list_for_each(output, &toplevel->server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
                wlr_log(WLR_INFO, "Scheduled frame for output %s on toplevel map",
                        output->wlr_output->name);
            }
        }
    } else {
        wlr_log(WLR_ERROR, "Toplevel %p not mapped or no surface", toplevel);
    }
}

static void xdg_toplevel_unmap(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, unmap);
    if (toplevel == toplevel->server->grabbed_toplevel) {
        reset_cursor_mode(toplevel->server);
    }
    wl_list_remove(&toplevel->link);
}

// In tinywl.c

// Ensure you have these includes at the top:
// #include <wlr/types/wlr_scene.h> // For wlr_scene_get_scene_output

static void xdg_toplevel_commit(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, commit);
    struct wlr_surface *surface = toplevel->xdg_toplevel->base->surface;
    wlr_log(WLR_DEBUG, "xdg_toplevel_commit: toplevel=%p, title='%s', has_buffer=%d, mapped=%d",
            toplevel,
            toplevel->xdg_toplevel->title ? toplevel->xdg_toplevel->title : "(null)",
            wlr_surface_has_buffer(surface),
            surface->mapped);

    if (toplevel->xdg_toplevel->base->initial_commit) {
        wlr_xdg_toplevel_set_size(toplevel->xdg_toplevel, 0, 0);
    }

    if (surface && surface->mapped && wlr_surface_has_buffer(surface)) {
        struct tinywl_output *output;
        wl_list_for_each(output, &toplevel->server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
                wlr_log(WLR_DEBUG, "Scheduled frame for output %s on surface commit",
                        output->wlr_output->name);
            }
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
    wlr_log(WLR_INFO, "New XDG toplevel detected: %p, title='%s', app_id='%s'",
            xdg_toplevel,
            xdg_toplevel->title ? xdg_toplevel->title : "(null)",
            xdg_toplevel->app_id ? xdg_toplevel->app_id : "(null)");

    struct tinywl_toplevel *toplevel = calloc(1, sizeof(*toplevel));
    if (!toplevel) {
        wlr_log(WLR_ERROR, "Failed to allocate tinywl_toplevel");
        return;
    }
    toplevel->server = server;
    toplevel->xdg_toplevel = xdg_toplevel;
    toplevel->scene_tree = wlr_scene_xdg_surface_create(&server->scene->tree, xdg_toplevel->base);
    if (!toplevel->scene_tree) {
        wlr_log(WLR_ERROR, "Failed to create scene tree for toplevel");
        free(toplevel);
        return;
    }
    toplevel->scene_tree->node.data = toplevel;
    xdg_toplevel->base->data = toplevel->scene_tree;

    // Register listeners
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

    wlr_log(WLR_INFO, "Toplevel %p initialized and added to scene tree", toplevel);
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

/*
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
}*/

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
    
    setenv("MESA_VK_VERSION_OVERRIDE", "1.2", 1);
    setenv("MESA_LOADER_DRIVER_OVERRIDE", "zink", 1);
   // setenv("GALLIUM_DRIVER", "zink", 1);
   // setenv("GALLIUM_DRIVER", "d3d12", 1); 
    setenv("ZINK_DEBUG", "nofp64,nofast_color_clear", 1);
    setenv("VK_DRIVER_FILES", "/usr/share/vulkan/icd.d/vulkan_icd.json", 1);
    setenv("ZINK_DESCRIPTORS", "lazy", 1);
    setenv("ZINK_NO_TIMELINES", "1", 1);
    setenv("ZINK_NO_DECOMPRESS", "1", 1);

    printf("Starting compositor with surfaceless EGL display\n");

    // Print environment variables
    printf("VK_DRIVER_FILES=%s\n", getenv("VK_DRIVER_FILES"));
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
 //struct wlr_egl *wlr_egl = NULL;
/*
    // Setup EGL surfaceless display
    wlr_egl = setup_surfaceless_egl(&server);
    if (!wlr_egl) {
        fprintf(stderr, "Failed to setup surfaceless EGL display\n");
        cleanup_egl(&server);
        return 1;
    }
*/

    server.wl_display = wl_display_create();
    if (!server.wl_display) {
        wlr_log(WLR_ERROR, "Cannot create Wayland display");
        return 1;
    }


server.seat = wlr_seat_create(server.wl_display, "seat0");
    if (!server.seat) {
        wlr_log(WLR_ERROR, "Failed to create seat");
        wl_display_destroy(server.wl_display);
        return 1;
    }
    server.request_cursor.notify = seat_request_cursor;
    wl_signal_add(&server.seat->events.request_set_cursor, &server.request_cursor);
    server.request_set_selection.notify = seat_request_set_selection;
    wl_signal_add(&server.seat->events.request_set_selection, &server.request_set_selection);
    wlr_log(WLR_INFO, "Compositor seat 'seat0' created: %p", (void*)server.seat);


wlr_log(WLR_INFO, "Setting compositor display");
wlr_backend_set_compositor_display(server.wl_display);
    const char *backends_env = getenv("WLR_BACKENDS");
    if (backends_env && strcmp(backends_env, "RDP") == 0) {
        wlr_log(WLR_INFO, "Creating RDP backend");
//        server.backend = wlr_RDP_backend_create(server.wl_display);


 setenv("WLR_BACKENDS", "RDP",1);

server.backend = wlr_RDP_backend_create(server.wl_display);
  if (!server.backend) {
            wlr_log(WLR_ERROR, "Failed to create RDP backend");
            wlr_seat_destroy(server.seat); 
            wl_display_destroy(server.wl_display);
            return 1;
        }
        // ***** SET THE GLOBAL SEAT FOR RDP BACKEND *****
        wlr_RDP_backend_set_compositor_seat(server.seat); 
        wlr_log(WLR_INFO, "RDP Backend: Compositor seat '%s' (%p) set.", server.seat->name, (void*)server.seat);

    } else {
        wlr_log(WLR_INFO, "Autocreating backend (not RDP).");
        server.backend = wlr_backend_autocreate(wl_display_get_event_loop(server.wl_display), NULL);
        if (!server.backend) {
            wlr_log(WLR_ERROR, "Failed to autocreate backend");
            wlr_seat_destroy(server.seat); 
            wl_display_destroy(server.wl_display);
            return 1;
        }
    }

    server.renderer = wlr_gles2_renderer_create_surfaceless();
    if (server.renderer == NULL) {
        wlr_log(WLR_ERROR, "failed to create wlr_renderer");
        return 1;
    }
/*//
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
*/

const char *vendor = (const char *)glGetString(GL_VENDOR);
    const char *renderer = (const char *)glGetString(GL_RENDERER);
    const char *version = (const char *)glGetString(GL_VERSION);
    const char *shading_lang = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);

    wlr_log(WLR_INFO, "GL_VENDOR: %s", vendor);
    wlr_log(WLR_INFO, "GL_RENDERER: %s", renderer);
    wlr_log(WLR_INFO, "GL_VERSION: %s", version);
    wlr_log(WLR_INFO, "GL_SHADING_LANGUAGE_VERSION: %s", shading_lang);


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