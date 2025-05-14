#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>  // For setenv
#include <assert.h>
#include <drm_fourcc.h>
#include <gbm.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wayland-server-protocol.h>
#include <wayland-util.h>
#include <wlr/render/egl.h>
#include <wlr/render/interface.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/util/box.h>
#include <wlr/util/log.h>
#include "render/egl.h"
#include "render/gles2.h"
#include "render/pixel_format.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-server-core.h>
#include <wlr/render/egl.h>
#include <wlr/render/interface.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/util/log.h>
#include <wlr/render/gles2.h>
#include <string.h>  // For strstr
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/eglext.h>
#include <xf86drm.h>
#include "util/time.h"
#include <wlr/util/transform.h>


static struct wlr_gles2_buffer *create_buffer(struct wlr_gles2_renderer *renderer,
    struct wlr_buffer *wlr_buffer);

__attribute__((unused))
static bool gles2_read_pixels(struct wlr_renderer *wlr_renderer,
    uint32_t drm_format, uint32_t stride, uint32_t width, uint32_t height,
    uint32_t src_x, uint32_t src_y, uint32_t dst_x, uint32_t dst_y, void *data);

// For version checking and vertex array support
#ifndef GL_MAJOR_VERSION
#define GL_MAJOR_VERSION 0x821B
#define GL_MINOR_VERSION 0x821C
#endif



struct wlr_renderer *wlr_gles2_renderer_create_surfaceless(void);






#include <GLES2/gl2.h>
#include "render/gles2.h"

// Colored quads
const GLchar quad_vertex_src[] =
"uniform mat3 proj;\n"
"uniform vec4 color;\n"
"attribute vec2 pos;\n"
"attribute vec2 texcoord;\n"
"varying vec4 v_color;\n"
"varying vec2 v_texcoord;\n"
"\n"
"void main() {\n"
"	gl_Position = vec4(proj * vec3(pos, 1.0), 1.0);\n"
"	v_color = color;\n"
"	v_texcoord = texcoord;\n"
"}\n";

const GLchar quad_fragment_src[] =
"precision mediump float;\n"
"varying vec4 v_color;\n"
"varying vec2 v_texcoord;\n"
"\n"
"void main() {\n"
"	gl_FragColor = v_color;\n"
"}\n";

// Textured quads
const GLchar tex_vertex_src[] =
"uniform mat3 proj;\n"
"uniform bool invert_y;\n"
"attribute vec2 pos;\n"
"attribute vec2 texcoord;\n"
"varying vec2 v_texcoord;\n"
"\n"
"void main() {\n"
"	gl_Position = vec4(proj * vec3(pos, 1.0), 1.0);\n"
"	if (invert_y) {\n"
"		v_texcoord = vec2(texcoord.x, 1.0 - texcoord.y);\n"
"	} else {\n"
"		v_texcoord = texcoord;\n"
"	}\n"
"}\n";

const GLchar tex_fragment_src_rgba[] =
"precision mediump float;\n"
"varying vec2 v_texcoord;\n"
"uniform sampler2D tex;\n"
"uniform float alpha;\n"
"\n"
"void main() {\n"
"	gl_FragColor = texture2D(tex, v_texcoord) * alpha;\n"
"}\n";

const GLchar tex_fragment_src_rgbx[] =
"precision mediump float;\n"
"varying vec2 v_texcoord;\n"
"uniform sampler2D tex;\n"
"uniform float alpha;\n"
"\n"
"void main() {\n"
"	gl_FragColor = vec4(texture2D(tex, v_texcoord).rgb, 1.0) * alpha;\n"
"}\n";

const GLchar tex_fragment_src_external[] =
"#extension GL_OES_EGL_image_external : require\n\n"
"precision mediump float;\n"
"varying vec2 v_texcoord;\n"
"uniform samplerExternalOES texture0;\n"
"uniform float alpha;\n"
"\n"
"void main() {\n"
"	gl_FragColor = texture2D(texture0, v_texcoord) * alpha;\n"
"}\n";





static const struct wlr_renderer_impl renderer_impl;
static const struct wlr_render_timer_impl render_timer_impl;

// Move check_list_integrity definition earlier to avoid implicit declaration
static bool check_list_integrity(struct wl_list *list, const char *name) {
    if (!list) {
        wlr_log(WLR_ERROR, "Null list passed to check_list_integrity for %s", name);
        return false;
    }
    if (list->prev == NULL || list->next == NULL) {
        wlr_log(WLR_ERROR, "List %s has uninitialized prev/next pointers", name);
        return false;
    }
    if (list->prev->next != list || list->next->prev != list) {
        wlr_log(WLR_ERROR, "List %s is corrupted (prev/next mismatch)", name);
        return false;
    }
    return true;
}

// Matrix function implementations to avoid implicit declaration errors
static void wlr_matrix_multiply(float dest[9], const float a[9], const float b[9]) {
    float result[9];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i * 3 + j] = 0;
            for (int k = 0; k < 3; k++) {
                result[i * 3 + j] += a[i * 3 + k] * b[k * 3 + j];
            }
        }
    }
    memcpy(dest, result, 9 * sizeof(float));
}
__attribute__((unused))
static void wlr_matrix_transpose(float dest[9], const float src[9]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            dest[i * 3 + j] = src[j * 3 + i];
        }
    }
}

static void wlr_matrix_identity(float matrix[9]) {
    matrix[0] = 1; matrix[1] = 0; matrix[2] = 0;
    matrix[3] = 0; matrix[4] = 1; matrix[5] = 0;
    matrix[6] = 0; matrix[7] = 0; matrix[8] = 1;
}

static void wlr_matrix_scale(float matrix[9], float x, float y) {
    float scale[9] = { x, 0, 0, 0, y, 0, 0, 0, 1 };
    wlr_matrix_multiply(matrix, matrix, scale);
}

static void wlr_matrix_translate(float matrix[9], float x, float y) {
    float translate[9] = { 1, 0, x, 0, 1, y, 0, 0, 1 };
    wlr_matrix_multiply(matrix, matrix, translate);
}
__attribute__((unused))
static void wlr_matrix_project_box(float matrix[9], const struct wlr_box *box,
        enum wl_output_transform transform, float rotation, const float projection[9]) {
    wlr_matrix_identity(matrix);
    wlr_matrix_scale(matrix, 2.0f / box->width, 2.0f / box->height);
    wlr_matrix_translate(matrix, -box->x - box->width / 2.0f, -box->y - box->height / 2.0f);
    // Ignore transform and rotation for simplicity; adjust if needed
    wlr_matrix_multiply(matrix, projection, matrix);
}


// Add these function declarations or implementations
static GLuint gl_shader_program_create(const char *vert_src, const char *frag_src) {
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(vert, 1, &vert_src, NULL);
    glShaderSource(frag, 1, &frag_src, NULL);
    
    glCompileShader(vert);
    glCompileShader(frag);
    
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    
    glDeleteShader(vert);
    glDeleteShader(frag);
    
    return prog;
}

