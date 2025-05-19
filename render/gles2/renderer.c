#include <assert.h>
#include <drm_fourcc.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <wayland-server-protocol.h>
#include <wayland-util.h>
#include <wlr/render/egl.h>
#include <wlr/render/interface.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/util/box.h>
#include <wlr/util/log.h>
#include <xf86drm.h>
#include "render/egl.h"
#include "render/gles2.h"
#include "render/pixel_format.h"
#include "util/time.h"

#include "common_vert_src.h"
#include "quad_frag_src.h"
#include "tex_rgba_frag_src.h"
#include "tex_rgbx_frag_src.h"
#include "tex_external_frag_src.h"

static const struct wlr_renderer_impl renderer_impl;
static const struct wlr_render_timer_impl render_timer_impl;

bool wlr_renderer_is_gles2(struct wlr_renderer *wlr_renderer) {
    return wlr_renderer->impl == &renderer_impl;
}

struct wlr_gles2_renderer *gles2_get_renderer(
        struct wlr_renderer *wlr_renderer) {
    assert(wlr_renderer_is_gles2(wlr_renderer));
    struct wlr_gles2_renderer *renderer = wl_container_of(wlr_renderer, renderer, wlr_renderer);
    return renderer;
}

bool wlr_render_timer_is_gles2(struct wlr_render_timer *timer) {
    return timer->impl == &render_timer_impl;
}

struct wlr_gles2_render_timer *gles2_get_render_timer(struct wlr_render_timer *wlr_timer) {
    assert(wlr_render_timer_is_gles2(wlr_timer));
    struct wlr_gles2_render_timer *timer = wl_container_of(wlr_timer, timer, base);
    return timer;
}

static void destroy_buffer(struct wlr_gles2_buffer *buffer) {
    wl_list_remove(&buffer->link);
    wlr_addon_finish(&buffer->addon);

    struct wlr_egl_context prev_ctx;
    wlr_egl_make_current(buffer->renderer->egl, &prev_ctx);

    push_gles2_debug(buffer->renderer);

    glDeleteFramebuffers(1, &buffer->fbo);
    glDeleteRenderbuffers(1, &buffer->rbo);
    glDeleteTextures(1, &buffer->tex);

    pop_gles2_debug(buffer->renderer);

    wlr_egl_destroy_image(buffer->renderer->egl, buffer->image);

    wlr_egl_restore_context(&prev_ctx);

    free(buffer);
}

static void handle_buffer_destroy(struct wlr_addon *addon) {
    struct wlr_gles2_buffer *buffer =
        wl_container_of(addon, buffer, addon);
    destroy_buffer(buffer);
}

static const struct wlr_addon_interface buffer_addon_impl = {
    .name = "wlr_gles2_buffer",
    .destroy = handle_buffer_destroy,
};

