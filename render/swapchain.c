
/*
#include <assert.h>
#include <stdlib.h>
#include <wlr/util/log.h>
#include <wlr/render/allocator.h>
#include <wlr/render/swapchain.h>
#include <wlr/types/wlr_buffer.h>
#include "render/drm_format_set.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>  // For clock_gettime and struct timespec
#include <wlr/util/log.h>
#include <wlr/render/allocator.h>
#include <wlr/render/swapchain.h>
#include <wlr/types/wlr_buffer.h>
#include "render/drm_format_set.h"

// Define WLR_WARN if it doesn't exist
#ifndef WLR_WARN
#define WLR_WARN WLR_DEBUG
#endif

static void swapchain_handle_allocator_destroy(struct wl_listener *listener,
		void *data) {
	struct wlr_swapchain *swapchain =
		wl_container_of(listener, swapchain, allocator_destroy);
	swapchain->allocator = NULL;
	wl_list_remove(&swapchain->allocator_destroy.link);
	wl_list_init(&swapchain->allocator_destroy.link);
}

struct wlr_swapchain *wlr_swapchain_create(
		struct wlr_allocator *alloc, int width, int height,
		const struct wlr_drm_format *format) {
	struct wlr_swapchain *swapchain = calloc(1, sizeof(*swapchain));
	if (swapchain == NULL) {
		return NULL;
	}
	swapchain->allocator = alloc;
	swapchain->width = width;
	swapchain->height = height;

	if (!wlr_drm_format_copy(&swapchain->format, format)) {
		free(swapchain);
		return NULL;
	}

	swapchain->allocator_destroy.notify = swapchain_handle_allocator_destroy;
	wl_signal_add(&alloc->events.destroy, &swapchain->allocator_destroy);

	return swapchain;
}

static void slot_reset(struct wlr_swapchain_slot *slot) {
	if (slot->acquired) {
		wl_list_remove(&slot->release.link);
	}
	wlr_buffer_drop(slot->buffer);
	*slot = (struct wlr_swapchain_slot){0};
}

void wlr_swapchain_destroy(struct wlr_swapchain *swapchain) {
	if (swapchain == NULL) {
		return;
	}
	for (size_t i = 0; i < WLR_SWAPCHAIN_CAP; i++) {
		slot_reset(&swapchain->slots[i]);
	}
	wl_list_remove(&swapchain->allocator_destroy.link);
	wlr_drm_format_finish(&swapchain->format);
	free(swapchain);
}

static void slot_handle_release(struct wl_listener *listener, void *data) {
	struct wlr_swapchain_slot *slot =
		wl_container_of(listener, slot, release);
	wl_list_remove(&slot->release.link);
	slot->acquired = false;
	// Log when a buffer is released to help with debugging
	wlr_log(WLR_DEBUG, "Buffer in slot released");
}

static struct wlr_buffer *slot_acquire(struct wlr_swapchain *swapchain,
		struct wlr_swapchain_slot *slot) {
	assert(!slot->acquired);
	assert(slot->buffer != NULL);

	slot->acquired = true;

	slot->release.notify = slot_handle_release;
	wl_signal_add(&slot->buffer->events.release, &slot->release);

	return wlr_buffer_lock(slot->buffer);
}

struct wlr_buffer *wlr_swapchain_acquire(struct wlr_swapchain *swapchain) {
    // Track least recently used slot for smart buffer reuse
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    
    // Count used and total slots for logging
    size_t used_slots = 0;
    size_t free_slot_index = WLR_SWAPCHAIN_CAP; // Invalid index to start
    const size_t total_slots = WLR_SWAPCHAIN_CAP;
    
    // First pass: Count used slots and find a free slot
    for (size_t i = 0; i < WLR_SWAPCHAIN_CAP; i++) {
        if (swapchain->slots[i].acquired) {
            used_slots++;
        } else if (free_slot_index == WLR_SWAPCHAIN_CAP) {
            // Remember the first free slot we find
            free_slot_index = i;
        }
    }
    
    wlr_log(WLR_DEBUG, "Swapchain slots: %zu/%zu", used_slots, total_slots);

    // If we found a free slot
    if (free_slot_index < WLR_SWAPCHAIN_CAP) {
        struct wlr_swapchain_slot *free_slot = &swapchain->slots[free_slot_index];
        
        // If the slot already has a buffer, reuse it
        if (free_slot->buffer != NULL) {
            wlr_log(WLR_DEBUG, "Acquiring existing buffer in slot %zu", free_slot_index);
            return slot_acquire(swapchain, free_slot);
        }
        
        // Otherwise, allocate a new buffer for this slot
        wlr_log(WLR_DEBUG, "Found free slot %zu for new buffer allocation", free_slot_index);
        
        if (swapchain->allocator == NULL) {
            wlr_log(WLR_ERROR, "No allocator available for swapchain");
            return NULL;
        }

        wlr_log(WLR_DEBUG, "Allocating new swapchain buffer");
        free_slot->buffer = wlr_allocator_create_buffer(swapchain->allocator,
            swapchain->width, swapchain->height, &swapchain->format);
        if (free_slot->buffer == NULL) {
            wlr_log(WLR_ERROR, "Failed to allocate buffer");
            return NULL;
        }
        wlr_log(WLR_DEBUG, "Allocated new buffer for slot %zu", free_slot_index);
        return slot_acquire(swapchain, free_slot);
    }

    // If all slots are acquired, we need to force-release the least recently used one
    wlr_log(WLR_ERROR, "All swapchain slots are in use. Forcing release of a buffer.");
    
    // Find a slot to force-release (we'll use slot 0 if we can't determine the oldest)
    size_t slot_to_release = 0;
    
    // Force release the chosen slot
    struct wlr_swapchain_slot *slot = &swapchain->slots[slot_to_release];
    if (slot->acquired) {
        wlr_log(WLR_DEBUG, "Force-releasing slot %zu", slot_to_release);
        wl_list_remove(&slot->release.link);
        slot->acquired = false;
        
        // Re-acquire it
        return slot_acquire(swapchain, slot);
    }
    
    // This should never happen if our slot counting was correct
    wlr_log(WLR_ERROR, "Logic error: couldn't find any slot to force-release");
    return NULL;
}

bool wlr_swapchain_has_buffer(struct wlr_swapchain *swapchain,
		struct wlr_buffer *buffer) {
	for (size_t i = 0; i < WLR_SWAPCHAIN_CAP; i++) {
		struct wlr_swapchain_slot *slot = &swapchain->slots[i];
		if (slot->buffer == buffer) {
			return true;
		}
	}
	return false;
}*/