bool wlr_renderer_is_gles2(struct wlr_renderer *wlr_renderer) {
	return wlr_renderer->impl == &renderer_impl;
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

struct wlr_gles2_renderer *gles2_get_renderer(struct wlr_renderer *wlr_renderer) {
    assert(wlr_renderer_is_gles2(wlr_renderer));
    return (struct wlr_gles2_renderer *)wlr_renderer; // Directly cast wlr_renderer to the correct type
}

static struct wlr_gles2_renderer *gles2_get_renderer_in_context(struct wlr_renderer *wlr_renderer) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);
    if (eglGetCurrentContext() != renderer->egl->context) {
        wlr_log(WLR_DEBUG, "EGL context not current, making it current");
        struct wlr_egl_context prev_ctx = {
            .display = eglGetCurrentDisplay(),
            .context = eglGetCurrentContext(),
            .draw_surface = eglGetCurrentSurface(EGL_DRAW),
            .read_surface = eglGetCurrentSurface(EGL_READ),
        };
        if (!wlr_egl_make_current(renderer->egl, &prev_ctx)) {
            wlr_log(WLR_ERROR, "Failed to make EGL context current");
            return NULL;
        }
    }
    if (!renderer->current_buffer) {
        wlr_log(WLR_DEBUG, "No buffer bound, proceeding without buffer");
    }
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
    if (!buffer) {
        wlr_log(WLR_ERROR, "Attempt to destroy NULL buffer");
        return;
    }
    printf("[DEBUG] Starting destruction of buffer %p (texture: %u, fbo: %u)\n", 
           buffer, buffer->tex, buffer->fbo);
    wl_list_remove(&buffer->link);
    wlr_addon_finish(&buffer->addon);
    if (!buffer->renderer || !buffer->renderer->egl) {
        wlr_log(WLR_ERROR, "Invalid renderer for buffer %p", buffer);
        free(buffer);
        return;
    }
    struct wlr_egl_context prev_ctx = {
        .display = eglGetCurrentDisplay(),
        .context = eglGetCurrentContext(),
        .draw_surface = eglGetCurrentSurface(EGL_DRAW),
        .read_surface = eglGetCurrentSurface(EGL_READ),
    };
    if (!wlr_egl_make_current(buffer->renderer->egl, &prev_ctx)) {
        wlr_log(WLR_ERROR, "Failed to make EGL context current in destroy_buffer");
        wlr_egl_restore_context(&prev_ctx);
        free(buffer);
        return;
    }
    push_gles2_debug(buffer->renderer);
    if (buffer->tex && glIsTexture(buffer->tex)) {
        glDeleteTextures(1, &buffer->tex);
    }
    if (buffer->fbo && glIsFramebuffer(buffer->fbo)) {
        glDeleteFramebuffers(1, &buffer->fbo);
    }
    if (buffer->rbo && glIsRenderbuffer(buffer->rbo)) {
        glDeleteRenderbuffers(1, &buffer->rbo);
    }
    pop_gles2_debug(buffer->renderer);
    if (buffer->image) {
        wlr_egl_destroy_image(buffer->renderer->egl, buffer->image);
    }
    wlr_egl_restore_context(&prev_ctx);
    buffer->buffer = NULL;
    buffer->renderer = NULL;
    buffer->image = NULL;
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

static const struct wlr_drm_format_set *renderer_get_texture_formats(
        struct wlr_renderer *renderer, uint32_t buffer_caps) {
    // Implementation depends on your renderer
    // Example minimal implementation that returns a static format set
    static struct wlr_drm_format_set format_set = {0};
    
    // Initialize if needed (only once)
    if (format_set.len == 0) {
        // Add at least one format - adjust based on your actual renderer capabilities
        // This is a very simplified example
        wlr_drm_format_set_add(&format_set, DRM_FORMAT_ARGB8888, 1); 
    }
    
    return &format_set;
}

struct wlr_gles2_buffer *gles2_buffer_get_or_create(struct wlr_gles2_renderer *renderer,
        struct wlr_buffer *wlr_buffer) {
    struct wlr_addon *addon =
        wlr_addon_find(&wlr_buffer->addons, renderer, &buffer_addon_impl);
    if (addon) {
        struct wlr_gles2_buffer *buffer = wl_container_of(addon, buffer, addon);
        return buffer;
    }
    return create_buffer(renderer, wlr_buffer);
}


static struct wlr_gles2_buffer *create_buffer(struct wlr_gles2_renderer *renderer,
        struct wlr_buffer *wlr_buffer) {
    wlr_log(WLR_DEBUG, "Creating buffer for %p (size: %dx%d)", wlr_buffer,
            wlr_buffer->width, wlr_buffer->height);

    if (!renderer || !wlr_buffer || !renderer->egl) {
        wlr_log(WLR_ERROR, "Invalid input: renderer=%p, buffer=%p, egl=%p",
                renderer, wlr_buffer, renderer ? renderer->egl : NULL);
        return NULL;
    }

    struct wlr_gles2_buffer *buffer = calloc(1, sizeof(*buffer));
    if (!buffer) {
        wlr_log(WLR_ERROR, "Failed to allocate wlr_gles2_buffer");
        return NULL;
    }

    buffer->buffer = wlr_buffer;
    buffer->renderer = renderer;
    buffer->tex = 0;
    buffer->fbo = 0;
    buffer->rbo = 0;
    buffer->image = EGL_NO_IMAGE_KHR;

    void *data_ptr;
    uint32_t format;
    size_t stride;
    if (!wlr_buffer_begin_data_ptr_access(wlr_buffer, WLR_BUFFER_DATA_PTR_ACCESS_READ, 
            &data_ptr, &format, &stride)) {
        wlr_log(WLR_ERROR, "Failed to access buffer data");
        free(buffer);
        return NULL;
    }

    struct wlr_egl_context prev_ctx = {
        .display = eglGetCurrentDisplay(),
        .context = eglGetCurrentContext(),
        .draw_surface = eglGetCurrentSurface(EGL_DRAW),
        .read_surface = eglGetCurrentSurface(EGL_READ),
    };
    if (!wlr_egl_make_current(renderer->egl, &prev_ctx)) {
        wlr_log(WLR_ERROR, "Failed to make EGL context current");
        wlr_buffer_end_data_ptr_access(wlr_buffer);
        free(buffer);
        return NULL;
    }

    push_gles2_debug(renderer);
    glFinish();
    glGenTextures(1, &buffer->tex);
    if (buffer->tex == 0) {
        wlr_log(WLR_ERROR, "Failed to generate texture");
        goto error_cleanup;
    }
    wlr_log(WLR_DEBUG, "Generated texture ID: %u for buffer %p", buffer->tex, buffer);

    glBindTexture(GL_TEXTURE_2D, buffer->tex);
    GLboolean is_texture = glIsTexture(buffer->tex);
    wlr_log(WLR_DEBUG, "Texture %u validity check: %d", buffer->tex, is_texture);
    if (!is_texture) {
        wlr_log(WLR_ERROR, "Generated texture ID %u is not valid", buffer->tex);
        goto error_cleanup;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLint bound_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_texture);
    wlr_log(WLR_DEBUG, "Texture %u bound, verified binding: %d", buffer->tex, bound_texture);
    if ((GLuint)bound_texture != buffer->tex) {
        wlr_log(WLR_ERROR, "Texture binding mismatch: expected %u, got %d", buffer->tex, bound_texture);
        goto error_cleanup;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wlr_buffer->width, wlr_buffer->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data_ptr);
    GLenum tex_error = glGetError();
    if (tex_error != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "glTexImage2D failed with error: 0x%x", tex_error);
        goto error_cleanup;
    }

    wlr_buffer_end_data_ptr_access(wlr_buffer);

    glGenFramebuffers(1, &buffer->fbo);
    if (buffer->fbo == 0) {
        wlr_log(WLR_ERROR, "Failed to generate framebuffer");
        goto error_cleanup;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, buffer->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->tex, 0);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        wlr_log(WLR_ERROR, "Framebuffer incomplete, status: 0x%x", status);
        goto error_cleanup;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    pop_gles2_debug(renderer);

    wlr_addon_init(&buffer->addon, &wlr_buffer->addons, renderer, &buffer_addon_impl);
    wl_list_init(&buffer->link);

    // Ensure renderer->buffers is initialized before checking integrity
    if (renderer->buffers.prev == NULL || renderer->buffers.next == NULL) {
        wlr_log(WLR_DEBUG, "Initializing renderer->buffers list");
        wl_list_init(&renderer->buffers);
    }

    // Check buffer list integrity before insertion
    if (!check_list_integrity(&renderer->buffers, "renderer->buffers")) {
        wlr_log(WLR_ERROR, "Buffer list corrupted, reinitializing");
        wl_list_init(&renderer->buffers);
    }
    wl_list_insert(&renderer->buffers, &buffer->link);
    wlr_log(WLR_DEBUG, "Inserted buffer %p into renderer->buffers", buffer);

    wlr_egl_restore_context(&prev_ctx);
    return buffer;

