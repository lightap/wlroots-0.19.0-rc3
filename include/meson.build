subdir('wlr')

exclude_files = ['meson.build', 'config.h.in', 'version.h.in']
if not features.get('drm-backend')
	exclude_files += 'backend/drm.h'
	exclude_files += 'types/wlr_drm_lease_v1.h'
endif
if not features.get('libinput-backend')
	exclude_files += 'backend/libinput.h'
endif
if not features.get('RDP-backend')
	exclude_files += 'backend/RDP.h'
endif
if not features.get('xwayland')
	exclude_files += 'xwayland.h'
endif
if not features.get('gles2-renderer')
	exclude_files += ['render/egl.h', 'render/gles2.h']
endif
if not features.get('vulkan-renderer')
	exclude_files += 'render/vulkan.h'
endif
if not features.get('session')
	exclude_files += 'backend/session.h'
endif
if not features.get('tinywl', false)  # Add conditional for tinywl feature
	exclude_files += 'tinywl/tinywl.h'
endif

install_subdir('wlr',
	install_dir: get_option('includedir') / versioned_name,
	exclude_files: exclude_files,
)

foreach name, have : internal_features
	internal_config.set10('HAVE_' + name.underscorify().to_upper(), have)
endforeach
wlr_files += configure_file(
	output: 'config.h',
	configuration: internal_config,
)