GLuint gles2_buffer_get_fbo(struct wlr_gles2_buffer *buffer) {
    if (buffer->external_only) {
        wlr_log(WLR_ERROR, "DMA-BUF format is external-only");
        return 0;
    }

    if (buffer->fbo) {
        return buffer->fbo;
    }

    push_gles2_debug(buffer->renderer);

    if (!buffer->rbo) {
        glGenRenderbuffers(1, &buffer->rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, buffer->rbo);
        buffer->renderer->procs.glEGLImageTargetRenderbufferStorageOES(GL_RENDERBUFFER,
            buffer->image);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    glGenFramebuffers(1, &buffer->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, buffer->fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_RENDERBUFFER, buffer->rbo);
    GLenum fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (fb_status != GL_FRAMEBUFFER_COMPLETE) {
        wlr_log(WLR_ERROR, "Failed to create FBO");
        glDeleteFramebuffers(1, &buffer->fbo);
        buffer->fbo = 0;
    }

    pop_gles2_debug(buffer->renderer);

    return buffer->fbo;
}

// Helper function to check list integrity
static bool check_list_integrity(struct wl_list *list, const char *name) {
    if (!list) {
        wlr_log(WLR_ERROR, "%s: Null list pointer", name);
        return false;
    }
    if (list->prev == NULL || list->next == NULL) {
        wlr_log(WLR_ERROR, "%s: Invalid prev/next pointers (prev=%p, next=%p)",
                name, list->prev, list->next);
        return false;
    }
    return true;
}
struct wlr_gles2_buffer *gles2_buffer_get_or_create(struct wlr_gles2_renderer *renderer,
        struct wlr_buffer *wlr_buffer) {
    struct wlr_addon *addon =
        wlr_addon_find(&wlr_buffer->addons, renderer, &buffer_addon_impl);
    if (addon) {
        struct wlr_gles2_buffer *buffer = wl_container_of(addon, buffer, addon);
        return buffer;
    }

    wlr_log(WLR_DEBUG, "Entering gles2_buffer_get_or_create for buffer %p", wlr_buffer);

    // Step 1: Validate inputs
    if (!renderer) {
        wlr_log(WLR_ERROR, "Null renderer in gles2_buffer_get_or_create");
        return NULL;
    }
    if (!wlr_buffer) {
        wlr_log(WLR_ERROR, "Null wlr_buffer in gles2_buffer_get_or_create");
        return NULL;
    }
    if (!renderer->egl) {
        wlr_log(WLR_ERROR, "Null EGL context in renderer");
        return NULL;
    }

    wlr_log(WLR_DEBUG, "Buffer dimensions: %dx%d", wlr_buffer->width, wlr_buffer->height);

    // Step 2: Allocate buffer structure
    struct wlr_gles2_buffer *buffer = calloc(1, sizeof(*buffer));
    if (!buffer) {
        wlr_log(WLR_ERROR, "Failed to allocate wlr_gles2_buffer");
        return NULL;
    }

    // Step 3: Initialize buffer fields
    buffer->buffer = wlr_buffer;
    buffer->renderer = renderer;
    buffer->texture = 0;
    buffer->fbo = 0;
    buffer->rbo = 0;
    buffer->image = EGL_NO_IMAGE_KHR;

    // Step 4: Access buffer data
    void *data_ptr;
    uint32_t format;
    size_t stride;
    if (!wlr_buffer_begin_data_ptr_access(wlr_buffer, WLR_BUFFER_DATA_PTR_ACCESS_READ,
            &data_ptr, &format, &stride)) {
        wlr_log(WLR_ERROR, "Failed to access buffer data");
        free(buffer);
        return NULL;
    }
    if (!data_ptr) {
        wlr_log(WLR_ERROR, "Null data pointer for buffer");
        wlr_buffer_end_data_ptr_access(wlr_buffer);
        free(buffer);
        return NULL;
    }
    wlr_log(WLR_DEBUG, "Buffer data: ptr=%p, format=0x%x, stride=%zu", data_ptr, format, stride);

    // Validate buffer format
    if (format != DRM_FORMAT_XRGB8888) {
        wlr_log(WLR_ERROR, "Unsupported buffer format: 0x%x, expected XRGB8888", format);
        wlr_buffer_end_data_ptr_access(wlr_buffer);
        free(buffer);
        return NULL;
    }

    // Validate stride
    size_t expected_stride = wlr_buffer->width * 4;
    if (stride != expected_stride) {
        wlr_log(WLR_ERROR, "Unexpected stride: got %zu, expected %zu", stride, expected_stride);
        wlr_buffer_end_data_ptr_access(wlr_buffer);
        free(buffer);
        return NULL;
    }

    // Validate dimensions
    if (wlr_buffer->width <= 0 || wlr_buffer->height <= 0) {
        wlr_log(WLR_ERROR, "Invalid buffer dimensions: %dx%d", wlr_buffer->width, wlr_buffer->height);
        wlr_buffer_end_data_ptr_access(wlr_buffer);
        free(buffer);
        return NULL;
    }

    // Step 5: Save and make EGL context current
    struct wlr_egl_context prev_ctx = {0};
    if (!wlr_egl_make_current(renderer->egl, &prev_ctx)) {
        wlr_log(WLR_ERROR, "Failed to make EGL context current, EGL error: 0x%x", eglGetError());
        wlr_buffer_end_data_ptr_access(wlr_buffer);
        free(buffer);
        return NULL;
    }

    // Step 6: Check for GL_EXT_texture_format_BGRA8888 extension
    const char *extensions = (const char *)glGetString(GL_EXTENSIONS);
    bool has_bgra = extensions && strstr(extensions, "GL_EXT_texture_format_BGRA8888") != NULL;
    wlr_log(WLR_DEBUG, "GL_EXT_texture_format_BGRA8888 supported: %d", has_bgra);

    // Step 7: Generate texture
    glGenTextures(1, &buffer->texture);
    if (buffer->texture == 0) {
        wlr_log(WLR_ERROR, "Failed to generate texture");
        wlr_egl_restore_context(&prev_ctx);
        wlr_buffer_end_data_ptr_access(wlr_buffer);
        free(buffer);
        return NULL;
    }
    wlr_log(WLR_DEBUG, "Generated new texture ID: %u for buffer %p", buffer->texture, wlr_buffer);

    // Step 8: Bind and configure texture
    glBindTexture(GL_TEXTURE_2D, buffer->texture);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "glBindTexture failed with error: 0x%x", err);
        glDeleteTextures(1, &buffer->texture);
        wlr_egl_restore_context(&prev_ctx);
        wlr_buffer_end_data_ptr_access(wlr_buffer);
        free(buffer);
        return NULL;
    }

    GLboolean is_texture = glIsTexture(buffer->texture);
    wlr_log(WLR_DEBUG, "Texture %u validity check: %d", buffer->texture, is_texture);
    if (!is_texture) {
        wlr_log(WLR_ERROR, "Generated texture ID %u is not valid", buffer->texture);
        glDeleteTextures(1, &buffer->texture);
        wlr_egl_restore_context(&prev_ctx);
        wlr_buffer_end_data_ptr_access(wlr_buffer);
        free(buffer);
        return NULL;
    }

    GLint max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    if (wlr_buffer->width <= 0 || wlr_buffer->height <= 0 || wlr_buffer->width > max_texture_size || wlr_buffer->height > max_texture_size) {
        wlr_log(WLR_ERROR, "Buffer dimensions %dx%d exceed max texture size %d",
                wlr_buffer->width, wlr_buffer->height, max_texture_size);
        glDeleteTextures(1, &buffer->texture);
        wlr_egl_restore_context(&prev_ctx);
        wlr_buffer_end_data_ptr_access(wlr_buffer);
        free(buffer);
        return NULL;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "glTexParameteri failed with error: 0x%x", err);
        glDeleteTextures(1, &buffer->texture);
        wlr_egl_restore_context(&prev_ctx);
        wlr_buffer_end_data_ptr_access(wlr_buffer);
        free(buffer);
        return NULL;
    }

    // Step 9: Upload texture data
    glBindTexture(GL_TEXTURE_2D, buffer->texture);
   // GLenum internal_format = has_bgra ? GL_BGRA_EXT : GL_RGBA;
  //  GLenum data_format = has_bgra ? GL_BGRA_EXT : GL_RGBA;
    uint8_t *temp_buffer = NULL;

   if (!has_bgra) {
    // Convert BGRA (XRGB8888) to RGBA
    temp_buffer = malloc(wlr_buffer->width * wlr_buffer->height * 4);
    if (!temp_buffer) {
        wlr_log(WLR_ERROR, "Failed to allocate temp buffer for RGBA conversion");
        glDeleteTextures(1, &buffer->texture);
        wlr_egl_restore_context(&prev_ctx);
        wlr_buffer_end_data_ptr_access(wlr_buffer);
        free(buffer);
        return NULL;
    }
    int total_pixels = wlr_buffer->width * wlr_buffer->height;
    if (total_pixels <= 0) {
        wlr_log(WLR_ERROR, "Invalid buffer size: %dx%d", wlr_buffer->width, wlr_buffer->height);
        glDeleteTextures(1, &buffer->texture);
        wlr_egl_restore_context(&prev_ctx);
        wlr_buffer_end_data_ptr_access(wlr_buffer);
        free(temp_buffer);
        free(buffer);
        return NULL;
    }
    uint8_t *src = (uint8_t *)data_ptr;
    for (int i = 0; i < total_pixels * 4; i += 4) {
        temp_buffer[i + 0] = src[i + 2]; // R
        temp_buffer[i + 1] = src[i + 1]; // G
        temp_buffer[i + 2] = src[i + 0]; // B
        temp_buffer[i + 3] = src[i + 3]; // A
    }
    data_ptr = temp_buffer;
}

   wlr_log(WLR_DEBUG, "Calling glTexImage2D: internalFmt=GL_RGBA, w=%d, h=%d, format=GL_BGRA_EXT, type=GL_UNSIGNED_BYTE",
        wlr_buffer->width, wlr_buffer->height);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wlr_buffer->width, wlr_buffer->height,
             0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data_ptr);
