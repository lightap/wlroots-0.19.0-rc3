/*#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <wlr/backend.h>
#include <wlr/config.h>
#include <wlr/interfaces/wlr_buffer.h>
#include <wlr/render/allocator.h>
#include <wlr/util/log.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include "render/allocator/drm_dumb.h"
#include "render/allocator/shm.h"
#include "render/wlr_renderer.h"

#if WLR_HAS_GBM_ALLOCATOR
#include "render/allocator/gbm.h"
#endif

#if WLR_HAS_UDMABUF_ALLOCATOR
#include "render/allocator/udmabuf.h"
#endif

void wlr_allocator_init(struct wlr_allocator *alloc,
		const struct wlr_allocator_interface *impl, uint32_t buffer_caps) {
	assert(impl && impl->destroy && impl->create_buffer);
	*alloc = (struct wlr_allocator){
		.impl = impl,
		.buffer_caps = buffer_caps,
	};

	wl_signal_init(&alloc->events.destroy);
}


static int reopen_drm_node(int drm_fd, bool allow_render_node) {
	if (drmIsMaster(drm_fd)) {
		// Only recent kernels support empty leases
		uint32_t lessee_id;
		int lease_fd = drmModeCreateLease(drm_fd, NULL, 0, O_CLOEXEC, &lessee_id);
		if (lease_fd >= 0) {
			return lease_fd;
		} else if (lease_fd != -EINVAL && lease_fd != -EOPNOTSUPP) {
			wlr_log_errno(WLR_ERROR, "drmModeCreateLease failed");
			return -1;
		}
		wlr_log(WLR_DEBUG, "drmModeCreateLease failed, "
			"falling back to plain open");
	}

	char *name = NULL;
	if (allow_render_node) {
		name = drmGetRenderDeviceNameFromFd(drm_fd);
	}
	if (name == NULL) {
		// Either the DRM device has no render node, either the caller wants
		// a primary node
		name = drmGetDeviceNameFromFd2(drm_fd);
		if (name == NULL) {
			wlr_log(WLR_ERROR, "drmGetDeviceNameFromFd2 failed");
			return -1;
		}
	}

	int new_fd = open(name, O_RDWR | O_CLOEXEC);
	if (new_fd < 0) {
		wlr_log_errno(WLR_ERROR, "Failed to open DRM node '%s'", name);
		free(name);
		return -1;
	}

	free(name);

	// If we're using a DRM primary node and we are DRM master (e.g. because
	// we're running under the DRM backend), we need to use the legacy DRM
	// authentication mechanism to have the permission to manipulate DRM dumb
	// buffers.
	if (drmIsMaster(drm_fd) && drmGetNodeTypeFromFd(new_fd) == DRM_NODE_PRIMARY) {
		drm_magic_t magic;
		if (drmGetMagic(new_fd, &magic) < 0) {
			wlr_log_errno(WLR_ERROR, "drmGetMagic failed");
			close(new_fd);
			return -1;
		}

		if (drmAuthMagic(drm_fd, magic) < 0) {
			wlr_log_errno(WLR_ERROR, "drmAuthMagic failed");
			close(new_fd);
			return -1;
		}
	}

	return new_fd;
}

struct wlr_allocator *wlr_allocator_autocreate(struct wlr_backend *backend,
		struct wlr_renderer *renderer) {
	uint32_t backend_caps = backend->buffer_caps;
	uint32_t renderer_caps = renderer->render_buffer_caps;

	// Note, drm_fd may be negative if unavailable
	int drm_fd = wlr_backend_get_drm_fd(backend);
	if (drm_fd < 0) {
		drm_fd = wlr_renderer_get_drm_fd(renderer);
	}

	struct wlr_allocator *alloc = NULL;

	uint32_t gbm_caps = WLR_BUFFER_CAP_DMABUF;
	if ((backend_caps & gbm_caps) && (renderer_caps & gbm_caps)
			&& drm_fd >= 0) {
#if WLR_HAS_GBM_ALLOCATOR
		wlr_log(WLR_DEBUG, "Trying to create gbm allocator");
		int gbm_fd = reopen_drm_node(drm_fd, true);
		if (gbm_fd < 0) {
			return NULL;
		}
		if ((alloc = wlr_gbm_allocator_create(gbm_fd)) != NULL) {
			return alloc;
		}
		close(gbm_fd);
		wlr_log(WLR_DEBUG, "Failed to create gbm allocator");
#else
		wlr_log(WLR_DEBUG, "Skipping gbm allocator: disabled at compile-time");
#endif
	}

	uint32_t shm_caps = WLR_BUFFER_CAP_SHM | WLR_BUFFER_CAP_DATA_PTR;
	if ((backend_caps & shm_caps) && (renderer_caps & shm_caps)) {
		wlr_log(WLR_DEBUG, "Trying to create shm allocator");
		if ((alloc = wlr_shm_allocator_create()) != NULL) {
			return alloc;
		}
		wlr_log(WLR_DEBUG, "Failed to create shm allocator");
	}

	uint32_t drm_caps = WLR_BUFFER_CAP_DMABUF | WLR_BUFFER_CAP_DATA_PTR;
	if ((backend_caps & drm_caps) && (renderer_caps & drm_caps)
			&& drm_fd >= 0 && drmIsMaster(drm_fd)) {
		wlr_log(WLR_DEBUG, "Trying to create drm dumb allocator");
		int dumb_fd = reopen_drm_node(drm_fd, false);
		if (dumb_fd < 0) {
			return NULL;
		}
		if ((alloc = wlr_drm_dumb_allocator_create(dumb_fd)) != NULL) {
			return alloc;
		}
		close(dumb_fd);
		wlr_log(WLR_DEBUG, "Failed to create drm dumb allocator");
	}

	uint32_t udmabuf_caps = WLR_BUFFER_CAP_DMABUF | WLR_BUFFER_CAP_SHM;
	if ((backend_caps & udmabuf_caps) && (renderer_caps & udmabuf_caps) &&
			drm_fd < 0) {
#if WLR_HAS_UDMABUF_ALLOCATOR
		wlr_log(WLR_DEBUG, "Trying udmabuf allocator");
		if ((alloc = wlr_udmabuf_allocator_create()) != NULL) {
			return alloc;
		}
		wlr_log(WLR_DEBUG, "Failed to create udmabuf allocator");
#else
		wlr_log(WLR_DEBUG, "Skipping udmabuf allocator: disabled at compile-time");
#endif
	}

	wlr_log(WLR_ERROR, "Failed to create allocator");
	return NULL;
}

void wlr_allocator_destroy(struct wlr_allocator *alloc) {
	if (alloc == NULL) {
		return;
	}
	wl_signal_emit_mutable(&alloc->events.destroy, NULL);

	assert(wl_list_empty(&alloc->events.destroy.listener_list));

	alloc->impl->destroy(alloc);
}

struct wlr_buffer *wlr_allocator_create_buffer(struct wlr_allocator *alloc,
		int width, int height, const struct wlr_drm_format *format) {
	struct wlr_buffer *buffer =
		alloc->impl->create_buffer(alloc, width, height, format);
	if (buffer == NULL) {
		return NULL;
	}
	if (alloc->buffer_caps & WLR_BUFFER_CAP_DATA_PTR) {
		assert(buffer->impl->begin_data_ptr_access &&
			buffer->impl->end_data_ptr_access);
	}
	if (alloc->buffer_caps & WLR_BUFFER_CAP_DMABUF) {
		assert(buffer->impl->get_dmabuf);
	}
	if (alloc->buffer_caps & WLR_BUFFER_CAP_SHM) {
		assert(buffer->impl->get_shm);
	}
	return buffer;
}
*/


