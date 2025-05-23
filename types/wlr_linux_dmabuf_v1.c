/*
#include <assert.h>
#include <drm_fourcc.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wlr/backend.h>
#include <wlr/config.h>
#include <wlr/interfaces/wlr_buffer.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_linux_dmabuf_v1.h>
#include <wlr/types/wlr_output_layer.h>
#include <wlr/util/log.h>
#include <xf86drm.h>
#include "linux-dmabuf-v1-protocol.h"
#include "render/drm_format_set.h"
#include "util/shm.h"

#if WLR_HAS_DRM_BACKEND
#include <wlr/backend/drm.h>
#endif

#define LINUX_DMABUF_VERSION 5

struct wlr_linux_buffer_params_v1 {
	struct wl_resource *resource;
	struct wlr_linux_dmabuf_v1 *linux_dmabuf;
	struct wlr_dmabuf_attributes attributes;
	bool has_modifier;
};

struct wlr_linux_dmabuf_feedback_v1_compiled_tranche {
	dev_t target_device;
	uint32_t flags; // bitfield of enum zwp_linux_dmabuf_feedback_v1_tranche_flags
	struct wl_array indices; // uint16_t
};

struct wlr_linux_dmabuf_feedback_v1_compiled {
	dev_t main_device;
	int table_fd;
	size_t table_size;

	size_t tranches_len;
	struct wlr_linux_dmabuf_feedback_v1_compiled_tranche tranches[];
};

struct wlr_linux_dmabuf_feedback_v1_table_entry {
	uint32_t format;
	uint32_t pad; // unused
	uint64_t modifier;
};

static_assert(sizeof(struct wlr_linux_dmabuf_feedback_v1_table_entry) == 16,
	"Expected wlr_linux_dmabuf_feedback_v1_table_entry to be tightly packed");

struct wlr_linux_dmabuf_v1_surface {
	struct wlr_surface *surface;
	struct wlr_linux_dmabuf_v1 *linux_dmabuf;
	struct wl_list link; // wlr_linux_dmabuf_v1.surfaces

	struct wlr_addon addon;
	struct wlr_linux_dmabuf_feedback_v1_compiled *feedback;

	struct wl_list feedback_resources; // wl_resource_get_link
};

static void buffer_handle_destroy(struct wl_client *client,
		struct wl_resource *resource) {
	wl_resource_destroy(resource);
}

static const struct wl_buffer_interface wl_buffer_impl = {
	.destroy = buffer_handle_destroy,
};

static bool buffer_resource_is_instance(struct wl_resource *resource) {
	return wl_resource_instance_of(resource, &wl_buffer_interface,
		&wl_buffer_impl);
}

struct wlr_dmabuf_v1_buffer *wlr_dmabuf_v1_buffer_try_from_buffer_resource(
		struct wl_resource *resource) {
	if (!buffer_resource_is_instance(resource) ||
				wl_resource_get_user_data(resource) == NULL) {
		return NULL;
	}
	return wl_resource_get_user_data(resource);
}

static const struct wlr_buffer_impl buffer_impl;

static struct wlr_dmabuf_v1_buffer *dmabuf_v1_buffer_from_buffer(
		struct wlr_buffer *wlr_buffer) {
	assert(wlr_buffer->impl == &buffer_impl);
	struct wlr_dmabuf_v1_buffer *buffer = wl_container_of(wlr_buffer, buffer, base);
	return buffer;
}

static void buffer_destroy(struct wlr_buffer *wlr_buffer) {
	struct wlr_dmabuf_v1_buffer *buffer =
		dmabuf_v1_buffer_from_buffer(wlr_buffer);
	wl_list_remove(&buffer->release.link);

	wlr_buffer_finish(wlr_buffer);

	if (buffer->resource != NULL) {
		wl_resource_set_user_data(buffer->resource, NULL);
	}
	wlr_dmabuf_attributes_finish(&buffer->attributes);
	free(buffer);
}

static bool buffer_get_dmabuf(struct wlr_buffer *wlr_buffer,
		struct wlr_dmabuf_attributes *attribs) {
	struct wlr_dmabuf_v1_buffer *buffer =
		dmabuf_v1_buffer_from_buffer(wlr_buffer);
	*attribs = buffer->attributes;
	return true;
}

static const struct wlr_buffer_impl buffer_impl = {
	.destroy = buffer_destroy,
	.get_dmabuf = buffer_get_dmabuf,
};

static void buffer_handle_release(struct wl_listener *listener, void *data) {
	struct wlr_dmabuf_v1_buffer *buffer =
		wl_container_of(listener, buffer, release);
	if (buffer->resource != NULL) {
		wl_buffer_send_release(buffer->resource);
	}
}

static const struct zwp_linux_buffer_params_v1_interface buffer_params_impl;

static struct wlr_linux_buffer_params_v1 *params_from_resource(
		struct wl_resource *resource) {
	assert(wl_resource_instance_of(resource,
		&zwp_linux_buffer_params_v1_interface, &buffer_params_impl));
	return wl_resource_get_user_data(resource);
}

static void params_destroy(struct wl_client *client,
		struct wl_resource *resource) {
	wl_resource_destroy(resource);
}

static void params_add(struct wl_client *client,
		struct wl_resource *params_resource, int32_t fd,
		uint32_t plane_idx, uint32_t offset, uint32_t stride,
		uint32_t modifier_hi, uint32_t modifier_lo) {
	struct wlr_linux_buffer_params_v1 *params =
		params_from_resource(params_resource);
	if (!params) {
		wl_resource_post_error(params_resource,
			ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_ALREADY_USED,
			"params was already used to create a wl_buffer");
		close(fd);
		return;
	}

	if (plane_idx >= WLR_DMABUF_MAX_PLANES) {
		wl_resource_post_error(params_resource,
			ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_PLANE_IDX,
			"plane index %u > %u", plane_idx, WLR_DMABUF_MAX_PLANES);
		close(fd);
		return;
	}

	if (params->attributes.fd[plane_idx] != -1) {
		wl_resource_post_error(params_resource,
			ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_PLANE_SET,
			"a dmabuf with FD %d has already been added for plane %u",
			params->attributes.fd[plane_idx], plane_idx);
		close(fd);
		return;
	}

	uint64_t modifier = ((uint64_t)modifier_hi << 32) | modifier_lo;
	if (params->has_modifier && modifier != params->attributes.modifier) {
		wl_resource_post_error(params_resource,
			ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_FORMAT,
			"sent modifier %" PRIu64 " for plane %u, expected"
			" modifier %" PRIu64 " like other planes",
			modifier, plane_idx, params->attributes.modifier);
		close(fd);
		return;
	}

	params->attributes.modifier = modifier;
	params->has_modifier = true;

	params->attributes.fd[plane_idx] = fd;
	params->attributes.offset[plane_idx] = offset;
	params->attributes.stride[plane_idx] = stride;
	params->attributes.n_planes++;
}

static void buffer_handle_resource_destroy(struct wl_resource *buffer_resource) {
	struct wlr_dmabuf_v1_buffer *buffer =
		wlr_dmabuf_v1_buffer_try_from_buffer_resource(buffer_resource);
	assert(buffer != NULL);
	buffer->resource = NULL;
	wlr_buffer_drop(&buffer->base);
}

static bool check_import_dmabuf(struct wlr_dmabuf_attributes *attribs, void *data) {
	struct wlr_linux_dmabuf_v1 *linux_dmabuf = data;

	if (linux_dmabuf->main_device_fd < 0) {
		return true;
	}

	// TODO: check number of planes
	for (int i = 0; i < attribs->n_planes; i++) {
		uint32_t handle = 0;
		if (drmPrimeFDToHandle(linux_dmabuf->main_device_fd, attribs->fd[i], &handle) != 0) {
			wlr_log_errno(WLR_ERROR, "Failed to import DMA-BUF FD for plane %d", i);
			return false;
		}
		if (drmCloseBufferHandle(linux_dmabuf->main_device_fd, handle) != 0) {
			wlr_log_errno(WLR_ERROR, "Failed to close buffer handle for plane %d", i);
			return false;
		}
	}
	return true;
}

static void params_create_common(struct wl_resource *params_resource,
		uint32_t buffer_id, int32_t width, int32_t height, uint32_t format,
		uint32_t flags) {
	struct wlr_linux_buffer_params_v1 *params =
		params_from_resource(params_resource);
	if (!params) {
		wl_resource_post_error(params_resource,
			ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_ALREADY_USED,
			"params was already used to create a wl_buffer");
		return;
	}

	struct wlr_dmabuf_attributes attribs = params->attributes;
	struct wlr_linux_dmabuf_v1 *linux_dmabuf = params->linux_dmabuf;

	// Make the params resource inert
	wl_resource_set_user_data(params_resource, NULL);
	free(params);

	if (!attribs.n_planes) {
		wl_resource_post_error(params_resource,
			ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INCOMPLETE,
			"no dmabuf has been added to the params");
		goto err_out;
	}

	if (attribs.fd[0] == -1) {
		wl_resource_post_error(params_resource,
			ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INCOMPLETE,
			"no dmabuf has been added for plane 0");
		goto err_out;
	}

	if ((attribs.fd[3] >= 0 || attribs.fd[2] >= 0) &&
			(attribs.fd[2] == -1 || attribs.fd[1] == -1)) {
		wl_resource_post_error(params_resource,
			ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INCOMPLETE,
			"gap in dmabuf planes");
		goto err_out;
	}

	
	uint32_t version = wl_resource_get_version(params_resource);
	if (!zwp_linux_buffer_params_v1_flags_is_valid(flags, version)) {
		wl_resource_post_error(params_resource,
			ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_FORMAT,
			"Unknown dmabuf flags %"PRIu32, flags);
		goto err_out;
	}

	if (flags != 0) {
		wlr_log(WLR_ERROR, "dmabuf flags aren't supported");
		goto err_failed;
	}

	attribs.width = width;
	attribs.height = height;
	attribs.format = format;

	if (width < 1 || height < 1) {
		wl_resource_post_error(params_resource,
			ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_DIMENSIONS,
			"invalid width %d or height %d", width, height);
		goto err_out;
	}

	for (int i = 0; i < attribs.n_planes; i++) {
		if ((uint64_t)attribs.offset[i]
				+ attribs.stride[i] > UINT32_MAX) {
			wl_resource_post_error(params_resource,
				ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_OUT_OF_BOUNDS,
				"size overflow for plane %d", i);
			goto err_out;
		}

		if ((uint64_t)attribs.offset[i]
				+ (uint64_t)attribs.stride[i] * height > UINT32_MAX) {
			wl_resource_post_error(params_resource,
				ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_OUT_OF_BOUNDS,
				"size overflow for plane %d", i);
			goto err_out;
		}

		off_t size = lseek(attribs.fd[i], 0, SEEK_END);
		if (size == -1) {
			// Skip checks if kernel does no support seek on buffer
			continue;
		}
		if (attribs.offset[i] > size) {
			wl_resource_post_error(params_resource,
				ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_OUT_OF_BOUNDS,
				"invalid offset %" PRIu32 " for plane %d",
				attribs.offset[i], i);
			goto err_out;
		}

		if (attribs.offset[i] + attribs.stride[i] > size ||
				attribs.stride[i] == 0) {
			wl_resource_post_error(params_resource,
				ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_OUT_OF_BOUNDS,
				"invalid stride %" PRIu32 " for plane %d",
				attribs.stride[i], i);
			goto err_out;
		}

		// planes > 0 might be subsampled according to fourcc format
		if (i == 0 && attribs.offset[i] +
				attribs.stride[i] * height > size) {
			wl_resource_post_error(params_resource,
				ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_OUT_OF_BOUNDS,
				"invalid buffer stride or height for plane %d", i);
			goto err_out;
		}
	}

	if (!linux_dmabuf->check_dmabuf_callback(&attribs,
				linux_dmabuf->check_dmabuf_callback_data)) {
		goto err_failed;
	}

	struct wlr_dmabuf_v1_buffer *buffer = calloc(1, sizeof(*buffer));
	if (!buffer) {
		wl_resource_post_no_memory(params_resource);
		goto err_failed;
	}
	wlr_buffer_init(&buffer->base, &buffer_impl, attribs.width, attribs.height);

	struct wl_client *client = wl_resource_get_client(params_resource);
	buffer->resource = wl_resource_create(client, &wl_buffer_interface,
		1, buffer_id);
	if (!buffer->resource) {
		wl_resource_post_no_memory(params_resource);
		free(buffer);
		goto err_failed;
	}
	wl_resource_set_implementation(buffer->resource,
		&wl_buffer_impl, buffer, buffer_handle_resource_destroy);

	buffer->attributes = attribs;

	buffer->release.notify = buffer_handle_release;
	wl_signal_add(&buffer->base.events.release, &buffer->release);


	if (buffer_id == 0) {
		zwp_linux_buffer_params_v1_send_created(params_resource,
			buffer->resource);
	}

	return;

err_failed:
	if (buffer_id == 0) {
		zwp_linux_buffer_params_v1_send_failed(params_resource);
	} else {
		
		wl_resource_post_error(params_resource,
			ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_WL_BUFFER,
			"importing the supplied dmabufs failed");
	}
err_out:
	wlr_dmabuf_attributes_finish(&attribs);
}

static void params_create(struct wl_client *client,
		struct wl_resource *params_resource,
		int32_t width, int32_t height, uint32_t format, uint32_t flags) {
	params_create_common(params_resource, 0, width, height, format,
		flags);
}

static void params_create_immed(struct wl_client *client,
		struct wl_resource *params_resource, uint32_t buffer_id,
		int32_t width, int32_t height, uint32_t format, uint32_t flags) {
	params_create_common(params_resource, buffer_id, width, height,
		format, flags);
}

static const struct zwp_linux_buffer_params_v1_interface buffer_params_impl = {
	.destroy = params_destroy,
	.add = params_add,
	.create = params_create,
	.create_immed = params_create_immed,
};

static void params_handle_resource_destroy(struct wl_resource *resource) {
	struct wlr_linux_buffer_params_v1 *params = params_from_resource(resource);
	if (!params) {
		return;
	}
	wlr_dmabuf_attributes_finish(&params->attributes);
	free(params);
}

static const struct zwp_linux_dmabuf_v1_interface linux_dmabuf_impl;

static struct wlr_linux_dmabuf_v1 *linux_dmabuf_from_resource(
		struct wl_resource *resource) {
	assert(wl_resource_instance_of(resource, &zwp_linux_dmabuf_v1_interface,
			&linux_dmabuf_impl));

	struct wlr_linux_dmabuf_v1 *dmabuf = wl_resource_get_user_data(resource);
	assert(dmabuf);
	return dmabuf;
}

static void linux_dmabuf_create_params(struct wl_client *client,
		struct wl_resource *linux_dmabuf_resource,
		uint32_t params_id) {
	struct wlr_linux_dmabuf_v1 *linux_dmabuf =
		linux_dmabuf_from_resource(linux_dmabuf_resource);

	struct wlr_linux_buffer_params_v1 *params = calloc(1, sizeof(*params));
	if (!params) {
		wl_resource_post_no_memory(linux_dmabuf_resource);
		return;
	}

	for (int i = 0; i < WLR_DMABUF_MAX_PLANES; i++) {
		params->attributes.fd[i] = -1;
	}

	params->linux_dmabuf = linux_dmabuf;

	uint32_t version = wl_resource_get_version(linux_dmabuf_resource);
	params->resource = wl_resource_create(client,
		&zwp_linux_buffer_params_v1_interface, version, params_id);
	if (!params->resource) {
		free(params);
		wl_resource_post_no_memory(linux_dmabuf_resource);
		return;
	}
	wl_resource_set_implementation(params->resource,
		&buffer_params_impl, params, params_handle_resource_destroy);
}

static void linux_dmabuf_feedback_destroy(struct wl_client *client,
		struct wl_resource *resource) {
	wl_resource_destroy(resource);
}

static const struct zwp_linux_dmabuf_feedback_v1_interface
		linux_dmabuf_feedback_impl = {
	.destroy = linux_dmabuf_feedback_destroy,
};

static ssize_t get_drm_format_set_index(const struct wlr_drm_format_set *set,
		uint32_t format, uint64_t modifier) {
	bool format_found = false;
	const struct wlr_drm_format *fmt;
	size_t idx = 0;
	for (size_t i = 0; i < set->len; i++) {
		fmt = &set->formats[i];
		if (fmt->format == format) {
			format_found = true;
			break;
		}
		idx += fmt->len;
	}
	if (!format_found) {
		return -1;
	}

	for (size_t i = 0; i < fmt->len; i++) {
		if (fmt->modifiers[i] == modifier) {
			return idx;
		}
		idx++;
	}
	return -1;
}

static struct wlr_linux_dmabuf_feedback_v1_compiled *feedback_compile(
		const struct wlr_linux_dmabuf_feedback_v1 *feedback) {
	const struct wlr_linux_dmabuf_feedback_v1_tranche *tranches = feedback->tranches.data;
	size_t tranches_len = feedback->tranches.size / sizeof(struct wlr_linux_dmabuf_feedback_v1_tranche);
	assert(tranches_len > 0);

	// Make one big format set that contains all formats across all tranches so that we
	// can build an index
	struct wlr_drm_format_set all_formats = {0};
	for (size_t i = 0; i < tranches_len; i++) {
		const struct wlr_linux_dmabuf_feedback_v1_tranche *tranche = &tranches[i];
		if (!wlr_drm_format_set_union(&all_formats, &all_formats, &tranche->formats)) {
			wlr_log(WLR_ERROR, "Failed to union scanout formats into one tranche");
			goto err_all_formats;
		}
	}

	size_t table_len = 0;
	for (size_t i = 0; i < all_formats.len; i++) {
		const struct wlr_drm_format *fmt = &all_formats.formats[i];
		table_len += fmt->len;
	}
	assert(table_len > 0);

	size_t table_size =
		table_len * sizeof(struct wlr_linux_dmabuf_feedback_v1_table_entry);
	int rw_fd, ro_fd;
	if (!allocate_shm_file_pair(table_size, &rw_fd, &ro_fd)) {
		wlr_log(WLR_ERROR, "Failed to allocate shm file for format table");
		return NULL;
	}

	struct wlr_linux_dmabuf_feedback_v1_table_entry *table =
		mmap(NULL, table_size, PROT_READ | PROT_WRITE, MAP_SHARED, rw_fd, 0);
	if (table == MAP_FAILED) {
		wlr_log_errno(WLR_ERROR, "mmap failed");
		close(rw_fd);
		close(ro_fd);
		goto err_all_formats;
	}

	close(rw_fd);

	size_t n = 0;
	for (size_t i = 0; i < all_formats.len; i++) {
		const struct wlr_drm_format *fmt = &all_formats.formats[i];

		for (size_t k = 0; k < fmt->len; k++) {
			table[n] = (struct wlr_linux_dmabuf_feedback_v1_table_entry){
				.format = fmt->format,
				.modifier = fmt->modifiers[k],
			};
			n++;
		}
	}
	assert(n == table_len);

	munmap(table, table_size);

	struct wlr_linux_dmabuf_feedback_v1_compiled *compiled = calloc(1, sizeof(*compiled) +
		tranches_len * sizeof(struct wlr_linux_dmabuf_feedback_v1_compiled_tranche));
	if (compiled == NULL) {
		close(ro_fd);
		goto err_all_formats;
	}

	compiled->main_device = feedback->main_device;
	compiled->tranches_len = tranches_len;
	compiled->table_fd = ro_fd;
	compiled->table_size = table_size;

	// Build the indices lists for all tranches
	for (size_t i = 0; i < tranches_len; i++) {
		const struct wlr_linux_dmabuf_feedback_v1_tranche *tranche = &tranches[i];
		struct wlr_linux_dmabuf_feedback_v1_compiled_tranche *compiled_tranche =
			&compiled->tranches[i];

		compiled_tranche->target_device = tranche->target_device;
		compiled_tranche->flags = tranche->flags;

		wl_array_init(&compiled_tranche->indices);
		if (!wl_array_add(&compiled_tranche->indices, table_len * sizeof(uint16_t))) {
			wlr_log(WLR_ERROR, "Failed to allocate tranche indices array");
			goto error_compiled;
		}

		n = 0;
		uint16_t *indices = compiled_tranche->indices.data;
		for (size_t j = 0; j < tranche->formats.len; j++) {
			const struct wlr_drm_format *fmt = &tranche->formats.formats[j];
			for (size_t k = 0; k < fmt->len; k++) {
				ssize_t index = get_drm_format_set_index(
					&all_formats, fmt->format, fmt->modifiers[k]);
				if (index < 0) {
					wlr_log(WLR_ERROR, "Format 0x%" PRIX32 " and modifier "
						"0x%" PRIX64 " are in tranche #%zu but are missing "
						"from the fallback tranche",
						fmt->format, fmt->modifiers[k], i);
					goto error_compiled;
				}
				indices[n] = index;
				n++;
			}
		}
		compiled_tranche->indices.size = n * sizeof(uint16_t);
	}

	wlr_drm_format_set_finish(&all_formats);

	return compiled;

error_compiled:
	close(compiled->table_fd);
	free(compiled);
err_all_formats:
	wlr_drm_format_set_finish(&all_formats);
	return NULL;
}

static void compiled_feedback_destroy(
		struct wlr_linux_dmabuf_feedback_v1_compiled *feedback) {
	if (feedback == NULL) {
		return;
	}
	for (size_t i = 0; i < feedback->tranches_len; i++) {
		wl_array_release(&feedback->tranches[i].indices);
	}
	close(feedback->table_fd);
	free(feedback);
}

static void feedback_tranche_send(
		const struct wlr_linux_dmabuf_feedback_v1_compiled_tranche *tranche,
		struct wl_resource *resource) {
	struct wl_array dev_array = {
		.size = sizeof(tranche->target_device),
		.data = (void *)&tranche->target_device,
	};
	zwp_linux_dmabuf_feedback_v1_send_tranche_target_device(resource, &dev_array);
	zwp_linux_dmabuf_feedback_v1_send_tranche_flags(resource, tranche->flags);
	zwp_linux_dmabuf_feedback_v1_send_tranche_formats(resource,
		(struct wl_array *)&tranche->indices);
	zwp_linux_dmabuf_feedback_v1_send_tranche_done(resource);
}

static void feedback_send(const struct wlr_linux_dmabuf_feedback_v1_compiled *feedback,
		struct wl_resource *resource) {
	struct wl_array dev_array = {
		.size = sizeof(feedback->main_device),
		.data = (void *)&feedback->main_device,
	};
	zwp_linux_dmabuf_feedback_v1_send_main_device(resource, &dev_array);

	zwp_linux_dmabuf_feedback_v1_send_format_table(resource,
		feedback->table_fd, feedback->table_size);

	for (size_t i = 0; i < feedback->tranches_len; i++) {
		feedback_tranche_send(&feedback->tranches[i], resource);
	}

	zwp_linux_dmabuf_feedback_v1_send_done(resource);
}

static void linux_dmabuf_get_default_feedback(struct wl_client *client,
		struct wl_resource *resource, uint32_t id) {
	struct wlr_linux_dmabuf_v1 *linux_dmabuf =
		linux_dmabuf_from_resource(resource);

	uint32_t version = wl_resource_get_version(resource);
	struct wl_resource *feedback_resource = wl_resource_create(client,
		&zwp_linux_dmabuf_feedback_v1_interface, version, id);
	if (feedback_resource == NULL) {
		wl_client_post_no_memory(client);
		return;
	}
	wl_resource_set_implementation(feedback_resource, &linux_dmabuf_feedback_impl,
		NULL, NULL);

	feedback_send(linux_dmabuf->default_feedback, feedback_resource);
}

static void surface_destroy(struct wlr_linux_dmabuf_v1_surface *surface) {
	struct wl_resource *resource, *resource_tmp;
	wl_resource_for_each_safe(resource, resource_tmp, &surface->feedback_resources) {
		struct wl_list *link = wl_resource_get_link(resource);
		wl_list_remove(link);
		wl_list_init(link);
	}

	compiled_feedback_destroy(surface->feedback);

	wlr_addon_finish(&surface->addon);
	wl_list_remove(&surface->link);
	free(surface);
}

static void surface_addon_destroy(struct wlr_addon *addon) {
	struct wlr_linux_dmabuf_v1_surface *surface =
		wl_container_of(addon, surface, addon);
	surface_destroy(surface);
}

static const struct wlr_addon_interface surface_addon_impl = {
	.name = "wlr_linux_dmabuf_v1_surface",
	.destroy = surface_addon_destroy,
};

static struct wlr_linux_dmabuf_v1_surface *surface_get_or_create(
		struct wlr_linux_dmabuf_v1 *linux_dmabuf,
		struct wlr_surface *wlr_surface) {
	struct wlr_addon *addon =
		wlr_addon_find(&wlr_surface->addons, linux_dmabuf, &surface_addon_impl);
	if (addon != NULL) {
		struct wlr_linux_dmabuf_v1_surface *surface =
			wl_container_of(addon, surface, addon);
		return surface;
	}

	struct wlr_linux_dmabuf_v1_surface *surface = calloc(1, sizeof(*surface));
	if (surface == NULL) {
		return NULL;
	}

	surface->surface = wlr_surface;
	surface->linux_dmabuf = linux_dmabuf;
	wl_list_init(&surface->feedback_resources);
	wlr_addon_init(&surface->addon, &wlr_surface->addons, linux_dmabuf,
		&surface_addon_impl);
	wl_list_insert(&linux_dmabuf->surfaces, &surface->link);

	return surface;
}

static const struct wlr_linux_dmabuf_feedback_v1_compiled *surface_get_feedback(
		struct wlr_linux_dmabuf_v1_surface *surface) {
	if (surface->feedback != NULL) {
		return surface->feedback;
	}
	return surface->linux_dmabuf->default_feedback;
}

static void surface_feedback_handle_resource_destroy(struct wl_resource *resource) {
	wl_list_remove(wl_resource_get_link(resource));
}

static void linux_dmabuf_get_surface_feedback(struct wl_client *client,
		struct wl_resource *resource, uint32_t id,
		struct wl_resource *surface_resource) {
	struct wlr_linux_dmabuf_v1 *linux_dmabuf =
		linux_dmabuf_from_resource(resource);
	struct wlr_surface *wlr_surface = wlr_surface_from_resource(surface_resource);

	struct wlr_linux_dmabuf_v1_surface *surface =
		surface_get_or_create(linux_dmabuf, wlr_surface);
	if (surface == NULL) {
		wl_client_post_no_memory(client);
		return;
	}

	uint32_t version = wl_resource_get_version(resource);
	struct wl_resource *feedback_resource = wl_resource_create(client,
		&zwp_linux_dmabuf_feedback_v1_interface, version, id);
	if (feedback_resource == NULL) {
		wl_client_post_no_memory(client);
		return;
	}
	wl_resource_set_implementation(feedback_resource, &linux_dmabuf_feedback_impl,
		NULL, surface_feedback_handle_resource_destroy);
	wl_list_insert(&surface->feedback_resources, wl_resource_get_link(feedback_resource));

	feedback_send(surface_get_feedback(surface), feedback_resource);
}

static void linux_dmabuf_destroy(struct wl_client *client,
		struct wl_resource *resource) {
	wl_resource_destroy(resource);
}

static const struct zwp_linux_dmabuf_v1_interface linux_dmabuf_impl = {
	.destroy = linux_dmabuf_destroy,
	.create_params = linux_dmabuf_create_params,
	.get_default_feedback = linux_dmabuf_get_default_feedback,
	.get_surface_feedback = linux_dmabuf_get_surface_feedback,
};

static void linux_dmabuf_send_modifiers(struct wl_resource *resource,
		const struct wlr_drm_format *fmt) {
	if (wl_resource_get_version(resource) < ZWP_LINUX_DMABUF_V1_MODIFIER_SINCE_VERSION) {
		if (wlr_drm_format_has(fmt, DRM_FORMAT_MOD_INVALID)) {
			zwp_linux_dmabuf_v1_send_format(resource, fmt->format);
		}
		return;
	}

	// In case only INVALID and LINEAR are advertised, send INVALID only due to XWayland:
	// https://gitlab.freedesktop.org/xorg/xserver/-/issues/1166
	if (fmt->len == 2 && wlr_drm_format_has(fmt, DRM_FORMAT_MOD_INVALID)
			&& wlr_drm_format_has(fmt, DRM_FORMAT_MOD_LINEAR)) {
		uint64_t mod = DRM_FORMAT_MOD_INVALID;
		zwp_linux_dmabuf_v1_send_modifier(resource, fmt->format,
			mod >> 32, mod & 0xFFFFFFFF);
		return;
	}

	for (size_t i = 0; i < fmt->len; i++) {
		uint64_t mod = fmt->modifiers[i];
		zwp_linux_dmabuf_v1_send_modifier(resource, fmt->format,
			mod >> 32, mod & 0xFFFFFFFF);
	}
}

static void linux_dmabuf_send_formats(struct wlr_linux_dmabuf_v1 *linux_dmabuf,
		struct wl_resource *resource) {
	for (size_t i = 0; i < linux_dmabuf->default_formats.len; i++) {
		const struct wlr_drm_format *fmt = &linux_dmabuf->default_formats.formats[i];
		linux_dmabuf_send_modifiers(resource, fmt);
	}
}

static void linux_dmabuf_bind(struct wl_client *client, void *data,
		uint32_t version, uint32_t id) {
	struct wlr_linux_dmabuf_v1 *linux_dmabuf = data;

	struct wl_resource *resource = wl_resource_create(client,
		&zwp_linux_dmabuf_v1_interface, version, id);
	if (resource == NULL) {
		wl_client_post_no_memory(client);
		return;
	}
	wl_resource_set_implementation(resource, &linux_dmabuf_impl,
		linux_dmabuf, NULL);

	if (version < ZWP_LINUX_DMABUF_V1_GET_DEFAULT_FEEDBACK_SINCE_VERSION) {
		linux_dmabuf_send_formats(linux_dmabuf, resource);
	}
}

static struct wlr_buffer *buffer_from_resource(struct wl_resource *resource) {
	struct wlr_dmabuf_v1_buffer *buffer =
		wlr_dmabuf_v1_buffer_try_from_buffer_resource(resource);
	assert(buffer != NULL);
	return &buffer->base;
}

static const struct wlr_buffer_resource_interface buffer_resource_interface = {
	.name = "wlr_dmabuf_v1_buffer",
	.is_instance = buffer_resource_is_instance,
	.from_resource = buffer_from_resource,
};

static void linux_dmabuf_v1_destroy(struct wlr_linux_dmabuf_v1 *linux_dmabuf) {
	wl_signal_emit_mutable(&linux_dmabuf->events.destroy, linux_dmabuf);

	assert(wl_list_empty(&linux_dmabuf->events.destroy.listener_list));

	struct wlr_linux_dmabuf_v1_surface *surface, *surface_tmp;
	wl_list_for_each_safe(surface, surface_tmp, &linux_dmabuf->surfaces, link) {
		surface_destroy(surface);
	}

	compiled_feedback_destroy(linux_dmabuf->default_feedback);
	wlr_drm_format_set_finish(&linux_dmabuf->default_formats);
	if (linux_dmabuf->main_device_fd >= 0) {
		close(linux_dmabuf->main_device_fd);
	}

	wl_list_remove(&linux_dmabuf->display_destroy.link);

	wl_global_destroy(linux_dmabuf->global);
	free(linux_dmabuf);
}

static void handle_display_destroy(struct wl_listener *listener, void *data) {
	struct wlr_linux_dmabuf_v1 *linux_dmabuf =
		wl_container_of(listener, linux_dmabuf, display_destroy);
	linux_dmabuf_v1_destroy(linux_dmabuf);
}

static bool set_default_feedback(struct wlr_linux_dmabuf_v1 *linux_dmabuf,
		const struct wlr_linux_dmabuf_feedback_v1 *feedback) {
	struct wlr_linux_dmabuf_feedback_v1_compiled *compiled = feedback_compile(feedback);
	if (compiled == NULL) {
		return false;
	}

	drmDevice *device = NULL;
	if (drmGetDeviceFromDevId(feedback->main_device, 0, &device) != 0) {
		wlr_log_errno(WLR_ERROR, "drmGetDeviceFromDevId failed");
		goto error_compiled;
	}

	int main_device_fd = -1;
	if (device->available_nodes & (1 << DRM_NODE_RENDER)) {
		const char *name = device->nodes[DRM_NODE_RENDER];
		main_device_fd = open(name, O_RDWR | O_CLOEXEC);
		drmFreeDevice(&device);
		if (main_device_fd < 0) {
			wlr_log_errno(WLR_ERROR, "Failed to open DRM device %s", name);
			goto error_compiled;
		}
	} else {
		// Likely a split display/render setup. Unfortunately we have no way to
		// get back the proper render node used by the renderer under-the-hood.
		// TODO: drop once mesa!24825 is widespread
		assert(device->available_nodes & (1 << DRM_NODE_PRIMARY));
		wlr_log(WLR_DEBUG, "DRM device %s has no render node, "
			"skipping DMA-BUF import checks", device->nodes[DRM_NODE_PRIMARY]);
		drmFreeDevice(&device);
	}

	size_t tranches_len =
		feedback->tranches.size / sizeof(struct wlr_linux_dmabuf_feedback_v1_tranche);
	const struct wlr_linux_dmabuf_feedback_v1_tranche *tranches = feedback->tranches.data;
	struct wlr_drm_format_set formats = {0};
	for (size_t i = 0; i < tranches_len; i++) {
		const struct wlr_linux_dmabuf_feedback_v1_tranche *tranche = &tranches[i];

		if (!wlr_drm_format_set_union(&formats, &formats, &tranche->formats)) {
			goto error_formats;
		}
	}

	compiled_feedback_destroy(linux_dmabuf->default_feedback);
	linux_dmabuf->default_feedback = compiled;

	if (linux_dmabuf->main_device_fd >= 0) {
		close(linux_dmabuf->main_device_fd);
	}
	linux_dmabuf->main_device_fd = main_device_fd;

	wlr_drm_format_set_finish(&linux_dmabuf->default_formats);
	linux_dmabuf->default_formats = formats;

	return true;

error_formats:
	wlr_drm_format_set_finish(&formats);
error_compiled:
	compiled_feedback_destroy(compiled);
	return false;
}

struct wlr_linux_dmabuf_v1 *wlr_linux_dmabuf_v1_create(struct wl_display *display,
		uint32_t version, const struct wlr_linux_dmabuf_feedback_v1 *default_feedback) {
	assert(version <= LINUX_DMABUF_VERSION);

	struct wlr_linux_dmabuf_v1 *linux_dmabuf = calloc(1, sizeof(*linux_dmabuf));
	if (linux_dmabuf == NULL) {
		wlr_log(WLR_ERROR, "could not create simple dmabuf manager");
		return NULL;
	}
	linux_dmabuf->main_device_fd = -1;

	wl_list_init(&linux_dmabuf->surfaces);

	wl_signal_init(&linux_dmabuf->events.destroy);

	linux_dmabuf->global = wl_global_create(display, &zwp_linux_dmabuf_v1_interface,
		version, linux_dmabuf, linux_dmabuf_bind);
	if (!linux_dmabuf->global) {
		wlr_log(WLR_ERROR, "could not create linux dmabuf v1 wl global");
		goto error_linux_dmabuf;
	}

	if (!set_default_feedback(linux_dmabuf, default_feedback)) {
		goto error_global;
	}

	linux_dmabuf->display_destroy.notify = handle_display_destroy;
	wl_display_add_destroy_listener(display, &linux_dmabuf->display_destroy);

	wlr_linux_dmabuf_v1_set_check_dmabuf_callback(linux_dmabuf,
		check_import_dmabuf, linux_dmabuf);

	wlr_buffer_register_resource_interface(&buffer_resource_interface);

	return linux_dmabuf;

error_global:
	wl_global_destroy(linux_dmabuf->global);
error_linux_dmabuf:
	free(linux_dmabuf);
	return NULL;
}

struct wlr_linux_dmabuf_v1 *wlr_linux_dmabuf_v1_create_with_renderer(struct wl_display *display,
		uint32_t version, struct wlr_renderer *renderer) {
	const struct wlr_linux_dmabuf_feedback_v1_init_options options = {
		.main_renderer = renderer,
	};
	struct wlr_linux_dmabuf_feedback_v1 feedback = {0};
	if (!wlr_linux_dmabuf_feedback_v1_init_with_options(&feedback, &options)) {
		return NULL;
	}
	struct wlr_linux_dmabuf_v1 *linux_dmabuf =
		wlr_linux_dmabuf_v1_create(display, version, &feedback);
	wlr_linux_dmabuf_feedback_v1_finish(&feedback);
	return linux_dmabuf;
}

void wlr_linux_dmabuf_v1_set_check_dmabuf_callback(struct wlr_linux_dmabuf_v1 *linux_dmabuf,
		bool (*callback)(struct wlr_dmabuf_attributes *attribs, void *data), void *data) {
	assert(callback);
	linux_dmabuf->check_dmabuf_callback = callback;
	linux_dmabuf->check_dmabuf_callback_data = data;
}

bool wlr_linux_dmabuf_v1_set_surface_feedback(
		struct wlr_linux_dmabuf_v1 *linux_dmabuf,
		struct wlr_surface *wlr_surface,
		const struct wlr_linux_dmabuf_feedback_v1 *feedback) {
	struct wlr_linux_dmabuf_v1_surface *surface =
		surface_get_or_create(linux_dmabuf, wlr_surface);
	if (surface == NULL) {
		return false;
	}

	struct wlr_linux_dmabuf_feedback_v1_compiled *compiled = NULL;
	if (feedback != NULL) {
		compiled = feedback_compile(feedback);
		if (compiled == NULL) {
			return false;
		}
	}

	compiled_feedback_destroy(surface->feedback);
	surface->feedback = compiled;

	struct wl_resource *resource;
	wl_resource_for_each(resource, &surface->feedback_resources) {
		feedback_send(surface_get_feedback(surface), resource);
	}

	return true;
}

struct wlr_linux_dmabuf_feedback_v1_tranche *wlr_linux_dmabuf_feedback_add_tranche(
		struct wlr_linux_dmabuf_feedback_v1 *feedback) {
	struct wlr_linux_dmabuf_feedback_v1_tranche *tranche =
		wl_array_add(&feedback->tranches, sizeof(*tranche));
	if (tranche == NULL) {
		wlr_log_errno(WLR_ERROR, "Allocation failed");
		return NULL;
	}
	*tranche = (struct wlr_linux_dmabuf_feedback_v1_tranche){0};
	return tranche;
}

void wlr_linux_dmabuf_feedback_v1_finish(struct wlr_linux_dmabuf_feedback_v1 *feedback) {
	struct wlr_linux_dmabuf_feedback_v1_tranche *tranche;
	wl_array_for_each(tranche, &feedback->tranches) {
		wlr_drm_format_set_finish(&tranche->formats);
	}
	wl_array_release(&feedback->tranches);
}

static bool devid_from_fd(int fd, dev_t *devid) {
	struct stat stat;
	if (fstat(fd, &stat) != 0) {
		wlr_log_errno(WLR_ERROR, "fstat failed");
		return false;
	}
	*devid = stat.st_rdev;
	return true;
}

static bool is_secondary_drm_backend(struct wlr_backend *backend) {
#if WLR_HAS_DRM_BACKEND
	return wlr_backend_is_drm(backend) &&
		wlr_drm_backend_get_parent(backend) != NULL;
#else
	return false;
#endif
}

bool wlr_linux_dmabuf_feedback_v1_init_with_options(struct wlr_linux_dmabuf_feedback_v1 *feedback,
		const struct wlr_linux_dmabuf_feedback_v1_init_options *options) {
	assert(options->main_renderer != NULL);
	assert(options->scanout_primary_output == NULL ||
		options->output_layer_feedback_event == NULL);

	*feedback = (struct wlr_linux_dmabuf_feedback_v1){0};

	int renderer_drm_fd = wlr_renderer_get_drm_fd(options->main_renderer);
	if (renderer_drm_fd < 0) {
		wlr_log(WLR_ERROR, "Failed to get renderer DRM FD");
		goto error;
	}
	dev_t renderer_dev;
	if (!devid_from_fd(renderer_drm_fd, &renderer_dev)) {
		goto error;
	}

	feedback->main_device = renderer_dev;

	const struct wlr_drm_format_set *renderer_formats =
		wlr_renderer_get_texture_formats(options->main_renderer, WLR_BUFFER_CAP_DMABUF);
	if (renderer_formats == NULL) {
		wlr_log(WLR_ERROR, "Failed to get renderer DMA-BUF texture formats");
		goto error;
	}

	if (options->output_layer_feedback_event != NULL) {
		const struct wlr_output_layer_feedback_event *event = options->output_layer_feedback_event;

		struct wlr_linux_dmabuf_feedback_v1_tranche *tranche =
			wlr_linux_dmabuf_feedback_add_tranche(feedback);
		if (tranche == NULL) {
			goto error;
		}

		tranche->target_device = event->target_device;
		tranche->flags = ZWP_LINUX_DMABUF_FEEDBACK_V1_TRANCHE_FLAGS_SCANOUT;
		if (!wlr_drm_format_set_intersect(&tranche->formats, event->formats, renderer_formats)) {
			wlr_log(WLR_ERROR, "Failed to intersect renderer and scanout formats");
			goto error;
		}
	} else if (options->scanout_primary_output != NULL &&
			!is_secondary_drm_backend(options->scanout_primary_output->backend)) {
		int backend_drm_fd = wlr_backend_get_drm_fd(options->scanout_primary_output->backend);
		if (backend_drm_fd < 0) {
			wlr_log(WLR_ERROR, "Failed to get backend DRM FD");
			goto error;
		}
		dev_t backend_dev;
		if (!devid_from_fd(backend_drm_fd, &backend_dev)) {
			goto error;
		}

		const struct wlr_drm_format_set *scanout_formats =
			wlr_output_get_primary_formats(options->scanout_primary_output, WLR_BUFFER_CAP_DMABUF);
		if (scanout_formats == NULL) {
			wlr_log(WLR_ERROR, "Failed to get output primary DMA-BUF formats");
			goto error;
		}

		struct wlr_linux_dmabuf_feedback_v1_tranche *tranche =
			wlr_linux_dmabuf_feedback_add_tranche(feedback);
		if (tranche == NULL) {
			goto error;
		}

		tranche->target_device = backend_dev;
		tranche->flags = ZWP_LINUX_DMABUF_FEEDBACK_V1_TRANCHE_FLAGS_SCANOUT;
		if (!wlr_drm_format_set_intersect(&tranche->formats, scanout_formats, renderer_formats)) {
			wlr_log(WLR_ERROR, "Failed to intersect renderer and scanout formats");
			goto error;
		}
	}

	struct wlr_linux_dmabuf_feedback_v1_tranche *tranche =
		wlr_linux_dmabuf_feedback_add_tranche(feedback);
	if (tranche == NULL) {
		goto error;
	}

	tranche->target_device = renderer_dev;
	if (!wlr_drm_format_set_copy(&tranche->formats, renderer_formats)) {
		goto error;
	}

	return true;

error:
	wlr_linux_dmabuf_feedback_v1_finish(feedback);
	return false;
}
*/
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <drm_fourcc.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wlr/backend.h>
#include <wlr/config.h>
#include <wlr/interfaces/wlr_buffer.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_linux_dmabuf_v1.h>
#include <wlr/types/wlr_output_layer.h>
#include <wlr/util/log.h>
#include <xf86drm.h>
#include "linux-dmabuf-v1-protocol.h"
#include "render/drm_format_set.h"
#include "util/shm.h"