GLenum tex_upload_err = glGetError();
if (tex_upload_err != GL_NO_ERROR) {
    wlr_log(WLR_ERROR, "!!!!!!!! GL Error after glTexImage2D (BGRA_EXT): 0x%x !!!!!!!!", tex_upload_err);
    // ... existing cleanup and return NULL path ...
    glDeleteTextures(1, &buffer->texture);
    wlr_buffer_end_data_ptr_access(wlr_buffer); // Ensure this is called before restore
    wlr_egl_restore_context(&prev_ctx);
    free(buffer);
    return NULL;
}
wlr_log(WLR_DEBUG, "glTexImage2D with GL_BGRA_EXT seems successful.");

    if (temp_buffer) free(temp_buffer);

    // Step 10: End buffer data access
    wlr_buffer_end_data_ptr_access(wlr_buffer);

    // Step 11: Generate framebuffer
    glGenFramebuffers(1, &buffer->fbo);
    if (buffer->fbo == 0) {
        wlr_log(WLR_ERROR, "Failed to generate framebuffer");
        glDeleteTextures(1, &buffer->texture);
        wlr_egl_restore_context(&prev_ctx);
        free(buffer);
        return NULL;
    }

    // Step 12: Bind and set up framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, buffer->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->texture, 0);

    // Step 13: Check framebuffer status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        wlr_log(WLR_ERROR, "Framebuffer incomplete, status: 0x%x", status);
        glDeleteFramebuffers(1, &buffer->fbo);
        glDeleteTextures(1, &buffer->texture);
        wlr_egl_restore_context(&prev_ctx);
        free(buffer);
        return NULL;
    }

    // Step 14: Unbind framebuffer and texture
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Step 15: Initialize Wayland resources
    wlr_addon_init(&buffer->addon, &wlr_buffer->addons, renderer, &buffer_addon_impl);
    wl_list_init(&buffer->link);

    // Check and reinitialize renderer->buffers if corrupted
    if (!check_list_integrity(&renderer->buffers, "renderer->buffers")) {
        wlr_log(WLR_DEBUG, "renderer->buffers appears corrupted, reinitializing");
        wl_list_init(&renderer->buffers);
    }

    // Insert buffer into list
    wl_list_insert(&renderer->buffers, &buffer->link);
    if (buffer->link.prev == NULL || buffer->link.next == NULL) {
        wlr_log(WLR_ERROR, "Failed to insert buffer into renderer->buffers");
        wlr_addon_finish(&buffer->addon);
        wl_list_remove(&buffer->link);
        glDeleteFramebuffers(1, &buffer->fbo);
        glDeleteTextures(1, &buffer->texture);
        wlr_egl_restore_context(&prev_ctx);
        free(buffer);
        return NULL;
    }

    // Step 16: Restore EGL context
    wlr_egl_restore_context(&prev_ctx);

    wlr_log(WLR_DEBUG, "Created GL FBO %u for buffer %dx%d", buffer->fbo, wlr_buffer->width, wlr_buffer->height);
    return buffer;
}

static const struct wlr_drm_format_set *gles2_get_texture_formats(
        struct wlr_renderer *wlr_renderer, uint32_t buffer_caps) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);
    if (buffer_caps & WLR_BUFFER_CAP_DMABUF) {
        return wlr_egl_get_dmabuf_texture_formats(renderer->egl);
    } else if (buffer_caps & WLR_BUFFER_CAP_DATA_PTR) {
        return &renderer->shm_texture_formats;
    } else {
        return NULL;
    }
}

static const struct wlr_drm_format_set *gles2_get_render_formats(
        struct wlr_renderer *wlr_renderer) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);
    return wlr_egl_get_dmabuf_render_formats(renderer->egl);
}

static int gles2_get_drm_fd(struct wlr_renderer *wlr_renderer) {
    struct wlr_gles2_renderer *renderer =
        gles2_get_renderer(wlr_renderer);

    if (renderer->drm_fd < 0) {
        renderer->drm_fd = wlr_egl_dup_drm_fd(renderer->egl);
    }

    return renderer->drm_fd;
}

struct wlr_egl *wlr_gles2_renderer_get_egl(struct wlr_renderer *wlr_renderer) {
    struct wlr_gles2_renderer *renderer =
        gles2_get_renderer(wlr_renderer);
    return renderer->egl;
}

static void gles2_destroy(struct wlr_renderer *wlr_renderer) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);

    wlr_egl_make_current(renderer->egl, NULL);

    struct wlr_gles2_texture *tex, *tex_tmp;
    wl_list_for_each_safe(tex, tex_tmp, &renderer->textures, link) {
        gles2_texture_destroy(tex);
    }

    struct wlr_gles2_buffer *buffer, *buffer_tmp;
    wl_list_for_each_safe(buffer, buffer_tmp, &renderer->buffers, link) {
        destroy_buffer(buffer);
    }

    push_gles2_debug(renderer);
    glDeleteProgram(renderer->shaders.quad.program);
    glDeleteProgram(renderer->shaders.tex_rgba.program);
    glDeleteProgram(renderer->shaders.tex_rgbx.program);
    glDeleteProgram(renderer->shaders.tex_ext.program);
    pop_gles2_debug(renderer);

    if (renderer->exts.KHR_debug) {
        glDisable(GL_DEBUG_OUTPUT_KHR);
        renderer->procs.glDebugMessageCallbackKHR(NULL, NULL);
    }

    wlr_egl_unset_current(renderer->egl);
    wlr_egl_destroy(renderer->egl);

    wlr_drm_format_set_finish(&renderer->shm_texture_formats);

    if (renderer->drm_fd >= 0) {
        close(renderer->drm_fd);
    }

    free(renderer);
}

static struct wlr_render_pass *gles2_begin_buffer_pass(struct wlr_renderer *wlr_renderer,
        struct wlr_buffer *wlr_buffer, const struct wlr_buffer_pass_options *options) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);

    struct wlr_egl_context prev_ctx = {0};
    if (!wlr_egl_make_current(renderer->egl, &prev_ctx)) {
        return NULL;
    }

    struct wlr_gles2_render_timer *timer = NULL;
    if (options->timer) {
        timer = gles2_get_render_timer(options->timer);
        clock_gettime(CLOCK_MONOTONIC, &timer->cpu_start);
    }

    struct wlr_gles2_buffer *buffer = gles2_buffer_get_or_create(renderer, wlr_buffer);
    if (!buffer) {
        return NULL;
    }

    struct wlr_gles2_render_pass *pass = begin_gles2_buffer_pass(buffer,
        &prev_ctx, timer, options->signal_timeline, options->signal_point);
    if (!pass) {
        return NULL;
    }
    return &pass->base;
}

GLuint wlr_gles2_renderer_get_buffer_fbo(struct wlr_renderer *wlr_renderer,
        struct wlr_buffer *wlr_buffer) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);
    GLuint fbo = 0;

    struct wlr_egl_context prev_ctx = {0};
    if (!wlr_egl_make_current(renderer->egl, &prev_ctx)) {
        return 0;
    }

    struct wlr_gles2_buffer *buffer = gles2_buffer_get_or_create(renderer, wlr_buffer);
    if (buffer) {
        fbo = gles2_buffer_get_fbo(buffer);
    }

    wlr_egl_restore_context(&prev_ctx);
    return fbo;
}

static struct wlr_render_timer *gles2_render_timer_create(struct wlr_renderer *wlr_renderer) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);
    if (!renderer->exts.EXT_disjoint_timer_query) {
        wlr_log(WLR_ERROR, "can't create timer, EXT_disjoint_timer_query not available");
        return NULL;
    }

    struct wlr_gles2_render_timer *timer = calloc(1, sizeof(*timer));
    if (!timer) {
        return NULL;
    }
    timer->base.impl = &render_timer_impl;
    timer->renderer = renderer;

    struct wlr_egl_context prev_ctx;
    wlr_egl_make_current(renderer->egl, &prev_ctx);
    renderer->procs.glGenQueriesEXT(1, &timer->id);
    wlr_egl_restore_context(&prev_ctx);

    return &timer->base;
}

