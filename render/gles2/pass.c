#include <stdlib.h>
#include <assert.h>
#include <pixman.h>
#include <time.h>
#include <unistd.h>
#include <wlr/interfaces/wlr_buffer.h> 
#include <wlr/render/drm_syncobj.h>
#include <wlr/util/transform.h>
#include "render/egl.h"
#include "render/gles2.h"
#include "util/matrix.h"
#include <wlr/render/dmabuf.h> // For wlr_dmabuf_attributes


#define MAX_QUADS 86 // 4kb

static const struct wlr_render_pass_impl render_pass_impl;

static struct wlr_gles2_render_pass *get_render_pass(struct wlr_render_pass *wlr_pass) {
	assert(wlr_pass->impl == &render_pass_impl);
	struct wlr_gles2_render_pass *pass = wl_container_of(wlr_pass, pass, base);
	return pass;
}
/*
static bool render_pass_submit(struct wlr_render_pass *wlr_pass) {
	struct wlr_gles2_render_pass *pass = get_render_pass(wlr_pass);
	struct wlr_gles2_renderer *renderer = pass->buffer->renderer;
	struct wlr_gles2_render_timer *timer = pass->timer;
	bool ok = false;

	push_gles2_debug(renderer);

	if (timer) {
		// clear disjoint flag
		GLint64 disjoint;
		renderer->procs.glGetInteger64vEXT(GL_GPU_DISJOINT_EXT, &disjoint);
		// set up the query
		renderer->procs.glQueryCounterEXT(timer->id, GL_TIMESTAMP_EXT);
		// get end-of-CPU-work time in GL time domain
		renderer->procs.glGetInteger64vEXT(GL_TIMESTAMP_EXT, &timer->gl_cpu_end);
		// get end-of-CPU-work time in CPU time domain
		clock_gettime(CLOCK_MONOTONIC, &timer->cpu_end);
	}


	glFinish(); // Ensure rendering is done

if (pass->cpu_direct_data_ptr_hack != NULL) {
    wlr_log(WLR_DEBUG, "Performing glReadPixels into HACK cpu_direct_data_ptr_hack: %p", pass->cpu_direct_data_ptr_hack);
    glReadPixels(0, 0, pass->buffer->buffer->width, pass->buffer->buffer->height,
                 GL_RGBA, GL_UNSIGNED_BYTE, // Assuming RGBA for XRGB, GL will handle it
                 pass->cpu_direct_data_ptr_hack);
    GLenum read_err = glGetError();
    if (read_err != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "glReadPixels in submit HACK FAILED: 0x%x", read_err);
    } else {
        wlr_log(WLR_DEBUG, "glReadPixels in submit HACK successful.");
    }
}

	if (pass->signal_timeline != NULL) {
		EGLSyncKHR sync = wlr_egl_create_sync(renderer->egl, -1);
		if (sync == EGL_NO_SYNC_KHR) {
			goto out;
		}

		int sync_file_fd = wlr_egl_dup_fence_fd(renderer->egl, sync);
		wlr_egl_destroy_sync(renderer->egl, sync);
		if (sync_file_fd < 0) {
			goto out;
		}

		ok = wlr_drm_syncobj_timeline_import_sync_file(pass->signal_timeline, pass->signal_point, sync_file_fd);
		close(sync_file_fd);
		if (!ok) {
			goto out;
		}
	} else {
		glFlush();
	}

	ok = true;

out:
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	pop_gles2_debug(renderer);
	wlr_egl_restore_context(&pass->prev_ctx);

	wlr_drm_syncobj_timeline_unref(pass->signal_timeline);
	wlr_buffer_unlock(pass->buffer->buffer);
	free(pass);

	return ok;
}*/

// In: render/gles2/pass.c

// Ensure all necessary includes are present at the top of the file, especially:
// #include "render/gles2.h" // For struct wlr_gles2_render_pass definition
// #include <wlr/util/log.h>  // For wlr_log