error_cleanup:
    push_gles2_debug(renderer);
    if (buffer->fbo) glDeleteFramebuffers(1, &buffer->fbo);
    if (buffer->tex) glDeleteTextures(1, &buffer->tex);
    pop_gles2_debug(renderer);
    wlr_egl_restore_context(&prev_ctx);
    wlr_buffer_end_data_ptr_access(wlr_buffer);
    free(buffer);
    return NULL;
}

/*
static bool gles2_bind_buffer(struct wlr_renderer *wlr_renderer, struct wlr_buffer *wlr_buffer) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);
    wlr_log(WLR_DEBUG, "Binding buffer %p, current_buffer: %p", wlr_buffer, renderer->current_buffer);

    if (renderer->current_buffer) {
        wlr_log(WLR_DEBUG, "Unbinding current buffer %p", renderer->current_buffer);
        assert(eglGetCurrentContext() == renderer->egl->context);
        push_gles2_debug(renderer);
        glFlush();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        pop_gles2_debug(renderer);
        wlr_buffer_unlock(renderer->current_buffer->buffer);
        renderer->current_buffer = NULL;
    }

    if (!wlr_buffer) {
        wlr_log(WLR_DEBUG, "Null buffer, unsetting EGL context");
        wlr_egl_unset_current(renderer->egl);
        return true;
    }

    struct wlr_egl_context prev_ctx = {
        .display = eglGetCurrentDisplay(),
        .context = eglGetCurrentContext(),
        .draw_surface = eglGetCurrentSurface(EGL_DRAW),
        .read_surface = eglGetCurrentSurface(EGL_READ),
    };
    if (!wlr_egl_make_current(renderer->egl, &prev_ctx)) {
        wlr_log(WLR_ERROR, "Failed to make EGL context current");
        return false;
    }

    if (!check_list_integrity(&renderer->buffers, "renderer->buffers")) {
        wlr_log(WLR_ERROR, "Buffer list corrupted, reinitializing");
        wl_list_init(&renderer->buffers);
    }

    struct wlr_gles2_buffer *buffer = gles2_buffer_get_or_create(renderer, wlr_buffer);
    if (!buffer) {
        wlr_log(WLR_ERROR, "Failed to get or create buffer");
        wlr_egl_restore_context(&prev_ctx);
        return false;
    }

    int buffer_count = wl_list_length(&renderer->buffers);
    const int MAX_BUFFERS = 16;
    wlr_log(WLR_DEBUG, "Current buffer count: %d", buffer_count);
    if (buffer_count >= MAX_BUFFERS) {
        wlr_log(WLR_DEBUG, "Buffer limit (%d) reached, removing excess", MAX_BUFFERS);
        int to_remove = buffer_count - MAX_BUFFERS + 1;
        struct wlr_gles2_buffer *old_buffer, *tmp;
        wl_list_for_each_safe(old_buffer, tmp, &renderer->buffers, link) {
            if (to_remove <= 0) break;
            if (old_buffer != renderer->current_buffer && old_buffer != buffer) {
                wlr_log(WLR_DEBUG, "Destroying old buffer %p (texture: %u)", old_buffer, old_buffer->tex);
                destroy_buffer(old_buffer);
                to_remove--;
            }
        }
    }

    wlr_log(WLR_DEBUG, "Binding buffer %p, FBO %u", buffer, buffer->fbo);
    if (!buffer || buffer->fbo == 0) {
        wlr_log(WLR_ERROR, "Invalid buffer or FBO");
        wlr_egl_restore_context(&prev_ctx);
        return false;
    }
    wlr_buffer_lock(wlr_buffer);
    renderer->current_buffer = buffer;

    struct wlr_dmabuf_attributes dmabuf;
    if (wlr_buffer_get_dmabuf(wlr_buffer, &dmabuf)) {
        wlr_log(WLR_DEBUG, "DMA-BUF buffer, texture %u assumed valid", buffer->tex);
    } else {
        void *data;
        uint32_t format;
        size_t stride;
        if (wlr_buffer_begin_data_ptr_access(wlr_buffer, WLR_BUFFER_DATA_PTR_ACCESS_READ,
                &data, &format, &stride)) {
            wlr_log(WLR_DEBUG, "Updating SHM texture %u: %dx%d", buffer->tex,
                    wlr_buffer->width, wlr_buffer->height);
            push_gles2_debug(renderer);

            if (buffer->tex == 0) {
                if (buffer->fbo) glDeleteFramebuffers(1, &buffer->fbo);
                glGenTextures(1, &buffer->tex);
                glBindTexture(GL_TEXTURE_2D, buffer->tex);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glGenFramebuffers(1, &buffer->fbo);
            }

            glBindTexture(GL_TEXTURE_2D, buffer->tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wlr_buffer->width, wlr_buffer->height,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            GLenum err = glGetError();
            if (err != GL_NO_ERROR) {
                wlr_log(WLR_ERROR, "glTexImage2D failed: 0x%x", err);
                wlr_buffer_end_data_ptr_access(wlr_buffer);
                wlr_buffer_unlock(wlr_buffer);
                renderer->current_buffer = NULL;
                pop_gles2_debug(renderer);
                wlr_egl_restore_context(&prev_ctx);
                return false;
            }
            wlr_buffer_end_data_ptr_access(wlr_buffer);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, buffer->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->tex, 0);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        wlr_log(WLR_ERROR, "Framebuffer incomplete: 0x%x", status);
        wlr_buffer_unlock(wlr_buffer);
        renderer->current_buffer = NULL;
        pop_gles2_debug(renderer);
        wlr_egl_restore_context(&prev_ctx);
        return false;
    }

    wlr_log(WLR_DEBUG, "Bound FBO %u successfully", buffer->fbo);
    pop_gles2_debug(renderer);
    wlr_egl_restore_context(&prev_ctx);
    return true;
}*/