static int gles2_get_render_time(struct wlr_render_timer *wlr_timer) {
    struct wlr_gles2_render_timer *timer = gles2_get_render_timer(wlr_timer);
    struct wlr_gles2_renderer *renderer = timer->renderer;

    struct wlr_egl_context prev_ctx;
    wlr_egl_make_current(renderer->egl, &prev_ctx);

    GLint64 disjoint;
    renderer->procs.glGetInteger64vEXT(GL_GPU_DISJOINT_EXT, &disjoint);
    if (disjoint) {
        wlr_log(WLR_ERROR, "a disjoint operation occurred and the render timer is invalid");
        wlr_egl_restore_context(&prev_ctx);
        return -1;
    }

    GLint available;
    renderer->procs.glGetQueryObjectivEXT(timer->id,
        GL_QUERY_RESULT_AVAILABLE_EXT, &available);
    if (!available) {
        wlr_log(WLR_ERROR, "timer was read too early, gpu isn't done!");
        wlr_egl_restore_context(&prev_ctx);
        return -1;
    }

    GLuint64 gl_render_end;
    renderer->procs.glGetQueryObjectui64vEXT(timer->id, GL_QUERY_RESULT_EXT,
        &gl_render_end);

    int64_t cpu_nsec_total = timespec_to_nsec(&timer->cpu_end) - timespec_to_nsec(&timer->cpu_start);

    wlr_egl_restore_context(&prev_ctx);
    return gl_render_end - timer->gl_cpu_end + cpu_nsec_total;
}

static void gles2_render_timer_destroy(struct wlr_render_timer *wlr_timer) {
    struct wlr_gles2_render_timer *timer = wl_container_of(wlr_timer, timer, base);
    struct wlr_gles2_renderer *renderer = timer->renderer;

    struct wlr_egl_context prev_ctx;
    wlr_egl_make_current(renderer->egl, &prev_ctx);
    renderer->procs.glDeleteQueriesEXT(1, &timer->id);
    wlr_egl_restore_context(&prev_ctx);
    free(timer);
}

static const struct wlr_renderer_impl renderer_impl = {
    .destroy = gles2_destroy,
    .get_texture_formats = gles2_get_texture_formats,
    .get_render_formats = gles2_get_render_formats,
    .get_drm_fd = gles2_get_drm_fd,
    .texture_from_buffer = gles2_texture_from_buffer,
    .begin_buffer_pass = gles2_begin_buffer_pass,
    .render_timer_create = gles2_render_timer_create,
};

static const struct wlr_render_timer_impl render_timer_impl = {
    .get_duration_ns = gles2_get_render_time,
    .destroy = gles2_render_timer_destroy,
};

void push_gles2_debug_(struct wlr_gles2_renderer *renderer,
        const char *file, const char *func) {
    if (!renderer->procs.glPushDebugGroupKHR) {
        return;
    }

    int len = snprintf(NULL, 0, "%s:%s", file, func) + 1;
    char str[len];
    snprintf(str, len, "%s:%s", file, func);
    renderer->procs.glPushDebugGroupKHR(GL_DEBUG_SOURCE_APPLICATION_KHR, 1, -1, str);
}

void pop_gles2_debug(struct wlr_gles2_renderer *renderer) {
    if (renderer->procs.glPopDebugGroupKHR) {
        renderer->procs.glPopDebugGroupKHR();
    }
}

static enum wlr_log_importance gles2_log_importance_to_wlr(GLenum type) {
    switch (type) {
    case GL_DEBUG_TYPE_ERROR_KHR:               return WLR_ERROR;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR: return WLR_DEBUG;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR:  return WLR_ERROR;
    case GL_DEBUG_TYPE_PORTABILITY_KHR:         return WLR_DEBUG;
    case GL_DEBUG_TYPE_PERFORMANCE_KHR:         return WLR_DEBUG;
    case GL_DEBUG_TYPE_OTHER_KHR:               return WLR_DEBUG;
    case GL_DEBUG_TYPE_MARKER_KHR:              return WLR_DEBUG;
    case GL_DEBUG_TYPE_PUSH_GROUP_KHR:          return WLR_DEBUG;
    case GL_DEBUG_TYPE_POP_GROUP_KHR:           return WLR_DEBUG;
    default:                                    return WLR_DEBUG;
    }
}

static void gles2_log(GLenum src, GLenum type, GLuint id, GLenum severity,
        GLsizei len, const GLchar *msg, const void *user) {
    _wlr_log(gles2_log_importance_to_wlr(type), "[GLES2] %s", msg);
}
/*
static GLuint compile_shader(struct wlr_gles2_renderer *renderer,
        GLenum type, const GLchar *src) {
    push_gles2_debug(renderer);

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok == GL_FALSE) {
        wlr_log(WLR_ERROR, "Failed to compile shader");
        glDeleteShader(shader);
        shader = 0;
    }

    pop_gles2_debug(renderer);
    return shader;
}*/

static GLuint link_program(struct wlr_gles2_renderer *renderer,
        const char *vert_src, const char *frag_src) {
    wlr_log(WLR_DEBUG, "Compiling shader program: vert_src=%p, frag_src=%p", vert_src, frag_src);

    // Compile vertex shader
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vert_src, NULL);
    glCompileShader(vert);

    GLint vert_status;
    glGetShaderiv(vert, GL_COMPILE_STATUS, &vert_status);
    if (vert_status != GL_TRUE) {
        char info_log[512] = {0};
        glGetShaderInfoLog(vert, sizeof(info_log) - 1, NULL, info_log);
        info_log[sizeof(info_log) - 1] = '\0';
        wlr_log(WLR_ERROR, "Failed to compile vertex shader: %s", info_log[0] ? info_log : "No info log available");
        wlr_log(WLR_DEBUG, "Vertex shader source:\n%s", vert_src);
        glDeleteShader(vert);
        return 0;
    }
    wlr_log(WLR_DEBUG, "Vertex shader compiled successfully");

    // Compile fragment shader
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &frag_src, NULL);
    glCompileShader(frag);

    GLint frag_status;
    glGetShaderiv(frag, GL_COMPILE_STATUS, &frag_status);
    if (frag_status != GL_TRUE) {
        char info_log[512] = {0};
        glGetShaderInfoLog(frag, sizeof(info_log) - 1, NULL, info_log);
        info_log[sizeof(info_log) - 1] = '\0';
        wlr_log(WLR_ERROR, "Failed to compile fragment shader: %s", info_log[0] ? info_log : "No info log available");
        wlr_log(WLR_DEBUG, "Fragment shader source:\n%s", frag_src);
        glDeleteShader(frag);
        glDeleteShader(vert);
        return 0;
    }
    wlr_log(WLR_DEBUG, "Fragment shader compiled successfully");

    // Link program
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    GLint link_status;
    glGetProgramiv(prog, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        char info_log[512] = {0};
        glGetProgramInfoLog(prog, sizeof(info_log) - 1, NULL, info_log);
        info_log[sizeof(info_log) - 1] = '\0';
        wlr_log(WLR_ERROR, "Shader link failed: %s", info_log[0] ? info_log : "No info log available");
        glDeleteProgram(prog);
        glDeleteShader(frag);
        glDeleteShader(vert);
        return 0;
    }
    wlr_log(WLR_DEBUG, "Shader program linked successfully: program=%u", prog);

    glDetachShader(prog, vert);
    glDetachShader(prog, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);

    return prog;
}

