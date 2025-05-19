#ifndef WLR_BACKEND_RDP_H
#define WLR_BACKEND_RDP_H

#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/interfaces/wlr_output.h>
#include <wlr/util/log.h>
#include <wlr/render/drm_format_set.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/render/allocator.h>
#include <freerdp/freerdp.h>
#include <freerdp/listener.h>
#include <time.h>
#include <wlr/render/egl.h>

/* Forward declarations */
struct wlr_RDP_output;
struct wlr_buffer;
struct wlr_RDP_backend;

/**
 * The RDP backend structure, storing global backend state and
 * references to all RDP outputs and peers.
 */
struct wlr_RDP_backend {
    struct wlr_backend backend;
    struct wl_display *display;
    struct wl_event_loop *event_loop;
    bool started;
    struct wl_list outputs;
    struct wl_list peers;
    struct wlr_renderer *renderer;
    struct wlr_allocator *allocator;
    freerdp_listener *listener;
    struct wl_event_source *fd_event_sources[32];
    int fd_count;
      struct wlr_egl *egl; 
    struct wlr_egl *egl_instance;       
};

/**
 * Our per-output structure: one RDP "virtual monitor."
 */
struct wlr_RDP_output {
    struct wlr_output wlr_output;
    struct wlr_RDP_backend *backend;
    struct wl_list link;

    struct wlr_buffer *pending_buffer;  // Currently acquired buffer
    struct wl_listener buffer_release;  // Listener for buffer release events
    struct wl_event_source *frame_timer; // Timer for frame scheduling

};

/**
 * Entry point: create an RDP backend
 */
struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display);

/**
 * Create an RDP output
 */
struct wlr_RDP_output *wlr_RDP_output_create(
    struct wlr_RDP_backend *backend, const char *name,
    int width, int height, int refresh_hz);

/**
 * Checks if the given wlr_backend is an RDP backend
 */
bool wlr_backend_is_RDP(struct wlr_backend *backend);

/**
 * Cast a wlr_backend to wlr_RDP_backend
 */
struct wlr_RDP_backend *RDP_backend_from_backend(struct wlr_backend *backend);

/**
 * RDP peer management
 */
void set_global_rdp_peer(freerdp_peer *peer);
freerdp_peer* get_global_rdp_peer(void);



//freerdp_peer *global_rdp_peer ;
/**
 * Surface transmission
 */
void rdp_transmit_surface(struct wlr_buffer *buffer);

#endif