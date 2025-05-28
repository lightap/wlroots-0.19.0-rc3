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
    struct wl_list popups;        // List of tinywl_popup objects
    struct wl_listener new_popup; // Listener for new xdg_popup events



  
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

    // This 'scene_tree' should be the *main frame* for the window,
    // including decorations AND client content area.
    struct wlr_scene_tree *scene_tree; // Overall window frame

    // This will point to the scene tree specifically for the client's content
    // (i.e., the tree returned by wlr_scene_xdg_surface_create for the base xdg_surface)
    struct wlr_scene_tree *client_xdg_scene_tree; // Tree for xdg_toplevel->base

    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener commit;
    struct wl_listener destroy;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    struct wl_listener request_maximize;
    struct wl_listener request_fullscreen;
    struct wlr_texture *cached_texture;
    uint32_t last_commit_seq;
    bool needs_redraw;
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

struct tinywl_popup {
    struct wl_listener map;       // Listener for surface map events
    struct wl_listener unmap;     // Listener for surface unmap events
    struct wl_listener destroy;   // Listener for popup destroy
    struct wl_listener commit;    // Listener for surface commit
    struct wlr_xdg_popup *xdg_popup; // The underlying xdg_popup
    struct tinywl_server *server;    // Reference to the compositor
    struct wlr_scene_tree *scene_tree; // Scene node for rendering
    struct wl_list link;          // Link for list management
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
    rules.layout = "gb";
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

// Helper function (ensure this is correctly defined)
static bool is_descendant(struct wlr_scene_node *node, struct wlr_scene_node *ancestor_candidate) {
    if (!node || !ancestor_candidate) {
        return false;
    }
    struct wlr_scene_node *current = node;
    while (current != NULL) {
        if (current == ancestor_candidate) {
            return true;
        }
        if (current->parent == NULL) {
            break;
        }
        current = &current->parent->node;
    }
    return false;
}
/*
static struct tinywl_toplevel *desktop_toplevel_at(
        struct tinywl_server *server, double lx, double ly,
        struct wlr_surface **surface_out, double *sx_out, double *sy_out) {

    wlr_log(WLR_DEBUG, "[DTA_ENTRY] Cursor lx=%.2f, ly=%.2f", lx, ly);

    struct wlr_scene_node *hit_node = wlr_scene_node_at(
        &server->scene->tree.node, lx, ly, sx_out, sy_out); // sx_out, sy_out are local to hit_node

    struct tinywl_toplevel *toplevel = NULL;
    bool picked_by_geometry_check = false;

    // 1. Attempt to find toplevel via scene graph hit
    if (hit_node != NULL && hit_node->type == WLR_SCENE_NODE_BUFFER) {
        wlr_log(WLR_DEBUG, "[DTA] Initial Hit Node: %p. Initial sx=%.2f, sy=%.2f",
                (void*)hit_node, *sx_out, *sy_out);
        
        struct wlr_scene_node *iter_node = hit_node;
        while (iter_node != NULL) { // Find associated toplevel by traversing up from hit_node
            if (iter_node->data != NULL) {
                struct tinywl_toplevel *potential_toplevel = iter_node->data;
                struct tinywl_toplevel *t_iter_list;
                wl_list_for_each(t_iter_list, &server->toplevels, link) {
                    if (t_iter_list == potential_toplevel && t_iter_list->scene_tree && &t_iter_list->scene_tree->node == iter_node) {
                        toplevel = t_iter_list;
                        break;
                    }
                }
                if (toplevel) break;
            }
            if (iter_node->parent == NULL) { iter_node = NULL; }
            else { iter_node = &iter_node->parent->node; }
        }

        if (toplevel == NULL) { // Fallback: check if hit_node is descendant of any toplevel's frame
            struct tinywl_toplevel *t_iter_list;
            wl_list_for_each(t_iter_list, &server->toplevels, link) {
                if (t_iter_list->scene_tree && is_descendant(hit_node, &t_iter_list->scene_tree->node)) {
                    toplevel = t_iter_list;
                    break;
                }
            }
        }
        if (toplevel) {
             wlr_log(WLR_DEBUG, "[DTA_PICK_SCENE] Toplevel %p found via scene graph.", (void*)toplevel);
        }
    } else {
         wlr_log(WLR_DEBUG, "[DTA] No buffer node hit by wlr_scene_node_at. Initial sx=%.2f, sy=%.2f", *sx_out, *sy_out);
    }

    // 2. If no toplevel found via scene graph, or if you want to prioritize geometry for edges:
    //    Iterate toplevels and check if (lx, ly) is within their configured geometry.
    //    This acts as a broader hitbox.
    if (toplevel == NULL) { // Only do geometry check if scene hit failed to find our toplevel
        wlr_log(WLR_DEBUG, "[DTA_PICK_GEOM_TRY] No toplevel from scene hit. Trying geometry check.");
        struct tinywl_toplevel *iter_toplevel;
        wl_list_for_each_reverse(iter_toplevel, &server->toplevels, link) { // Reverse for Z-order
            if (!iter_toplevel->scene_tree || !iter_toplevel->xdg_toplevel || 
                !iter_toplevel->xdg_toplevel->base || !iter_toplevel->xdg_toplevel->base->surface ||
                !iter_toplevel->client_xdg_scene_tree) { // Ensure client_xdg_scene_tree exists
                continue;
            }

            int frame_abs_x, frame_abs_y;
            if (!wlr_scene_node_coords(&iter_toplevel->scene_tree->node, &frame_abs_x, &frame_abs_y)) {
                continue;
            }

            struct wlr_box geometry = iter_toplevel->xdg_toplevel->base->geometry;
            if (geometry.width <= 0 || geometry.height <= 0) { // Skip if no valid geometry
                // Fallback to surface size if geometry isn't good
                if (wlr_surface_has_buffer(iter_toplevel->xdg_toplevel->base->surface)) {
                    geometry.width = iter_toplevel->xdg_toplevel->base->surface->current.width;
                    geometry.height = iter_toplevel->xdg_toplevel->base->surface->current.height;
                } else {
                    continue;
                }
            }


            // Assuming client_xdg_scene_tree is at (0,0) within scene_tree (the frame)
            // as per server_new_xdg_toplevel modifications (border_size=0, titlebar_height=0)
            double client_content_origin_abs_x = (double)frame_abs_x + iter_toplevel->client_xdg_scene_tree->node.x;
            double client_content_origin_abs_y = (double)frame_abs_y + iter_toplevel->client_xdg_scene_tree->node.y;
            
            // The interactive area is defined by the client content's origin and its configured geometry size
            if (lx >= client_content_origin_abs_x && lx < client_content_origin_abs_x + geometry.width &&
                ly >= client_content_origin_abs_y && ly < client_content_origin_abs_y + geometry.height) {
                
                toplevel = iter_toplevel;
                *surface_out = toplevel->xdg_toplevel->base->surface;
                
                // sx, sy should be relative to the client content area's origin
                *sx_out = lx - client_content_origin_abs_x;
                *sy_out = ly - client_content_origin_abs_y;
                
                wlr_log(WLR_DEBUG, "[DTA_PICK_GEOM_HIT] Toplevel %p picked by geometry. sx=%.2f, sy=%.2f", 
                        (void*)toplevel, *sx_out, *sy_out);
                picked_by_geometry_check = true; 
                break; 
            }
        }
    }

    if (toplevel == NULL) {
        wlr_log(WLR_DEBUG, "[DTA_PICK_FAIL] No toplevel found by any method.");
        return NULL;
    }

    // If picked by geometry, sx_out and sy_out are already calculated and correct.
    // The rest of the function transforms coordinates if picked by *scene_graph_hit*
    // and hit_node is different from the main client surface buffer node.
    if (picked_by_geometry_check) {
        // Ensure surface_out is correctly set if not already
        if (*surface_out == NULL && toplevel->xdg_toplevel && toplevel->xdg_toplevel->base) {
             *surface_out = toplevel->xdg_toplevel->base->surface;
        }
        if (!*surface_out) {
            wlr_log(WLR_ERROR, "[DTA] Toplevel %p (picked by geom) missing base surface.", (void*)toplevel);
            return NULL;
        }
        return toplevel;
    }

    // --- This part executes if toplevel was found by SCENE GRAPH HIT ---
    // --- and hit_node was not NULL/irrelevant initially.               ---
    assert(hit_node != NULL); // Should be true if !picked_by_geometry_check and toplevel != NULL

    *surface_out = toplevel->xdg_toplevel->base->surface;
    if (!*surface_out) {
        wlr_log(WLR_ERROR, "[DTA_SCENE_HIT] Toplevel %p has no base xdg surface.", (void*)toplevel);
        return NULL;
    }
    wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT] Target client surface: %p", (void*)*surface_out);

    int hit_node_abs_x, hit_node_abs_y;
    if (!wlr_scene_node_coords(hit_node, &hit_node_abs_x, &hit_node_abs_y)) {
        wlr_log(WLR_ERROR, "[DTA_SCENE_HIT_COORD] Could not get abs coords of hit_node %p.", (void*)hit_node);
        return NULL;
    }
    wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT_COORD] Hit Node %p Abs Coords: x=%d, y=%d. Initial local sx=%.2f, sy=%.2f", 
            (void*)hit_node, hit_node_abs_x, hit_node_abs_y, *sx_out, *sy_out);

    struct wlr_scene_node *main_client_surface_buffer_node = NULL;
    if (toplevel->client_xdg_scene_tree) { // Find the specific buffer node for the main client surface
        struct wlr_scene_node *child_iter;
        wl_list_for_each(child_iter, &toplevel->client_xdg_scene_tree->children, link) {
            if (child_iter->type == WLR_SCENE_NODE_BUFFER) {
                struct wlr_scene_buffer *buf = wlr_scene_buffer_from_node(child_iter);
                struct wlr_scene_surface *scene_surf = wlr_scene_surface_try_from_buffer(buf);
                if (scene_surf && scene_surf->surface == *surface_out) {
                    main_client_surface_buffer_node = child_iter;
                    break;
                }
            }
        }
    }
     if (temp_buffer_node) {
         wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT_CLIENT_NODE] Found main_client_surface_buffer_node: %p", (void*)main_client_surface_buffer_node);
    } else {
         wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT_CLIENT_NODE] Main client surface buffer node NOT found for surface %p.", (void*)*surface_out);
    }

    // If hit_node IS the client's main surface buffer, initial sx_out, sy_out are correct.
    if (main_client_surface_buffer_node && hit_node == main_client_surface_buffer_node) {
        wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT_TRANSFORM] Hit node IS main client surface buffer. No transformation. Final sx=%.2f, sy=%.2f", *sx_out, *sy_out);
        return toplevel;
    }

    // If hit_node is NOT the main client surface buffer (e.g., a decoration, or if client buffer node wasn't found distinctly),
    // transform coordinates to be relative to the *expected* client content area origin.
    wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT_TRANSFORM_EXPECTED] Transforming based on expected client position (hit_node %p, client_surf_node %p).",
            (void*)hit_node, (void*)main_client_surface_buffer_node);
    if (!toplevel->scene_tree || !toplevel->client_xdg_scene_tree) {
        wlr_log(WLR_ERROR, "[DTA_SCENE_HIT_COORD] Toplevel %p missing scene_tree or client_xdg_scene_tree.", (void*)toplevel);
        return NULL;
    }

    int frame_abs_x, frame_abs_y;
    if (!wlr_scene_node_coords(&toplevel->scene_tree->node, &frame_abs_x, &frame_abs_y)) {
        wlr_log(WLR_ERROR, "[DTA_SCENE_HIT_COORD] Could not get abs coords of frame node %p.", (void*)&toplevel->scene_tree->node);
        return NULL;
    }
    
    // Position of client_xdg_scene_tree relative to scene_tree (frame) - should be (0,0)
    double client_rel_x_in_frame = toplevel->client_xdg_scene_tree->node.x;
    double client_rel_y_in_frame = toplevel->client_xdg_scene_tree->node.y;

    double expected_client_area_abs_x = (double)frame_abs_x + client_rel_x_in_frame;
    double expected_client_area_abs_y = (double)frame_abs_y + client_rel_y_in_frame;
     wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT_COORD] Frame abs: (%d,%d). Client_rel: (%.1f,%.1f). Expected client abs: (%.1f,%.1f)",
            frame_abs_x, frame_abs_y, client_rel_x_in_frame, client_rel_y_in_frame, expected_client_area_abs_x, expected_client_area_abs_y);


    double final_sx = (double)hit_node_abs_x + *sx_out - expected_client_area_abs_x;
    double final_sy = (double)hit_node_abs_y + *sy_out - expected_client_area_abs_y;

    *sx_out = final_sx;
    *sy_out = final_sy;
    wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT_TRANSFORM_EXPECTED] Transformed sx=%.2f, sy=%.2f", *sx_out, *sy_out);
    return toplevel;
}*/


static struct tinywl_toplevel *desktop_toplevel_at(
        struct tinywl_server *server, double lx, double ly,
        struct wlr_surface **surface_out, double *sx_out, double *sy_out) {

