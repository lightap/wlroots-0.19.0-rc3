#include <drm_fourcc.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <wlr/util/log.h>
#include "render/gles2.h"
#include "render/pixel_format.h"

/*
 * The DRM formats are little endian while the GL formats are big endian,
 * so DRM_FORMAT_ARGB8888 is actually compatible with GL_BGRA_EXT.
 */
static const struct wlr_gles2_pixel_format formats[] = {
	{
		.drm_format = DRM_FORMAT_ARGB8888,
		.gl_format = GL_BGRA_EXT,
		.gl_type = GL_UNSIGNED_BYTE,
		.gl_internalformat = GL_BGRA_EXT,
	},
	{
		.drm_format = DRM_FORMAT_XRGB8888,
		.gl_format = GL_BGRA_EXT,
		.gl_type = GL_UNSIGNED_BYTE,
		.gl_internalformat = GL_BGRA_EXT,
	},
	{
		.drm_format = DRM_FORMAT_XBGR8888,
		.gl_format = GL_RGBA,
		.gl_type = GL_UNSIGNED_BYTE,
		.gl_internalformat = GL_RGBA,
	},
	{
		.drm_format = DRM_FORMAT_ABGR8888,
		.gl_format = GL_RGBA,
		.gl_type = GL_UNSIGNED_BYTE,
		.gl_internalformat = GL_RGBA,
	},
	{
		.drm_format = DRM_FORMAT_BGR888,
		.gl_format = GL_RGB,
		.gl_type = GL_UNSIGNED_BYTE,
		.gl_internalformat = GL_RGB,
	},
#if WLR_LITTLE_ENDIAN
	{
		.drm_format = DRM_FORMAT_RGBX4444,
		.gl_format = GL_RGBA,
		.gl_type = GL_UNSIGNED_SHORT_4_4_4_4,
		.gl_internalformat = GL_RGBA,
	},
	{
		.drm_format = DRM_FORMAT_RGBA4444,
		.gl_format = GL_RGBA,
		.gl_type = GL_UNSIGNED_SHORT_4_4_4_4,
		.gl_internalformat = GL_RGBA,
	},
	{
		.drm_format = DRM_FORMAT_RGBX5551,
		.gl_format = GL_RGBA,
		.gl_type = GL_UNSIGNED_SHORT_5_5_5_1,
		.gl_internalformat = GL_RGBA,
	},
	{
		.drm_format = DRM_FORMAT_RGBA5551,
		.gl_format = GL_RGBA,
		.gl_type = GL_UNSIGNED_SHORT_5_5_5_1,
		.gl_internalformat = GL_RGBA,
	},
	{
		.drm_format = DRM_FORMAT_RGB565,
		.gl_format = GL_RGB,
		.gl_type = GL_UNSIGNED_SHORT_5_6_5,
		.gl_internalformat = GL_RGB,
	},
	{
		.drm_format = DRM_FORMAT_XBGR2101010,
		.gl_format = GL_RGBA,
		.gl_type = GL_UNSIGNED_INT_2_10_10_10_REV_EXT,
		.gl_internalformat = GL_RGBA,
	},
	{
		.drm_format = DRM_FORMAT_ABGR2101010,
		.gl_format = GL_RGBA,
		.gl_type = GL_UNSIGNED_INT_2_10_10_10_REV_EXT,
		.gl_internalformat = GL_RGBA,
	},
	{
		.drm_format = DRM_FORMAT_XBGR16161616F,
		.gl_format = GL_RGBA,
		.gl_type = GL_HALF_FLOAT_OES,
		.gl_internalformat = GL_RGBA,
	},
	{
		.drm_format = DRM_FORMAT_ABGR16161616F,
		.gl_format = GL_RGBA,
		.gl_type = GL_HALF_FLOAT_OES,
		.gl_internalformat = GL_RGBA,
	},
	{
		.drm_format = DRM_FORMAT_XBGR16161616,
		.gl_format = GL_RGBA,
		.gl_type = GL_UNSIGNED_SHORT,
		.gl_internalformat = GL_RGBA16_EXT,
	},
	{
		.drm_format = DRM_FORMAT_ABGR16161616,
		.gl_format = GL_RGBA,
		.gl_type = GL_UNSIGNED_SHORT,
		.gl_internalformat = GL_RGBA16_EXT,
	},
#endif
};