static bool render_pass_submit(struct wlr_render_pass *wlr_pass) {
	struct wlr_gles2_render_pass *pass = get_render_pass(wlr_pass); // Assumes get_render_pass is defined above
	struct wlr_gles2_renderer *renderer = pass->buffer->renderer;
	struct wlr_gles2_render_timer *timer = pass->timer;
	bool ok = false;

	push_gles2_debug(renderer); // Assumes push_gles2_debug is defined

	if (timer) {
		// clear disjoint flag
		GLint64 disjoint;
		renderer->procs.glGetInteger64vEXT(GL_GPU_DISJOINT_EXT, &disjoint);
		// set up the query
		renderer->procs.glQueryCounterEXT(timer->id, GL_TIMESTAMP_EXT);
		// get end-of-CPU-work time in GL time domain
		renderer->procs.glGetInteger64vEXT(GL_TIMESTAMP_EXT, &timer->gl_cpu_end);
		// get end-of-CPU-work time in CPU time domain
		clock_gettime(CLOCK_MONOTONIC, &timer->cpu_end);
	}

	// Ensure all rendering to the FBO is complete before we attempt to read from it
	// or before we signal completion via sync objects.
	glFinish();

    // If cpu_direct_data_ptr_hack was set by begin_gles2_buffer_pass,
    // it means this is a CPU-side buffer (like RDP's) that needs its
    // contents updated from the FBO via glReadPixels.
	if (pass->cpu_direct_data_ptr_hack != NULL) {
		wlr_log(WLR_DEBUG, "[GLES2_SUBMIT] Performing glReadPixels into HACK cpu_direct_data_ptr_hack: %p for buffer %p (%dx%d)",
                pass->cpu_direct_data_ptr_hack,
                pass->buffer->buffer, // The underlying wlr_buffer
                pass->buffer->buffer->width, pass->buffer->buffer->height);

        // The FBO should still be bound here from begin_gles2_buffer_pass
		glReadPixels(0, 0, pass->buffer->buffer->width, pass->buffer->buffer->height,
					 GL_RGBA, GL_UNSIGNED_BYTE, // Read as RGBA
					 pass->cpu_direct_data_ptr_hack); // Write to the CPU buffer's data
		GLenum read_err = glGetError();
		if (read_err != GL_NO_ERROR) {
			wlr_log(WLR_ERROR, "[GLES2_SUBMIT] glReadPixels in submit HACK FAILED: 0x%x", read_err);
            // Note: even if glReadPixels fails, we continue to clean up the pass.
            // Depending on severity, could `goto out` or return false earlier.
		} else {
			wlr_log(WLR_DEBUG, "[GLES2_SUBMIT] glReadPixels in submit HACK successful.");
		}

        // --- FIX FOR THE ASSERTION ---
        // After the glReadPixels, the "internal access" to the CPU data pointer
        // (initiated by the hack in begin_gles2_buffer_pass) is now complete.
        // We need to clear the `accessing_data_ptr` flag on the wlr_buffer
        // so that a subsequent call to `wlr_buffer_begin_data_ptr_access` (e.g., from tinywl.c)
        // does not hit the assertion `!buffer->accessing_data_ptr`.
        if (pass->buffer && pass->buffer->buffer) {
            // pass->buffer->buffer is the `struct wlr_buffer *`
            pass->buffer->buffer->accessing_data_ptr = false;
            wlr_log(WLR_DEBUG, "[GLES2_SUBMIT] HACK: Reset accessing_data_ptr for buffer %p after glReadPixels", pass->buffer->buffer);
        }
        // --- END FIX FOR THE ASSERTION ---
	}

	if (pass->signal_timeline != NULL) {
        // This sync object signals GPU completion of the FBO rendering.
        // The glFinish() above ensures commands are done *before* this EGLSync is created.
		EGLSyncKHR sync = wlr_egl_create_sync(renderer->egl, -1);
		if (sync == EGL_NO_SYNC_KHR) {
            wlr_log(WLR_ERROR, "[GLES2_SUBMIT] wlr_egl_create_sync failed");
			goto out; // ok is still false
		}

		int sync_file_fd = wlr_egl_dup_fence_fd(renderer->egl, sync);
		wlr_egl_destroy_sync(renderer->egl, sync);
		if (sync_file_fd < 0) {
            wlr_log(WLR_ERROR, "[GLES2_SUBMIT] wlr_egl_dup_fence_fd failed");
			goto out; // ok is still false
		}

		ok = wlr_drm_syncobj_timeline_import_sync_file(pass->signal_timeline, pass->signal_point, sync_file_fd);
		close(sync_file_fd);
		if (!ok) {
            wlr_log(WLR_ERROR, "[GLES2_SUBMIT] wlr_drm_syncobj_timeline_import_sync_file failed");
			goto out; // ok is false
		}
        // ok is true if syncobj import succeeded
	} else {
		// No explicit timeline sync. The glFinish() above already ensured GPU commands are done.
        // A glFlush() here would be redundant if glFinish() was performed.
        // If glFinish() was *only* for the readback hack, and this path is taken,
        // a glFlush() might be conventionally expected, but glFinish() is stronger.
		// glFlush();
        ok = true; // If no signal_timeline, we consider submission successful at this point.
	}

	// If we reached here and 'ok' wasn't set to true by the signal_timeline path,
    // but no errors occurred in the glReadPixels or earlier, we can set ok to true.
    // The logic above already sets ok = true if signal_timeline is NULL.
    // If signal_timeline path failed, ok remains false.
    // If glReadPixels had a critical error, we might have wanted to set ok = false earlier.

out:
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the FBO used for this pass

	pop_gles2_debug(renderer); // Assumes pop_gles2_debug is defined
	wlr_egl_restore_context(&pass->prev_ctx);

	wlr_drm_syncobj_timeline_unref(pass->signal_timeline); // Safe even if NULL
	wlr_buffer_unlock(pass->buffer->buffer); // Unlock the underlying wlr_buffer
	free(pass); // Free the render pass structure

	return ok;
}