static bool check_gl_ext(const char *exts, const char *ext) {
    size_t extlen = strlen(ext);
    const char *end = exts + strlen(exts);

    while (exts < end) {
        if (exts[0] == ' ') {
            exts++;
            continue;
        }
        size_t n = strcspn(exts, " ");
        if (n == extlen && strncmp(ext, exts, n) == 0) {
            return true;
        }
        exts += n;
    }
    return false;
}

static void load_gl_proc(void *proc_ptr, const char *name) {
    void *proc = (void *)eglGetProcAddress(name);
    if (proc == NULL) {
        wlr_log(WLR_ERROR, "eglGetProcAddress(%s) failed", name);
        abort();
    }
    *(void **)proc_ptr = proc;
}

struct wlr_renderer *wlr_gles2_renderer_create_with_drm_fd(int drm_fd) {
    struct wlr_egl *egl = wlr_egl_create_with_drm_fd(drm_fd);
    if (egl == NULL) {
        wlr_log(WLR_ERROR, "Could not initialize EGL");
        return NULL;
    }

    struct wlr_renderer *renderer = wlr_gles2_renderer_create(egl);
    if (!renderer) {
        wlr_log(WLR_ERROR, "Failed to create GLES2 renderer");
        wlr_egl_destroy(egl);
        return NULL;
    }

    return renderer;
}

struct wlr_renderer *wlr_gles2_renderer_create(struct wlr_egl *egl) {
    if (!wlr_egl_make_current(egl, NULL)) {
        return NULL;
    }

    const char *exts_str = (const char *)glGetString(GL_EXTENSIONS);
    if (exts_str == NULL) {
        wlr_log(WLR_ERROR, "Failed to get GL_EXTENSIONS");
        return NULL;
    }

    struct wlr_gles2_renderer *renderer = calloc(1, sizeof(*renderer));
    if (renderer == NULL) {
        return NULL;
    }
    wlr_renderer_init(&renderer->wlr_renderer, &renderer_impl, WLR_BUFFER_CAP_DMABUF);
    renderer->wlr_renderer.features.output_color_transform = false;

    wl_list_init(&renderer->buffers);
    wl_list_init(&renderer->textures);

    renderer->egl = egl;
    renderer->exts_str = exts_str;
    renderer->drm_fd = -1;

    wlr_log(WLR_INFO, "Creating GLES2 renderer");
    wlr_log(WLR_INFO, "Using %s", glGetString(GL_VERSION));
    wlr_log(WLR_INFO, "GL vendor: %s", glGetString(GL_VENDOR));
    wlr_log(WLR_INFO, "GL renderer: %s", glGetString(GL_RENDERER));
    wlr_log(WLR_INFO, "Supported GLES2 extensions: %s", exts_str);

    if (!renderer->egl->exts.EXT_image_dma_buf_import) {
        wlr_log(WLR_ERROR, "EGL_EXT_image_dma_buf_import not supported");
        free(renderer);
        return NULL;
    }
    if (!check_gl_ext(exts_str, "GL_EXT_texture_format_BGRA8888")) {
        wlr_log(WLR_ERROR, "BGRA8888 format not supported by GLES2");
        free(renderer);
        return NULL;
    }
    if (!check_gl_ext(exts_str, "GL_EXT_unpack_subimage")) {
        wlr_log(WLR_ERROR, "GL_EXT_unpack_subimage not supported");
        free(renderer);
        return NULL;
    }

    renderer->exts.EXT_read_format_bgra =
        check_gl_ext(exts_str, "GL_EXT_read_format_bgra");

    renderer->exts.EXT_texture_type_2_10_10_10_REV =
        check_gl_ext(exts_str, "GL_EXT_texture_type_2_10_10_10_REV");

    renderer->exts.OES_texture_half_float_linear =
        check_gl_ext(exts_str, "GL_OES_texture_half_float_linear");

    renderer->exts.EXT_texture_norm16 =
        check_gl_ext(exts_str, "GL_EXT_texture_norm16");

    if (check_gl_ext(exts_str, "GL_KHR_debug")) {
        renderer->exts.KHR_debug = true;
        load_gl_proc(&renderer->procs.glDebugMessageCallbackKHR,
            "glDebugMessageCallbackKHR");
        load_gl_proc(&renderer->procs.glDebugMessageControlKHR,
            "glDebugMessageControlKHR");
    }

    if (check_gl_ext(exts_str, "GL_OES_EGL_image_external")) {
        renderer->exts.OES_egl_image_external = true;
        load_gl_proc(&renderer->procs.glEGLImageTargetTexture2DOES,
            "glEGLImageTargetTexture2DOES");
    }

    if (check_gl_ext(exts_str, "GL_OES_EGL_image")) {
        renderer->exts.OES_egl_image = true;
        load_gl_proc(&renderer->procs.glEGLImageTargetRenderbufferStorageOES,
            "glEGLImageTargetRenderbufferStorageOES");
    }

    if (check_gl_ext(exts_str, "GL_KHR_robustness")) {
        GLint notif_strategy = 0;
        glGetIntegerv(GL_RESET_NOTIFICATION_STRATEGY_KHR, &notif_strategy);
        switch (notif_strategy) {
        case GL_LOSE_CONTEXT_ON_RESET_KHR:
            wlr_log(WLR_DEBUG, "GPU reset notifications are enabled");
            load_gl_proc(&renderer->procs.glGetGraphicsResetStatusKHR,
                "glGetGraphicsResetStatusKHR");
            break;
        case GL_NO_RESET_NOTIFICATION_KHR:
            wlr_log(WLR_DEBUG, "GPU reset notifications are disabled");
            break;
        }
    }

    if (check_gl_ext(exts_str, "GL_EXT_disjoint_timer_query")) {
        renderer->exts.EXT_disjoint_timer_query = true;
        load_gl_proc(&renderer->procs.glGenQueriesEXT, "glGenQueriesEXT");
        load_gl_proc(&renderer->procs.glDeleteQueriesEXT, "glDeleteQueriesEXT");
        load_gl_proc(&renderer->procs.glQueryCounterEXT, "glQueryCounterEXT");
        load_gl_proc(&renderer->procs.glGetQueryObjectivEXT, "glGetQueryObjectivEXT");
        load_gl_proc(&renderer->procs.glGetQueryObjectui64vEXT, "glGetQueryObjectui64vEXT");
        if (eglGetProcAddress("glGetInteger64vEXT")) {
            load_gl_proc(&renderer->procs.glGetInteger64vEXT, "glGetInteger64vEXT");
        } else {
            load_gl_proc(&renderer->procs.glGetInteger64vEXT, "glGetInteger64v");
        }
    }