    wlr_log(WLR_DEBUG, "[DTA_ENTRY] Cursor at lx=%.2f, ly=%.2f", lx, ly);

    struct wlr_scene_node *hit_node = wlr_scene_node_at(
        &server->scene->tree.node, lx, ly, sx_out, sy_out); // sx_out, sy_out are local to hit_node
    struct tinywl_toplevel *toplevel = NULL;
    bool picked_by_geometry_check = false;

    // 1. Attempt to find toplevel via scene graph hit
    if (hit_node != NULL && hit_node->type == WLR_SCENE_NODE_BUFFER) {
        wlr_log(WLR_DEBUG, "[DTA] Initial Hit Node: %p. Initial sx=%.2f, sy=%.2f",
                (void*)hit_node, *sx_out, *sy_out);
        
        struct wlr_scene_node *iter_node = hit_node;
        while (iter_node != NULL) {
            if (iter_node->data != NULL) {
                struct tinywl_toplevel *potential_toplevel = iter_node->data;
                struct tinywl_toplevel *t_iter_list;
                wl_list_for_each(t_iter_list, &server->toplevels, link) {
                    if (t_iter_list == potential_toplevel && t_iter_list->scene_tree && &t_iter_list->scene_tree->node == iter_node) {
                        toplevel = t_iter_list;
                        break;
                    }
                }
                if (toplevel) break;
            }
            if (iter_node->parent == NULL) { iter_node = NULL; }
            else { iter_node = &iter_node->parent->node; }
        }

        if (toplevel == NULL) {
            struct tinywl_toplevel *t_iter_list;
            wl_list_for_each(t_iter_list, &server->toplevels, link) {
                if (t_iter_list->scene_tree && is_descendant(hit_node, &t_iter_list->scene_tree->node)) {
                    toplevel = t_iter_list;
                    break;
                }
            }
        }
        if (toplevel) {
            wlr_log(WLR_DEBUG, "[DTA_PICK_SCENE] Toplevel %p found via scene graph.", (void*)toplevel);
        }
    } else {
        wlr_log(WLR_DEBUG, "[DTA] No buffer node hit by wlr_scene_node_at. Initial sx=%.2f, sy=%.2f", *sx_out, *sy_out);
    }

    // 2. Fallback to geometry-based picking
    if (toplevel == NULL) {
        wlr_log(WLR_DEBUG, "[DTA_PICK_GEOM_TRY] No toplevel from scene hit. Trying geometry check.");
        struct tinywl_toplevel *iter_toplevel;
        wl_list_for_each_reverse(iter_toplevel, &server->toplevels, link) {
            if (!iter_toplevel->scene_tree || !iter_toplevel->xdg_toplevel || 
                !iter_toplevel->xdg_toplevel->base || !iter_toplevel->xdg_toplevel->base->surface ||
                !iter_toplevel->client_xdg_scene_tree) {
                continue;
            }

            int frame_abs_x, frame_abs_y;
            if (!wlr_scene_node_coords(&iter_toplevel->scene_tree->node, &frame_abs_x, &frame_abs_y)) {
                continue;
            }

            // Use geometry, but fall back to surface size if geometry is invalid
            struct wlr_box geometry;
          geometry = iter_toplevel->xdg_toplevel->base->geometry; // Direct access if available
            struct wlr_surface *surface = iter_toplevel->xdg_toplevel->base->surface;
            if (geometry.width <= 0 || geometry.height <= 0) {
                if (wlr_surface_has_buffer(surface)) {
                    geometry.width = surface->current.width;
                    geometry.height = surface->current.height;
                    geometry.x = 0;
                    geometry.y = 0;
                    wlr_log(WLR_DEBUG, "[DTA_PICK_GEOM] Fallback to surface size: %dx%d",
                            geometry.width, geometry.height);
                } else {
                    continue;
                }
            }

            // Include decorations in the hitbox (adjust these values based on your decoration setup)
            const int border_size = 0; // Adjust if you have server-side borders
            const int titlebar_height = 0; // Adjust if you have a titlebar
            geometry.x -= border_size;
            geometry.y -= titlebar_height;
            geometry.width += 2 * border_size;
            geometry.height += titlebar_height + border_size;

            // Calculate the absolute position of the window frame (including decorations)
            double frame_origin_abs_x = (double)frame_abs_x;
            double frame_origin_abs_y = (double)frame_abs_y;

            // Check if cursor is within the full window frame
            if (lx >= frame_origin_abs_x + geometry.x &&
                lx < frame_origin_abs_x + geometry.x + geometry.width &&
                ly >= frame_origin_abs_y + geometry.y &&
                ly < frame_origin_abs_y + geometry.y + geometry.height) {
                
                toplevel = iter_toplevel;
                *surface_out = surface;
                
                // Calculate sx, sy relative to the client content area
                double client_rel_x = iter_toplevel->client_xdg_scene_tree->node.x;
                double client_rel_y = iter_toplevel->client_xdg_scene_tree->node.y;
                *sx_out = lx - frame_origin_abs_x - client_rel_x;
                *sy_out = ly - frame_origin_abs_y - client_rel_y;

                wlr_log(WLR_DEBUG, "[DTA_PICK_GEOM_HIT] Toplevel %p picked by geometry. "
                        "Frame abs: (%.2f,%.2f), Geometry: (%d,%d,%d,%d), Client rel: (%.2f,%.2f), "
                        "sx=%.2f, sy=%.2f",
                        (void*)toplevel, frame_origin_abs_x, frame_origin_abs_y,
                        geometry.x, geometry.y, geometry.width, geometry.height,
                        client_rel_x, client_rel_y, *sx_out, *sy_out);
                picked_by_geometry_check = true;
                break;
            }
        }
    }