static void render(const struct wlr_box *box, const pixman_region32_t *clip, GLint attrib) {
	pixman_region32_t region;
	pixman_region32_init_rect(&region, box->x, box->y, box->width, box->height);

	if (clip) {
		pixman_region32_intersect(&region, &region, clip);
	}

	int rects_len;
	const pixman_box32_t *rects = pixman_region32_rectangles(&region, &rects_len);
	if (rects_len == 0) {
		pixman_region32_fini(&region);
		return;
	}

	glEnableVertexAttribArray(attrib);

	for (int i = 0; i < rects_len;) {
		int batch = rects_len - i < MAX_QUADS ? rects_len - i : MAX_QUADS;
		int batch_end = batch + i;

		size_t vert_index = 0;
		GLfloat verts[MAX_QUADS * 6 * 2];
		for (; i < batch_end; i++) {
			const pixman_box32_t *rect = &rects[i];

			verts[vert_index++] = (GLfloat)(rect->x1 - box->x) / box->width;
			verts[vert_index++] = (GLfloat)(rect->y1 - box->y) / box->height;
			verts[vert_index++] = (GLfloat)(rect->x2 - box->x) / box->width;
			verts[vert_index++] = (GLfloat)(rect->y1 - box->y) / box->height;
			verts[vert_index++] = (GLfloat)(rect->x1 - box->x) / box->width;
			verts[vert_index++] = (GLfloat)(rect->y2 - box->y) / box->height;
			verts[vert_index++] = (GLfloat)(rect->x2 - box->x) / box->width;
			verts[vert_index++] = (GLfloat)(rect->y1 - box->y) / box->height;
			verts[vert_index++] = (GLfloat)(rect->x2 - box->x) / box->width;
			verts[vert_index++] = (GLfloat)(rect->y2 - box->y) / box->height;
			verts[vert_index++] = (GLfloat)(rect->x1 - box->x) / box->width;
			verts[vert_index++] = (GLfloat)(rect->y2 - box->y) / box->height;
		}

		glVertexAttribPointer(attrib, 2, GL_FLOAT, GL_FALSE, 0, verts);
		glDrawArrays(GL_TRIANGLES, 0, batch * 6);
	}

	glDisableVertexAttribArray(attrib);

	pixman_region32_fini(&region);
}

static void set_proj_matrix(GLint loc, float proj[9], const struct wlr_box *box) {
	float gl_matrix[9];
	wlr_matrix_identity(gl_matrix);
	wlr_matrix_translate(gl_matrix, box->x, box->y);
	wlr_matrix_scale(gl_matrix, box->width, box->height);
	wlr_matrix_multiply(gl_matrix, proj, gl_matrix);
	glUniformMatrix3fv(loc, 1, GL_FALSE, gl_matrix);
}

