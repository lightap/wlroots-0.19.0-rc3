#include <assert.h>
#include <drm_fourcc.h>
#include <stdlib.h>
#include <wlr/render/allocator.h>
#include <wlr/render/swapchain.h>
#include <wlr/util/log.h>
#include <xf86drm.h>

#include "render/drm_format_set.h"
#include "types/wlr_output.h"
#include <stdio.h>

static struct wlr_swapchain *create_swapchain(struct wlr_output *output,
		int width, int height, uint32_t render_format, bool allow_modifiers) {
	struct wlr_allocator *allocator = output->allocator;
	assert(output->allocator != NULL);

	// For RDP (surfaceless) backend, create a default format explicitly
	if (!(allocator->buffer_caps & WLR_BUFFER_CAP_DMABUF)) {
		wlr_log(WLR_DEBUG, "Using default format for surfaceless output '%s'", output->name);
		struct wlr_drm_format *format = calloc(1, sizeof(struct wlr_drm_format) + sizeof(uint64_t));
		if (!format) {
			wlr_log(WLR_ERROR, "Failed to allocate format for surfaceless swapchain");
			return NULL;
		}

		format->format = DRM_FORMAT_XRGB8888;
		format->modifiers = (uint64_t *)(format + 1); // Place modifiers after the struct
		format->modifiers[0] = DRM_FORMAT_MOD_INVALID;
		format->len = 1;
		format->capacity = 1;

		struct wlr_swapchain *swapchain = wlr_swapchain_create(allocator, width, height, format);
		free(format);
		return swapchain;
	}

	// Standard path for hardware-accelerated backends
	const struct wlr_drm_format_set *display_formats =
		wlr_output_get_primary_formats(output, allocator->buffer_caps);
	struct wlr_drm_format format = {0};
	if (!output_pick_format(output, display_formats, &format, render_format)) {
		wlr_log(WLR_ERROR, "Failed to pick primary buffer format for output '%s'",
			output->name);
		return NULL;
	}

	char *format_name = drmGetFormatName(format.format);
	wlr_log(WLR_DEBUG, "Choosing primary buffer format %s (0x%08"PRIX32") for output '%s'",
		format_name ? format_name : "<unknown>", format.format, output->name);
	free(format_name);

	if (!allow_modifiers && (format.len != 1 || format.modifiers[0] != DRM_FORMAT_MOD_LINEAR)) {
		if (!wlr_drm_format_has(&format, DRM_FORMAT_MOD_INVALID)) {
			wlr_log(WLR_DEBUG, "Implicit modifiers not supported");
			wlr_drm_format_finish(&format);
			return NULL;
		}

		format.len = 0;
		if (!wlr_drm_format_add(&format, DRM_FORMAT_MOD_INVALID)) {
			wlr_log(WLR_DEBUG, "Failed to add implicit modifier to format");
			wlr_drm_format_finish(&format);
			return NULL;
		}
	}

	struct wlr_swapchain *swapchain = wlr_swapchain_create(allocator, width, height, &format);
	wlr_drm_format_finish(&format);
	return swapchain;
}