__attribute__((unused))
static void gles2_begin(struct wlr_renderer *wlr_renderer, uint32_t width, uint32_t height) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);
    if (eglGetCurrentContext() != renderer->egl->context) {
        struct wlr_egl_context prev_ctx = {
            .display = eglGetCurrentDisplay(),
            .context = eglGetCurrentContext(),
            .draw_surface = eglGetCurrentSurface(EGL_DRAW),
            .read_surface = eglGetCurrentSurface(EGL_READ),
        };
        if (!wlr_egl_make_current(renderer->egl, &prev_ctx)) {
            wlr_log(WLR_ERROR, "Failed to make EGL context current in gles2_begin");
            wlr_egl_restore_context(&prev_ctx);
            return;
        }
        wlr_egl_restore_context(&prev_ctx);
    }
    push_gles2_debug(renderer);
    glViewport(0, 0, width, height);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    pop_gles2_debug(renderer);
}

__attribute__((unused))
static void gles2_end(struct wlr_renderer *wlr_renderer) {
	gles2_get_renderer_in_context(wlr_renderer);
	// no-op
}

__attribute__((unused))
static void gles2_clear(struct wlr_renderer *wlr_renderer,
		const float color[static 4]) {
	struct wlr_gles2_renderer *renderer =
		gles2_get_renderer_in_context(wlr_renderer);

	push_gles2_debug(renderer);
	glClearColor(color[0], color[1], color[2], color[3]);
	glClear(GL_COLOR_BUFFER_BIT);
	pop_gles2_debug(renderer);
}
__attribute__((unused))
static void gles2_scissor(struct wlr_renderer *wlr_renderer,
		struct wlr_box *box) {
	struct wlr_gles2_renderer *renderer =
		gles2_get_renderer_in_context(wlr_renderer);

	push_gles2_debug(renderer);
	if (box != NULL) {
		glScissor(box->x, box->y, box->width, box->height);
		glEnable(GL_SCISSOR_TEST);
	} else {
		glDisable(GL_SCISSOR_TEST);
	}
	pop_gles2_debug(renderer);
}


/*
__attribute__((unused))
static bool gles2_render_subtexture_with_matrix(
        struct wlr_renderer *wlr_renderer, struct wlr_texture *wlr_texture,
        const struct wlr_fbox *box, const float matrix[static 9], float alpha) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer_in_context(wlr_renderer);
    struct wlr_gles2_texture *texture = gles2_get_texture(wlr_texture);
    wlr_log(WLR_DEBUG, "Rendering texture %u: %dx%d at %f,%f (%fx%f), alpha=%f",
            texture->tex, wlr_texture->width, wlr_texture->height,
            box->x, box->y, box->width, box->height, alpha);

    assert(texture->renderer == renderer);

    struct wlr_gles2_tex_shader *shader = NULL;
    switch (texture->target) {
    case GL_TEXTURE_2D:
        shader = texture->has_alpha ? &renderer->shaders.tex_rgba : &renderer->shaders.tex_rgbx;
        break;
    case GL_TEXTURE_EXTERNAL_OES:
        shader = &renderer->shaders.tex_ext;
        if (!renderer->exts.OES_egl_image_external) {
            wlr_log(WLR_ERROR, "GL_TEXTURE_EXTERNAL_OES not supported");
            return false;
        }
        break;
    default:
        abort();
    }

    float gl_matrix[9];
    wlr_matrix_multiply(gl_matrix, renderer->projection_matrix, matrix);
    wlr_matrix_multiply(gl_matrix, flip_180, gl_matrix);
    wlr_matrix_transpose(gl_matrix, gl_matrix);

    push_gles2_debug(renderer);

    if (!texture->has_alpha && alpha == 1.0) {
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(texture->target, texture->tex);
    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glUseProgram(shader->program);
    glUniformMatrix3fv(shader->proj, 1, GL_FALSE, gl_matrix);
    glUniform1i(shader->tex, 0);
    glUniform1f(shader->alpha, alpha);
    glUniform1i(shader->invert_y, 0);

    const GLfloat verts[] = {
        box->x + box->width, box->y,               // top right
        box->x,              box->y,               // top left
        box->x + box->width, box->y + box->height, // bottom right
        box->x,              box->y + box->height, // bottom left
    };
    const GLfloat texcoord[] = {
        1.0f, 0.0f, // top right
        0.0f, 0.0f, // top left
        1.0f, 1.0f, // bottom right
        0.0f, 1.0f, // bottom left
    };

    glVertexAttribPointer(shader->pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glVertexAttribPointer(shader->tex_attrib, 2, GL_FLOAT, GL_FALSE, 0, texcoord);

    glEnableVertexAttribArray(shader->pos_attrib);
    glEnableVertexAttribArray(shader->tex_attrib);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(shader->pos_attrib);
    glDisableVertexAttribArray(shader->tex_attrib);

    glBindTexture(texture->target, 0);
    pop_gles2_debug(renderer);
    return true;
}

__attribute__((unused))
static void gles2_render_quad_with_matrix(struct wlr_renderer *wlr_renderer,
        const float color[static 4], const float matrix[static 9]) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer_in_context(wlr_renderer);

    float gl_matrix[9];
    wlr_matrix_multiply(gl_matrix, renderer->projection_matrix, matrix);
    wlr_matrix_multiply(gl_matrix, flip_180, gl_matrix);
    wlr_matrix_transpose(gl_matrix, gl_matrix);

    push_gles2_debug(renderer);

    if (color[3] == 1.0) {
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
    }

    glUseProgram(renderer->shaders.quad.program);
    glUniformMatrix3fv(renderer->shaders.quad.proj, 1, GL_FALSE, gl_matrix);
    glUniform4f(renderer->shaders.quad.color, color[0], color[1], color[2], color[3]);

    const GLfloat verts[] = {
        1.0f,  1.0f, // top right
       -1.0f,  1.0f, // top left
        1.0f, -1.0f, // bottom right
       -1.0f, -1.0f, // bottom left
    };

    glVertexAttribPointer(renderer->shaders.quad.pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glEnableVertexAttribArray(renderer->shaders.quad.pos_attrib);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(renderer->shaders.quad.pos_attrib);

    pop_gles2_debug(renderer);
}*/


__attribute__((unused))
static const uint32_t *gles2_get_shm_texture_formats(
        struct wlr_renderer *wlr_renderer, size_t *len) {
   // struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);
    
    // Explicitly include both ARGB8888 and XRGB8888
    static const uint32_t formats[] = {
        WL_SHM_FORMAT_ARGB8888,
        WL_SHM_FORMAT_XRGB8888,
        // Add other formats as needed
    };
    
    *len = sizeof(formats) / sizeof(formats[0]);
    return formats;
}

__attribute__((unused))
static const struct wlr_drm_format_set *gles2_get_dmabuf_texture_formats(
		struct wlr_renderer *wlr_renderer) {
	struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);
	return wlr_egl_get_dmabuf_texture_formats(renderer->egl);
}

static const struct wlr_drm_format_set *gles2_get_render_formats(
		struct wlr_renderer *wlr_renderer) {
	struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);
	return wlr_egl_get_dmabuf_render_formats(renderer->egl);
}