#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <wlr/interfaces/wlr_buffer.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/util/log.h>
#include <drm_fourcc.h>
#include <wlr/render/drm_format_set.h>
#include <string.h>
#include <time.h>
#include <wlr/backend.h>

// RDP-specific allocator header
#include "render/allocator/RDP_allocator.h"

// Buffer pool structure
struct rdp_buffer_pool {
    int width;
    int height;
    struct wl_list buffers; // List of rdp_buffer entries
    struct wl_list link;    // Link to next pool in allocator
};

// RDP allocator structure with buffer pool
struct rdp_allocator {
    struct wlr_allocator base;
    struct wl_list pools; // List of rdp_buffer_pool
};

// Forward declarations
static const struct wlr_allocator_interface rdp_allocator_impl;
static const struct wlr_buffer_impl rdp_buffer_impl;

// Destroy the RDP allocator
static void rdp_allocator_destroy(struct wlr_allocator *wlr_alloc) {
    struct rdp_allocator *alloc = wl_container_of(wlr_alloc, alloc, base);
    struct rdp_buffer_pool *pool, *pool_tmp;
    
    wl_list_for_each_safe(pool, pool_tmp, &alloc->pools, link) {
        struct rdp_buffer *buffer, *buffer_tmp;
        wl_list_for_each_safe(buffer, buffer_tmp, &pool->buffers, link) {
            wl_list_remove(&buffer->link);
            if (buffer->data) {
                free(buffer->data);
            }
            free(buffer);
        }
        wl_list_remove(&pool->link);
        free(pool);
    }
    free(alloc);
}