    if (toplevel == NULL) {
        wlr_log(WLR_DEBUG, "[DTA_PICK_FAIL] No toplevel found by any method.");
        *surface_out = NULL;
        *sx_out = 0;
        *sy_out = 0;
        return NULL;
    }

    // If picked by geometry, coordinates are already correct
    if (picked_by_geometry_check) {
        if (*surface_out == NULL && toplevel->xdg_toplevel && toplevel->xdg_toplevel->base) {
            *surface_out = toplevel->xdg_toplevel->base->surface;
        }
        if (!*surface_out) {
            wlr_log(WLR_ERROR, "[DTA] Toplevel %p (picked by geom) missing base surface.", (void*)toplevel);
            return NULL;
        }
        return toplevel;
    }

    // Handle scene graph hit case
    assert(hit_node != NULL);
    *surface_out = toplevel->xdg_toplevel->base->surface;
    if (!*surface_out) {
        wlr_log(WLR_ERROR, "[DTA_SCENE_HIT] Toplevel %p has no base xdg surface.", (void*)toplevel);
        return NULL;
    }
    wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT] Target client surface: %p", (void*)*surface_out);

    int hit_node_abs_x, hit_node_abs_y;
    if (!wlr_scene_node_coords(hit_node, &hit_node_abs_x, &hit_node_abs_y)) {
        wlr_log(WLR_ERROR, "[DTA_SCENE_HIT_COORD] Could not get abs coords of hit_node %p.", (void*)hit_node);
        return NULL;
    }
    wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT_COORD] Hit Node %p Abs Coords: x=%d, y=%d. Initial local sx=%.2f, sy=%.2f",
            (void*)hit_node, hit_node_abs_x, hit_node_abs_y, *sx_out, *sy_out);

    // Find the main client surface buffer node
 struct wlr_scene_node *temp_buffer_node = NULL;
    if (toplevel->client_xdg_scene_tree) {
        struct wlr_scene_node *child_iter;
        wl_list_for_each(child_iter, &toplevel->client_xdg_scene_tree->children, link) {
            if (child_iter->type == WLR_SCENE_NODE_BUFFER) {
                struct wlr_scene_buffer *buf = wlr_scene_buffer_from_node(child_iter);
                struct wlr_scene_surface *scene_surf = wlr_scene_surface_try_from_buffer(buf);
                if (scene_surf && scene_surf->surface == *surface_out) {
                    temp_buffer_node = child_iter;
                    break;
                }
            }
        }
    }

    if (temp_buffer_node) {
        wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT_CLIENT_NODE] Found main_client_surface_buffer_node: %p", (void*)temp_buffer_node);
    } else {
        wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT_CLIENT_NODE] Main client surface buffer node NOT found for surface %p.", (void*)*surface_out);
    }

    // If hit_node is the client surface buffer, sx_out, sy_out are correct
    if (temp_buffer_node && hit_node == temp_buffer_node) {
        wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT_TRANSFORM] Hit node IS main client surface buffer. No transformation. Final sx=%.2f, sy=%.2f", *sx_out, *sy_out);
        return toplevel;
    }

    // Transform coordinates to client content area
    if (!toplevel->scene_tree || !toplevel->client_xdg_scene_tree) {
        wlr_log(WLR_ERROR, "[DTA_SCENE_HIT_COORD] Toplevel %p missing scene_tree or client_xdg_scene_tree.", (void*)toplevel);
        return NULL;
    }

    int frame_abs_x, frame_abs_y;
    if (!wlr_scene_node_coords(&toplevel->scene_tree->node, &frame_abs_x, &frame_abs_y)) {
        wlr_log(WLR_ERROR, "[DTA_SCENE_HIT_COORD] Could not get abs coords of frame node %p.", (void*)&toplevel->scene_tree->node);
        return NULL;
    }

    double client_rel_x = toplevel->client_xdg_scene_tree->node.x;
    double client_rel_y = toplevel->client_xdg_scene_tree->node.y;
    double client_abs_x = (double)frame_abs_x + client_rel_x;
    double client_abs_y = (double)frame_abs_y + client_rel_y;

    *sx_out = lx - client_abs_x;
    *sy_out = ly - client_abs_y;

    wlr_log(WLR_DEBUG, "[DTA_SCENE_HIT_TRANSFORM] Transformed sx=%.2f, sy=%.2f. "
            "Frame abs: (%.2f,%.2f), Client rel: (%.2f,%.2f), Client abs: (%.2f,%.2f)",
            *sx_out, *sy_out, (double)frame_abs_x, (double)frame_abs_y,
            client_rel_x, client_rel_y, client_abs_x, client_abs_y);

    return toplevel;
}

///////////////////////////////////////////////////

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