__attribute__((unused))
static uint32_t gles2_preferred_read_format(struct wlr_renderer *wlr_renderer) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer_in_context(wlr_renderer);

    push_gles2_debug(renderer);

    GLint gl_format = -1, gl_type = -1;
    glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &gl_format);
    glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &gl_type);

    GLint alpha_size = -1;
    if (renderer->current_buffer) {
        glBindRenderbuffer(GL_RENDERBUFFER, renderer->current_buffer->rbo);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_ALPHA_SIZE, &alpha_size);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    pop_gles2_debug(renderer);

    const struct wlr_gles2_pixel_format *fmt =
        get_gles2_format_from_gl(gl_format, gl_type, alpha_size > 0);
    if (fmt != NULL) {
        return fmt->drm_format;
    }

    if (renderer->exts.EXT_read_format_bgra) {
        return DRM_FORMAT_XRGB8888;
    }
    return DRM_FORMAT_XBGR8888;
}

static bool gles2_read_pixels(struct wlr_renderer *wlr_renderer,
    uint32_t drm_format, uint32_t stride, uint32_t width, uint32_t height,
    uint32_t src_x, uint32_t src_y, uint32_t dst_x, uint32_t dst_y, void *data) {
    struct wlr_gles2_renderer *renderer = gles2_get_renderer_in_context(wlr_renderer);

    const struct wlr_gles2_pixel_format *fmt = get_gles2_format_from_drm(drm_format);
    if (fmt == NULL || !is_gles2_pixel_format_supported(renderer, fmt)) {
        wlr_log(WLR_ERROR, "Cannot read pixels: unsupported pixel format 0x%"PRIX32, drm_format);
        return false;
    }

    push_gles2_debug(renderer);
    glFinish();
    glGetError();

    unsigned char *p = (unsigned char *)data + dst_y * stride;
    uint32_t pack_stride = width * 4; // Assume 4 bytes per pixel (RGBA)
    if (pack_stride == stride && dst_x == 0) {
        glReadPixels(src_x, src_y, width, height, fmt->gl_format, fmt->gl_type, p);
    } else {
        for (size_t i = 0; i < height; ++i) {
            uint32_t y = src_y + i;
            glReadPixels(src_x, y, width, 1, fmt->gl_format, fmt->gl_type, p + i * stride + dst_x * 4);
        }
    }

    pop_gles2_debug(renderer);
    return glGetError() == GL_NO_ERROR;
}



static int gles2_get_drm_fd(struct wlr_renderer *wlr_renderer) {
	struct wlr_gles2_renderer *renderer =
		gles2_get_renderer(wlr_renderer);

	if (renderer->drm_fd < 0) {
		renderer->drm_fd = wlr_egl_dup_drm_fd(renderer->egl);
	}

	return renderer->drm_fd;
}
__attribute__((unused))
static uint32_t gles2_get_render_buffer_caps(struct wlr_renderer *wlr_renderer) {
	return WLR_BUFFER_CAP_DMABUF;
}

struct wlr_egl *wlr_gles2_renderer_get_egl(struct wlr_renderer *wlr_renderer) {
	struct wlr_gles2_renderer *renderer =
		gles2_get_renderer(wlr_renderer);
	return renderer->egl;
}

static void gles2_destroy(struct wlr_renderer *wlr_renderer) {
	struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);

	wlr_egl_make_current(renderer->egl, NULL);

	

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
/*
static const struct wlr_renderer_impl renderer_impl = {
	.destroy = gles2_destroy,
	.get_texture_formats = gles2_get_texture_formats,
	.get_render_formats = gles2_get_render_formats,
	.get_drm_fd = gles2_get_drm_fd,
	.texture_from_buffer = gles2_texture_from_buffer,
	.begin_buffer_pass = gles2_begin_buffer_pass,
	.render_timer_create = gles2_render_timer_create,
};*/



// Corrected renderer_impl initialization
static const struct wlr_renderer_impl renderer_impl = {
    .destroy = gles2_destroy,
    .get_render_formats = gles2_get_render_formats,
    .get_drm_fd = gles2_get_drm_fd,
    .texture_from_buffer = gles2_texture_from_buffer,
    .begin_buffer_pass = gles2_begin_buffer_pass,
    .get_texture_formats = renderer_get_texture_formats,
    // Removed .bind_buffer as it may not be part of wlr_renderer_impl
};


