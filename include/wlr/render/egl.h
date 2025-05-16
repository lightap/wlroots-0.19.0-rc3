
#ifndef WLR_USE_UNSTABLE
#error "Add -DWLR_USE_UNSTABLE to enable unstable wlroots features"
#endif

#ifndef WLR_RENDER_EGL_H
#define WLR_RENDER_EGL_H

#ifndef EGL_NO_RDP
#define EGL_NO_RDP
#endif
#ifndef EGL_NO_PLATFORM_SPECIFIC_TYPES
#define EGL_NO_PLATFORM_SPECIFIC_TYPES
#endif

#include <wlr/config.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <pixman.h>
#include <stdbool.h>
#include <wayland-server-core.h>
#include <wlr/render/dmabuf.h>
#include <wlr/render/drm_format_set.h>

struct wlr_egl;



/**
 * Create a struct wlr_egl with an existing EGL display and context.
 *
 * This is typically used by compositors which want to customize EGL
 * initialization.
 */
struct wlr_egl *wlr_egl_create_with_context(EGLDisplay display,
	EGLContext context);

/**
 * Get the EGL display used by the struct wlr_egl.
 *
 * This is typically used by compositors which need to perform custom OpenGL
 * operations.
 */
EGLDisplay wlr_egl_get_display(struct wlr_egl *egl);

/**
 * Get the EGL context used by the struct wlr_egl.
 *
 * This is typically used by compositors which need to perform custom OpenGL
 * operations.
 */
EGLContext wlr_egl_get_context(struct wlr_egl *egl);


/**
 * Create a struct wlr_egl for a surfaceless renderer.
 *
 * This is used for GPU-accelerated rendering without a display device, e.g., with FreeRDP.
 */
struct wlr_egl *wlr_egl_create_surfaceless(void);

//struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display, struct wlr_egl *egl);

struct wlr_egl *wlr_egl_create(EGLint platform, void *remote_display, EGLint *config_attribs, EGLint visual_id);
void wlr_egl_free(struct wlr_egl *egl);
struct wlr_renderer *wlr_gles2_renderer_create(struct wlr_egl *egl);
struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display);

#endif