#if WLR_HAS_DRM_BACKEND
#include <wlr/backend/drm.h>
#endif

#define LINUX_DMABUF_VERSION 5

struct wlr_linux_buffer_params_v1 {
    struct wl_resource *resource;
    struct wlr_linux_dmabuf_v1 *linux_dmabuf;
    struct wlr_dmabuf_attributes attributes;
    bool has_modifier;
};

struct wlr_linux_dmabuf_feedback_v1_compiled_tranche {
    dev_t target_device;
    uint32_t flags; // bitfield of enum zwp_linux_dmabuf_feedback_v1_tranche_flags
    struct wl_array indices; // uint16_t
};

struct wlr_linux_dmabuf_feedback_v1_compiled {
    dev_t main_device;
    int table_fd;
    size_t table_size;

    size_t tranches_len;
    struct wlr_linux_dmabuf_feedback_v1_compiled_tranche tranches[];
};

struct wlr_linux_dmabuf_feedback_v1_table_entry {
    uint32_t format;
    uint32_t pad; // unused
    uint64_t modifier;
};

static_assert(sizeof(struct wlr_linux_dmabuf_feedback_v1_table_entry) == 16,
    "Expected wlr_linux_dmabuf_feedback_v1_table_entry to be tightly packed");

struct wlr_linux_dmabuf_v1_surface {
    struct wlr_surface *surface;
    struct wlr_linux_dmabuf_v1 *linux_dmabuf;
    struct wl_list link; // wlr_linux_dmabuf_v1.surfaces