#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <wlr/util/log.h>
#include <wlr/render/allocator.h>
#include <wlr/render/swapchain.h>
#include <wlr/types/wlr_buffer.h>
#include "render/drm_format_set.h"

// Define WLR_WARN if it doesn't exist
#ifndef WLR_WARN
#define WLR_WARN WLR_DEBUG
#endif

// Helper macro for timing
#define LOG_TIMING_START(name) \
    struct timespec _start_##name, _end_##name; \
    clock_gettime(CLOCK_MONOTONIC, &_start_##name); \
    wlr_log(WLR_DEBUG, "Starting %s at %ld.%09ld", #name, _start_##name.tv_sec, _start_##name.tv_nsec);

#define LOG_TIMING_END(name) \
    clock_gettime(CLOCK_MONOTONIC, &_end_##name); \
    double _elapsed_##name = (_end_##name.tv_sec - _start_##name.tv_sec) * 1000.0 + \
                             (_end_##name.tv_nsec - _start_##name.tv_nsec) / 1000000.0; \
    wlr_log(WLR_DEBUG, "Finished %s, took %.2f ms", #name, _elapsed_##name);

static void swapchain_handle_allocator_destroy(struct wl_listener *listener,
		void *data) {
	struct wlr_swapchain *swapchain =
		wl_container_of(listener, swapchain, allocator_destroy);
	swapchain->allocator = NULL;
	wl_list_remove(&swapchain->allocator_destroy.link);
	wl_list_init(&swapchain->allocator_destroy.link);
}

struct wlr_swapchain *wlr_swapchain_create(
		struct wlr_allocator *alloc, int width, int height,
		const struct wlr_drm_format *format) {
	LOG_TIMING_START(swapchain_create);
	struct wlr_swapchain *swapchain = calloc(1, sizeof(*swapchain));
	if (swapchain == NULL) {
		wlr_log(WLR_ERROR, "Failed to allocate swapchain");
		LOG_TIMING_END(swapchain_create);
		return NULL;
	}
	swapchain->allocator = alloc;
	swapchain->width = width;
	swapchain->height = height;

	if (!wlr_drm_format_copy(&swapchain->format, format)) {
		wlr_log(WLR_ERROR, "Failed to copy DRM format");
		free(swapchain);
		LOG_TIMING_END(swapchain_create);
		return NULL;
	}

	swapchain->allocator_destroy.notify = swapchain_handle_allocator_destroy;
	wl_signal_add(&alloc->events.destroy, &swapchain->allocator_destroy);

	// Pre-allocate at least two buffers to ensure double-buffering
	size_t num_buffers = 3; // Minimum for double-buffering
	if (num_buffers > WLR_SWAPCHAIN_CAP) {
		num_buffers = WLR_SWAPCHAIN_CAP;
	}
	wlr_log(WLR_DEBUG, "Pre-allocating %zu buffers for swapchain", num_buffers);
	for (size_t i = 0; i < num_buffers; i++) {
		struct wlr_swapchain_slot *slot = &swapchain->slots[i];
		slot->buffer = wlr_allocator_create_buffer(alloc, width, height, format);
		if (slot->buffer == NULL) {
			wlr_log(WLR_ERROR, "Failed to pre-allocate buffer for slot %zu", i);
			wlr_swapchain_destroy(swapchain);
			LOG_TIMING_END(swapchain_create);
			return NULL;
		}
		wlr_log(WLR_DEBUG, "Pre-allocated buffer for slot %zu", i);
	}

	LOG_TIMING_END(swapchain_create);
	return swapchain;
}

static void slot_reset(struct wlr_swapchain_slot *slot) {
	if (slot->acquired) {
		wl_list_remove(&slot->release.link);
	}
	wlr_buffer_drop(slot->buffer);
	*slot = (struct wlr_swapchain_slot){0};
}

void wlr_swapchain_destroy(struct wlr_swapchain *swapchain) {
	if (swapchain == NULL) {
		return;
	}
	for (size_t i = 0; i < WLR_SWAPCHAIN_CAP; i++) {
		slot_reset(&swapchain->slots[i]);
	}
	wl_list_remove(&swapchain->allocator_destroy.link);
	wlr_drm_format_finish(&swapchain->format);
	free(swapchain);
}

static void slot_handle_release(struct wl_listener *listener, void *data) {
	struct wlr_swapchain_slot *slot =
		wl_container_of(listener, slot, release);
	wl_list_remove(&slot->release.link);
	slot->acquired = false;
	wlr_log(WLR_DEBUG, "Buffer in slot released");
}

static struct wlr_buffer *slot_acquire(struct wlr_swapchain *swapchain,
		struct wlr_swapchain_slot *slot) {
	LOG_TIMING_START(slot_acquire);
	assert(!slot->acquired);
	assert(slot->buffer != NULL);

	slot->acquired = true;

	slot->release.notify = slot_handle_release;
	wl_signal_add(&slot->buffer->events.release, &slot->release);

	struct wlr_buffer *buffer = wlr_buffer_lock(slot->buffer);
	wlr_log(WLR_DEBUG, "Acquired buffer %p from slot", buffer);
	LOG_TIMING_END(slot_acquire);
	return buffer;
}

struct wlr_buffer *wlr_swapchain_acquire(struct wlr_swapchain *swapchain) {
	LOG_TIMING_START(swapchain_acquire);
	// Count used and total slots for logging
	size_t used_slots = 0;
	size_t free_slot_index = WLR_SWAPCHAIN_CAP; // Invalid index to start
	const size_t total_slots = WLR_SWAPCHAIN_CAP;

	// Find a free slot with an existing buffer
	for (size_t i = 0; i < WLR_SWAPCHAIN_CAP; i++) {
		struct wlr_swapchain_slot *slot = &swapchain->slots[i];
		if (slot->acquired) {
			used_slots++;
		} else if (slot->buffer != NULL && free_slot_index == WLR_SWAPCHAIN_CAP) {
			free_slot_index = i; // Prefer slots with buffers
		}
	}

	wlr_log(WLR_DEBUG, "Swapchain slots: %zu/%zu", used_slots, total_slots);

	// If we found a free slot with a buffer, use it
	if (free_slot_index < WLR_SWAPCHAIN_CAP) {
		struct wlr_swapchain_slot *free_slot = &swapchain->slots[free_slot_index];
		wlr_log(WLR_DEBUG, "Acquiring existing buffer in slot %zu", free_slot_index);
		struct wlr_buffer *buffer = slot_acquire(swapchain, free_slot);
		LOG_TIMING_END(swapchain_acquire);
		return buffer;
	}

	// Find a completely free slot for new allocation
	free_slot_index = WLR_SWAPCHAIN_CAP;
	for (size_t i = 0; i < WLR_SWAPCHAIN_CAP; i++) {
		if (!swapchain->slots[i].acquired && swapchain->slots[i].buffer == NULL) {
			free_slot_index = i;
			break;
		}
	}

	if (free_slot_index < WLR_SWAPCHAIN_CAP) {
		struct wlr_swapchain_slot *free_slot = &swapchain->slots[free_slot_index];
		if (swapchain->allocator == NULL) {
			wlr_log(WLR_ERROR, "No allocator available for swapchain");
			LOG_TIMING_END(swapchain_acquire);
			return NULL;
		}

		wlr_log(WLR_DEBUG, "Allocating new swapchain buffer for slot %zu", free_slot_index);
		free_slot->buffer = wlr_allocator_create_buffer(swapchain->allocator,
			swapchain->width, swapchain->height, &swapchain->format);
		if (free_slot->buffer == NULL) {
			wlr_log(WLR_ERROR, "Failed to allocate buffer for slot %zu", free_slot_index);
			LOG_TIMING_END(swapchain_acquire);
			return NULL;
		}
		wlr_log(WLR_DEBUG, "Allocated new buffer for slot %zu", free_slot_index);
		struct wlr_buffer *buffer = slot_acquire(swapchain, free_slot);
		LOG_TIMING_END(swapchain_acquire);
		return buffer;
	}

	// All slots are in use or have buffers; force-release the least recently used
	wlr_log(WLR_WARN, "All swapchain slots are in use. Forcing release of a buffer.");
	size_t slot_to_release = 0; // Simple choice; could use LRU logic
	struct wlr_swapchain_slot *slot = &swapchain->slots[slot_to_release];
	if (slot->acquired) {
		wlr_log(WLR_DEBUG, "Force-releasing slot %zu", slot_to_release);
		wl_list_remove(&slot->release.link);
		slot->acquired = false;
	}
	struct wlr_buffer *buffer = slot_acquire(swapchain, slot);
	LOG_TIMING_END(swapchain_acquire);
	return buffer;
}

bool wlr_swapchain_has_buffer(struct wlr_swapchain *swapchain,
		struct wlr_buffer *buffer) {
	for (size_t i = 0; i < WLR_SWAPCHAIN_CAP; i++) {
		struct wlr_swapchain_slot *slot = &swapchain->slots[i];
		if (slot->buffer == buffer) {
			return true;
		}
	}
	return false;
}