static bool test_swapchain(struct wlr_output *output,
		struct wlr_swapchain *swapchain, const struct wlr_output_state *state) {
	struct wlr_buffer *buffer = wlr_swapchain_acquire(swapchain);
	if (buffer == NULL) {
		return false;
	}

	struct wlr_output_state copy = *state;
	copy.committed |= WLR_OUTPUT_STATE_BUFFER;
	copy.buffer = buffer;
	bool ok = wlr_output_test_state(output, &copy);
	wlr_buffer_unlock(buffer);
	return ok;
}
bool wlr_output_configure_primary_swapchain(struct wlr_output *output,
		const struct wlr_output_state *state, struct wlr_swapchain **swapchain_ptr) {
	wlr_log(WLR_DEBUG, "Configuring primary swapchain for '%s'", output->name);
	printf("[DEBUG] Configuring swapchain - output: %s, swapchain_ptr: %p\n",
	       output->name, *swapchain_ptr);
	fflush(stdout);

	struct wlr_output_state empty_state;
	if (state == NULL) {
		wlr_output_state_init(&empty_state);
		state = &empty_state;
	}

	int width, height;
	output_pending_resolution(output, state, &width, &height);
	wlr_log(WLR_DEBUG, "Pending resolution: %dx%d", width, height);
	printf("[DEBUG] Pending resolution - width: %d, height: %d\n", width, height);
	fflush(stdout);

	uint32_t format = output->render_format;
	if (state->committed & WLR_OUTPUT_STATE_RENDER_FORMAT) {
		format = state->render_format;
		wlr_log(WLR_DEBUG, "Using state render format: 0x%08x", format);
	} else {
		wlr_log(WLR_DEBUG, "Using output render format: 0x%08x", format);
	}
	printf("[DEBUG] Render format: 0x%08x\n", format);
	fflush(stdout);

	// Re-use the existing swapchain if possible
	struct wlr_swapchain *old_swapchain = *swapchain_ptr;
	if (old_swapchain != NULL &&
			old_swapchain->width == width && old_swapchain->height == height &&
			old_swapchain->format.format == format) {
		wlr_log(WLR_DEBUG, "Reusing existing swapchain for '%s'", output->name);
		printf("[DEBUG] Reusing swapchain - output: %s, width: %d, height: %d, format: 0x%08x\n",
		       output->name, width, height, format);
		fflush(stdout);
		return true;
	}

	// Try creating swapchain with modifiers
	struct wlr_swapchain *swapchain = create_swapchain(output, width, height, format, true);
	if (swapchain == NULL && (output->allocator->buffer_caps & WLR_BUFFER_CAP_DMABUF)) {
		wlr_log(WLR_ERROR, "Failed to create swapchain for output '%s'", output->name);
		printf("[DEBUG] Swapchain creation failed with modifiers - output: %s\n", output->name);
		fflush(stdout);
		return false;
	}

	// For non-RDP backends, test and retry without modifiers if necessary
	if (swapchain != NULL && (output->allocator->buffer_caps & WLR_BUFFER_CAP_DMABUF)) {
		wlr_log(WLR_DEBUG, "Testing swapchain for output '%s'", output->name);
		if (!test_swapchain(output, swapchain, state)) {
			wlr_log(WLR_DEBUG, "Output test failed on '%s', retrying without modifiers",
				output->name);
			printf("[DEBUG] Swapchain test failed, retrying without modifiers - output: %s\n",
			       output->name);
			fflush(stdout);
			wlr_swapchain_destroy(swapchain);
			swapchain = create_swapchain(output, width, height, format, false);
			if (swapchain == NULL) {
				wlr_log(WLR_ERROR, "Failed to create modifier-less swapchain for output '%s'",
					output->name);
				printf("[DEBUG] Modifier-less swapchain creation failed - output: %s\n",
				       output->name);
				fflush(stdout);
				return false;
			}
			wlr_log(WLR_DEBUG, "Testing modifier-less swapchain for output '%s'", output->name);
			if (!test_swapchain(output, swapchain, state)) {
				wlr_log(WLR_ERROR, "Swapchain for output '%s' failed test", output->name);
				printf("[DEBUG] Modifier-less swapchain test failed - output: %s\n",
				       output->name);
				fflush(stdout);
				wlr_swapchain_destroy(swapchain);
				return false;
			}
		}
	}

	if (swapchain == NULL) {
		wlr_log(WLR_ERROR, "Failed to create swapchain for output '%s'", output->name);
		printf("[DEBUG] Swapchain creation failed - output: %s\n", output->name);
		fflush(stdout);
		return false;
	}

	wlr_log(WLR_DEBUG, "Swapchain configured for '%s', destroying old swapchain %p",
		output->name, old_swapchain);
	printf("[DEBUG] New swapchain created - output: %s, new: %p, old: %p\n",
	       output->name, swapchain, old_swapchain);
	fflush(stdout);
	wlr_swapchain_destroy(old_swapchain);
	*swapchain_ptr = swapchain;
	return true;
}