// Add this helper function to improve popup handling
static void ensure_popup_responsiveness(struct tinywl_server *server, struct wlr_surface *surface) {
    if (!surface) return;
    
    struct wlr_xdg_surface *xdg_surface = wlr_xdg_surface_try_from_wlr_surface(surface);
    if (!xdg_surface || xdg_surface->role != WLR_XDG_SURFACE_ROLE_POPUP) {
        return;
    }
    
    // Ensure popup gets frame callbacks
    if (surface->current.buffer) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        wlr_surface_send_frame_done(surface, &now);
    }
    
    // Schedule configure if needed
    if (!xdg_surface->configured) {
        wlr_xdg_surface_schedule_configure(xdg_surface);
    }
}
/////////////////////////////////////////////////////////////
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
    wlr_log(WLR_DEBUG, "[PROCESS_CURSOR_MOTION] Cursor at: (%.2f, %.2f)", server->cursor->x, server->cursor->y);
    
    struct tinywl_toplevel *toplevel = desktop_toplevel_at(server,
            server->cursor->x, server->cursor->y, &surface, &sx, &sy);
    
    wlr_log(WLR_DEBUG, "[PROCESS_CURSOR_MOTION] Surface: %p, Surface coords: (%.2f, %.2f), Toplevel: %p",
            surface, sx, sy, toplevel);
    
    if (!toplevel) {
        wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
    }
    
    if (surface) {
        ensure_popup_responsiveness(server, surface);
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

///////////////////////////////////////////////////////////////////////
/*static void server_cursor_button(struct wl_listener *listener, void *data) {
    struct tinywl_server *server =
        wl_container_of(listener, server, cursor_button);
    struct wlr_pointer_button_event *event = data;
    
    double sx, sy;
    struct wlr_surface *surface = NULL;
    struct tinywl_toplevel *toplevel = desktop_toplevel_at(server,
            server->cursor->x, server->cursor->y, &surface, &sx, &sy);
    
    // Handle button press events
    if (event->state == WL_POINTER_BUTTON_STATE_PRESSED) {
        // Only change focus for main windows, not for popups/submenus
        if (toplevel) {
            // Check if we're clicking on a popup surface
            bool is_popup_surface = false;
            if (surface) {
                struct wlr_xdg_surface *xdg_surface = wlr_xdg_surface_try_from_wlr_surface(surface);
                if (xdg_surface && xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
                    is_popup_surface = true;
                }
            }
            
            // Only focus toplevel if we're not clicking on a popup
            if (!is_popup_surface) {
                focus_toplevel(toplevel);
            }
        }
    } else if (event->state == WL_POINTER_BUTTON_STATE_RELEASED) {
        // Only reset cursor mode if we're not over a popup
        bool over_popup = false;
        if (surface) {
            struct wlr_xdg_surface *xdg_surface = wlr_xdg_surface_try_from_wlr_surface(surface);
            if (xdg_surface && xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
                over_popup = true;
            }
        }
        
        if (!over_popup) {
            reset_cursor_mode(server);
        }
    }
    
    // CRITICAL: Notify the client BEFORE scheduling frames
    // This ensures proper event ordering for GTK
    wlr_seat_pointer_notify_button(server->seat,
            event->time_msec, event->button, event->state);
    
    // For popup surfaces, we need to be extra careful about frame scheduling
    if (surface) {
        struct wlr_xdg_surface *xdg_surface = wlr_xdg_surface_try_from_wlr_surface(surface);
        if (xdg_surface && xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
            // For popups, schedule frame immediately to ensure responsiveness
            struct tinywl_output *output;
            wl_list_for_each(output, &server->outputs, link) {
                if (output->wlr_output && output->wlr_output->enabled) {
                    wlr_output_schedule_frame(output->wlr_output);
                }
            }
            
            // Also ensure the popup gets a commit event if needed
            if (xdg_surface->surface && !xdg_surface->surface->current.buffer) {
                struct timespec now;
                clock_gettime(CLOCK_MONOTONIC, &now);
                wlr_surface_send_frame_done(xdg_surface->surface, &now);
            }
        }
    }
    
    // Schedule frames for regular surfaces too, but after popup handling
    struct tinywl_output *output;
    wl_list_for_each(output, &server->outputs, link) {
        if (output->wlr_output && output->wlr_output->enabled) {
            wlr_output_schedule_frame(output->wlr_output);
        }
    }
}*/

static void server_cursor_button(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_button);
    struct wlr_pointer_button_event *event = data;

    // Log event for debugging
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    uint64_t now_ms = now.tv_sec * 1000 + now.tv_nsec / 1000000;
    wlr_log(WLR_DEBUG, "Button: button=%u, state=%s, event_time=%u, now=%lu, cursor_mode=%d",
            event->button, event->state == WL_POINTER_BUTTON_STATE_PRESSED ? "pressed" : "released",
            event->time_msec, now_ms, server->cursor_mode);

    // Find surface under cursor
    double sx, sy;
    struct wlr_surface *surface = NULL;
    struct tinywl_toplevel *toplevel = desktop_toplevel_at(server,
            server->cursor->x, server->cursor->y, &surface, &sx, &sy);

    // Update pointer focus
    if (surface) {
        wlr_seat_pointer_notify_enter(server->seat, surface, sx, sy);
        wlr_seat_pointer_notify_motion(server->seat, event->time_msec, sx, sy);
    } else {
        wlr_seat_pointer_clear_focus(server->seat);
    }

    // Notify button event to clients
    wlr_seat_pointer_notify_button(server->seat, event->time_msec, event->button, event->state);

    // Handle button press
    if (event->state == WL_POINTER_BUTTON_STATE_PRESSED) {
        if (toplevel && surface && toplevel->xdg_toplevel) {
            bool is_popup_surface = false;
            struct wlr_xdg_surface *xdg_surface = wlr_xdg_surface_try_from_wlr_surface(surface);
            if (xdg_surface && xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
                is_popup_surface = true;
            }
            if (!is_popup_surface && server->cursor_mode == TINYWL_CURSOR_PASSTHROUGH) {
                focus_toplevel(toplevel);

                // Initiate drag or resize based on keyboard modifiers
                bool alt_pressed = wlr_keyboard_get_modifiers(server->seat->keyboard_state.keyboard) & WLR_MODIFIER_ALT;
                bool ctrl_pressed = wlr_keyboard_get_modifiers(server->seat->keyboard_state.keyboard) & WLR_MODIFIER_CTRL;
                if (event->button == BTN_LEFT && alt_pressed) {
                    server->cursor_mode = TINYWL_CURSOR_MOVE;
                    server->grabbed_toplevel = toplevel;
                    server->grab_x = server->cursor->x;
                    server->grab_y = server->cursor->y;
                    // Initialize grab_geobox with cursor position and surface dimensions
                    int width = 800, height = 600; // Default size
                    if (surface && surface->current.width > 0 && surface->current.height > 0) {
                        width = surface->current.width;
                        height = surface->current.height;
                    }
                    server->grab_geobox = (struct wlr_box){
                        .x = (int)server->cursor->x,
                        .y = (int)server->cursor->y,
                        .width = width,
                        .height = height
                    };
                    wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "grab");
                    wlr_log(WLR_DEBUG, "Started window move: toplevel=%p, geobox=[%d,%d,%d,%d]",
                            toplevel, server->grab_geobox.x, server->grab_geobox.y,
                            server->grab_geobox.width, server->grab_geobox.height);
                } else if (event->button == BTN_LEFT && ctrl_pressed) {
                    server->cursor_mode = TINYWL_CURSOR_RESIZE;
                    server->grabbed_toplevel = toplevel;
                    server->grab_x = server->cursor->x;
                    server->grab_y = server->cursor->y;
                    // Initialize grab_geobox with cursor position and surface dimensions
                    int width = 800, height = 600; // Default size
                    if (surface && surface->current.width > 0 && surface->current.height > 0) {
                        width = surface->current.width;
                        height = surface->current.height;
                    }
                    server->grab_geobox = (struct wlr_box){
                        .x = (int)server->cursor->x,
                        .y = (int)server->cursor->y,
                        .width = width,
                        .height = height
                    };
                    server->resize_edges = WLR_EDGE_BOTTOM | WLR_EDGE_RIGHT;
                    wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "resize");
                    wlr_log(WLR_DEBUG, "Started window resize: toplevel=%p, geobox=[%d,%d,%d,%d], edges=%u",
                            toplevel, server->grab_geobox.x, server->grab_geobox.y,
                            server->grab_geobox.width, server->grab_geobox.height, server->resize_edges);
                }
            }
        }
    }
    // Handle button release
    else if (event->state == WL_POINTER_BUTTON_STATE_RELEASED) {
        // Reset cursor mode and grab state
        if (server->cursor_mode == TINYWL_CURSOR_MOVE || server->cursor_mode == TINYWL_CURSOR_RESIZE) {
            server->cursor_mode = TINYWL_CURSOR_PASSTHROUGH;
            server->grabbed_toplevel = NULL;
            server->grab_x = 0;
            server->grab_y = 0;
            server->grab_geobox = (struct wlr_box){0};
            server->resize_edges = 0;
            wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
            wlr_log(WLR_DEBUG, "Reset cursor mode to PASSTHROUGH on button release");
        }
    }

    // Handle popup surface frame updates
    if (surface) {
        struct wlr_xdg_surface *xdg_surface = wlr_xdg_surface_try_from_wlr_surface(surface);
        if (xdg_surface && xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
            struct tinywl_output *output;
            wl_list_for_each(output, &server->outputs, link) {
                if (output->wlr_output && output->wlr_output->enabled) {
                    wlr_output_schedule_frame(output->wlr_output);
                }
            }
            if (xdg_surface->surface && !xdg_surface->surface->current.buffer) {
                struct timespec now;
                clock_gettime(CLOCK_MONOTONIC, &now);
                wlr_surface_send_frame_done(xdg_surface->surface, &now);
            }
        }
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


static void popup_map(struct wl_listener *listener, void *data) {
    struct tinywl_popup *popup = wl_container_of(listener, popup, map);
    
    // Enable the popup in the scene
    wlr_scene_node_set_enabled(&popup->scene_tree->node, true);
    
    // Send the ack_configure immediately to complete the handshake
    if (popup->xdg_popup && popup->xdg_popup->base) {
        struct wlr_xdg_surface *xdg_surface = popup->xdg_popup->base;
        if (xdg_surface->configured) {
            // Send ack to complete popup configuration
            wlr_xdg_surface_schedule_configure(xdg_surface);
        }
    }
    
    // Schedule frames on all outputs to ensure popup is displayed
    struct tinywl_server *server = popup->server;
    if (server) {
        struct tinywl_output *output;
        wl_list_for_each(output, &server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
            }
        }
    }
    
    wlr_log(WLR_DEBUG, "Popup mapped and configured: %p", popup->xdg_popup);
}

static void popup_unmap(struct wl_listener *listener, void *data) {
    struct tinywl_popup *popup = wl_container_of(listener, popup, unmap);
    
    // Disable the popup in the scene
    wlr_scene_node_set_enabled(&popup->scene_tree->node, false);
    
    // Schedule frames to update display
    struct tinywl_server *server = popup->server;
    if (server) {
        struct tinywl_output *output;
        wl_list_for_each(output, &server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
            }
        }
    }
    
    wlr_log(WLR_DEBUG, "Popup unmapped: %p", popup->xdg_popup);
}

static void popup_destroy(struct wl_listener *listener, void *data) {
    struct tinywl_popup *popup = wl_container_of(listener, popup, destroy);
    
    wlr_log(WLR_DEBUG, "Popup destroyed: %p", popup->xdg_popup);
    
    // Clean up listeners
    wl_list_remove(&popup->map.link);
    wl_list_remove(&popup->unmap.link);
    wl_list_remove(&popup->destroy.link);
    wl_list_remove(&popup->commit.link);
    wl_list_remove(&popup->link);
    
    // Destroy scene node
    if (popup->scene_tree) {
        wlr_scene_node_destroy(&popup->scene_tree->node);
    }
    
    // Schedule frames to update display after popup destruction
    struct tinywl_server *server = popup->server;
    if (server) {
        struct tinywl_output *output;
        wl_list_for_each(output, &server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
            }
        }
    }
    
    free(popup);
}




static void popup_commit(struct wl_listener *listener, void *data) {
    struct tinywl_popup *popup = wl_container_of(listener, popup, commit);
    
    if (!popup->xdg_popup || !popup->xdg_popup->base) {
        return;
    }

    struct wlr_xdg_surface *xdg_surface = popup->xdg_popup->base;
    struct wlr_surface *surface = xdg_surface->surface;
    
    wlr_log(WLR_DEBUG, "Popup commit: %p (initial: %d, configured: %d, mapped: %d)", 
            popup->xdg_popup, xdg_surface->initial_commit, xdg_surface->configured, surface ? surface->mapped : 0);
    
    // Handle configuration properly with immediate response for GTK
    if (xdg_surface->initial_commit) {
        // For initial commit, configure immediately
        wlr_xdg_surface_schedule_configure(xdg_surface);
        wlr_log(WLR_DEBUG, "Scheduled initial configure for popup: %p", popup->xdg_popup);
    } else if (!xdg_surface->configured) {
        // Configure unconfigured popups immediately
        wlr_xdg_surface_schedule_configure(xdg_surface);
        wlr_log(WLR_DEBUG, "Scheduled configure for unconfigured popup: %p", popup->xdg_popup);
    }
    
    // CRITICAL: Always send frame_done for popup commits to prevent GTK freezing
    if (surface && surface->current.buffer) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        wlr_surface_send_frame_done(surface, &now);
    }
    
    // Schedule frames on all outputs
    struct tinywl_server *server = popup->server;
    if (server) {
        struct tinywl_output *output;
        wl_list_for_each(output, &server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
            }
        }
    }
    
    // If this is a commit after configure, the popup should be ready
    if (xdg_surface->configured && surface && surface->mapped) {
        wlr_log(WLR_DEBUG, "Popup ready and mapped: %p", popup->xdg_popup);
    }
}