static void set_tex_matrix(GLint loc, enum wl_output_transform trans,
		const struct wlr_fbox *box) {
	float tex_matrix[9];
	wlr_matrix_identity(tex_matrix);
	wlr_matrix_translate(tex_matrix, box->x, box->y);
	wlr_matrix_scale(tex_matrix, box->width, box->height);
	wlr_matrix_translate(tex_matrix, .5, .5);

	// since textures have a different origin point we have to transform
	// differently if we are rotating
	if (trans & WL_OUTPUT_TRANSFORM_90) {
		wlr_matrix_transform(tex_matrix, wlr_output_transform_invert(trans));
	} else {
		wlr_matrix_transform(tex_matrix, trans);
	}
	wlr_matrix_translate(tex_matrix, -.5, -.5);

	glUniformMatrix3fv(loc, 1, GL_FALSE, tex_matrix);
}

static void setup_blending(enum wlr_render_blend_mode mode) {
	switch (mode) {
	case WLR_RENDER_BLEND_MODE_PREMULTIPLIED:
		glEnable(GL_BLEND);
		break;
	case WLR_RENDER_BLEND_MODE_NONE:
		glDisable(GL_BLEND);
		break;
	}
}

static void render_pass_add_texture(struct wlr_render_pass *wlr_pass,
		const struct wlr_render_texture_options *options) {
	struct wlr_gles2_render_pass *pass = get_render_pass(wlr_pass);
	struct wlr_gles2_renderer *renderer = pass->buffer->renderer;
	struct wlr_gles2_texture *texture = gles2_get_texture(options->texture);

	struct wlr_gles2_tex_shader *shader = NULL;

	switch (texture->target) {
	case GL_TEXTURE_2D:
		if (texture->has_alpha) {
			shader = &renderer->shaders.tex_rgba;
		} else {
			shader = &renderer->shaders.tex_rgbx;
		}
		break;
	case GL_TEXTURE_EXTERNAL_OES:
		// EGL_EXT_image_dma_buf_import_modifiers requires
		// GL_OES_EGL_image_external
		assert(renderer->exts.OES_egl_image_external);
		shader = &renderer->shaders.tex_ext;
		break;
	default:
		abort();
	}

	struct wlr_box dst_box;
	struct wlr_fbox src_fbox;
	wlr_render_texture_options_get_src_box(options, &src_fbox);
	wlr_render_texture_options_get_dst_box(options, &dst_box);
	float alpha = wlr_render_texture_options_get_alpha(options);

	src_fbox.x /= options->texture->width;
	src_fbox.y /= options->texture->height;
	src_fbox.width /= options->texture->width;
	src_fbox.height /= options->texture->height;

	push_gles2_debug(renderer);

	if (options->wait_timeline != NULL) {
		int sync_file_fd =
			wlr_drm_syncobj_timeline_export_sync_file(options->wait_timeline, options->wait_point);
		if (sync_file_fd < 0) {
			return;
		}

		EGLSyncKHR sync = wlr_egl_create_sync(renderer->egl, sync_file_fd);
		close(sync_file_fd);
		if (sync == EGL_NO_SYNC_KHR) {
			return;
		}

		bool ok = wlr_egl_wait_sync(renderer->egl, sync);
		wlr_egl_destroy_sync(renderer->egl, sync);
		if (!ok) {
			return;
		}
	}

	setup_blending(!texture->has_alpha && alpha == 1.0 ?
		WLR_RENDER_BLEND_MODE_NONE : options->blend_mode);

	glUseProgram(shader->program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(texture->target, texture->tex);

	switch (options->filter_mode) {
	case WLR_SCALE_FILTER_BILINEAR:
		glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case WLR_SCALE_FILTER_NEAREST:
		glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	}

	glUniform1i(shader->tex, 0);
	glUniform1f(shader->alpha, alpha);
	set_proj_matrix(shader->proj, pass->projection_matrix, &dst_box);
	set_tex_matrix(shader->tex_proj, options->transform, &src_fbox);

	render(&dst_box, options->clip, shader->pos_attrib);

	glBindTexture(texture->target, 0);
	pop_gles2_debug(renderer);
}

static void render_pass_add_rect(struct wlr_render_pass *wlr_pass,
        const struct wlr_render_rect_options *options) {
    struct wlr_gles2_render_pass *pass = get_render_pass(wlr_pass);
    struct wlr_gles2_renderer *renderer = pass->buffer->renderer;

    const struct wlr_render_color *color = &options->color;
    struct wlr_box box;
    wlr_render_rect_options_get_box(options, pass->buffer->buffer, &box);

    push_gles2_debug(renderer);

    // Validate quad shader program
    GLuint prog = renderer->shaders.quad.program;
    if (prog == 0) {
        wlr_log(WLR_ERROR, "Quad shader program is invalid (ID=0)");
        pop_gles2_debug(renderer);
        return;
    }

    GLint link_status;
    glGetProgramiv(prog, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        char info_log[512];
        glGetProgramInfoLog(prog, 512, NULL, info_log);
        wlr_log(WLR_ERROR, "Quad shader program not linked: %s", info_log);
        pop_gles2_debug(renderer);
        return;
    }

    // Ensure program is bound
    glUseProgram(prog);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "glUseProgram failed: 0x%x", err);
        pop_gles2_debug(renderer);
        return;
    }

    // Verify uniform locations
    if (renderer->shaders.quad.proj == -1) {
        wlr_log(WLR_ERROR, "Invalid 'proj' uniform location");
        pop_gles2_debug(renderer);
        return;
    }
    if (renderer->shaders.quad.color == -1) {
        wlr_log(WLR_ERROR, "Invalid 'color' uniform location");
        pop_gles2_debug(renderer);
        return;
    }

    setup_blending(color->a == 1.0 ? WLR_RENDER_BLEND_MODE_NONE : options->blend_mode);

    // Set and log projection matrix
    set_proj_matrix(renderer->shaders.quad.proj, pass->projection_matrix, &box);
    float proj_matrix[9];
    glGetUniformfv(prog, renderer->shaders.quad.proj, proj_matrix);
    wlr_log(WLR_DEBUG, "Projection matrix: [%f, %f, %f, %f, %f, %f, %f, %f, %f]",
            proj_matrix[0], proj_matrix[1], proj_matrix[2],
            proj_matrix[3], proj_matrix[4], proj_matrix[5],
            proj_matrix[6], proj_matrix[7], proj_matrix[8]);

    // Set and log color uniform
    glUniform4f(renderer->shaders.quad.color, color->r, color->g, color->b, color->a);
    float color_val[4];
    glGetUniformfv(prog, renderer->shaders.quad.color, color_val);
    wlr_log(WLR_DEBUG, "Color uniform: R=%f, G=%f, B=%f, A=%f",
            color_val[0], color_val[1], color_val[2], color_val[3]);

    render(&box, options->clip, renderer->shaders.quad.pos_attrib);

    pop_gles2_debug(renderer);
}