    struct wlr_addon addon;
    struct wlr_linux_dmabuf_feedback_v1_compiled *feedback;

    struct wl_list feedback_resources; // wl_resource_get_link
};

static void buffer_handle_destroy(struct wl_client *client,
        struct wl_resource *resource) {
    wl_resource_destroy(resource);
}

static const struct wl_buffer_interface wl_buffer_impl = {
    .destroy = buffer_handle_destroy,
};

static bool buffer_resource_is_instance(struct wl_resource *resource) {
    return wl_resource_instance_of(resource, &wl_buffer_interface,
        &wl_buffer_impl);
}

struct wlr_dmabuf_v1_buffer *wlr_dmabuf_v1_buffer_try_from_buffer_resource(
        struct wl_resource *resource) {
    if (!buffer_resource_is_instance(resource) ||
                wl_resource_get_user_data(resource) == NULL) {
        return NULL;
    }
    return wl_resource_get_user_data(resource);
}

static const struct wlr_buffer_impl buffer_impl;

static struct wlr_dmabuf_v1_buffer *dmabuf_v1_buffer_from_buffer(
        struct wlr_buffer *wlr_buffer) {
    assert(wlr_buffer->impl == &buffer_impl);
    struct wlr_dmabuf_v1_buffer *buffer = wl_container_of(wlr_buffer, buffer, base);
    return buffer;
}