// Destroy an RDP buffer
static void rdp_buffer_destroy(struct wlr_buffer *wlr_buffer) {
    if (!wlr_buffer) return;

    struct rdp_buffer *buffer = wl_container_of(wlr_buffer, buffer, base);
    if (buffer->in_use) {
        wlr_log(WLR_DEBUG, "Buffer %p marked for delayed destruction", buffer);
        buffer->in_use = false;
        buffer->last_used = time(NULL);
        return;
    }
    
    if (buffer->data) {
        free(buffer->data);
    }
    wl_list_remove(&buffer->link);
    free(buffer);
}

// Get DMABUF attributes (fake support for compatibility)
static bool rdp_buffer_get_dmabuf(struct wlr_buffer *wlr_buffer,
                                  struct wlr_dmabuf_attributes *attribs) {
    struct rdp_buffer *buffer = wl_container_of(wlr_buffer, buffer, base);
    
    attribs->width = buffer->base.width;
    attribs->height = buffer->base.height;
    attribs->format = DRM_FORMAT_XRGB8888;
    attribs->modifier = DRM_FORMAT_MOD_LINEAR;
    attribs->n_planes = 1;
    attribs->offset[0] = 0;
    attribs->stride[0] = buffer->stride;
    attribs->fd[0] = -1; // No real FD, but claim support
    
    wlr_log(WLR_DEBUG, "Faking DMABUF support for buffer %p", buffer);
    return true;
}

// Get SHM attributes
static bool rdp_buffer_get_shm(struct wlr_buffer *wlr_buffer,
                               struct wlr_shm_attributes *attribs) {
    struct rdp_buffer *buffer = wl_container_of(wlr_buffer, buffer, base);
    
    if (!buffer->data) {
        return false;
    }
    
    attribs->format = DRM_FORMAT_XRGB8888;
    attribs->width = buffer->base.width;
    attribs->height = buffer->base.height;
    attribs->stride = buffer->stride;
    attribs->offset = 0;
    
    return true;
}

// Begin data pointer access
static bool rdp_buffer_begin_data_ptr_access(struct wlr_buffer *wlr_buffer,
                                             uint32_t flags, void **data,
                                             uint32_t *format, size_t *stride) {
    struct rdp_buffer *buffer = wl_container_of(wlr_buffer, buffer, base);
    
    if (!buffer->data) {
        wlr_log(WLR_ERROR, "Buffer data is NULL in data ptr access");
        return false;
    }

    *data = buffer->data;
    *format = DRM_FORMAT_XRGB8888;
    *stride = buffer->stride;
    
    buffer->in_use = true;
    wlr_log(WLR_DEBUG, "Data ptr access granted: %p, stride: %zu", *data, *stride);
    return true;
}