// Ensure you have these includes (or equivalent for your project structure)
#include <time.h>
#include <wlr/types/wlr_compositor.h> // For wlr_surface_send_frame_done
#include <wlr/types/wlr_output_damage.h> // For wlr_output_damage_add_whole (optional but good)


#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/box.h>
#include <wlr/util/transform.h>
#include <wlr/util/log.h>
#include <wlr/render/wlr_renderer.h>



// Helper function to check if surface has been damaged/changed
static bool surface_needs_update(struct tinywl_toplevel *toplevel_view) {
    struct wlr_surface *surface = toplevel_view->xdg_toplevel->base->surface;
    
    // Check if surface has new commits since last render
    if (surface->current.seq != toplevel_view->last_commit_seq) {
        toplevel_view->last_commit_seq = surface->current.seq;
        return true;
    }
    
    // Check if we've marked it for redraw
    if (toplevel_view->needs_redraw) {
        return true;
    }
    
    // Check surface damage
    if (surface->current.committed & WLR_SURFACE_STATE_SURFACE_DAMAGE) {
        pixman_region32_t surface_damage;
        pixman_region32_init(&surface_damage);
        wlr_surface_get_effective_damage(surface, &surface_damage);
        bool has_damage = pixman_region32_not_empty(&surface_damage);
        pixman_region32_fini(&surface_damage);
        if (has_damage) {
            wlr_log(WLR_DEBUG, "Surface %p has damage, needs update", surface);
            return true;
        }
    }
    
    return false;
}

