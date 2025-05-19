#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <wlr/backend.h>
#include <wlr/render/interface.h>
#include <wlr/render/pixman.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_drm.h>
#include <wlr/types/wlr_linux_dmabuf_v1.h>
#include <wlr/types/wlr_shm.h>
#include <wlr/util/box.h>
#include <wlr/util/log.h>
#include <xf86drm.h>

#include <wlr/config.h>

#include "render/gles2.h"

#if WLR_HAS_GLES2_RENDERER
#include <wlr/render/egl.h>
#include <wlr/render/gles2.h>
#endif

#if WLR_HAS_VULKAN_RENDERER
#include <wlr/render/vulkan.h>
#endif // WLR_HAS_VULKAN_RENDERER

#include "render/wlr_renderer.h"
#include "util/env.h"
#include "backend/backend.h"
#include <drm_fourcc.h>
#include <stdio.h>



void wlr_renderer_init(struct wlr_renderer *renderer,
		const struct wlr_renderer_impl *impl, uint32_t render_buffer_caps) {
	assert(impl->begin_buffer_pass);
	assert(impl->get_texture_formats);
	assert(render_buffer_caps != 0);

	*renderer = (struct wlr_renderer){
		.impl = impl,
		.render_buffer_caps = render_buffer_caps,
	};

	wl_signal_init(&renderer->events.destroy);
	wl_signal_init(&renderer->events.lost);
}

void wlr_renderer_destroy(struct wlr_renderer *r) {
	if (!r) {
		return;
	}

	wl_signal_emit_mutable(&r->events.destroy, r);

	assert(wl_list_empty(&r->events.destroy.listener_list));
	assert(wl_list_empty(&r->events.lost.listener_list));

	if (r->impl && r->impl->destroy) {
		r->impl->destroy(r);
	} else {
		free(r);
	}
}

const struct wlr_drm_format_set *wlr_renderer_get_texture_formats(
		struct wlr_renderer *r, uint32_t buffer_caps) {
	return r->impl->get_texture_formats(r, buffer_caps);
}

const struct wlr_drm_format_set *wlr_renderer_get_render_formats(
		struct wlr_renderer *r) {
	if (!r->impl->get_render_formats) {
		return NULL;
	}
	return r->impl->get_render_formats(r);
}

bool wlr_renderer_init_wl_shm(struct wlr_renderer *r,
        struct wl_display *wl_display) {
    // Define required formats directly
    static const uint32_t formats[] = {
        DRM_FORMAT_ARGB8888,
        DRM_FORMAT_XRGB8888,
    };
    
    // Create SHM with just these two formats
    struct wlr_shm *shm = wlr_shm_create(wl_display, 2, formats, 2);
    
    return shm != NULL;
}

bool wlr_renderer_init_wl_display(struct wlr_renderer *r,
		struct wl_display *wl_display) {
	if (!wlr_renderer_init_wl_shm(r, wl_display)) {
		return false;
	}
	
	const struct wlr_drm_format_set *formats = 
		wlr_renderer_get_texture_formats(r, WLR_BUFFER_CAP_DMABUF);
	
	if (formats != NULL) {
		int drm_fd = wlr_renderer_get_drm_fd(r);
		if (drm_fd >= 0) {
			if (wlr_drm_create(wl_display, r) == NULL) {
				return false;
			}
		} else {
			wlr_log(WLR_INFO, "Cannot get renderer DRM FD, disabling wl_drm");
		}
		
		if (wlr_linux_dmabuf_v1_create_with_renderer(wl_display, 4, r) == NULL) {
			return false;
		}
	}
	return true;
}



struct wlr_renderer *renderer_autocreate_with_drm_fd(int drm_fd) {
	const char *renderer_env = getenv("WLR_RENDERER");
const char *vendor = (const char *)glGetString(GL_VENDOR);
    const char *renderer = (const char *)glGetString(GL_RENDERER);
    const char *version = (const char *)glGetString(GL_VERSION);
    const char *shading_lang = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);

    wlr_log(WLR_INFO, "renderer_autocreate GL_VENDOR: %s", vendor);
    wlr_log(WLR_INFO, "renderer_autocreate GL_RENDERER: %s", renderer);
    wlr_log(WLR_INFO, "renderer_autocreate GL_VERSION: %s", version);
    wlr_log(WLR_INFO, "renderer_autocreate GL_SHADING_LANGUAGE_VERSION: %s", shading_lang);

// Direct surfaceless path - no fallbacks when surfaceless is requested
    //if (egl_platform && strcmp(egl_platform, "surfaceless") == 0)
     {
        wlr_log(WLR_INFO, "Creating surfaceless GLES2 renderer");
        return wlr_gles2_renderer_create_surfaceless();
    }

	if (renderer_env != NULL) {
		if (strcmp(renderer_env, "gles2") == 0) {
			if (drm_fd >= 0) {
				return wlr_gles2_renderer_create_with_drm_fd(drm_fd);
			} else {
				wlr_log(WLR_ERROR, "GLES2 renderer requires a valid DRM FD");
				return NULL;
			}
		}
		if (strcmp(renderer_env, "vulkan") == 0) {
#ifdef WLR_RENDERER_VULKAN
			return wlr_vulkan_renderer_create();
#else
			wlr_log(WLR_ERROR, "Vulkan renderer not compiled in");
			return NULL;
#endif
		}
		if (strcmp(renderer_env, "pixman") == 0) {
			return wlr_pixman_renderer_create();
		}
		wlr_log(WLR_ERROR, "Unsupported WLR_RENDERER: %s", renderer_env);
		return NULL;
	}

	// Try Vulkan first
#ifdef WLR_RENDERER_VULKAN
	struct wlr_renderer *vulkan_renderer = wlr_vulkan_renderer_create();
	if (vulkan_renderer != NULL) {
		return vulkan_renderer;
	}
#endif

	// Then try GLES2
	if (drm_fd >= 0) {
		struct wlr_renderer *gles2_renderer = wlr_gles2_renderer_create_with_drm_fd(drm_fd);
		if (gles2_renderer != NULL) {
			return gles2_renderer;
		}
	}

	// Fall back to Pixman
	return wlr_pixman_renderer_create();
}

struct wlr_renderer *wlr_renderer_autocreate(struct wlr_backend *backend) {
	return renderer_autocreate_with_drm_fd(-1);
}

int wlr_renderer_get_drm_fd(struct wlr_renderer *r) {
//	if (!r->impl->get_drm_fd) {
//		return -1;
//	}
	return r->impl->get_drm_fd(r);
}

struct wlr_render_pass *wlr_renderer_begin_buffer_pass(struct wlr_renderer *renderer,
		struct wlr_buffer *buffer, const struct wlr_buffer_pass_options *options) {
	struct wlr_buffer_pass_options default_options = {0};
	if (!options) {
		options = &default_options;
	}

	return renderer->impl->begin_buffer_pass(renderer, buffer, options);
}

struct wlr_render_timer *wlr_render_timer_create(struct wlr_renderer *renderer) {
	if (!renderer->impl->render_timer_create) {
		return NULL;
	}
	return renderer->impl->render_timer_create(renderer);
}

int wlr_render_timer_get_duration_ns(struct wlr_render_timer *timer) {
	if (!timer->impl->get_duration_ns) {
		return -1;
	}
	return timer->impl->get_duration_ns(timer);
}

void wlr_render_timer_destroy(struct wlr_render_timer *timer) {
	if (!timer->impl->destroy) {
		return;
	}
	timer->impl->destroy(timer);
}