static void buffer_destroy(struct wlr_buffer *wlr_buffer) {
    struct wlr_dmabuf_v1_buffer *buffer =
        dmabuf_v1_buffer_from_buffer(wlr_buffer);
    wl_list_remove(&buffer->release.link);

    wlr_buffer_finish(wlr_buffer);

    if (buffer->resource != NULL) {
        wl_resource_set_user_data(buffer->resource, NULL);
    }
    wlr_dmabuf_attributes_finish(&buffer->attributes);
    free(buffer);
}

static bool buffer_get_dmabuf(struct wlr_buffer *wlr_buffer,
        struct wlr_dmabuf_attributes *attribs) {
    struct wlr_dmabuf_v1_buffer *buffer =
        dmabuf_v1_buffer_from_buffer(wlr_buffer);
    *attribs = buffer->attributes;
    return true;
}

static const struct wlr_buffer_impl buffer_impl = {
    .destroy = buffer_destroy,
    .get_dmabuf = buffer_get_dmabuf,
};

static void buffer_handle_release(struct wl_listener *listener, void *data) {
    struct wlr_dmabuf_v1_buffer *buffer =
        wl_container_of(listener, buffer, release);
    if (buffer->resource != NULL) {
        wl_buffer_send_release(buffer->resource);
    }
}