//with damage
static void output_frame(struct wl_listener *listener, void *data) {
    struct tinywl_output *output = wl_container_of(listener, output, frame);
    struct wlr_output *wlr_output = output->wlr_output;
    struct tinywl_server *server = output->server;
    struct wlr_scene *scene = server->scene;

    // Check if a frame is already pending
    if (wlr_output->frame_pending) {
        wlr_log(WLR_DEBUG, "[RENDER_FRAME] Frame pending for %s, skipping", wlr_output->name);
        return;
    }

    struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(scene, wlr_output);
    if (!scene_output) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] No scene output for %s", wlr_output->name);
        wlr_output_schedule_frame(wlr_output);
        return;
    }

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    // Validate output state
    if (!wlr_output || !wlr_output->enabled || !wlr_output->renderer || !wlr_output->allocator) {
        wlr_log(WLR_DEBUG, "[RENDER_FRAME] Output '%s' not ready", wlr_output->name);
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

    if (!wlr_renderer_is_gles2(wlr_output->renderer)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Renderer is not GLES2");
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

    if (!server->output_layout) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] No output layout available");
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

    struct wlr_box output_box;
    wlr_output_layout_get_box(server->output_layout, wlr_output, &output_box);
    if (output_box.width == 0 || output_box.height == 0) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Invalid output box for %s", wlr_output->name);
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

    // Initialize damage tracking
    struct pixman_region32 damage;
    pixman_region32_init(&damage);
    
    // Get damage from scene output - use simpler approach for compatibility
    bool needs_frame = false;
    
    // Check if scene output needs rendering
    if (scene_output && scene_output->damage_ring.current.data) {
        pixman_region32_copy(&damage, &scene_output->damage_ring.current);
        needs_frame = pixman_region32_not_empty(&damage);
    } else {
        // Fallback: assume full damage if we can't get specific damage info
        pixman_region32_init_rect(&damage, 0, 0, wlr_output->width, wlr_output->height);
        needs_frame = true;
    }

    // Add toplevel damage tracking
    struct pixman_region32 toplevel_damage;
    pixman_region32_init(&toplevel_damage);
    struct tinywl_toplevel *toplevel_view;
    bool toplevel_damaged = false;
    wl_list_for_each(toplevel_view, &server->toplevels, link) {
        if (!toplevel_view->xdg_toplevel || !toplevel_view->xdg_toplevel->base ||
            !toplevel_view->xdg_toplevel->base->surface->mapped) {
            continue;
        }
        struct wlr_surface *surface = toplevel_view->xdg_toplevel->base->surface;
        if (!wlr_surface_has_buffer(surface)) continue;

        pixman_region32_t surface_damage;
        pixman_region32_init(&surface_damage);
        if (surface->current.committed & WLR_SURFACE_STATE_SURFACE_DAMAGE) {
            wlr_surface_get_effective_damage(surface, &surface_damage);
            if (pixman_region32_not_empty(&surface_damage)) {
                double view_x = toplevel_view->scene_tree->node.x;
                double view_y = toplevel_view->scene_tree->node.y;
                pixman_region32_translate(&surface_damage, (int)view_x, (int)view_y);
                pixman_region32_union(&toplevel_damage, &toplevel_damage, &surface_damage);
                toplevel_damaged = true;
            }
        }
        pixman_region32_fini(&surface_damage);
    }
    if (toplevel_damaged) {
        pixman_region32_union(&damage, &damage, &toplevel_damage);
        needs_frame = true;
    }
    pixman_region32_fini(&toplevel_damage);

    // Add cursor damage
    static double last_cursor_x = -1, last_cursor_y = -1;
    bool cursor_moved = false;
    if (server->cursor && server->cursor_mgr && server->output_layout) {
    double cursor_x = fmax(0, fmin(server->cursor->x, output_box.width - 1));
    double cursor_y = fmax(0, fmin(server->cursor->y, output_box.height - 1));
    if (cursor_x != last_cursor_x || cursor_y != last_cursor_y) { // Process any movement
        cursor_moved = true;
        struct wlr_xcursor *xcursor = wlr_xcursor_manager_get_xcursor(server->cursor_mgr, "default", 1.0);
        if (xcursor && xcursor->images[0]) {
            struct wlr_xcursor_image *image = xcursor->images[0];
            pixman_region32_t cursor_damage;
            pixman_region32_init_rect(&cursor_damage,
                (int)last_cursor_x - image->hotspot_x,
                (int)last_cursor_y - image->hotspot_y,
                image->width, image->height);
            pixman_region32_union_rect(&cursor_damage, &cursor_damage,
                (int)cursor_x - image->hotspot_x,
                (int)cursor_y - image->hotspot_y,
                image->width, image->height);
            pixman_region32_union(&damage, &damage, &cursor_damage);
            pixman_region32_fini(&cursor_damage);
            wlr_log(WLR_DEBUG, "[RENDER_FRAME] Cursor moved: (%.2f,%.2f)", cursor_x, cursor_y);
            last_cursor_x = cursor_x;
            last_cursor_y = cursor_y;
        }
    }
}

    // Skip rendering if no damage or cursor movement
    if (!needs_frame && !cursor_moved) {
        wlr_log(WLR_DEBUG, "[RENDER_FRAME] No damage or cursor movement, skipping render");
        wlr_scene_output_send_frame_done(scene_output, &now);
        pixman_region32_fini(&damage);
        return;
    }

    // Begin rendering
    struct wlr_egl_context prev_ctx = {0};
    struct wlr_egl *egl = wlr_gles2_renderer_get_egl(wlr_output->renderer);
    if (!egl || !wlr_egl_make_current(egl, &prev_ctx)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Failed to make EGL context current");
        wlr_scene_output_send_frame_done(scene_output, &now);
        pixman_region32_fini(&damage);
        return;
    }

    struct wlr_output_state state;
    wlr_output_state_init(&state);

    // Prepare state
    wlr_output_state_set_enabled(&state, true);
    struct wlr_output_mode *current_wlr_mode = wlr_output->current_mode;
    if (current_wlr_mode) {
        wlr_output_state_set_mode(&state, current_wlr_mode);
    } else {
        wlr_log(WLR_ERROR, "[TINYWL_OUTPUT_FRAME] wlr_output->current_mode is NULL! Cannot set mode in output state for commit.");
    }
    wlr_output_state_set_damage(&state, &damage);

    struct wlr_buffer_pass_options pass_options = {
        .color_transform = NULL,
    };

    // Begin render pass
    struct wlr_render_pass *pass = wlr_output_begin_render_pass(wlr_output, &state, &pass_options);
    if (!pass) {
        wlr_log(WLR_ERROR, "[TINYWL_OUTPUT_FRAME] Failed to begin render pass");
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        wlr_scene_output_send_frame_done(scene_output, &now);
        pixman_region32_fini(&damage);
        return;
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] FBO incomplete");
        wlr_render_pass_submit(pass);
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state);
        wlr_scene_output_send_frame_done(scene_output, &now);
        pixman_region32_fini(&damage);
        return;
    }

    glViewport(0, 0, wlr_output->width, wlr_output->height);

    // Render background in damaged regions
    float background_color[] = {0.1f, 0.1f, 0.15f, 1.0f};
    int nrects;
    pixman_box32_t *rects = pixman_region32_rectangles(&damage, &nrects);
    for (int i = 0; i < nrects; i++) {
        struct wlr_render_rect_options background_opts = {
            .box = {
                .x = rects[i].x1,
                .y = rects[i].y1,
                .width = rects[i].x2 - rects[i].x1,
                .height = rects[i].y2 - rects[i].y1,
            },
            .color = {background_color[0], background_color[1], background_color[2], background_color[3]},
            .blend_mode = WLR_RENDER_BLEND_MODE_NONE,
        };
        wlr_log(WLR_DEBUG, "[RENDER_FRAME] Rendering background rect at (%d,%d) size=(%dx%d)",
                background_opts.box.x, background_opts.box.y,
                background_opts.box.width, background_opts.box.height);
        wlr_render_pass_add_rect(pass, &background_opts);
    }

    // Render toplevels
    int window_count = 0;
    struct tinywl_toplevel *toplevel_view_render; // Renamed to avoid redeclaration
    wl_list_for_each_reverse(toplevel_view_render, &server->toplevels, link) {
        if (!toplevel_view_render->xdg_toplevel || !toplevel_view_render->xdg_toplevel->base ||
            !toplevel_view_render->xdg_toplevel->base->surface->mapped) {
            continue;
        }
        
        struct wlr_xdg_surface *xdg_surface = toplevel_view_render->xdg_toplevel->base;
        struct wlr_surface *surface = xdg_surface->surface;
        
        if (!wlr_surface_has_buffer(surface)) {
            continue;
        }
        
        // Check if we need to update the cached texture
        bool needs_update = surface_needs_update(toplevel_view_render);
        
        if (needs_update || !toplevel_view_render->cached_texture) {
            // Get fresh texture and cache it
            toplevel_view_render->cached_texture = wlr_surface_get_texture(surface);
            toplevel_view_render->needs_redraw = false;
            
            wlr_log(WLR_DEBUG, "[RENDER_FRAME] Updated cached texture for toplevel");
        }
        
        if (!toplevel_view_render->cached_texture) {
            continue;
        }
        
        // Only render if this window or its region has damage
        bool has_damage = surface->current.committed & WLR_SURFACE_STATE_BUFFER;
        
        // For now, we'll render if there's any damage or if it's the first frame
        if (!has_damage && toplevel_view_render->last_commit_seq != 0) {
            // Skip rendering this window - no changes
            continue;
        }
        
        window_count++;
        double view_x = toplevel_view_render->scene_tree->node.x;
        double view_y = toplevel_view_render->scene_tree->node.y;
        int width = surface->current.width;
        int height = surface->current.height;
        
        struct wlr_fbox src_box = { .x = 0, .y = 0, .width = width, .height = height };
        struct wlr_box dst_box = {
            .x = (int)view_x,
            .y = wlr_output->height - (int)view_y - height,
            .width = width,
            .height = height
        };
        
        struct wlr_render_texture_options tex_opts = {
            .texture = toplevel_view_render->cached_texture,
            .src_box = src_box,
            .dst_box = dst_box,
            .blend_mode = WLR_RENDER_BLEND_MODE_PREMULTIPLIED,
            .transform = WL_OUTPUT_TRANSFORM_FLIPPED_180,
        };
        
        wlr_log(WLR_DEBUG, "[RENDER_FRAME] Rendering toplevel %d at (%d,%d) size=(%dx%d)",
                window_count, dst_box.x, dst_box.y, dst_box.width, dst_box.height);
        
        wlr_render_pass_add_texture(pass, &tex_opts);
    }


    // Render popups
    int popup_count = 0;
    struct tinywl_popup *popup_view;
    wl_list_for_each(popup_view, &server->popups, link) {
        if (!popup_view->xdg_popup || !popup_view->xdg_popup->base ||
            !popup_view->xdg_popup->base->surface->mapped || !popup_view->scene_tree ||
            !popup_view->scene_tree->node.enabled) {
            continue;
        }

        struct wlr_surface *surface = popup_view->xdg_popup->base->surface;
        if (!wlr_surface_has_buffer(surface)) {
            continue;
        }

        struct wlr_texture *texture = wlr_surface_get_texture(surface);
        if (!texture) {
            continue;
        }

        popup_count++;

        int popup_abs_lx, popup_abs_ly;
        wlr_scene_node_coords(&popup_view->scene_tree->node, &popup_abs_lx, &popup_abs_ly);

        int width = surface->current.width;
        int height = surface->current.height;

        struct wlr_fbox src_box = { .x = 0, .y = 0, .width = width, .height = height };
        struct wlr_box dst_box = {
            .x = popup_abs_lx - output_box.x,
            .y = wlr_output->height - (popup_abs_ly - output_box.y) - height,
            .width = width,
            .height = height
        };

        struct wlr_render_texture_options tex_opts = {
            .texture = texture,
            .src_box = src_box,
            .dst_box = dst_box,
            .blend_mode = WLR_RENDER_BLEND_MODE_PREMULTIPLIED,
            .transform = WL_OUTPUT_TRANSFORM_FLIPPED_180,
        };
        wlr_log(WLR_DEBUG, "[RENDER_FRAME] Rendering popup %d at (%d,%d) size=(%dx%d)",
                popup_count, dst_box.x, dst_box.y, dst_box.width, dst_box.height);
        wlr_render_pass_add_texture(pass, &tex_opts);
    }

    // Render cursor
    if (server->cursor && server->cursor_mgr && server->output_layout) {
        struct wlr_xcursor *xcursor = wlr_xcursor_manager_get_xcursor(server->cursor_mgr, "default", 1.0);
        if (xcursor && xcursor->images[0]) {
            struct wlr_xcursor_image *image = xcursor->images[0];
            static struct wlr_texture *cached_cursor_texture = NULL;
            static uint32_t cached_cursor_width = 0, cached_cursor_height = 0;

            if (!cached_cursor_texture ||
                cached_cursor_width != image->width ||
                cached_cursor_height != image->height) {
                if (cached_cursor_texture) {
                    wlr_texture_destroy(cached_cursor_texture);
                }
                cached_cursor_texture = wlr_texture_from_pixels(
                    server->renderer, DRM_FORMAT_ARGB8888, image->width * 4,
                    image->width, image->height, image->buffer
                );
                cached_cursor_width = image->width;
                cached_cursor_height = image->height;
            }

            if (cached_cursor_texture) {
                double cursor_x = fmax(0, fmin(server->cursor->x, output_box.width - 1));
                double cursor_y = fmax(0, fmin(server->cursor->y, output_box.height - 1));
                struct wlr_box cursor_dst_box = {
                    .x = (int)cursor_x - image->hotspot_x,
                    .y = wlr_output->height - ((int)cursor_y - image->hotspot_y) - (int)image->height,
                    .width = (int)image->width,
                    .height = (int)image->height
                };

                struct wlr_fbox cursor_src_box = { .x = 0, .y = 0, .width = image->width, .height = image->height };
                struct wlr_render_texture_options cursor_tex_opts = {
                    .texture = cached_cursor_texture,
                    .src_box = cursor_src_box,
                    .dst_box = cursor_dst_box,
                    .blend_mode = WLR_RENDER_BLEND_MODE_PREMULTIPLIED,
                    .transform = WL_OUTPUT_TRANSFORM_FLIPPED_180,
                };
                wlr_log(WLR_DEBUG, "[RENDER_FRAME] Rendering cursor at (%d,%d) size=(%dx%d)",
                        cursor_dst_box.x, cursor_dst_box.y, cursor_dst_box.width, cursor_dst_box.height);
                wlr_render_pass_add_texture(pass, &cursor_tex_opts);
            }
        }
    }

       // Submit render pass
    if (!wlr_render_pass_submit(pass)) {
        wlr_log(WLR_ERROR, "[RENDER_FRAME] Failed to submit render pass");
        wlr_egl_restore_context(&prev_ctx);
        wlr_output_state_finish(&state); // state is from wlr_output_begin_render_pass
        wlr_scene_output_send_frame_done(scene_output, &now);
        pixman_region32_fini(&damage);
        return;
    }
    // At this point, rendering is done. The 'state' variable (used with wlr_output_begin_render_pass)
    // now contains state.buffer which is the rendered frame.
    // We also need to ensure 'state' has other necessary fields set for a valid commit.

    // --- ENSURE 'state' IS FULLY PREPARED FOR COMMIT ---
    // The 'enabled' and 'mode' should have been set by wlr_output_begin_render_pass
    // if it was going to change them, or it uses the output's current.
    // However, it's safer to explicitly set what we intend to commit.
    wlr_output_state_set_enabled(&state, true);
 //   struct wlr_output_mode *current_wlr_mode = wlr_output->current_mode;
    if (current_wlr_mode) {
        wlr_output_state_set_mode(&state, current_wlr_mode);
    } else {
        wlr_log(WLR_ERROR, "[TINYWL_OUTPUT_FRAME] CRITICAL: wlr_output->current_mode is NULL before commit for %s.", wlr_output->name);
        // This is a problem, the output needs a mode to be valid.
    }
    // 'damage' is the pixman_region32_t you calculated. Set it into 'state.damage'.
    wlr_output_state_set_damage(&state, &damage);
    // --- End of ensuring 'state' is prepared ---


    // --- LOGGING BEFORE COMMIT (using the 'state' variable from begin_render_pass) ---
    struct timespec _pre_commit_ts, _post_commit_ts;
    wlr_log(WLR_ERROR, "--- TINYWL_OUTPUT_FRAME: Inspecting 'state' (from begin_render_pass) BEFORE commit for %s ---",
            wlr_output->name ? wlr_output->name : "(null)");
    wlr_log(WLR_ERROR, "  state.committed: 0x%X", state.committed);
    wlr_log(WLR_ERROR, "  state.enabled: %d", state.enabled);
    if (state.committed & WLR_OUTPUT_STATE_MODE) {
        if (state.mode) {
            wlr_log(WLR_ERROR, "  state.mode: %p (%dx%d @ %dHz)",
                    (void*)state.mode, state.mode->width, state.mode->height, state.mode->refresh);
        } else { wlr_log(WLR_ERROR, "  state.mode: NULL (but MODE bit is set!)"); }
    } else { wlr_log(WLR_ERROR, "  WLR_OUTPUT_STATE_MODE is NOT SET in committed flags."); }

    if (state.committed & WLR_OUTPUT_STATE_BUFFER) {
        if (state.buffer) {
            wlr_log(WLR_ERROR, "  state.buffer: %p (w:%d, h:%d)",
                    (void*)state.buffer, state.buffer->width, state.buffer->height);
        } else { wlr_log(WLR_ERROR, "  state.buffer: NULL (but BUFFER bit is set!)"); }
    } else { wlr_log(WLR_ERROR, "  WLR_OUTPUT_STATE_BUFFER is NOT SET in committed flags."); }

    if (state.committed & WLR_OUTPUT_STATE_DAMAGE) {
        wlr_log(WLR_ERROR, "  WLR_OUTPUT_STATE_DAMAGE is SET in committed flags. Damage empty: %d",
                !pixman_region32_not_empty(&state.damage));
    } else { wlr_log(WLR_ERROR, "  WLR_OUTPUT_STATE_DAMAGE is NOT SET in committed flags."); }
    // --- END OF INSPECTION LOG ---

    clock_gettime(CLOCK_MONOTONIC, &_pre_commit_ts);
    wlr_log(WLR_ERROR, "TINYWL_OUTPUT_FRAME: PRE wlr_output_commit_state() for %s. Timestamp: %ld.%09ld",
            wlr_output->name ? wlr_output->name : "(null)", _pre_commit_ts.tv_sec, _pre_commit_ts.tv_nsec);

   // bool committed = wlr_output_commit_state(wlr_output, &state); // COMMIT THE 'state' VARIABLE

    clock_gettime(CLOCK_MONOTONIC, &_post_commit_ts);
  //  long commit_ns_diff = (_post_commit_ts.tv_sec - _pre_commit_ts.tv_sec) * 1000000000L + (_post_commit_ts.tv_nsec - _pre_commit_ts.tv_nsec);
   // wlr_log(WLR_ERROR, "TINYWL_OUTPUT_FRAME: POST wlr_output_commit_state() for %s. Committed: %d. Call took %ld ns. Timestamp: %ld.%09ld",
     //       wlr_output->name ? wlr_output->name : "(null)",
       //     committed, commit_ns_diff, _post_commit_ts.tv_sec, _post_commit_ts.tv_nsec);
    // --- END OF CRUCIAL PART ---

    // RDP transmission (only for damaged regions)
    // This should now use 'state.buffer' which was committed.
    struct wlr_buffer *rendered_buffer = state.buffer;
    // ... (rest of your RDP transmission logic) ...
 //   if (rendered_buffer && (needs_frame || cursor_moved)) {
        struct wlr_buffer *locked_buffer = wlr_buffer_lock(rendered_buffer);
        if (locked_buffer) {
            void *bdata;
            uint32_t bfmt;
            size_t bstride;
            if (wlr_buffer_begin_data_ptr_access(locked_buffer,
                WLR_BUFFER_DATA_PTR_ACCESS_READ, &bdata, &bfmt, &bstride)) {
               
                wlr_buffer_end_data_ptr_access(locked_buffer);
                rdp_transmit_surface(locked_buffer);
            } else {
                rdp_transmit_surface(locked_buffer);
            }
            wlr_buffer_unlock(locked_buffer);
        }
  //  }