    if (renderer->exts.KHR_debug) {
        glEnable(GL_DEBUG_OUTPUT_KHR);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
        renderer->procs.glDebugMessageCallbackKHR(gles2_log, NULL);

        // Silence unwanted message types
        renderer->procs.glDebugMessageControlKHR(GL_DONT_CARE,
            GL_DEBUG_TYPE_POP_GROUP_KHR, GL_DONT_CARE, 0, NULL, GL_FALSE);
        renderer->procs.glDebugMessageControlKHR(GL_DONT_CARE,
            GL_DEBUG_TYPE_PUSH_GROUP_KHR, GL_DONT_CARE, 0, NULL, GL_FALSE);
    }

    push_gles2_debug(renderer);

    GLuint prog;
    renderer->shaders.quad.program = prog =
        link_program(renderer, common_vert_src, quad_frag_src);
    if (!renderer->shaders.quad.program) {
        goto error;
    }
    renderer->shaders.quad.proj = glGetUniformLocation(prog, "proj");
    renderer->shaders.quad.color = glGetUniformLocation(prog, "color");
    renderer->shaders.quad.pos_attrib = glGetAttribLocation(prog, "pos");

    renderer->shaders.tex_rgba.program = prog =
        link_program(renderer, common_vert_src, tex_rgba_frag_src);
    if (!renderer->shaders.tex_rgba.program) {
        goto error;
    }
    renderer->shaders.tex_rgba.proj = glGetUniformLocation(prog, "proj");
    renderer->shaders.tex_rgba.tex_proj = glGetUniformLocation(prog, "tex_proj");
    renderer->shaders.tex_rgba.tex = glGetUniformLocation(prog, "tex");
    renderer->shaders.tex_rgba.alpha = glGetUniformLocation(prog, "alpha");
    renderer->shaders.tex_rgba.pos_attrib = glGetAttribLocation(prog, "pos");

    renderer->shaders.tex_rgbx.program = prog =
        link_program(renderer, common_vert_src, tex_rgbx_frag_src);
    if (!renderer->shaders.tex_rgbx.program) {
        goto error;
    }
    renderer->shaders.tex_rgbx.proj = glGetUniformLocation(prog, "proj");
    renderer->shaders.tex_rgbx.tex_proj = glGetUniformLocation(prog, "tex_proj");
    renderer->shaders.tex_rgbx.tex = glGetUniformLocation(prog, "tex");
    renderer->shaders.tex_rgbx.alpha = glGetUniformLocation(prog, "alpha");
    renderer->shaders.tex_rgbx.pos_attrib = glGetAttribLocation(prog, "pos");

    if (renderer->exts.OES_egl_image_external) {
        renderer->shaders.tex_ext.program = prog =
            link_program(renderer, common_vert_src, tex_external_frag_src);
        if (!renderer->shaders.tex_ext.program) {
            goto error;
        }
        renderer->shaders.tex_ext.proj = glGetUniformLocation(prog, "proj");
        renderer->shaders.tex_ext.tex_proj = glGetUniformLocation(prog, "tex_proj");
        renderer->shaders.tex_ext.tex = glGetUniformLocation(prog, "tex");
        renderer->shaders.tex_ext.alpha = glGetUniformLocation(prog, "alpha");
        renderer->shaders.tex_ext.pos_attrib = glGetAttribLocation(prog, "pos");
    }

    pop_gles2_debug(renderer);

    wlr_egl_unset_current(renderer->egl);

    get_gles2_shm_formats(renderer, &renderer->shm_texture_formats);

    int drm_fd = wlr_renderer_get_drm_fd(&renderer->wlr_renderer);
    uint64_t cap_syncobj_timeline;
    if (drm_fd >= 0 && drmGetCap(drm_fd, DRM_CAP_SYNCOBJ_TIMELINE, &cap_syncobj_timeline) == 0) {
        renderer->wlr_renderer.features.timeline = egl->procs.eglDupNativeFenceFDANDROID &&
            egl->procs.eglWaitSyncKHR && cap_syncobj_timeline != 0;
    }

    return &renderer->wlr_renderer;

error:
    glDeleteProgram(renderer->shaders.quad.program);
    glDeleteProgram(renderer->shaders.tex_rgba.program);
    glDeleteProgram(renderer->shaders.tex_rgbx.program);
    glDeleteProgram(renderer->shaders.tex_ext.program);

    pop_gles2_debug(renderer);

    if (renderer->exts.KHR_debug) {
        glDisable(GL_DEBUG_OUTPUT_KHR);
        renderer->procs.glDebugMessageCallbackKHR(NULL, NULL);
    }

    wlr_egl_unset_current(renderer->egl);

    free(renderer);
    return NULL;
}

bool wlr_gles2_renderer_check_ext(struct wlr_renderer *wlr_renderer,
        const char *ext) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);
    return check_gl_ext(renderer->exts_str, ext);
}

/*
struct wlr_renderer *wlr_gles2_renderer_create_surfaceless(void) {
    wlr_log(WLR_DEBUG, "Entering wlr_gles2_renderer_create_surfaceless");

    // Initialize EGL
    struct wlr_egl *egl = wlr_egl_create_surfaceless();
    if (!egl) {
        wlr_log(WLR_ERROR, "Failed to create EGL context");
        return NULL;
    }
    wlr_log(WLR_DEBUG, "Created EGL context: %p", egl);

    EGLDisplay display = egl->display;
    EGLContext context = egl->context;


    // Allocate renderer
    struct wlr_gles2_renderer *renderer = calloc(1, sizeof(struct wlr_gles2_renderer));
    if (!renderer) {
        wlr_log(WLR_ERROR, "Failed to allocate renderer");
        eglDestroyContext(display, context);
        eglTerminate(display);
        free(egl);
        return NULL;
    }
    wlr_log(WLR_DEBUG, "Allocated renderer: %p", renderer);

    renderer->wlr_renderer.impl = &renderer_impl;
    renderer->egl = egl;
    renderer->drm_fd = -1;



    const char *vendor = (const char *)glGetString(GL_VENDOR);
    const char *renderer_str  = (const char *)glGetString(GL_RENDERER);
    const char *version = (const char *)glGetString(GL_VERSION);
    const char *shading_lang = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);

    wlr_log(WLR_INFO, "wlr_gles2_renderer GL_VENDOR: %s", vendor);
    wlr_log(WLR_INFO, "wlr_gles2_renderer GL_RENDERER: %s",renderer_str );
    wlr_log(WLR_INFO, "wlr_gles2_renderer GL_VERSION: %s", version);
    wlr_log(WLR_INFO, "wlr_gles2_renderer GL_SHADING_LANGUAGE_VERSION: %s", shading_lang);


    // Initialize buffers list
    wl_list_init(&renderer->buffers);
    wlr_log(WLR_DEBUG, "Initialized renderer->buffers: prev=%p, next=%p",
            renderer->buffers.prev, renderer->buffers.next);

    // Log Zink usage
    const char *driver = getenv("MESA_LOADER_DRIVER_OVERRIDE");
    if (driver && strcmp(driver, "zink") == 0) {
        wlr_log(WLR_INFO, "Using Zink (Vulkan) driver for hardware acceleration");
    } else {
        wlr_log(WLR_DEBUG, "MESA_LOADER_DRIVER_OVERRIDE: %s", driver ? driver : "unset");
    }

    // Initialize renderer
    wlr_renderer_init(&renderer->wlr_renderer, &renderer_impl, WLR_BUFFER_CAP_DMABUF);
    wlr_log(WLR_DEBUG, "Renderer initialized with DMABUF capability");

    wlr_log(WLR_DEBUG, "Exiting wlr_gles2_renderer_create_surfaceless");
    return &renderer->wlr_renderer;
}*/

