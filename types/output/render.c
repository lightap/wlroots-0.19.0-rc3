
#include <assert.h>
#include <drm_fourcc.h>
#include <stdlib.h>
#include <wlr/interfaces/wlr_output.h>
#include <wlr/render/allocator.h>
#include <wlr/render/interface.h>
#include <wlr/render/swapchain.h>
#include <wlr/util/log.h>
#include <xf86drm.h>
#include "render/drm_format_set.h"
#include "render/wlr_renderer.h"
#include "render/pixel_format.h"
#include "types/wlr_output.h"
#include <assert.h>
#include <drm_fourcc.h>
#include <stdlib.h>
#include <wlr/interfaces/wlr_output.h>
#include <wlr/render/interface.h>
#include <wlr/util/log.h>
#include <xf86drm.h>
#include "backend/backend.h"
#include "render/allocator/RDP_allocator.h"
#include "render/drm_format_set.h"
#include "render/swapchain.h"
#include "render/wlr_renderer.h"
#include "render/pixel_format.h"
#include "types/wlr_output.h"
#include <unistd.h>
#include <stdio.h>

bool wlr_output_init_render(struct wlr_output *output,
		struct wlr_allocator *allocator, struct wlr_renderer *renderer) {
	assert(allocator != NULL && renderer != NULL);

	if (!(output->backend->buffer_caps & allocator->buffer_caps)) {
		wlr_log(WLR_ERROR, "output backend and allocator buffer capabilities "
			"don't match");
		return false;
	} else if (!(renderer->render_buffer_caps & allocator->buffer_caps)) {
		wlr_log(WLR_ERROR, "renderer and allocator buffer capabilities "
			"don't match");
		return false;
	}

	wlr_swapchain_destroy(output->swapchain);
	output->swapchain = NULL;

	wlr_swapchain_destroy(output->cursor_swapchain);
	output->cursor_swapchain = NULL;

	output->allocator = allocator;

	output->renderer = renderer;

	return true;
}

static struct wlr_buffer *output_acquire_empty_buffer(struct wlr_output *output,
		const struct wlr_output_state *state) {
	assert(!(state->committed & WLR_OUTPUT_STATE_BUFFER));

	// wlr_output_configure_primary_swapchain() function will call
	// wlr_output_test_state(), which can call us again. This is dangerous: we
	// risk infinite recursion. However, a buffer will always be supplied in
	// wlr_output_test_state(), which will prevent us from being called.
	if (!wlr_output_configure_primary_swapchain(output, state,
			&output->swapchain)) {
		return NULL;
	}

	struct wlr_buffer *buffer = wlr_swapchain_acquire(output->swapchain);
	if (buffer == NULL) {
		return NULL;
	}

	struct wlr_render_pass *pass =
		wlr_renderer_begin_buffer_pass(output->renderer, buffer, NULL);
	if (pass == NULL) {
		wlr_buffer_unlock(buffer);
		return NULL;
	}

	wlr_render_pass_add_rect(pass, &(struct wlr_render_rect_options){
		.color = { 0, 0, 0, 0 },
		.blend_mode = WLR_RENDER_BLEND_MODE_NONE,
	});

	if (!wlr_render_pass_submit(pass)) {
		wlr_buffer_unlock(buffer);
		return NULL;
	}

	return buffer;
}

// This function may attach a new, empty buffer if necessary.
// If so, the new_back_buffer out parameter will be set to true.
bool output_ensure_buffer(struct wlr_output *output,
		struct wlr_output_state *state, bool *new_buffer) {
	assert(*new_buffer == false);

	// If we already have a buffer, we don't need to allocate a new one
	if (state->committed & WLR_OUTPUT_STATE_BUFFER) {
		return true;
	}

	// If the compositor hasn't called wlr_output_init_render(), they will use
	// their own logic to attach buffers
	if (output->renderer == NULL) {
		return true;
	}

	bool enabled = output->enabled;
	if (state->committed & WLR_OUTPUT_STATE_ENABLED) {
		enabled = state->enabled;
	}

	// If we're lighting up an output or changing its mode, make sure to
	// provide a new buffer
	bool needs_new_buffer = false;
	if ((state->committed & WLR_OUTPUT_STATE_ENABLED) && state->enabled) {
		needs_new_buffer = true;
	}
	if (state->committed & WLR_OUTPUT_STATE_MODE) {
		needs_new_buffer = true;
	}
	if (state->committed & WLR_OUTPUT_STATE_RENDER_FORMAT) {
		needs_new_buffer = true;
	}
	if (state->allow_reconfiguration && output->commit_seq == 0 && enabled) {
		// On first commit, require a new buffer if the compositor called a
		// mode-setting function, even if the mode won't change. This makes it
		// so the swapchain is created now.
		needs_new_buffer = true;
	}
	if (!needs_new_buffer) {
		return true;
	}

	wlr_log(WLR_DEBUG, "Attaching empty buffer to output for modeset");
	struct wlr_buffer *buffer = output_acquire_empty_buffer(output, state);
	if (buffer == NULL) {
		return false;
	}

	*new_buffer = true;
	wlr_output_state_set_buffer(state, buffer);
	wlr_buffer_unlock(buffer);
	return true;
}

void wlr_output_lock_attach_render(struct wlr_output *output, bool lock) {
	if (lock) {
		++output->attach_render_locks;
	} else {
		assert(output->attach_render_locks > 0);
		--output->attach_render_locks;
	}
	wlr_log(WLR_DEBUG, "%s direct scan-out on output '%s' (locks: %d)",
		lock ? "Disabling" : "Enabling", output->name,
		output->attach_render_locks);
}



bool output_pick_format(struct wlr_output *output,
		const struct wlr_drm_format_set *display_formats,
		struct wlr_drm_format *format, uint32_t fmt) {
	assert(output != NULL && output->renderer != NULL && output->allocator != NULL);

	wlr_log(WLR_DEBUG, "Surfaceless format selection: Creating minimal format for output %s",
		output->name ? output->name : "(null)");

	// Initialize format with space for one modifier
	*format = (struct wlr_drm_format){0};
	format->capacity = 1;
	format->modifiers = calloc(1, sizeof(uint64_t));
	if (!format->modifiers) {
		wlr_log(WLR_ERROR, "Failed to allocate drm format modifiers");
		return false;
	}

	// Hardcode DRM_FORMAT_XRGB8888 for RDP backend
	format->format = DRM_FORMAT_XRGB8888; // 0x34325258
	format->modifiers[0] = DRM_FORMAT_MOD_INVALID;
	format->len = 1;

	wlr_log(WLR_DEBUG, "Surfaceless: Created format 0x%"PRIX32 " for output %s",
		format->format, output->name ? output->name : "(null)");
	return true;
}

struct wlr_render_pass *wlr_output_begin_render_pass(struct wlr_output *output,
		struct wlr_output_state *state, struct wlr_buffer_pass_options *render_options) {
	if (!wlr_output_configure_primary_swapchain(output, state, &output->swapchain)) {
		return NULL;
	}

	struct wlr_buffer *buffer = wlr_swapchain_acquire(output->swapchain);
	if (buffer == NULL) {
		return NULL;
	}

	struct wlr_renderer *renderer = output->renderer;
	assert(renderer != NULL);
	struct wlr_render_pass *pass = wlr_renderer_begin_buffer_pass(renderer, buffer, render_options);
	if (pass == NULL) {
		return NULL;
	}

	wlr_output_state_set_buffer(state, buffer);
	wlr_buffer_unlock(buffer);
	return pass;
}