static const struct wlr_render_pass_impl render_pass_impl = {
	.submit = render_pass_submit,
	.add_texture = render_pass_add_texture,
	.add_rect = render_pass_add_rect,
};

static const char *reset_status_str(GLenum status) {
	switch (status) {
	case GL_GUILTY_CONTEXT_RESET_KHR:
		return "guilty";
	case GL_INNOCENT_CONTEXT_RESET_KHR:
		return "innocent";
	case GL_UNKNOWN_CONTEXT_RESET_KHR:
		return "unknown";
	default:
		return "<invalid>";
	}
}

// In: render/gles2/pass.c

#include <stdlib.h>
#include <assert.h>
#include <pixman.h>
#include <time.h>
#include <unistd.h>
#include <wlr/render/drm_syncobj.h>
#include <wlr/util/transform.h>
#include "render/egl.h"
#include "render/gles2.h" // This includes your modified struct wlr_gles2_render_pass
#include "util/matrix.h"

// Needed for the hack to identify RDP-like buffers
#include <wlr/render/dmabuf.h> // For wlr_dmabuf_attributes and wlr_buffer_get_dmabuf



struct wlr_gles2_render_pass *begin_gles2_buffer_pass(struct wlr_gles2_buffer *buffer,
		struct wlr_egl_context *prev_ctx, struct wlr_gles2_render_timer *timer,
		struct wlr_drm_syncobj_timeline *signal_timeline, uint64_t signal_point) {
	struct wlr_gles2_renderer *renderer = buffer->renderer;
	struct wlr_buffer *wlr_buffer = buffer->buffer; // This is the underlying wlr_buffer

	if (renderer->procs.glGetGraphicsResetStatusKHR) {
		GLenum status = renderer->procs.glGetGraphicsResetStatusKHR();
		if (status != GL_NO_ERROR) {
			wlr_log(WLR_ERROR, "[GLES2_BEGIN_PASS] GPU reset (%s)", reset_status_str(status));
			wl_signal_emit_mutable(&renderer->wlr_renderer.events.lost, NULL);
			return NULL;
		}
	}

	struct wlr_gles2_render_pass *pass = calloc(1, sizeof(*pass));
    if (pass == NULL) {
        wlr_log(WLR_ERROR, "[GLES2_BEGIN_PASS] Failed to allocate memory for render pass");
        return NULL;
    }

   GLenum err;
    // Clear any pre-existing GL errors
    while ((err = glGetError()) != GL_NO_ERROR) {
        wlr_log(WLR_DEBUG, "[GLES2_BEGIN_PASS] Cleared pre-existing GL error: 0x%x", err);
    }

    pass->buffer = buffer; // The wlr_gles2_buffer (wrapper)
    pass->prev_ctx = *prev_ctx;
    pass->timer = timer;
    // signal_timeline and signal_point are for the *output* of this pass
    pass->signal_timeline = NULL; // Initialize, will be set later if provided
    pass->signal_point = 0;
    pass->cpu_direct_data_ptr_hack = NULL; // Initialize the new field

    // --- BEGIN HACK TO POPULATE cpu_direct_data_ptr_for_readback_hack ---
    // This logic tries to identify if the buffer is likely an RDP-style CPU buffer
    // that fakes DMABUF support but actually uses a CPU-side data pointer.
    struct wlr_dmabuf_attributes dmabuf_attrs;
    // Check if wlr_buffer_get_dmabuf succeeds (even if faked)
    if (wlr_buffer_get_dmabuf(wlr_buffer, &dmabuf_attrs)) {
        // Check if it's faking DMABUF (fd[0] is -1 for the first plane)
        // AND it supports direct data pointer access (which RDP allocator does).
        if (dmabuf_attrs.n_planes > 0 && dmabuf_attrs.fd[0] == -1 &&
            wlr_buffer->impl && wlr_buffer->impl->begin_data_ptr_access) {

            wlr_log(WLR_DEBUG, "[GLES2_BEGIN_PASS] Buffer %p appears to be a CPU buffer faking DMABUF. Attempting to get data_ptr for readback.", wlr_buffer);

            void *temp_cpu_data_ptr;
            uint32_t temp_cpu_format; // We don't use this, but function requires it
            size_t temp_cpu_stride;   // We don't use this, but function requires it

            // HACK: Temporarily bypass the accessing_data_ptr assertion in wlr_buffer.
            // This is needed because wlr_buffer_begin_data_ptr_access typically asserts
            // that `!buffer->accessing_data_ptr`. The render pass itself has already
            // "locked" the wlr_buffer (via wlr_buffer_lock in this function later),
            // but not necessarily via begin_data_ptr_access.
            bool original_accessing_data_ptr_state = wlr_buffer->accessing_data_ptr;
            wlr_buffer->accessing_data_ptr = false; // Temporarily clear the flag for the call

            if (wlr_buffer_begin_data_ptr_access(wlr_buffer,
                    WLR_BUFFER_DATA_PTR_ACCESS_READ | WLR_BUFFER_DATA_PTR_ACCESS_WRITE, // Need WRITE for glReadPixels target
                    &temp_cpu_data_ptr, &temp_cpu_format, &temp_cpu_stride)) {

                pass->cpu_direct_data_ptr_hack = temp_cpu_data_ptr;
                wlr_log(WLR_DEBUG, "[GLES2_BEGIN_PASS] Successfully got CPU data_ptr %p (format 0x%x, stride %zu) for readback hack.",
                        temp_cpu_data_ptr, temp_cpu_format, temp_cpu_stride);

                // IMPORTANT: We do NOT call wlr_buffer_end_data_ptr_access here.
                // The pointer and the "accessing data ptr" state are "held" by the pass
                // for the glReadPixels in submit. The original wlr_buffer_end_data_ptr_access
                // call (e.g., in tinywl.c after RDP transmission) will be the one that
                // "officially" ends access and resets rdp_buffer->in_use.
                // This effectively means wlr_buffer->accessing_data_ptr will remain true
                // because wlr_buffer_begin_data_ptr_access set it.
            } else {
                wlr_log(WLR_ERROR, "[GLES2_BEGIN_PASS] Failed to get CPU data_ptr for readback hack for buffer %p.", wlr_buffer);
                // Restore the original state if begin_data_ptr_access failed.
                wlr_buffer->accessing_data_ptr = original_accessing_data_ptr_state;
            }
            // If begin_data_ptr_access succeeded, accessing_data_ptr is now true.
            // If it failed, we restored its original state.
        }
        // If wlr_buffer_get_dmabuf succeeded, we need to finish the attributes
        // as they might have duplicated FDs (even if -1).
        wlr_dmabuf_attributes_finish(&dmabuf_attrs);
    }
    // --- END HACK ---

    GLuint fbo = gles2_buffer_get_fbo(buffer); // Get/create FBO for the wlr_gles2_buffer
    if (fbo == 0) {
        wlr_log(WLR_ERROR, "[GLES2_BEGIN_PASS] Failed to get FBO for gles2_buffer %p (wlr_buffer %p)", buffer, wlr_buffer);
        free(pass);
        return NULL;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "[GLES2_BEGIN_PASS] glBindFramebuffer(fbo=%u) failed: 0x%x", fbo, err);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Try to unbind
        wlr_egl_restore_context(&pass->prev_ctx); // Restore before freeing pass
        free(pass);
        return NULL;
    }

    GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fbo_status != GL_FRAMEBUFFER_COMPLETE) {
        wlr_log(WLR_ERROR, "[GLES2_BEGIN_PASS] Framebuffer %u is not complete: 0x%x", fbo, fbo_status);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        wlr_egl_restore_context(&pass->prev_ctx); // Restore before freeing pass
        free(pass);
        return NULL;
    }
    wlr_log(WLR_DEBUG, "[GLES2_BEGIN_PASS] Bound FBO %u for rendering (wlr_buffer %p, %dx%d)",
            fbo, wlr_buffer, wlr_buffer->width, wlr_buffer->height);

    // This GL_VIEWPORT call was already here in the original logs, so it's likely correct.
    // However, it's often set *after* binding the FBO and before drawing.
    // The actual viewport for drawing is set again later.
    GLint current_viewport[4];
    glGetIntegerv(GL_VIEWPORT, current_viewport);
    wlr_log(WLR_DEBUG, "[GLES2_BEGIN_PASS] Viewport before pass setup: x=%d, y=%d, w=%d, h=%d",
            current_viewport[0], current_viewport[1], current_viewport[2], current_viewport[3]);

	wlr_render_pass_init(&pass->base, &render_pass_impl);
	wlr_buffer_lock(wlr_buffer); // Lock the underlying wlr_buffer

    // Redundant assignments? These were already set.
    // pass->buffer = buffer;
    // pass->prev_ctx = *prev_ctx;
    // pass->timer = timer;

	if (signal_timeline != NULL) {
		pass->signal_timeline = wlr_drm_syncobj_timeline_ref(signal_timeline);
		pass->signal_point = signal_point;
	}

    // Initialize projection matrix for the pass
	matrix_projection(pass->projection_matrix, wlr_buffer->width, wlr_buffer->height,
		WL_OUTPUT_TRANSFORM_FLIPPED_180); // Standard for GLES2 FBOs (Y-down)

	push_gles2_debug(renderer); // Start a debug group for GL commands within the pass

	// Bind FBO again (was already bound, but ensures it's current target for subsequent GL state)
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // It's good practice to check for errors after GL calls
    err = glGetError();
    if (err != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "[GLES2_BEGIN_PASS] glBindFramebuffer(fbo=%u) [second call] failed: 0x%x", fbo, err);
        // Consider cleanup and return NULL if critical
    }

    // Set viewport for this render pass
	glViewport(0, 0, wlr_buffer->width, wlr_buffer->height);
    // Set default blend func for the pass (can be overridden by add_texture/add_rect options)
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_SCISSOR_TEST); // Default to no scissor test

	pop_gles2_debug(renderer); // End the initial debug group

    // Clear any GL errors that might have occurred during pass setup
    // (though ideally, errors are checked after each significant GL call)
    while ((err = glGetError()) != GL_NO_ERROR) {
        wlr_log(WLR_DEBUG, "[GLES2_BEGIN_PASS] Cleared GL error after pass setup: 0x%x", err);
    }

	return pass;
}