static const struct wlr_render_timer_impl render_timer_impl = {
//	.get_duration_ns = gles2_get_render_time,
//	.destroy = gles2_render_timer_destroy,
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


extern const GLchar quad_vertex_src[];
extern const GLchar quad_fragment_src[];
extern const GLchar tex_vertex_src[];
extern const GLchar tex_fragment_src_rgba[];
extern const GLchar tex_fragment_src_rgbx[];
extern const GLchar tex_fragment_src_external[];
/*
struct wlr_renderer *wlr_gles2_renderer_create(struct wlr_egl *egl) {
    if (!eglMakeCurrent(egl->display, EGL_NO_SURFACE, EGL_NO_SURFACE, egl->context)) {
        wlr_log(WLR_ERROR, "Failed to make EGL context current");
        return NULL;
    }

    struct wlr_gles2_renderer *renderer = calloc(1, sizeof(*renderer));
    if (!renderer) {
        wlr_log(WLR_ERROR, "Failed to allocate renderer");
        return NULL;
    }

    renderer->egl = egl;
    renderer->wlr_renderer.impl = &renderer_impl;
    renderer->drm_fd = -1;
    wl_list_init(&renderer->buffers);
    wl_list_init(&renderer->textures);

    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    if (major < 2) {
        wlr_log(WLR_ERROR, "OpenGL ES 2.0 not supported (version %d.%d)", major, minor);
        free(renderer);
        return NULL;
    }
    wlr_log(WLR_INFO, "Using OpenGL ES %d.%d", major, minor);

    const char *exts = (const char *)glGetString(GL_EXTENSIONS);
    renderer->exts_str = exts ? strdup(exts) : NULL;
    if (!renderer->exts_str && exts) {
        wlr_log(WLR_ERROR, "Failed to duplicate GL extensions string");
        free(renderer);
        return NULL;
    }

    renderer->exts.OES_egl_image_external = check_gl_ext(renderer->exts_str, "GL_OES_EGL_image_external");
    renderer->exts.KHR_debug = check_gl_ext(renderer->exts_str, "GL_KHR_debug");
    renderer->exts.EXT_read_format_bgra = check_gl_ext(renderer->exts_str, "GL_EXT_read_format_bgra");

    if (renderer->exts.KHR_debug) {
        renderer->procs.glPushDebugGroupKHR = (PFNGLPUSHDEBUGGROUPKHRPROC)
            eglGetProcAddress("glPushDebugGroupKHR");
        renderer->procs.glPopDebugGroupKHR = (PFNGLPOPDEBUGGROUPKHRPROC)
            eglGetProcAddress("glPopDebugGroupKHR");
        glEnable(GL_DEBUG_OUTPUT_KHR);
    }

    if (renderer->exts.OES_egl_image_external) {
        renderer->procs.glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)
            eglGetProcAddress("glEGLImageTargetTexture2DOES");
    }

    renderer->shaders.quad.program = gl_shader_program_create(quad_vertex_src, quad_fragment_src);
    renderer->shaders.tex_rgba.program = gl_shader_program_create(tex_vertex_src, tex_fragment_src_rgba);
    renderer->shaders.tex_rgbx.program = gl_shader_program_create(tex_vertex_src, tex_fragment_src_rgbx);
    renderer->shaders.tex_ext.program = gl_shader_program_create(tex_vertex_src, tex_fragment_src_external);

    if (!renderer->shaders.quad.program || !renderer->shaders.tex_rgba.program ||
        !renderer->shaders.tex_rgbx.program || !renderer->shaders.tex_ext.program) {
        wlr_log(WLR_ERROR, "Failed to create shader programs");
        glDeleteProgram(renderer->shaders.quad.program);
        glDeleteProgram(renderer->shaders.tex_rgba.program);
        glDeleteProgram(renderer->shaders.tex_rgbx.program);
        glDeleteProgram(renderer->shaders.tex_ext.program);
        free((char *)renderer->exts_str); // Cast to remove const
        free(renderer);
        return NULL;
    }

    renderer->shaders.quad.proj = glGetUniformLocation(renderer->shaders.quad.program, "proj");
    renderer->shaders.quad.color = glGetUniformLocation(renderer->shaders.quad.program, "color");
    renderer->shaders.quad.pos_attrib = glGetAttribLocation(renderer->shaders.quad.program, "pos");

    renderer->shaders.tex_rgba.proj = glGetUniformLocation(renderer->shaders.tex_rgba.program, "proj");
    renderer->shaders.tex_rgba.tex = glGetUniformLocation(renderer->shaders.tex_rgba.program, "tex");
    renderer->shaders.tex_rgba.alpha = glGetUniformLocation(renderer->shaders.tex_rgba.program, "alpha");
    renderer->shaders.tex_rgba.pos_attrib = glGetAttribLocation(renderer->shaders.tex_rgba.program, "pos");
    renderer->shaders.tex_rgba.texcoord_attrib = glGetAttribLocation(renderer->shaders.tex_rgba.program, "texcoord");

    renderer->shaders.tex_rgbx.proj = glGetUniformLocation(renderer->shaders.tex_rgbx.program, "proj");
    renderer->shaders.tex_rgbx.tex = glGetUniformLocation(renderer->shaders.tex_rgbx.program, "tex");
    renderer->shaders.tex_rgbx.alpha = glGetUniformLocation(renderer->shaders.tex_rgbx.program, "alpha");
    renderer->shaders.tex_rgbx.pos_attrib = glGetAttribLocation(renderer->shaders.tex_rgbx.program, "pos");
    renderer->shaders.tex_rgbx.texcoord_attrib = glGetAttribLocation(renderer->shaders.tex_rgbx.program, "texcoord");

    renderer->shaders.tex_ext.proj = glGetUniformLocation(renderer->shaders.tex_ext.program, "proj");
    renderer->shaders.tex_ext.tex = glGetUniformLocation(renderer->shaders.tex_ext.program, "texture0");
    renderer->shaders.tex_ext.alpha = glGetUniformLocation(renderer->shaders.tex_ext.program, "alpha");
    renderer->shaders.tex_ext.pos_attrib = glGetAttribLocation(renderer->shaders.tex_ext.program, "pos");
    renderer->shaders.tex_ext.texcoord_attrib = glGetAttribLocation(renderer->shaders.tex_ext.program, "texcoord");

    const char *driver = getenv("MESA_LOADER_DRIVER_OVERRIDE");
    if (driver && strcmp(driver, "zink") == 0) {
        wlr_log(WLR_INFO, "Using Zink (Vulkan) driver for hardware acceleration");
    }

    wlr_renderer_init(&renderer->wlr_renderer, &renderer_impl, WLR_BUFFER_CAP_DMABUF);
    renderer->wlr_renderer.features.output_color_transform = false;
    wlr_log(WLR_DEBUG, "GLES2 renderer initialized successfully");
    return &renderer->wlr_renderer;
}*/


bool wlr_gles2_renderer_check_ext(struct wlr_renderer *wlr_renderer,
		const char *ext) {
	struct wlr_gles2_renderer *renderer = gles2_get_renderer(wlr_renderer);
	return check_gl_ext(renderer->exts_str, ext);
}


/*

struct wlr_renderer *wlr_gles2_renderer_create(struct wlr_egl *egl) {
    if (!eglMakeCurrent(egl->display, EGL_NO_SURFACE, EGL_NO_SURFACE, egl->context)) {
        wlr_log(WLR_ERROR, "Failed to make EGL context current");
        return NULL;
    }

    struct wlr_gles2_renderer *renderer = calloc(1, sizeof(struct wlr_gles2_renderer));
    if (renderer == NULL) {
        return NULL;
    }

    renderer->egl = egl;
    renderer->wlr_renderer.impl = &renderer_impl;
    renderer->drm_fd = -1;

    // Initialize lists and verify
    wl_list_init(&renderer->buffers);
    wl_list_init(&renderer->textures);
    
    // Verify list initialization
    if (renderer->buffers.prev == NULL || renderer->buffers.next == NULL) {
        wlr_log(WLR_ERROR, "Buffer list initialization failed");
        free(renderer);
        return NULL;
    }

    wlr_log(WLR_DEBUG, "Buffer list initialized - head: %p, prev: %p, next: %p",
        &renderer->buffers, renderer->buffers.prev, renderer->buffers.next);

    // Let Zink handle GPU acceleration naturally
    const char *driver = getenv("MESA_LOADER_DRIVER_OVERRIDE");
    if (driver && strcmp(driver, "zink") == 0) {
        wlr_log(WLR_INFO, "Using Zink (Vulkan) driver for hardware acceleration");
    }

    // Initialize basic shader programs
    renderer->shaders.quad.program = gl_shader_program_create(
        quad_vertex_src, quad_fragment_src);
    if (!renderer->shaders.quad.program) {
        free(renderer);
        return NULL;
    }

    wlr_renderer_init(&renderer->wlr_renderer, &renderer_impl);
    return &renderer->wlr_renderer;
}*/


struct wlr_renderer *wlr_gles2_renderer_create(struct wlr_egl *egl) {
    if (!eglMakeCurrent(egl->display, EGL_NO_SURFACE, EGL_NO_SURFACE, egl->context)) {
        wlr_log(WLR_ERROR, "Failed to make EGL context current");
        return NULL;
    }
    struct wlr_gles2_renderer *renderer = calloc(1, sizeof(*renderer));
    if (!renderer) {
        wlr_log(WLR_ERROR, "Failed to allocate renderer");
        return NULL;
    }
    renderer->egl = egl;
    renderer->wlr_renderer.impl = &renderer_impl;
    renderer->drm_fd = -1;
    wl_list_init(&renderer->buffers);
    wl_list_init(&renderer->textures);
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    if (major < 2) {
        wlr_log(WLR_ERROR, "OpenGL ES 2.0 not supported (version %d.%d)", major, minor);
        free(renderer);
        return NULL;
    }
    wlr_log(WLR_INFO, "Using OpenGL ES %d.%d", major, minor);
    const char *exts = (const char *)glGetString(GL_EXTENSIONS);
    renderer->exts_str = exts ? strdup(exts) : NULL;
    if (!renderer->exts_str && exts) {
        wlr_log(WLR_ERROR, "Failed to duplicate GL extensions string");
        free(renderer);
        return NULL;
    }
    renderer->exts.OES_egl_image_external = check_gl_ext(renderer->exts_str, "GL_OES_EGL_image_external");
    renderer->exts.KHR_debug = check_gl_ext(renderer->exts_str, "GL_KHR_debug");
    renderer->exts.EXT_read_format_bgra = check_gl_ext(renderer->exts_str, "GL_EXT_read_format_bgra");
    if (renderer->exts.KHR_debug) {
        renderer->procs.glPushDebugGroupKHR = (PFNGLPUSHDEBUGGROUPKHRPROC)
            eglGetProcAddress("glPushDebugGroupKHR");
        renderer->procs.glPopDebugGroupKHR = (PFNGLPOPDEBUGGROUPKHRPROC)
            eglGetProcAddress("glPopDebugGroupKHR");
        glEnable(GL_DEBUG_OUTPUT_KHR);
    }
    if (renderer->exts.OES_egl_image_external) {
        renderer->procs.glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)
            eglGetProcAddress("glEGLImageTargetTexture2DOES");
    }
    renderer->shaders.quad.program = gl_shader_program_create(quad_vertex_src, quad_fragment_src);
    renderer->shaders.tex_rgba.program = gl_shader_program_create(tex_vertex_src, tex_fragment_src_rgba);
    renderer->shaders.tex_rgbx.program = gl_shader_program_create(tex_vertex_src, tex_fragment_src_rgbx);
    renderer->shaders.tex_ext.program = gl_shader_program_create(tex_vertex_src, tex_fragment_src_external);
    if (!renderer->shaders.quad.program || !renderer->shaders.tex_rgba.program ||
        !renderer->shaders.tex_rgbx.program || !renderer->shaders.tex_ext.program) {
        wlr_log(WLR_ERROR, "Failed to create shader programs");
        glDeleteProgram(renderer->shaders.quad.program);
        glDeleteProgram(renderer->shaders.tex_rgba.program);
        glDeleteProgram(renderer->shaders.tex_rgbx.program);
        glDeleteProgram(renderer->shaders.tex_ext.program);
        free((char *)renderer->exts_str);
        free(renderer);
        return NULL;
    }
    renderer->shaders.quad.proj = glGetUniformLocation(renderer->shaders.quad.program, "proj");
    renderer->shaders.quad.color = glGetUniformLocation(renderer->shaders.quad.program, "color");
    renderer->shaders.quad.pos_attrib = glGetAttribLocation(renderer->shaders.quad.program, "pos");
    renderer->shaders.tex_rgba.proj = glGetUniformLocation(renderer->shaders.tex_rgba.program, "proj");
    renderer->shaders.tex_rgba.tex = glGetUniformLocation(renderer->shaders.tex_rgba.program, "tex");
    renderer->shaders.tex_rgba.alpha = glGetUniformLocation(renderer->shaders.tex_rgba.program, "alpha");
    renderer->shaders.tex_rgba.pos_attrib = glGetAttribLocation(renderer->shaders.tex_rgba.program, "pos");
    renderer->shaders.tex_rgba.texcoord_attrib = glGetAttribLocation(renderer->shaders.tex_rgba.program, "texcoord");
    renderer->shaders.tex_rgbx.proj = glGetUniformLocation(renderer->shaders.tex_rgbx.program, "proj");
    renderer->shaders.tex_rgbx.tex = glGetUniformLocation(renderer->shaders.tex_rgbx.program, "tex");
    renderer->shaders.tex_rgbx.alpha = glGetUniformLocation(renderer->shaders.tex_rgbx.program, "alpha");
    renderer->shaders.tex_rgbx.pos_attrib = glGetAttribLocation(renderer->shaders.tex_rgbx.program, "pos");
    renderer->shaders.tex_rgbx.texcoord_attrib = glGetAttribLocation(renderer->shaders.tex_rgbx.program, "texcoord");
    renderer->shaders.tex_ext.proj = glGetUniformLocation(renderer->shaders.tex_ext.program, "proj");
    renderer->shaders.tex_ext.tex = glGetUniformLocation(renderer->shaders.tex_ext.program, "texture0");
    renderer->shaders.tex_ext.alpha = glGetUniformLocation(renderer->shaders.tex_ext.program, "alpha");
    renderer->shaders.tex_ext.pos_attrib = glGetAttribLocation(renderer->shaders.tex_ext.program, "pos");
    renderer->shaders.tex_ext.texcoord_attrib = glGetAttribLocation(renderer->shaders.tex_ext.program, "texcoord");
    const char *driver = getenv("MESA_LOADER_DRIVER_OVERRIDE");
    if (driver && strcmp(driver, "zink") == 0) {
        wlr_log(WLR_INFO, "Using Zink (Vulkan) driver for hardware acceleration");
    }
    wlr_renderer_init(&renderer->wlr_renderer, &renderer_impl, WLR_BUFFER_CAP_DMABUF);
    renderer->wlr_renderer.features.output_color_transform = false;
    wlr_log(WLR_DEBUG, "GLES2 renderer initialized successfully");
    return &renderer->wlr_renderer;
}