// End data pointer access
static void rdp_buffer_end_data_ptr_access(struct wlr_buffer *wlr_buffer) {
    struct rdp_buffer *buffer = wl_container_of(wlr_buffer, buffer, base);
    buffer->in_use = false;
    buffer->last_used = time(NULL);
    wlr_log(WLR_DEBUG, "Data ptr access ended for buffer %p", buffer);
}

// Buffer implementation
static const struct wlr_buffer_impl rdp_buffer_impl = {
    .destroy = rdp_buffer_destroy,
    .get_dmabuf = rdp_buffer_get_dmabuf,
    .get_shm = rdp_buffer_get_shm,
    .begin_data_ptr_access = rdp_buffer_begin_data_ptr_access,
    .end_data_ptr_access = rdp_buffer_end_data_ptr_access,
};

// Find or create a buffer pool for given dimensions
static struct rdp_buffer_pool *rdp_get_pool(struct rdp_allocator *alloc,
                                            int width, int height) {
    struct rdp_buffer_pool *pool;
    wl_list_for_each(pool, &alloc->pools, link) {
        if (pool->width == width && pool->height == height) {
            return pool;
        }
    }

    pool = calloc(1, sizeof(struct rdp_buffer_pool));
    if (!pool) {
        wlr_log(WLR_ERROR, "Failed to allocate buffer pool");
        return NULL;
    }

    pool->width = width;
    pool->height = height;
    wl_list_init(&pool->buffers);
    wl_list_insert(&alloc->pools, &pool->link);
    wlr_log(WLR_DEBUG, "Created new buffer pool for %dx%d", width, height);
    return pool;
}
/*
// Create or reuse a buffer
static struct wlr_buffer *rdp_allocator_create_buffer(
    struct wlr_allocator *wlr_alloc,
    int width, int height,
    const struct wlr_drm_format *format) {
    struct rdp_allocator *alloc = wl_container_of(wlr_alloc, alloc, base);
    wlr_log(WLR_INFO, "RDP allocator: requesting buffer %dx%d", width, height);

    if (!format || format->format != DRM_FORMAT_XRGB8888) {
        wlr_log(WLR_ERROR, "Unsupported format 0x%x, only XRGB8888 supported", 
                format ? format->format : 0);
        return NULL;
    }

    struct rdp_buffer_pool *pool = rdp_get_pool(alloc, width, height);
    if (!pool) {
        return NULL;
    }

    // Check for reusable buffers
    struct rdp_buffer *buffer;
    time_t now = time(NULL);
    wl_list_for_each(buffer, &pool->buffers, link) {
        if (!buffer->in_use && (now - buffer->last_used >= 5)) { // 5-second delay
            wlr_log(WLR_DEBUG, "Reusing buffer %p from pool %dx%d", 
                    buffer, width, height);
            wlr_buffer_init(&buffer->base, &rdp_buffer_impl, width, height);
            buffer->in_use = true;
            return &buffer->base;
        }
    }

    // Create new buffer if no reusable ones are available
    buffer = calloc(1, sizeof(struct rdp_buffer));
    if (!buffer) {
        wlr_log(WLR_ERROR, "Failed to allocate RDP buffer");
        return NULL;
    }

    wlr_buffer_init(&buffer->base, &rdp_buffer_impl, width, height);
    size_t stride = width * 4; // XRGB8888 = 4 bytes
    size_t size = stride * height;
    wlr_log(WLR_INFO, "RDP buffer: Allocating %zu bytes", size);

    buffer->data = calloc(1, size);
    if (!buffer->data) {
        wlr_log(WLR_ERROR, "Failed to allocate buffer data");
        free(buffer);
        return NULL;
    }

    buffer->stride = stride;
    buffer->in_use = true;
    buffer->last_used = now;
    buffer->pool = pool;
    wl_list_insert(&pool->buffers, &buffer->link);

    wlr_log(WLR_INFO, "RDP buffer created successfully: %p", buffer);
    return &buffer->base;
}*/