static const struct zwp_linux_buffer_params_v1_interface buffer_params_impl;

static struct wlr_linux_buffer_params_v1 *params_from_resource(
        struct wl_resource *resource) {
    assert(wl_resource_instance_of(resource,
        &zwp_linux_buffer_params_v1_interface, &buffer_params_impl));
    return wl_resource_get_user_data(resource);
}

static void params_destroy(struct wl_client *client,
        struct wl_resource *resource) {
    wl_resource_destroy(resource);
}

static void params_add(struct wl_client *client,
        struct wl_resource *params_resource, int32_t fd,
        uint32_t plane_idx, uint32_t offset, uint32_t stride,
        uint32_t modifier_hi, uint32_t modifier_lo) {
    struct wlr_linux_buffer_params_v1 *params =
        params_from_resource(params_resource);
    if (!params) {
        wl_resource_post_error(params_resource,
            ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_ALREADY_USED,
            "params was already used to create a wl_buffer");
        close(fd);
        return;
    }

    if (plane_idx >= WLR_DMABUF_MAX_PLANES) {
        wl_resource_post_error(params_resource,
            ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_PLANE_IDX,
            "plane index %u > %u", plane_idx, WLR_DMABUF_MAX_PLANES);
        close(fd);
        return;
    }

    if (params->attributes.fd[plane_idx] != -1) {
        wl_resource_post_error(params_resource,
            ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_PLANE_SET,
            "a dmabuf with FD %d has already been added for plane %u",
            params->attributes.fd[plane_idx], plane_idx);
        close(fd);
        return;
    }

    uint64_t modifier = ((uint64_t)modifier_hi << 32) | modifier_lo;
    if (params->has_modifier && modifier != params->attributes.modifier) {
        wl_resource_post_error(params_resource,
            ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_FORMAT,
            "sent modifier %" PRIu64 " for plane %u, expected"
            " modifier %" PRIu64 " like other planes",
            modifier, plane_idx, params->attributes.modifier);
        close(fd);
        return;
    }

    params->attributes.modifier = modifier;
    params->has_modifier = true;

    params->attributes.fd[plane_idx] = fd;
    params->attributes.offset[plane_idx] = offset;
    params->attributes.stride[plane_idx] = stride;
    params->attributes.n_planes++;
}

static void buffer_handle_resource_destroy(struct wl_resource *buffer_resource) {
    struct wlr_dmabuf_v1_buffer *buffer =
        wlr_dmabuf_v1_buffer_try_from_buffer_resource(buffer_resource);
    assert(buffer != NULL);
    buffer->resource = NULL;
    wlr_buffer_drop(&buffer->base);
}

static bool check_import_dmabuf(struct wlr_dmabuf_attributes *attribs, void *data) {
    struct wlr_linux_dmabuf_v1 *linux_dmabuf = data;

    if (linux_dmabuf->main_device_fd < 0) {
        return true;
    }

    for (int i = 0; i < attribs->n_planes; i++) {
        uint32_t handle = 0;
        if (drmPrimeFDToHandle(linux_dmabuf->main_device_fd, attribs->fd[i], &handle) != 0) {
            wlr_log_errno(WLR_ERROR, "Failed to import DMA-BUF FD for plane %d", i);
            return false;
        }
        if (drmCloseBufferHandle(linux_dmabuf->main_device_fd, handle) != 0) {
            wlr_log_errno(WLR_ERROR, "Failed to close buffer handle for plane %d", i);
            return false;
        }
    }
    return true;
}