/*
struct wlr_renderer *wlr_gles2_renderer_create_surfaceless(void) {
    wlr_log(WLR_INFO, "Attempting to create surfaceless GLES2 renderer");

    struct wlr_egl *egl = calloc(1, sizeof(*egl));
    if (!egl) {
        wlr_log(WLR_ERROR, "Failed to allocate EGL structure");
        return NULL;
    }

    const char *extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    wlr_log(WLR_INFO, "Client EGL Extensions: %s", extensions ? extensions : "NULL");

    PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform_display = 
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    if (!get_platform_display) {
        wlr_log(WLR_ERROR, "Platform display function not available");
        free(egl);
        return NULL;
    }

    EGLDisplay display = get_platform_display(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, NULL);
    if (display == EGL_NO_DISPLAY) {
        wlr_log(WLR_ERROR, "Failed to create surfaceless display");
        free(egl);
        return NULL;
    }

    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        wlr_log(WLR_ERROR, "EGL initialization failed. Error: 0x%x", eglGetError());
        eglTerminate(display);
        free(egl);
        return NULL;
    }

    const EGLint config_attempts[][20] = {
        {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT | EGL_WINDOW_BIT | EGL_PIXMAP_BIT,
            EGL_RED_SIZE, 1,
            EGL_GREEN_SIZE, 1,
            EGL_BLUE_SIZE, 1,
            EGL_ALPHA_SIZE, EGL_DONT_CARE,
            EGL_DEPTH_SIZE, EGL_DONT_CARE,
            EGL_STENCIL_SIZE, EGL_DONT_CARE,
            EGL_NONE
        },
        {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_RED_SIZE, 1,
            EGL_GREEN_SIZE, 1,
            EGL_BLUE_SIZE, 1,
            EGL_NONE
        }
    };

    EGLConfig config = NULL;
    EGLint num_config = 0;
    for (size_t i = 0; i < sizeof(config_attempts) / sizeof(config_attempts[0]); i++) {
        if (eglChooseConfig(display, config_attempts[i], &config, 1, &num_config) && num_config > 0) {
            break;
        }
    }
    if (num_config == 0) {
        wlr_log(WLR_ERROR, "No matching EGL configurations found");
        eglTerminate(display);
        free(egl);
        return NULL;
    }

    EGLint ctx_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attribs);
    if (context == EGL_NO_CONTEXT) {
        wlr_log(WLR_ERROR, "Context creation failed. Error: 0x%x", eglGetError());
        eglTerminate(display);
        free(egl);
        return NULL;
    }

    egl->display = display;
    egl->context = context;

    struct wlr_gles2_renderer *renderer = calloc(1, sizeof(*renderer));
    if (!renderer) {
        wlr_log(WLR_ERROR, "Failed to allocate renderer");
        eglDestroyContext(display, context);
        eglTerminate(display);
        free(egl);
        return NULL;
    }

    renderer->wlr_renderer.impl = &renderer_impl;
    renderer->egl = egl;
    renderer->drm_fd = -1;

    wlr_renderer_init(&renderer->wlr_renderer, &renderer_impl, WLR_BUFFER_CAP_DMABUF);
    return &renderer->wlr_renderer;
}*/