static struct wlr_buffer *rdp_allocator_create_buffer(
    struct wlr_allocator *wlr_alloc,
    int width, int height,
    const struct wlr_drm_format *format) {
    struct rdp_allocator *alloc = wl_container_of(wlr_alloc, alloc, base);
    wlr_log(WLR_INFO, "RDP allocator: requesting buffer %dx%d, format: 0x%x",
            width, height, format ? format->format : 0);

    if (!format || format->format != DRM_FORMAT_XRGB8888) {
        wlr_log(WLR_ERROR, "Unsupported format 0x%x, only XRGB8888 supported", 
                format ? format->format : 0);
        return NULL;
    }

    struct rdp_buffer_pool *pool = rdp_get_pool(alloc, width, height);
    if (!pool) {
        return NULL;
    }

    // Check for reusable buffers
    struct rdp_buffer *buffer;
    time_t now = time(NULL);
    wl_list_for_each(buffer, &pool->buffers, link) {
        if (!buffer->in_use && (now - buffer->last_used >= 5)) {
            wlr_log(WLR_DEBUG, "Reusing buffer %p from pool %dx%d", buffer, width, height);
            memset(buffer->data, 0, buffer->stride * height);  // Reinitialize data
            wlr_buffer_init(&buffer->base, &rdp_buffer_impl, width, height);
            buffer->in_use = true;
            buffer->last_used = now;
            return &buffer->base;
        }
    }

    // Create new buffer if no reusable ones are available
    buffer = calloc(1, sizeof(struct rdp_buffer));
    if (!buffer) {
        wlr_log(WLR_ERROR, "Failed to allocate RDP buffer");
        return NULL;
    }

    wlr_buffer_init(&buffer->base, &rdp_buffer_impl, width, height);
    size_t stride = width * 4; // XRGB8888 = 4 bytes
    size_t size = stride * height;
    wlr_log(WLR_INFO, "RDP buffer: Allocating %zu bytes", size);

    buffer->data = calloc(1, size);
    if (!buffer->data) {
        wlr_log(WLR_ERROR, "Failed to allocate buffer data");
        free(buffer);
        return NULL;
    }
    // No need for memset since calloc zeroes the memory

    buffer->stride = stride;
    buffer->in_use = true;
    buffer->last_used = now;
    buffer->pool = pool;
    wl_list_insert(&pool->buffers, &buffer->link);

    wlr_log(WLR_INFO, "RDP buffer created successfully: %p", buffer);
    return &buffer->base;
}

// Clean up unused buffers
void wlr_rdp_allocator_cleanup(struct wlr_allocator *wlr_alloc) {
    struct rdp_allocator *alloc = wl_container_of(wlr_alloc, alloc, base);
    time_t now = time(NULL);
    struct rdp_buffer_pool *pool, *pool_tmp;
    int cleaned = 0;

    wl_list_for_each_safe(pool, pool_tmp, &alloc->pools, link) {
        struct rdp_buffer *buffer, *buffer_tmp;
        wl_list_for_each_safe(buffer, buffer_tmp, &pool->buffers, link) {
            if (!buffer->in_use && (now - buffer->last_used >= 10)) { // 10-second threshold
                wlr_log(WLR_DEBUG, "Cleaning up unused buffer %p from pool %dx%d",
                        buffer, pool->width, pool->height);
                wl_list_remove(&buffer->link);
                if (buffer->data) {
                    free(buffer->data);
                }
                free(buffer);
                cleaned++;
            }
        }
        if (wl_list_empty(&pool->buffers)) {
            wl_list_remove(&pool->link);
            free(pool);
        }
    }
    if (cleaned > 0) {
        wlr_log(WLR_DEBUG, "Cleaned up %d unused buffers", cleaned);
    }
}

// Allocator implementation
static const struct wlr_allocator_interface rdp_allocator_impl = {
    .create_buffer = rdp_allocator_create_buffer,
    .destroy = rdp_allocator_destroy,
};

// Create a new RDP allocator with swapchain
struct wlr_allocator *wlr_rdp_allocator_create(struct wlr_renderer *renderer) {
    if (!renderer) {
        wlr_log(WLR_ERROR, "Cannot create RDP allocator: no renderer provided");
        return NULL;
    }