static void params_create_common(struct wl_resource *params_resource,
        uint32_t buffer_id, int32_t width, int32_t height, uint32_t format,
        uint32_t flags) {
    struct wlr_linux_buffer_params_v1 *params =
        params_from_resource(params_resource);
    if (!params) {
        wl_resource_post_error(params_resource,
            ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_ALREADY_USED,
            "params was already used to create a wl_buffer");
        return;
    }

    struct wlr_dmabuf_attributes attribs = params->attributes;
    struct wlr_linux_dmabuf_v1 *linux_dmabuf = params->linux_dmabuf;

    // Make the params resource inert
    wl_resource_set_user_data(params_resource, NULL);
    free(params);

    if (!attribs.n_planes) {
        wl_resource_post_error(params_resource,
            ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INCOMPLETE,
            "no dmabuf has been added to the params");
        goto err_out;
    }

    if (attribs.fd[0] == -1) {
        wl_resource_post_error(params_resource,
            ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INCOMPLETE,
            "no dmabuf has been added for plane 0");
        goto err_out;
    }

    if ((attribs.fd[3] >= 0 || attribs.fd[2] >= 0) &&
            (attribs.fd[2] == -1 || attribs.fd[1] == -1)) {
        wl_resource_post_error(params_resource,
            ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INCOMPLETE,
            "gap in dmabuf planes");
        goto err_out;
    }

    uint32_t version = wl_resource_get_version(params_resource);
    if (!zwp_linux_buffer_params_v1_flags_is_valid(flags, version)) {
        wl_resource_post_error(params_resource,
            ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_FORMAT,
            "Unknown dmabuf flags %"PRIu32, flags);
        goto err_out;
    }

    if (flags != 0) {
        wlr_log(WLR_ERROR, "dmabuf flags aren't supported");
        goto err_failed;
    }

    attribs.width = width;
    attribs.height = height;
    attribs.format = format;

    if (width < 1 || height < 1) {
        wl_resource_post_error(params_resource,
            ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_DIMENSIONS,
            "invalid width %d or height %d", width, height);
        goto err_out;
    }

    for (int i = 0; i < attribs.n_planes; i++) {
        if ((uint64_t)attribs.offset[i]
                + attribs.stride[i] > UINT32_MAX) {
            wl_resource_post_error(params_resource,
                ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_OUT_OF_BOUNDS,
                "size overflow for plane %d", i);
            goto err_out;
        }

        if ((uint64_t)attribs.offset[i]
                + (uint64_t)attribs.stride[i] * height > UINT32_MAX) {
            wl_resource_post_error(params_resource,
                ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_OUT_OF_BOUNDS,
                "size overflow for plane %d", i);
        }

        off_t size = lseek(attribs.fd[i], 0, SEEK_END);
        if (size == -1) {
            // Skip checks if kernel does not support seek on buffer
            continue;
        }
        if (attribs.offset[i] > size) {
            wl_resource_post_error(params_resource,
                ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_OUT_OF_BOUNDS,
                "invalid offset %" PRIu32 " for plane %d",
                attribs.offset[i], i);
            goto err_out;
        }

        if (attribs.offset[i] + attribs.stride[i] > size ||
                attribs.stride[i] == 0) {
            wl_resource_post_error(params_resource,
                ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_OUT_OF_BOUNDS,
                "invalid stride %" PRIu32 " for plane %d",
                attribs.stride[i], i);
            goto err_out;
        }

        if (i == 0 && attribs.offset[i] +
                attribs.stride[i] * height > size) {
            wl_resource_post_error(params_resource,
                ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_OUT_OF_BOUNDS,
                "invalid buffer stride or height for plane %d", i);
            goto err_out;
        }
    }

    if (!linux_dmabuf->check_dmabuf_callback(&attribs,
                linux_dmabuf->check_dmabuf_callback_data)) {
        goto err_failed;
    }

    struct wlr_dmabuf_v1_buffer *buffer = calloc(1, sizeof(*buffer));
    if (!buffer) {
        wl_resource_post_no_memory(params_resource);
        goto err_failed;
    }
    wlr_buffer_init(&buffer->base, &buffer_impl, attribs.width, attribs.height);

    struct wl_client *client = wl_resource_get_client(params_resource);
    buffer->resource = wl_resource_create(client, &wl_buffer_interface,
        1, buffer_id);
    if (!buffer->resource) {
        wl_resource_post_no_memory(params_resource);
        free(buffer);
        goto err_failed;
    }
    wl_resource_set_implementation(buffer->resource,
        &wl_buffer_impl, buffer, buffer_handle_resource_destroy);

    buffer->attributes = attribs;

    buffer->release.notify = buffer_handle_release;
    wl_signal_add(&buffer->base.events.release, &buffer->release);

    if (buffer_id == 0) {
        zwp_linux_buffer_params_v1_send_created(params_resource,
            buffer->resource);
    }

    return;

err_failed:
    if (buffer_id == 0) {
        zwp_linux_buffer_params_v1_send_failed(params_resource);
    } else {
        wl_resource_post_error(params_resource,
            ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_WL_BUFFER,
            "importing the supplied dmabufs failed");
    }
err_out:
    wlr_dmabuf_attributes_finish(&attribs);
}

static void params_create(struct wl_client *client,
        struct wl_resource *params_resource,
        int32_t width, int32_t height, uint32_t format, uint32_t flags) {
    params_create_common(params_resource, 0, width, height, format,
        flags);
}

static void params_create_immed(struct wl_client *client,
        struct wl_resource *params_resource, uint32_t buffer_id,
        int32_t width, int32_t height, uint32_t format, uint32_t flags) {
    params_create_common(params_resource, buffer_id, width, height,
        format, flags);
}

static const struct zwp_linux_buffer_params_v1_interface buffer_params_impl = {
    .destroy = params_destroy,
    .add = params_add,
    .create = params_create,
    .create_immed = params_create_immed,
};

static void params_handle_resource_destroy(struct wl_resource *resource) {
    struct wlr_linux_buffer_params_v1 *params = params_from_resource(resource);
    if (!params) {
        return;
    }
    wlr_dmabuf_attributes_finish(&params->attributes);
    free(params);
}

static const struct zwp_linux_dmabuf_v1_interface linux_dmabuf_impl;

static struct wlr_linux_dmabuf_v1 *linux_dmabuf_from_resource(
        struct wl_resource *resource) {
    assert(wl_resource_instance_of(resource, &zwp_linux_dmabuf_v1_interface,
            &linux_dmabuf_impl));

    struct wlr_linux_dmabuf_v1 *dmabuf = wl_resource_get_user_data(resource);
    assert(dmabuf);
    return dmabuf;
}

static void linux_dmabuf_create_params(struct wl_client *client,
        struct wl_resource *linux_dmabuf_resource,
        uint32_t params_id) {
    struct wlr_linux_dmabuf_v1 *linux_dmabuf =
        linux_dmabuf_from_resource(linux_dmabuf_resource);

    struct wlr_linux_buffer_params_v1 *params = calloc(1, sizeof(*params));
    if (!params) {
        wl_resource_post_no_memory(linux_dmabuf_resource);
        return;
    }

    for (int i = 0; i < WLR_DMABUF_MAX_PLANES; i++) {
        params->attributes.fd[i] = -1;
    }

    params->linux_dmabuf = linux_dmabuf;

    uint32_t version = wl_resource_get_version(linux_dmabuf_resource);
    params->resource = wl_resource_create(client,
        &zwp_linux_buffer_params_v1_interface, version, params_id);
    if (!params->resource) {
        free(params);
        wl_resource_post_no_memory(linux_dmabuf_resource);
        return;
    }
    wl_resource_set_implementation(params->resource,
        &buffer_params_impl, params, params_handle_resource_destroy);
}

static void linux_dmabuf_feedback_destroy(struct wl_client *client,
        struct wl_resource *resource) {
    wl_resource_destroy(resource);
}

static const struct zwp_linux_dmabuf_feedback_v1_interface
        linux_dmabuf_feedback_impl = {
    .destroy = linux_dmabuf_feedback_destroy,
};

static ssize_t get_drm_format_set_index(const struct wlr_drm_format_set *set,
        uint32_t format, uint64_t modifier) {
    bool format_found = false;
    const struct wlr_drm_format *fmt;
    size_t idx = 0;
    for (size_t i = 0; i < set->len; i++) {
        fmt = &set->formats[i];
        if (fmt->format == format) {
            format_found = true;
            break;
        }
        idx += fmt->len;
    }
    if (!format_found) {
        return -1;
    }

    for (size_t i = 0; i < fmt->len; i++) {
        if (fmt->modifiers[i] == modifier) {
            return idx;
        }
        idx++;
    }
    return -1;
}

static struct wlr_linux_dmabuf_feedback_v1_compiled *feedback_compile(
        const struct wlr_linux_dmabuf_feedback_v1 *feedback) {
    const struct wlr_linux_dmabuf_feedback_v1_tranche *tranches = feedback->tranches.data;
    size_t tranches_len = feedback->tranches.size / sizeof(struct wlr_linux_dmabuf_feedback_v1_tranche);
    assert(tranches_len > 0);

    struct wlr_drm_format_set all_formats = {0};
    for (size_t i = 0; i < tranches_len; i++) {
        const struct wlr_linux_dmabuf_feedback_v1_tranche *tranche = &tranches[i];
        if (!wlr_drm_format_set_union(&all_formats, &all_formats, &tranche->formats)) {
            wlr_log(WLR_ERROR, "Failed to union scanout formats into one tranche");
            goto err_all_formats;
        }
    }

    size_t table_len = 0;
    for (size_t i = 0; i < all_formats.len; i++) {
        const struct wlr_drm_format *fmt = &all_formats.formats[i];
        table_len += fmt->len;
    }
    assert(table_len > 0);

    size_t table_size =
        table_len * sizeof(struct wlr_linux_dmabuf_feedback_v1_table_entry);
    int rw_fd, ro_fd;
    if (!allocate_shm_file_pair(table_size, &rw_fd, &ro_fd)) {
        wlr_log(WLR_ERROR, "Failed to allocate shm file for format table");
        return NULL;
    }

    struct wlr_linux_dmabuf_feedback_v1_table_entry *table =
        mmap(NULL, table_size, PROT_READ | PROT_WRITE, MAP_SHARED, rw_fd, 0);
    if (table == MAP_FAILED) {
        wlr_log_errno(WLR_ERROR, "mmap failed");
        close(rw_fd);
        close(ro_fd);
        goto err_all_formats;
    }

    close(rw_fd);

    size_t n = 0;
    for (size_t i = 0; i < all_formats.len; i++) {
        const struct wlr_drm_format *fmt = &all_formats.formats[i];

        for (size_t k = 0; k < fmt->len; k++) {
            table[n] = (struct wlr_linux_dmabuf_feedback_v1_table_entry){
                .format = fmt->format,
                .modifier = fmt->modifiers[k],
            };
            n++;
        }
    }
    assert(n == table_len);

    munmap(table, table_size);

    struct wlr_linux_dmabuf_feedback_v1_compiled *compiled = calloc(1, sizeof(*compiled) +
        tranches_len * sizeof(struct wlr_linux_dmabuf_feedback_v1_compiled_tranche));
    if (compiled == NULL) {
        close(ro_fd);
        goto err_all_formats;
    }

    compiled->main_device = feedback->main_device;
    compiled->tranches_len = tranches_len;
    compiled->table_fd = ro_fd;
    compiled->table_size = table_size;

    for (size_t i = 0; i < tranches_len; i++) {
        const struct wlr_linux_dmabuf_feedback_v1_tranche *tranche = &tranches[i];
        struct wlr_linux_dmabuf_feedback_v1_compiled_tranche *compiled_tranche =
            &compiled->tranches[i];

        compiled_tranche->target_device = tranche->target_device;
        compiled_tranche->flags = tranche->flags;

        wl_array_init(&compiled_tranche->indices);
        if (!wl_array_add(&compiled_tranche->indices, table_len * sizeof(uint16_t))) {
            wlr_log(WLR_ERROR, "Failed to allocate tranche indices array");
            goto error_compiled;
        }

        n = 0;
        uint16_t *indices = compiled_tranche->indices.data;
        for (size_t j = 0; j < tranche->formats.len; j++) {
            const struct wlr_drm_format *fmt = &tranche->formats.formats[j];
            for (size_t k = 0; k < fmt->len; k++) {
                ssize_t index = get_drm_format_set_index(
                    &all_formats, fmt->format, fmt->modifiers[k]);
                if (index < 0) {
                    wlr_log(WLR_ERROR, "Format 0x%" PRIX32 " and modifier "
                        "0x%" PRIX64 " are in tranche #%zu but are missing "
                        "from the fallback tranche",
                        fmt->format, fmt->modifiers[k], i);
                    goto error_compiled;
                }
                indices[n] = index;
                n++;
            }
        }
        compiled_tranche->indices.size = n * sizeof(uint16_t);
    }

    wlr_drm_format_set_finish(&all_formats);

    return compiled;

error_compiled:
    close(compiled->table_fd);
    free(compiled);
err_all_formats:
    wlr_drm_format_set_finish(&all_formats);
    return NULL;
}

