// tinywl.h
#ifndef TINYWL_H
#define TINYWL_H

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

#include <render/allocator/RDP_allocator.h>
#include <wlr/backend/RDP.h>
#include <wlr/render/swapchain.h>



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
    struct wl_listener output_frame;
      EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
   EGLSurface egl_surface;
   struct wl_event_source *frame_timer; // Add timer for delayed reschedule
 struct wlr_egl *egl; // For EGL context
 
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


#endif // TINYWL_H