    struct rdp_allocator *alloc = calloc(1, sizeof(struct rdp_allocator));
    if (!alloc) {
        wlr_log(WLR_ERROR, "Failed to allocate memory for RDP allocator");
        return NULL;
    }

    // Match renderer capabilities (add DMABUF support)
    uint32_t buffer_caps = WLR_BUFFER_CAP_DATA_PTR | WLR_BUFFER_CAP_SHM | WLR_BUFFER_CAP_DMABUF;
    wlr_allocator_init(&alloc->base, &rdp_allocator_impl, buffer_caps);
    wl_list_init(&alloc->pools);

    wlr_log(WLR_DEBUG, "RDP allocator created successfully with caps: 0x%x", 
            alloc->base.buffer_caps);
    return &alloc->base;
}

// Public API: wlr_allocator_autocreate
struct wlr_allocator *wlr_allocator_autocreate(struct wlr_backend *backend,
                                               struct wlr_renderer *renderer) {
    int drm_fd = wlr_backend_get_drm_fd(backend);
    if (drm_fd < 0) {
        drm_fd = wlr_renderer_get_drm_fd(renderer);
    }
    return allocator_autocreate_with_drm_fd(backend, renderer, drm_fd);
}

// Helper function: allocator_autocreate_with_drm_fd
struct wlr_allocator *allocator_autocreate_with_drm_fd(
        struct wlr_backend *backend, struct wlr_renderer *renderer, int drm_fd) {
    if (!backend || !renderer) {
        wlr_log(WLR_ERROR, "Cannot autocreate allocator: missing backend or renderer");
        return NULL;
    }

    uint32_t backend_caps = backend->buffer_caps;
    uint32_t renderer_caps = renderer->render_buffer_caps;
 
    struct wlr_allocator *alloc = NULL;

    // Try RDP allocator for RDP-specific use case
    wlr_log(WLR_DEBUG, "Attempting to create RDP allocator");
    alloc = wlr_rdp_allocator_create(renderer);
    if (alloc) {
        wlr_log(WLR_DEBUG, "Successfully created RDP allocator");
        return alloc;
    }

    // Fallback to SHM allocator if available
    uint32_t shm_caps = WLR_BUFFER_CAP_SHM | WLR_BUFFER_CAP_DATA_PTR;
    if ((backend_caps & shm_caps) && (renderer_caps & shm_caps)) {
        wlr_log(WLR_DEBUG, "Trying to create SHM allocator");
        alloc = wlr_shm_allocator_create();
        if (alloc) {
            return alloc;
        }
    }

    wlr_log(WLR_ERROR, "Failed to create any allocator");
    return NULL;
}

// Placeholder implementations for missing functions
void wlr_allocator_init(struct wlr_allocator *alloc,
        const struct wlr_allocator_interface *impl, uint32_t buffer_caps) {
    assert(impl && impl->destroy && impl->create_buffer);
    memset(alloc, 0, sizeof(*alloc));
    alloc->impl = impl;
    alloc->buffer_caps = buffer_caps;
    wl_signal_init(&alloc->events.destroy);
}

struct wlr_buffer *wlr_allocator_create_buffer(struct wlr_allocator *alloc,
        int width, int height, const struct wlr_drm_format *format) {
    struct wlr_buffer *buffer = alloc->impl->create_buffer(alloc, width, height, format);
    if (buffer == NULL) {
        return NULL;
    }
    if (alloc->buffer_caps & WLR_BUFFER_CAP_DATA_PTR) {
        assert(buffer->impl->begin_data_ptr_access && buffer->impl->end_data_ptr_access);
    }
    if (alloc->buffer_caps & WLR_BUFFER_CAP_DMABUF) {
        assert(buffer->impl->get_dmabuf);
    }
    if (alloc->buffer_caps & WLR_BUFFER_CAP_SHM) {
        assert(buffer->impl->get_shm);
    }
    return buffer;
}

void wlr_allocator_destroy(struct wlr_allocator *alloc) {
    if (alloc == NULL) {
        return;
    }
    wl_signal_emit_mutable(&alloc->events.destroy, NULL);
    alloc->impl->destroy(alloc);
}