/*
 * Return true if supported for texturing, even if other operations like
 * reading aren't supported.
 */
bool is_gles2_pixel_format_supported(const struct wlr_gles2_renderer *renderer,
		const struct wlr_gles2_pixel_format *format) {
	if (format->gl_type == GL_UNSIGNED_INT_2_10_10_10_REV_EXT
			&& !renderer->exts.EXT_texture_type_2_10_10_10_REV) {
		wlr_log(WLR_DEBUG, "Format 0x%"PRIX32" unsupported: missing EXT_texture_type_2_10_10_10_REV", format->drm_format);
		return false;
	}
	if (format->gl_type == GL_HALF_FLOAT_OES
			&& !renderer->exts.OES_texture_half_float_linear) {
		wlr_log(WLR_DEBUG, "Format 0x%"PRIX32" unsupported: missing OES_texture_half_float_linear", format->drm_format);
		return false;
	}
	if (format->gl_type == GL_UNSIGNED_SHORT
			&& !renderer->exts.EXT_texture_norm16) {
		wlr_log(WLR_DEBUG, "Format 0x%"PRIX32" unsupported: missing EXT_texture_norm16", format->drm_format);
		return false;
	}
/* Check if GL_BGRA_EXT is supported through a different approach */

if (format->gl_format == GL_BGRA_EXT) {
    /* 
     * Instead of checking for a specific extension, test if the format is supported
     * by trying to determine if the GL implementation reports GL_BGRA_EXT as valid
     */
    GLint supported_formats[32];
    GLint num_formats = 0;
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &num_formats);
    if (num_formats > 32) num_formats = 32;
    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, supported_formats);
    
    bool bgra_supported = false;
    for (int i = 0; i < num_formats; i++) {
        if (supported_formats[i] == GL_BGRA_EXT) {
            bgra_supported = true;
            break;
        }
    }
    
    if (!bgra_supported) {
        wlr_log(WLR_DEBUG, "Format 0x%"PRIX32" unsupported: GL_BGRA_EXT not in supported formats", format->drm_format);
        return false;
    }
    
    wlr_log(WLR_DEBUG, "Format 0x%"PRIX32" supported (BGRA detected in formats)", format->drm_format);
    return true;
}
	wlr_log(WLR_DEBUG, "Format 0x%"PRIX32" supported", format->drm_format);
	return true;
}

const struct wlr_gles2_pixel_format *get_gles2_format_from_drm(uint32_t fmt) {
	for (size_t i = 0; i < sizeof(formats) / sizeof(*formats); ++i) {
		if (formats[i].drm_format == fmt) {
			return &formats[i];
		}
	}
	return NULL;
}

const struct wlr_gles2_pixel_format *get_gles2_format_from_gl(
		GLint gl_format, GLint gl_type, bool alpha) {
	for (size_t i = 0; i < sizeof(formats) / sizeof(*formats); ++i) {
		if (formats[i].gl_format != gl_format ||
				formats[i].gl_type != gl_type) {
			continue;
		}

		if (pixel_format_has_alpha(formats[i].drm_format) != alpha) {
			continue;
		}

		return &formats[i];
	}
	return NULL;
}

void get_gles2_shm_formats(const struct wlr_gles2_renderer *renderer,
		struct wlr_drm_format_set *out) {
	for (size_t i = 0; i < sizeof(formats) / sizeof(formats[0]); i++) {
		if (!is_gles2_pixel_format_supported(renderer, &formats[i])) {
			continue;
		}
		wlr_drm_format_set_add(out, formats[i].drm_format, DRM_FORMAT_MOD_INVALID);
		wlr_drm_format_set_add(out, formats[i].drm_format, DRM_FORMAT_MOD_LINEAR);
	}
}