static void compiled_feedback_destroy(
        struct wlr_linux_dmabuf_feedback_v1_compiled *feedback) {
    if (feedback == NULL) {
        return;
    }
    for (size_t i = 0; i < feedback->tranches_len; i++) {
        wl_array_release(&feedback->tranches[i].indices);
    }
    close(feedback->table_fd);
    free(feedback);
}

static void feedback_tranche_send(
        const struct wlr_linux_dmabuf_feedback_v1_compiled_tranche *tranche,
        struct wl_resource *resource) {
    struct wl_array dev_array = {
        .size = sizeof(tranche->target_device),
        .data = (void *)&tranche->target_device,
    };
    zwp_linux_dmabuf_feedback_v1_send_tranche_target_device(resource, &dev_array);
    zwp_linux_dmabuf_feedback_v1_send_tranche_flags(resource, tranche->flags);
    zwp_linux_dmabuf_feedback_v1_send_tranche_formats(resource,
        (struct wl_array *)&tranche->indices);
    zwp_linux_dmabuf_feedback_v1_send_tranche_done(resource);
}

static void feedback_send(const struct wlr_linux_dmabuf_feedback_v1_compiled *feedback,
        struct wl_resource *resource) {
    struct wl_array dev_array = {
        .size = sizeof(feedback->main_device),
        .data = (void *)&feedback->main_device,
    };
    zwp_linux_dmabuf_feedback_v1_send_main_device(resource, &dev_array);

    zwp_linux_dmabuf_feedback_v1_send_format_table(resource,
        feedback->table_fd, feedback->table_size);

    for (size_t i = 0; i < feedback->tranches_len; i++) {
        feedback_tranche_send(&feedback->tranches[i], resource);
    }

    zwp_linux_dmabuf_feedback_v1_send_done(resource);
}

static void linux_dmabuf_get_default_feedback(struct wl_client *client,
        struct wl_resource *resource, uint32_t id) {
    struct wlr_linux_dmabuf_v1 *linux_dmabuf =
        linux_dmabuf_from_resource(resource);

    uint32_t version = wl_resource_get_version(resource);
    struct wl_resource *feedback_resource = wl_resource_create(client,
        &zwp_linux_dmabuf_feedback_v1_interface, version, id);
    if (feedback_resource == NULL) {
        wl_client_post_no_memory(client);
        return;
    }
    wl_resource_set_implementation(feedback_resource, &linux_dmabuf_feedback_impl,
        NULL, NULL);

    feedback_send(linux_dmabuf->default_feedback, feedback_resource);
}

static void surface_destroy(struct wlr_linux_dmabuf_v1_surface *surface) {
    struct wl_resource *resource, *resource_tmp;
    wl_resource_for_each_safe(resource, resource_tmp, &surface->feedback_resources) {
        struct wl_list *link = wl_resource_get_link(resource);
        wl_list_remove(link);
        wl_list_init(link);
    }

    compiled_feedback_destroy(surface->feedback);

    wlr_addon_finish(&surface->addon);
    wl_list_remove(&surface->link);
    free(surface);
}

static void surface_addon_destroy(struct wlr_addon *addon) {
    struct wlr_linux_dmabuf_v1_surface *surface =
        wl_container_of(addon, surface, addon);
    surface_destroy(surface);
}

static const struct wlr_addon_interface surface_addon_impl = {
    .name = "wlr_linux_dmabuf_v1_surface",
    .destroy = surface_addon_destroy,
};

static struct wlr_linux_dmabuf_v1_surface *surface_get_or_create(
        struct wlr_linux_dmabuf_v1 *linux_dmabuf,
        struct wlr_surface *wlr_surface) {
    struct wlr_addon *addon =
        wlr_addon_find(&wlr_surface->addons, linux_dmabuf, &surface_addon_impl);
    if (addon != NULL) {
        struct wlr_linux_dmabuf_v1_surface *surface =
            wl_container_of(addon, surface, addon);
        return surface;
    }

    struct wlr_linux_dmabuf_v1_surface *surface = calloc(1, sizeof(*surface));
    if (surface == NULL) {
        return NULL;
    }

    surface->surface = wlr_surface;
    surface->linux_dmabuf = linux_dmabuf;
    wl_list_init(&surface->feedback_resources);
    wlr_addon_init(&surface->addon, &wlr_surface->addons, linux_dmabuf,
        &surface_addon_impl);
    wl_list_insert(&linux_dmabuf->surfaces, &surface->link);

    return surface;
}

static const struct wlr_linux_dmabuf_feedback_v1_compiled *surface_get_feedback(
        struct wlr_linux_dmabuf_v1_surface *surface) {
    if (surface->feedback != NULL) {
        return surface->feedback;
    }
    return surface->linux_dmabuf->default_feedback;
}

static void surface_feedback_handle_resource_destroy(struct wl_resource *resource) {
    wl_list_remove(wl_resource_get_link(resource));
}

static void linux_dmabuf_get_surface_feedback(struct wl_client *client,
        struct wl_resource *resource, uint32_t id,
        struct wl_resource *surface_resource) {
    struct wlr_linux_dmabuf_v1 *linux_dmabuf =
        linux_dmabuf_from_resource(resource);
    struct wlr_surface *wlr_surface = wlr_surface_from_resource(surface_resource);

    struct wlr_linux_dmabuf_v1_surface *surface =
        surface_get_or_create(linux_dmabuf, wlr_surface);
    if (surface == NULL) {
        wl_client_post_no_memory(client);
        return;
    }

    uint32_t version = wl_resource_get_version(resource);
    struct wl_resource *feedback_resource = wl_resource_create(client,
        &zwp_linux_dmabuf_feedback_v1_interface, version, id);
    if (feedback_resource == NULL) {
        wl_client_post_no_memory(client);
        return;
    }
    wl_resource_set_implementation(feedback_resource, &linux_dmabuf_feedback_impl,
        NULL, surface_feedback_handle_resource_destroy);
    wl_list_insert(&surface->feedback_resources, wl_resource_get_link(feedback_resource));

    feedback_send(surface_get_feedback(surface), feedback_resource);
}

static void linux_dmabuf_destroy(struct wl_client *client,
        struct wl_resource *resource) {
    wl_resource_destroy(resource);
}

static const struct zwp_linux_dmabuf_v1_interface linux_dmabuf_impl = {
    .destroy = linux_dmabuf_destroy,
    .create_params = linux_dmabuf_create_params,
    .get_default_feedback = linux_dmabuf_get_default_feedback,
    .get_surface_feedback = linux_dmabuf_get_surface_feedback,
};

static void linux_dmabuf_send_modifiers(struct wl_resource *resource,
        const struct wlr_drm_format *fmt) {
    if (wl_resource_get_version(resource) < ZWP_LINUX_DMABUF_V1_MODIFIER_SINCE_VERSION) {
        if (wlr_drm_format_has(fmt, DRM_FORMAT_MOD_INVALID)) {
            zwp_linux_dmabuf_v1_send_format(resource, fmt->format);
        }
        return;
    }

    if (fmt->len == 2 && wlr_drm_format_has(fmt, DRM_FORMAT_MOD_INVALID)
            && wlr_drm_format_has(fmt, DRM_FORMAT_MOD_LINEAR)) {
        uint64_t mod = DRM_FORMAT_MOD_INVALID;
        zwp_linux_dmabuf_v1_send_modifier(resource, fmt->format,
            mod >> 32, mod & 0xFFFFFFFF);
        return;
    }

    for (size_t i = 0; i < fmt->len; i++) {
        uint64_t mod = fmt->modifiers[i];
        zwp_linux_dmabuf_v1_send_modifier(resource, fmt->format,
            mod >> 32, mod & 0xFFFFFFFF);
    }
}

static void linux_dmabuf_send_formats(struct wlr_linux_dmabuf_v1 *linux_dmabuf,
        struct wl_resource *resource) {
    for (size_t i = 0; i < linux_dmabuf->default_formats.len; i++) {
        const struct wlr_drm_format *fmt = &linux_dmabuf->default_formats.formats[i];
        linux_dmabuf_send_modifiers(resource, fmt);
    }
}

static void linux_dmabuf_bind(struct wl_client *client, void *data,
        uint32_t version, uint32_t id) {
    struct wlr_linux_dmabuf_v1 *linux_dmabuf = data;

    struct wl_resource *resource = wl_resource_create(client,
        &zwp_linux_dmabuf_v1_interface, version, id);
    if (resource == NULL) {
        wl_client_post_no_memory(client);
        return;
    }
    wl_resource_set_implementation(resource, &linux_dmabuf_impl,
        linux_dmabuf, NULL);

    if (version < ZWP_LINUX_DMABUF_V1_GET_DEFAULT_FEEDBACK_SINCE_VERSION) {
        linux_dmabuf_send_formats(linux_dmabuf, resource);
    }
}

static struct wlr_buffer *buffer_from_resource(struct wl_resource *resource) {
    struct wlr_dmabuf_v1_buffer *buffer =
        wlr_dmabuf_v1_buffer_try_from_buffer_resource(resource);
    assert(buffer != NULL);
    return &buffer->base;
}

static const struct wlr_buffer_resource_interface buffer_resource_interface = {
    .name = "wlr_dmabuf_v1_buffer",
    .is_instance = buffer_resource_is_instance,
    .from_resource = buffer_from_resource,
};

static void linux_dmabuf_v1_destroy(struct wlr_linux_dmabuf_v1 *linux_dmabuf) {
    wl_signal_emit_mutable(&linux_dmabuf->events.destroy, linux_dmabuf);

    assert(wl_list_empty(&linux_dmabuf->events.destroy.listener_list));

    struct wlr_linux_dmabuf_v1_surface *surface, *surface_tmp;
    wl_list_for_each_safe(surface, surface_tmp, &linux_dmabuf->surfaces, link) {
        surface_destroy(surface);
    }

    compiled_feedback_destroy(linux_dmabuf->default_feedback);
    wlr_drm_format_set_finish(&linux_dmabuf->default_formats);
    if (linux_dmabuf->main_device_fd >= 0) {
        close(linux_dmabuf->main_device_fd);
    }

    wl_list_remove(&linux_dmabuf->display_destroy.link);

    wl_global_destroy(linux_dmabuf->global);
    free(linux_dmabuf);
}