struct wlr_renderer *wlr_gles2_renderer_create_surfaceless(void) {
    wlr_log(WLR_INFO, "Attempting to create surfaceless GLES2 renderer");

    // Allocate wlr_egl structure
    struct wlr_egl *egl = calloc(1, sizeof(struct wlr_egl));
    if (!egl) {
        wlr_log(WLR_ERROR, "Failed to allocate EGL structure");
        return NULL;
    }

    // Log system EGL information
    const char *egl_vendor = eglQueryString(EGL_NO_DISPLAY, EGL_VENDOR);
    const char *egl_version = eglQueryString(EGL_NO_DISPLAY, EGL_VERSION);
    wlr_log(WLR_INFO, "EGL Vendor: %s", egl_vendor ? egl_vendor : "Unknown");
    wlr_log(WLR_INFO, "EGL Version: %s", egl_version ? egl_version : "Unknown");

    const char *extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    wlr_log(WLR_INFO, "Client EGL Extensions: %s", extensions ? extensions : "NULL");

    // Check for surfaceless support
    bool has_surfaceless = strstr(extensions, "EGL_MESA_platform_surfaceless") != NULL;
    bool has_platform_base = strstr(extensions, "EGL_EXT_platform_base") != NULL;
    wlr_log(WLR_INFO, "Surfaceless platform support: %s", has_surfaceless ? "YES" : "NO");
    wlr_log(WLR_INFO, "Platform base extension: %s", has_platform_base ? "YES" : "NO");

    // Use platform display function if available
    PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform_display = 
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");

    if (!get_platform_display) {
        wlr_log(WLR_ERROR, "Platform display function not available");
        free(egl);
        return NULL;
    }

    // Try creating display with surfaceless platform
    EGLDisplay display = get_platform_display(
        EGL_PLATFORM_SURFACELESS_MESA, 
        EGL_DEFAULT_DISPLAY, 
        NULL
    );

    if (display == EGL_NO_DISPLAY) {
        wlr_log(WLR_ERROR, "Failed to create surfaceless display");
        free(egl);
        return NULL;
    }

    // Initialize EGL
    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        wlr_log(WLR_ERROR, "EGL initialization failed. Error: 0x%x", eglGetError());
        eglTerminate(display);
        free(egl);
        return NULL;
    }
    wlr_log(WLR_INFO, "EGL initialized, version: %d.%d", major, minor);

    // Diagnostic: Get number of EGL configurations
    EGLint num_config_total = 0;
    eglGetConfigs(display, NULL, 0, &num_config_total);
    wlr_log(WLR_INFO, "Available EGL configurations: %d", num_config_total);

    // Multiple configuration attempts
    const EGLint config_attempts[][20] = {
        {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT | EGL_WINDOW_BIT | EGL_PIXMAP_BIT,
            EGL_RED_SIZE, 1,
            EGL_GREEN_SIZE, 1,
            EGL_BLUE_SIZE, 1,
            EGL_ALPHA_SIZE, EGL_DONT_CARE,
            EGL_DEPTH_SIZE, EGL_DONT_CARE,
            EGL_STENCIL_SIZE, EGL_DONT_CARE,
            EGL_NONE
        },
        {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_RED_SIZE, 1,
            EGL_GREEN_SIZE, 1,
            EGL_BLUE_SIZE, 1,
            EGL_NONE
        }
    };

    EGLConfig config = NULL;
    EGLint num_config = 0;

    // Try different configurations
    for (size_t i = 0; i < sizeof(config_attempts) / sizeof(config_attempts[0]); i++) {
        wlr_log(WLR_INFO, "Attempting EGL configuration attempt %zu", i);
        
        if (eglChooseConfig(display, config_attempts[i], &config, 1, &num_config)) {
            if (num_config > 0) {
                wlr_log(WLR_INFO, "Successfully found EGL configuration");
                break;
            }
        } else {
            wlr_log(WLR_ERROR, "EGL config selection failed. Error code: 0x%x", eglGetError());
        }
    }

    if (num_config == 0) {
        wlr_log(WLR_ERROR, "No matching EGL configurations found after multiple attempts");
        eglTerminate(display);
        free(egl);
        return NULL;
    }

    // Detailed configuration diagnostics
    EGLint red_size, green_size, blue_size, alpha_size;
    eglGetConfigAttrib(display, config, EGL_RED_SIZE, &red_size);
    eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &green_size);
    eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &blue_size);
    eglGetConfigAttrib(display, config, EGL_ALPHA_SIZE, &alpha_size);
    wlr_log(WLR_INFO, "Selected EGL Config Details:");
    wlr_log(WLR_INFO, "  Red Size: %d", red_size);
    wlr_log(WLR_INFO, "  Green Size: %d", green_size);
    wlr_log(WLR_INFO, "  Blue Size: %d", blue_size);
    wlr_log(WLR_INFO, "  Alpha Size: %d", alpha_size);

    // Context creation
    EGLint ctx_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attribs);
    if (context == EGL_NO_CONTEXT) {
        wlr_log(WLR_ERROR, "Context creation failed. Error: 0x%x", eglGetError());
        eglTerminate(display);
        free(egl);
        return NULL;
    }

    // Store in the EGL structure
    egl->display = display;
    egl->context = context;

    struct wlr_gles2_renderer *renderer = calloc(1, sizeof(struct wlr_gles2_renderer));
    if (!renderer) {
        wlr_log(WLR_ERROR, "Failed to allocate renderer");
        eglDestroyContext(display, context);
        eglTerminate(display);
        free(egl);
        return NULL;
    }

    renderer->wlr_renderer.impl = &renderer_impl;
    renderer->egl = egl;
    renderer->drm_fd = -1;

    // Check for Zink driver
    const char *driver = getenv("MESA_LOADER_DRIVER_OVERRIDE");
    if (driver && strcmp(driver, "zink") == 0) {
        wlr_log(WLR_INFO, "Using Zink (Vulkan) driver for hardware acceleration");
    }

//    wlr_renderer_init(&renderer->wlr_renderer, &renderer_impl);
 wlr_renderer_init(&renderer->wlr_renderer, &renderer_impl, WLR_BUFFER_CAP_DMABUF);
   
    return &renderer->wlr_renderer;
}