struct wlr_renderer *wlr_gles2_renderer_create_surfaceless(void) {
    wlr_log(WLR_DEBUG, "Entering wlr_gles2_renderer_create_surfaceless");

    // Initialize EGL
    struct wlr_egl *egl = wlr_egl_create_surfaceless();
    if (!egl) {
        wlr_log(WLR_ERROR, "Failed to create EGL context");
        return NULL;
    }
    wlr_log(WLR_DEBUG, "Created EGL context: %p", egl);

    EGLDisplay display = egl->display;
    EGLContext context = egl->context;

    // Make EGL context current
    if (!wlr_egl_make_current(egl, NULL)) {
        wlr_log(WLR_ERROR, "Failed to make EGL context current");
        eglDestroyContext(display, context);
        eglTerminate(display);
        free(egl);
        return NULL;
    }

    // Allocate renderer
    struct wlr_gles2_renderer *renderer = calloc(1, sizeof(struct wlr_gles2_renderer));
    if (!renderer) {
        wlr_log(WLR_ERROR, "Failed to allocate renderer");
        wlr_egl_unset_current(egl);
        eglDestroyContext(display, context);
        eglTerminate(display);
        free(egl);
        return NULL;
    }
    wlr_log(WLR_DEBUG, "Allocated renderer: %p", renderer);

    renderer->wlr_renderer.impl = &renderer_impl;
    renderer->egl = egl;
    renderer->drm_fd = -1;

    // Log GL details
    const char *vendor = (const char *)glGetString(GL_VENDOR);
    const char *renderer_str = (const char *)glGetString(GL_RENDERER);
    const char *version = (const char *)glGetString(GL_VERSION);
    const char *shading_lang = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    const char *exts_str = (const char *)glGetString(GL_EXTENSIONS);
    wlr_log(WLR_INFO, "wlr_gles2_renderer GL_VENDOR: %s", vendor ? vendor : "unknown");
    wlr_log(WLR_INFO, "wlr_gles2_renderer GL_RENDERER: %s", renderer_str ? renderer_str : "unknown");
    wlr_log(WLR_INFO, "wlr_gles2_renderer GL_VERSION: %s", version ? version : "unknown");
    wlr_log(WLR_INFO, "wlr_gles2_renderer GL_SHADING_LANGUAGE_VERSION: %s", shading_lang ? shading_lang : "unknown");
    wlr_log(WLR_INFO, "Supported GLES2 extensions: %s", exts_str ? exts_str : "none");

    // Log Zink usage
    const char *driver = getenv("MESA_LOADER_DRIVER_OVERRIDE");
    if (driver && strcmp(driver, "zink") == 0) {
        wlr_log(WLR_INFO, "Using Zink (Vulkan) driver for hardware acceleration");
    } else {
        wlr_log(WLR_DEBUG, "MESA_LOADER_DRIVER_OVERRIDE: %s", driver ? driver : "unset");
    }

    // Initialize renderer
    wlr_renderer_init(&renderer->wlr_renderer, &renderer_impl, WLR_BUFFER_CAP_DMABUF);
    wlr_log(WLR_DEBUG, "Renderer initialized with DMABUF capability");

    // Initialize buffers and textures lists
    wl_list_init(&renderer->buffers);
    wl_list_init(&renderer->textures);
    wlr_log(WLR_DEBUG, "Initialized renderer->buffers: prev=%p, next=%p",
            renderer->buffers.prev, renderer->buffers.next);

    // Check required extensions
    if (!exts_str) {
        wlr_log(WLR_ERROR, "Failed to get GL_EXTENSIONS");
        wlr_egl_unset_current(egl);
        free(renderer);
        eglDestroyContext(display, context);
        eglTerminate(display);
        free(egl);
        return NULL;
    }
    renderer->exts_str = exts_str;

    if (!check_gl_ext(exts_str, "GL_EXT_texture_format_BGRA8888")) {
        wlr_log(WLR_ERROR, "BGRA8888 format not supported by GLES2");
        wlr_egl_unset_current(egl);
        free(renderer);
        eglDestroyContext(display, context);
        eglTerminate(display);
        free(egl);
        return NULL;
    }
    if (!check_gl_ext(exts_str, "GL_EXT_unpack_subimage")) {
        wlr_log(WLR_ERROR, "GL_EXT_unpack_subimage not supported");
        wlr_egl_unset_current(egl);
        free(renderer);
        eglDestroyContext(display, context);
        eglTerminate(display);
        free(egl);
        return NULL;
    }

    // Load optional extensions
    renderer->exts.EXT_read_format_bgra = check_gl_ext(exts_str, "GL_EXT_read_format_bgra");
    renderer->exts.EXT_texture_type_2_10_10_10_REV = check_gl_ext(exts_str, "GL_EXT_texture_type_2_10_10_10_REV");
    renderer->exts.OES_texture_half_float_linear = check_gl_ext(exts_str, "GL_OES_texture_half_float_linear");
    renderer->exts.EXT_texture_norm16 = check_gl_ext(exts_str, "GL_EXT_texture_norm16");

    if (check_gl_ext(exts_str, "GL_KHR_debug")) {
        renderer->exts.KHR_debug = true;
        load_gl_proc(&renderer->procs.glDebugMessageCallbackKHR, "glDebugMessageCallbackKHR");
        load_gl_proc(&renderer->procs.glDebugMessageControlKHR, "glDebugMessageControlKHR");
        glEnable(GL_DEBUG_OUTPUT_KHR);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
        renderer->procs.glDebugMessageCallbackKHR(gles2_log, NULL);
        renderer->procs.glDebugMessageControlKHR(GL_DONT_CARE, GL_DEBUG_TYPE_POP_GROUP_KHR, GL_DONT_CARE, 0, NULL, GL_FALSE);
        renderer->procs.glDebugMessageControlKHR(GL_DONT_CARE, GL_DEBUG_TYPE_PUSH_GROUP_KHR, GL_DONT_CARE, 0, NULL, GL_FALSE);
    }