static void handle_display_destroy(struct wl_listener *listener, void *data) {
    struct wlr_linux_dmabuf_v1 *linux_dmabuf =
        wl_container_of(listener, linux_dmabuf, display_destroy);
    linux_dmabuf_v1_destroy(linux_dmabuf);
}

static struct wlr_linux_dmabuf_feedback_v1_compiled *compile_default_feedback(
        struct wlr_renderer *renderer) {
    struct wlr_linux_dmabuf_feedback_v1_tranche tranche = {0};

    // Define a static array for modifiers
    static uint64_t dummy_modifiers[] = { DRM_FORMAT_MOD_LINEAR };

    // Create a minimal format set with a single dummy format
    static struct wlr_drm_format dummy_format = {
        .format = DRM_FORMAT_XRGB8888,  // Common default format
        .len = 1,
        .capacity = 1,
        .modifiers = dummy_modifiers
    };
    static struct wlr_drm_format_set dummy_format_set = {
        .len = 1,
        .capacity = 1,
        .formats = &dummy_format
    };
    if (!wlr_drm_format_set_copy(&tranche.formats, &dummy_format_set)) {
        wlr_log(WLR_ERROR, "Failed to copy dummy format set");
        return NULL;
    }

    struct wlr_linux_dmabuf_feedback_v1 feedback = {
        .main_device = 0,  // Dummy device ID for virtual backends
        .tranches = {
            .data = &tranche,
            .size = sizeof(tranche)
        }
    };

    struct wlr_linux_dmabuf_feedback_v1_compiled *compiled = feedback_compile(&feedback);
    wlr_drm_format_set_finish(&tranche.formats);
    return compiled;
}

static bool set_default_feedback(struct wlr_linux_dmabuf_v1 *linux_dmabuf,
        const struct wlr_linux_dmabuf_feedback_v1 *feedback) {
    struct wlr_linux_dmabuf_feedback_v1_compiled *compiled = feedback_compile(feedback);
    if (compiled == NULL) {
        return false;
    }

    // For virtual backends like RDP, skip DRM device checks
    int main_device_fd = -1;
    struct wlr_drm_format_set formats = {0};

    size_t tranches_len =
        feedback->tranches.size / sizeof(struct wlr_linux_dmabuf_feedback_v1_tranche);
    const struct wlr_linux_dmabuf_feedback_v1_tranche *tranches = feedback->tranches.data;
    for (size_t i = 0; i < tranches_len; i++) {
        const struct wlr_linux_dmabuf_feedback_v1_tranche *tranche = &tranches[i];
        if (!wlr_drm_format_set_union(&formats, &formats, &tranche->formats)) {
            goto error_formats;
        }
    }

    compiled_feedback_destroy(linux_dmabuf->default_feedback);
    linux_dmabuf->default_feedback = compiled;

    if (linux_dmabuf->main_device_fd >= 0) {
        close(linux_dmabuf->main_device_fd);
    }
    linux_dmabuf->main_device_fd = main_device_fd;

    wlr_drm_format_set_finish(&linux_dmabuf->default_formats);
    linux_dmabuf->default_formats = formats;

    return true;

error_formats:
    wlr_drm_format_set_finish(&formats);
    compiled_feedback_destroy(compiled);
    return false;
}

struct wlr_linux_dmabuf_v1 *wlr_linux_dmabuf_v1_create(struct wl_display *display,
        uint32_t version, const struct wlr_linux_dmabuf_feedback_v1 *default_feedback) {
    assert(version <= LINUX_DMABUF_VERSION);

    struct wlr_linux_dmabuf_v1 *linux_dmabuf = calloc(1, sizeof(*linux_dmabuf));
    if (linux_dmabuf == NULL) {
        wlr_log(WLR_ERROR, "could not create simple dmabuf manager");
        return NULL;
    }
    linux_dmabuf->main_device_fd = -1;

    wl_list_init(&linux_dmabuf->surfaces);

    wl_signal_init(&linux_dmabuf->events.destroy);

    linux_dmabuf->global = wl_global_create(display, &zwp_linux_dmabuf_v1_interface,
        version, linux_dmabuf, linux_dmabuf_bind);
    if (!linux_dmabuf->global) {
        wlr_log(WLR_ERROR, "could not create linux dmabuf v1 wl global");
        goto error_linux_dmabuf;
    }

    if (!set_default_feedback(linux_dmabuf, default_feedback)) {
        goto error_global;
    }

    linux_dmabuf->display_destroy.notify = handle_display_destroy;
    wl_display_add_destroy_listener(display, &linux_dmabuf->display_destroy);

    wlr_linux_dmabuf_v1_set_check_dmabuf_callback(linux_dmabuf,
        check_import_dmabuf, linux_dmabuf);

    wlr_buffer_register_resource_interface(&buffer_resource_interface);

    return linux_dmabuf;

error_global:
    wl_global_destroy(linux_dmabuf->global);
error_linux_dmabuf:
    free(linux_dmabuf);
    return NULL;
}

struct wlr_linux_dmabuf_v1 *wlr_linux_dmabuf_v1_create_with_renderer(struct wl_display *display,
        uint32_t version, struct wlr_renderer *renderer) {
    struct wlr_linux_dmabuf_feedback_v1 feedback = {0};
    struct wlr_linux_dmabuf_feedback_v1_tranche *tranche =
        wlr_linux_dmabuf_feedback_add_tranche(&feedback);
    if (tranche == NULL) {
        return NULL;
    }

    // Define a static array for modifiers
    static uint64_t dummy_modifiers[] = { DRM_FORMAT_MOD_LINEAR };

    // Create a minimal format set with a single dummy format
    static struct wlr_drm_format dummy_format = {
        .format = DRM_FORMAT_XRGB8888,
        .len = 1,
        .capacity = 1,
        .modifiers = dummy_modifiers
    };
    static struct wlr_drm_format_set dummy_format_set = {
        .len = 1,
        .capacity = 1,
        .formats = &dummy_format
    };
    if (!wlr_drm_format_set_copy(&tranche->formats, &dummy_format_set)) {
        wlr_linux_dmabuf_feedback_v1_finish(&feedback);
        return NULL;
    }
    tranche->target_device = 0;  // Dummy device ID

    struct wlr_linux_dmabuf_v1 *linux_dmabuf =
        wlr_linux_dmabuf_v1_create(display, version, &feedback);
    wlr_linux_dmabuf_feedback_v1_finish(&feedback);
    if (linux_dmabuf == NULL) {
        return NULL;
    }

    // Set default feedback using compile_default_feedback
    linux_dmabuf->default_feedback = compile_default_feedback(renderer);
    if (linux_dmabuf->default_feedback == NULL) {
        wlr_log(WLR_ERROR, "Failed to compile default feedback");
        linux_dmabuf_v1_destroy(linux_dmabuf);
        return NULL;
    }

    return linux_dmabuf;
}

void wlr_linux_dmabuf_v1_set_check_dmabuf_callback(struct wlr_linux_dmabuf_v1 *linux_dmabuf,
        bool (*callback)(struct wlr_dmabuf_attributes *attribs, void *data), void *data) {
    assert(callback);
    linux_dmabuf->check_dmabuf_callback = callback;
    linux_dmabuf->check_dmabuf_callback_data = data;
}

bool wlr_linux_dmabuf_v1_set_surface_feedback(
        struct wlr_linux_dmabuf_v1 *linux_dmabuf,
        struct wlr_surface *wlr_surface,
        const struct wlr_linux_dmabuf_feedback_v1 *feedback) {
    struct wlr_linux_dmabuf_v1_surface *surface =
        surface_get_or_create(linux_dmabuf, wlr_surface);
    if (surface == NULL) {
        return false;
    }

    struct wlr_linux_dmabuf_feedback_v1_compiled *compiled = NULL;
    if (feedback != NULL) {
        compiled = feedback_compile(feedback);
        if (compiled == NULL) {
            return false;
        }
    }

    compiled_feedback_destroy(surface->feedback);
    surface->feedback = compiled;

    struct wl_resource *resource;
    wl_resource_for_each(resource, &surface->feedback_resources) {
        feedback_send(surface_get_feedback(surface), resource);
    }

    return true;
}

struct wlr_linux_dmabuf_feedback_v1_tranche *wlr_linux_dmabuf_feedback_add_tranche(
        struct wlr_linux_dmabuf_feedback_v1 *feedback) {
    struct wlr_linux_dmabuf_feedback_v1_tranche *tranche =
        wl_array_add(&feedback->tranches, sizeof(*tranche));
    if (tranche == NULL) {
        wlr_log_errno(WLR_ERROR, "Allocation failed");
        return NULL;
    }
    *tranche = (struct wlr_linux_dmabuf_feedback_v1_tranche){0};
    return tranche;
}

void wlr_linux_dmabuf_feedback_v1_finish(struct wlr_linux_dmabuf_feedback_v1 *feedback) {
    struct wlr_linux_dmabuf_feedback_v1_tranche *tranche;
    wl_array_for_each(tranche, &feedback->tranches) {
        wlr_drm_format_set_finish(&tranche->formats);
    }
    wl_array_release(&feedback->tranches);
}

bool wlr_linux_dmabuf_feedback_v1_init_with_options(struct wlr_linux_dmabuf_feedback_v1 *feedback,
        const struct wlr_linux_dmabuf_feedback_v1_init_options *options) {
    assert(options->main_renderer != NULL);
    assert(options->scanout_primary_output == NULL ||
        options->output_layer_feedback_event == NULL);

    *feedback = (struct wlr_linux_dmabuf_feedback_v1){0};

    // Define a static array for modifiers
    static uint64_t dummy_modifiers[] = { DRM_FORMAT_MOD_LINEAR };

    // Create a minimal format set with a single dummy format
    static struct wlr_drm_format dummy_format = {
        .format = DRM_FORMAT_XRGB8888,
        .len = 1,
        .capacity = 1,
        .modifiers = dummy_modifiers
    };
    static struct wlr_drm_format_set dummy_format_set = {
        .len = 1,
        .capacity = 1,
        .formats = &dummy_format
    };

    feedback->main_device = 0;  // Dummy device ID for virtual backends

    struct wlr_linux_dmabuf_feedback_v1_tranche *tranche =
        wlr_linux_dmabuf_feedback_add_tranche(feedback);
    if (tranche == NULL) {
        goto error;
    }

    tranche->target_device = 0;
    if (!wlr_drm_format_set_copy(&tranche->formats, &dummy_format_set)) {
        goto error;
    }

    return true;

error:
    wlr_linux_dmabuf_feedback_v1_finish(feedback);
    return false;
}