//
    // Cleanup
    wlr_egl_restore_context(&prev_ctx);
    wlr_output_state_finish(&state);
    wlr_scene_output_send_frame_done(scene_output, &now);
    pixman_region32_fini(&damage);
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
    bool is_gtk = toplevel->xdg_toplevel->app_id && strstr(toplevel->xdg_toplevel->app_id, "gtk");
    
    wlr_log(WLR_DEBUG, "xdg_toplevel_commit: toplevel=%p, title='%s', has_buffer=%d, mapped=%d, gtk=%s",
            toplevel,
            toplevel->xdg_toplevel->title ? toplevel->xdg_toplevel->title : "(null)",
            wlr_surface_has_buffer(surface),
            surface->mapped,
            is_gtk ? "yes" : "no");

    if (toplevel->xdg_toplevel->base->initial_commit) {
        wlr_xdg_toplevel_set_size(toplevel->xdg_toplevel, 0, 0);
    }

    if (surface && surface->mapped && wlr_surface_has_buffer(surface)) {
        // Use surface dimensions as a fallback geometry
        int width = surface->current.width;
        int height = surface->current.height;
        
        struct wlr_output *output = wlr_output_layout_output_at(
            toplevel->server->output_layout,
            toplevel->scene_tree->node.x + width / 2,
            toplevel->scene_tree->node.y + height / 2);
        
        if (output && output->enabled) {
            wlr_output_schedule_frame(output);
            wlr_log(WLR_DEBUG, "Scheduled frame for output %s on surface commit (GTK=%s)",
                    output->name, is_gtk ? "yes" : "no");
            if (is_gtk) {
                wl_display_flush_clients(toplevel->server->wl_display);
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
    wlr_log(WLR_INFO, "New XDG toplevel: %p, title='%s'", (void*)xdg_toplevel, xdg_toplevel->title);

    struct tinywl_toplevel *toplevel = calloc(1, sizeof(*toplevel));
    if (!toplevel) {
        wlr_log(WLR_ERROR, "Failed to allocate tinywl_toplevel");
        return;
    }
    toplevel->server = server;
    toplevel->xdg_toplevel = xdg_toplevel;

    // 1. Create the main frame scene tree for this toplevel.
    // This tree's node.data will point to our 'toplevel' (view) struct.
    toplevel->scene_tree = wlr_scene_tree_create(&server->scene->tree);
    if (!toplevel->scene_tree) {
        wlr_log(WLR_ERROR, "Failed to create scene_tree (frame) for toplevel");
        free(toplevel);
        return;
    }
    toplevel->scene_tree->node.data = toplevel; // Associate frame with our struct
    xdg_toplevel->base->data = toplevel; // Also associate xdg_surface with our struct for easier lookup

    // 2. Create the scene tree for the client's actual XDG surface (content area).
    // This will be a child of the main frame tree.
    toplevel->client_xdg_scene_tree = wlr_scene_xdg_surface_create(toplevel->scene_tree, xdg_toplevel->base);
    if (!toplevel->client_xdg_scene_tree) {
        wlr_log(WLR_ERROR, "Failed to create client_xdg_scene_tree for toplevel");
        wlr_scene_node_destroy(&toplevel->scene_tree->node); // Clean up frame tree
        free(toplevel);
        return;
    }
    // xdg_toplevel->base->data already points to 'toplevel'.
    // The wlr_scene_xdg_surface_create function handles subsurfaces within this client_xdg_scene_tree.

    // --- POSITION THE CLIENT CONTENT AREA AND ADD DECORATIONS ---
    // This is a crucial part. For simplicity, let's assume a fixed title bar height
    // and border size. In a real compositor, this would be configurable and dynamic.
//    const int titlebar_height = 0; // Example
  //  const int border_size = 0;      // Example

    // Position the client content tree within the frame tree
    wlr_scene_node_set_position(&toplevel->client_xdg_scene_tree->node, 0, 0);

    // TODO: If you are drawing server-side decorations:
    // Create wlr_scene_buffer nodes for your title bar and borders here.
    // Add them as children of `toplevel->scene_tree`.
    // Position them appropriately.
    // For example:
    // struct wlr_buffer *titlebar_buf = create_my_titlebar_buffer(width, titlebar_height);
    // struct wlr_scene_buffer *titlebar_node = wlr_scene_buffer_create(toplevel->scene_tree, titlebar_buf);
    // wlr_scene_node_set_position(&titlebar_node->node, border_size, border_size); // Or 0,0 if borders are outside
    // wlr_buffer_unlock(titlebar_buf); // if locked after creation
    // ... and so on for other decoration parts.

    // For now, without actual decoration rendering, the client content will just be offset.
    // The picking logic needs to account for this structure.

    // Register listeners
    toplevel->map.notify = xdg_toplevel_map;
    wl_signal_add(&xdg_toplevel->base->surface->events.map, &toplevel->map);
    toplevel->unmap.notify = xdg_toplevel_unmap;
    wl_signal_add(&xdg_toplevel->base->surface->events.unmap, &toplevel->unmap);
    toplevel->commit.notify = xdg_toplevel_commit;
    wl_signal_add(&xdg_toplevel->base->surface->events.commit, &toplevel->commit);
    toplevel->destroy.notify = xdg_toplevel_destroy;
    wl_signal_add(&xdg_toplevel->events.destroy, &toplevel->destroy); // Listen on xdg_toplevel, not base surface for this
    toplevel->request_move.notify = xdg_toplevel_request_move;
    wl_signal_add(&xdg_toplevel->events.request_move, &toplevel->request_move);
    toplevel->request_resize.notify = xdg_toplevel_request_resize;
    wl_signal_add(&xdg_toplevel->events.request_resize, &toplevel->request_resize);
    toplevel->request_maximize.notify = xdg_toplevel_request_maximize;
    wl_signal_add(&xdg_toplevel->events.request_maximize, &toplevel->request_maximize);
    toplevel->request_fullscreen.notify = xdg_toplevel_request_fullscreen;
    wl_signal_add(&xdg_toplevel->events.request_fullscreen, &toplevel->request_fullscreen);

    wlr_log(WLR_INFO, "Toplevel %p initialized (frame tree %p, client content tree %p)",
            (void*)toplevel, (void*)toplevel->scene_tree, (void*)toplevel->client_xdg_scene_tree);
}
/*
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
*/
/*
static void server_new_xdg_popup(struct wl_listener *listener, void *data) {
    if (!listener || !data) {
        wlr_log(WLR_ERROR, "Invalid listener=%p or data=%p in server_new_xdg_popup", listener, data);
        return;
    }

    struct tinywl_server *server = wl_container_of(listener, server, new_xdg_popup);
    if (!server) {
        wlr_log(WLR_ERROR, "Failed to get server from listener in server_new_xdg_popup");
        return;
    }

    wlr_log(WLR_DEBUG, "server_new_xdg_popup: server=%p, wl_display=%p, xdg_shell=%p, scene=%p",
            server, server->wl_display, server->xdg_shell, server->scene);

    // Check server state
    if (!server->wl_display || !server->xdg_shell || !server->scene) {
        wlr_log(WLR_ERROR, "Server has invalid state: wl_display=%p, xdg_shell=%p, scene=%p",
                server->wl_display, server->xdg_shell, server->scene);
        return;
    }

    struct wlr_xdg_popup *xdg_popup = data;
    wlr_log(WLR_INFO, "New XDG popup detected: %p, parent=%p", xdg_popup, xdg_popup->parent);

    // Allocate popup structure
    struct tinywl_popup *popup = calloc(1, sizeof(struct tinywl_popup));
    if (!popup) {
        wlr_log(WLR_ERROR, "Failed to allocate tinywl_popup");
        return;
    }

    // Initialize popup fields
    popup->xdg_popup = xdg_popup;
    popup->server = server;

    // Find the parent scene tree
    struct wlr_xdg_surface *parent = wlr_xdg_surface_try_from_wlr_surface(xdg_popup->parent);
    if (!parent) {
        wlr_log(WLR_ERROR, "No valid parent XDG surface for popup");
        free(popup);
        return;
    }
    struct wlr_scene_tree *parent_tree = parent->data;
    if (!parent_tree) {
        wlr_log(WLR_ERROR, "Parent surface has no scene tree");
        free(popup);
        return;
    }

    // Create scene tree for the popup
    popup->scene_tree = wlr_scene_xdg_surface_create(parent_tree, xdg_popup->base);
    if (!popup->scene_tree) {
        wlr_log(WLR_ERROR, "Failed to create scene tree for popup");
        free(popup);
        return;
    }
    popup->scene_tree->node.data = popup;
    xdg_popup->base->data = popup->scene_tree;

    // Set up event listeners
    popup->map.notify = popup_map;
    wl_signal_add(&xdg_popup->base->surface->events.map, &popup->map);
    popup->unmap.notify = popup_unmap;
    wl_signal_add(&xdg_popup->base->surface->events.unmap, &popup->unmap);
    popup->destroy.notify = popup_destroy;
    wl_signal_add(&xdg_popup->events.destroy, &popup->destroy);
    popup->commit.notify = popup_commit;
    wl_signal_add(&xdg_popup->base->surface->events.commit, &popup->commit);

    // Add to server's popup list
    wl_list_insert(&server->popups, &popup->link);
    wlr_log(WLR_DEBUG, "Popup %p created, parent surface=%p, scene_tree=%p",
            xdg_popup, parent, popup->scene_tree);

    // Constrain popup geometry
    if (xdg_popup->base->initial_commit) {
        wlr_xdg_surface_schedule_configure(xdg_popup->base);
    }

    // Schedule a frame to ensure the popup is rendered
    struct tinywl_output *output;
    wl_list_for_each(output, &server->outputs, link) {
        if (output->wlr_output && output->wlr_output->enabled) {
            wlr_output_schedule_frame(output->wlr_output);
            wlr_log(WLR_DEBUG, "Scheduled frame for output %s on popup creation", output->wlr_output->name);
        }
    }
}*/

static void server_new_xdg_popup(struct wl_listener *listener, void *data) {
    if (!listener || !data) {
        wlr_log(WLR_ERROR, "Invalid listener=%p or data=%p in server_new_xdg_popup", listener, data);
        return;
    }

    struct tinywl_server *server = wl_container_of(listener, server, new_xdg_popup);
    if (!server || !server->wl_display || !server->xdg_shell || !server->scene) {
        wlr_log(WLR_ERROR, "Invalid server state: server=%p, display=%p, xdg_shell=%p, scene=%p",
                server, server ? server->wl_display : NULL, server ? server->xdg_shell : NULL, server ? server->scene : NULL);
        return;
    }

    struct wlr_xdg_popup *xdg_popup = data;
    wlr_log(WLR_INFO, "New XDG popup: %p, parent=%p, surface=%p, initial_commit=%d",
            xdg_popup, xdg_popup->parent, xdg_popup->base ? xdg_popup->base->surface : NULL,
            xdg_popup->base ? xdg_popup->base->initial_commit : 0);

    // Validate popup surface and role
    if (!xdg_popup->base || !xdg_popup->base->surface || xdg_popup->base->role != WLR_XDG_SURFACE_ROLE_POPUP) {
        wlr_log(WLR_ERROR, "Invalid popup: base=%p, surface=%p, role=%d",
                xdg_popup->base, xdg_popup->base ? xdg_popup->base->surface : NULL,
                xdg_popup->base ? xdg_popup->base->role : -1);
        return;
    }

    // Validate parent surface
    struct wlr_xdg_surface *parent = wlr_xdg_surface_try_from_wlr_surface(xdg_popup->parent);
    if (!parent || !parent->surface) {
        wlr_log(WLR_ERROR, "No valid parent XDG surface: parent=%p, parent->surface=%p",
                parent, parent ? parent->surface : NULL);
        return;
    }

    // Ensure parent is a toplevel (popups cannot have popup parents)
    if (parent->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
        wlr_log(WLR_ERROR, "Parent surface is not a toplevel: role=%d", parent->role);
        return;
    }

    // Validate parent scene tree
    struct wlr_scene_tree *parent_tree = parent->data;
    if (!parent_tree || parent_tree->node.type != WLR_SCENE_NODE_TREE) {
        wlr_log(WLR_ERROR, "Invalid parent scene tree: parent_tree=%p, type=%d",
                parent_tree, parent_tree ? parent_tree->node.type : -1);
        return;
    }

    // Check parent surface state
    if (!parent->surface->mapped || !wlr_surface_has_buffer(parent->surface)) {
        wlr_log(WLR_ERROR, "Parent surface not ready: mapped=%d, has_buffer=%d",
                parent->surface->mapped, wlr_surface_has_buffer(parent->surface));
        return; // Defer until parent is ready
    }

    // Allocate popup structure
    struct tinywl_popup *popup = calloc(1, sizeof(struct tinywl_popup));
    if (!popup) {
        wlr_log(WLR_ERROR, "Failed to allocate tinywl_popup");
        return;
    }

    popup->xdg_popup = xdg_popup;
    popup->server = server;

    // Create scene tree for popup
    popup->scene_tree = wlr_scene_xdg_surface_create(parent_tree, xdg_popup->base);
    if (!popup->scene_tree) {
        wlr_log(WLR_ERROR, "Failed to create scene tree for popup: xdg_surface=%p", xdg_popup->base);
        free(popup);
        return;
    }

    // Set data pointers
    popup->scene_tree->node.data = popup;
    xdg_popup->base->data = popup; // Store popup for consistency with toplevels

    // Set up event listeners
    popup->map.notify = popup_map;
    wl_signal_add(&xdg_popup->base->surface->events.map, &popup->map);
    popup->unmap.notify = popup_unmap;
    wl_signal_add(&xdg_popup->base->surface->events.unmap, &popup->unmap);
    popup->destroy.notify = popup_destroy;
    wl_signal_add(&xdg_popup->events.destroy, &popup->destroy);
    popup->commit.notify = popup_commit;
    wl_signal_add(&xdg_popup->base->surface->events.commit, &popup->commit);

    // Add to server's popup list
    wl_list_insert(&server->popups, &popup->link);
    wlr_log(WLR_DEBUG, "Popup created: %p, parent=%p, scene_tree=%p, xdg_surface=%p",
            xdg_popup, parent->surface, popup->scene_tree, xdg_popup->base);

    // Schedule configure if needed
    if (xdg_popup->base->initial_commit) {
        wlr_xdg_surface_schedule_configure(xdg_popup->base);
        wlr_log(WLR_DEBUG, "Scheduled configure for popup %p", xdg_popup);
    }

    // Schedule frame
    struct tinywl_output *output;
    wl_list_for_each(output, &server->outputs, link) {
        if (output->wlr_output && output->wlr_output->enabled) {
            wlr_output_schedule_frame(output->wlr_output);
        }
    }
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
    wl_list_init(&server.popups);

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