    if (check_gl_ext(exts_str, "GL_OES_EGL_image_external")) {
        renderer->exts.OES_egl_image_external = true;
        load_gl_proc(&renderer->procs.glEGLImageTargetTexture2DOES, "glEGLImageTargetTexture2DOES");
    }

    if (check_gl_ext(exts_str, "GL_OES_EGL_image")) {
        renderer->exts.OES_egl_image = true;
        load_gl_proc(&renderer->procs.glEGLImageTargetRenderbufferStorageOES, "glEGLImageTargetRenderbufferStorageOES");
    }

    if (check_gl_ext(exts_str, "GL_EXT_disjoint_timer_query")) {
        renderer->exts.EXT_disjoint_timer_query = true;
        load_gl_proc(&renderer->procs.glGenQueriesEXT, "glGenQueriesEXT");
        load_gl_proc(&renderer->procs.glDeleteQueriesEXT, "glDeleteQueriesEXT");
        load_gl_proc(&renderer->procs.glQueryCounterEXT, "glQueryCounterEXT");
        load_gl_proc(&renderer->procs.glGetQueryObjectivEXT, "glGetQueryObjectivEXT");
        load_gl_proc(&renderer->procs.glGetQueryObjectui64vEXT, "glGetQueryObjectui64vEXT");
        if (eglGetProcAddress("glGetInteger64vEXT")) {
            load_gl_proc(&renderer->procs.glGetInteger64vEXT, "glGetInteger64vEXT");
        } else {
            load_gl_proc(&renderer->procs.glGetInteger64vEXT, "glGetInteger64v");
        }
    }

    // Initialize shaders
    wlr_log(WLR_DEBUG, "Starting quad shader setup in surfaceless");
    push_gles2_debug(renderer);

    GLuint prog;
    renderer->shaders.quad.program = prog = link_program(renderer, common_vert_src, quad_frag_src);
    if (!renderer->shaders.quad.program) {
        wlr_log(WLR_ERROR, "Failed to create quad shader program");
        goto error;
    }
    renderer->shaders.quad.proj = glGetUniformLocation(prog, "proj");
    renderer->shaders.quad.color = glGetUniformLocation(prog, "color");
    renderer->shaders.quad.pos_attrib = glGetAttribLocation(prog, "pos");
    wlr_log(WLR_DEBUG, "Quad shader created: program=%u, proj=%d, color=%d, pos_attrib=%d",
            prog, renderer->shaders.quad.proj, renderer->shaders.quad.color, renderer->shaders.quad.pos_attrib);

    renderer->shaders.tex_rgba.program = prog = link_program(renderer, common_vert_src, tex_rgba_frag_src);
    if (!renderer->shaders.tex_rgba.program) {
        wlr_log(WLR_ERROR, "Failed to create tex_rgba shader program");
        glDeleteProgram(renderer->shaders.quad.program);
        goto error;
    }
    renderer->shaders.tex_rgba.proj = glGetUniformLocation(prog, "proj");
    renderer->shaders.tex_rgba.tex_proj = glGetUniformLocation(prog, "tex_proj");
    renderer->shaders.tex_rgba.tex = glGetUniformLocation(prog, "tex");
    renderer->shaders.tex_rgba.alpha = glGetUniformLocation(prog, "alpha");
    renderer->shaders.tex_rgba.pos_attrib = glGetAttribLocation(prog, "pos");

    renderer->shaders.tex_rgbx.program = prog = link_program(renderer, common_vert_src, tex_rgbx_frag_src);
    if (!renderer->shaders.tex_rgbx.program) {
        wlr_log(WLR_ERROR, "Failed to create tex_rgbx shader program");
        glDeleteProgram(renderer->shaders.quad.program);
        glDeleteProgram(renderer->shaders.tex_rgba.program);
        goto error;
    }
    renderer->shaders.tex_rgbx.proj = glGetUniformLocation(prog, "proj");
    renderer->shaders.tex_rgbx.tex_proj = glGetUniformLocation(prog, "tex_proj");
    renderer->shaders.tex_rgbx.tex = glGetUniformLocation(prog, "tex");
    renderer->shaders.tex_rgbx.alpha = glGetUniformLocation(prog, "alpha");
    renderer->shaders.tex_rgbx.pos_attrib = glGetAttribLocation(prog, "pos");

    if (renderer->exts.OES_egl_image_external) {
        renderer->shaders.tex_ext.program = prog = link_program(renderer, common_vert_src, tex_external_frag_src);
        if (!renderer->shaders.tex_ext.program) {
            wlr_log(WLR_ERROR, "Failed to create tex_ext shader program");
            glDeleteProgram(renderer->shaders.quad.program);
            glDeleteProgram(renderer->shaders.tex_rgba.program);
            glDeleteProgram(renderer->shaders.tex_rgbx.program);
            goto error;
        }
        renderer->shaders.tex_ext.proj = glGetUniformLocation(prog, "proj");
        renderer->shaders.tex_ext.tex_proj = glGetUniformLocation(prog, "tex_proj");
        renderer->shaders.tex_ext.tex = glGetUniformLocation(prog, "tex");
        renderer->shaders.tex_ext.alpha = glGetUniformLocation(prog, "alpha");
        renderer->shaders.tex_ext.pos_attrib = glGetAttribLocation(prog, "pos");
    }

    pop_gles2_debug(renderer);
    wlr_egl_unset_current(egl);

    // Initialize SHM texture formats
    get_gles2_shm_formats(renderer, &renderer->shm_texture_formats);

    // Check for timeline support
    int drm_fd = wlr_renderer_get_drm_fd(&renderer->wlr_renderer);
    uint64_t cap_syncobj_timeline;
    if (drm_fd >= 0 && drmGetCap(drm_fd, DRM_CAP_SYNCOBJ_TIMELINE, &cap_syncobj_timeline) == 0) {
        renderer->wlr_renderer.features.timeline = egl->procs.eglDupNativeFenceFDANDROID &&
            egl->procs.eglWaitSyncKHR && cap_syncobj_timeline != 0;
    }

    wlr_log(WLR_DEBUG, "Exiting wlr_gles2_renderer_create_surfaceless");
    return &renderer->wlr_renderer;

error:
    glDeleteProgram(renderer->shaders.quad.program);
    glDeleteProgram(renderer->shaders.tex_rgba.program);
    glDeleteProgram(renderer->shaders.tex_rgbx.program);
    glDeleteProgram(renderer->shaders.tex_ext.program);
    pop_gles2_debug(renderer);
    if (renderer->exts.KHR_debug) {
        glDisable(GL_DEBUG_OUTPUT_KHR);
        renderer->procs.glDebugMessageCallbackKHR(NULL, NULL);
    }
    wlr_egl_unset_current(egl);
    free(renderer);
    eglDestroyContext(display, context);
    eglTerminate(display);
    free(egl);
    return NULL;
}