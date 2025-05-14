project(
	'wlroots',
	'c',
	version: '0.19.0-rc3',
	license: 'MIT',
	meson_version: '>=1.3',
	default_options: [
		'c_std=' + (meson.version().version_compare('>=1.4.0') ? 'c23,c11' : 'c11'),
		'warning_level=2',
		'werror=true',
		'wrap_mode=nodownload',
	],
)

version = meson.project_version().split('-')[0]
version_major = version.split('.')[0]
version_minor = version.split('.')[1]
versioned_name = '@0@-@1@.@2@'.format(meson.project_name(), version_major, version_minor)

little_endian = target_machine.endian() == 'little'
big_endian = target_machine.endian() == 'big'

add_project_arguments([
	'-D_POSIX_C_SOURCE=200809L',
	'-DWLR_USE_UNSTABLE',
	'-DWLR_PRIVATE=',
	'-DWLR_LITTLE_ENDIAN=@0@'.format(little_endian.to_int()),
	'-DWLR_BIG_ENDIAN=@0@'.format(big_endian.to_int()),
], language: 'c')

cc = meson.get_compiler('c')

add_project_arguments(cc.get_supported_arguments([
	'-Wundef',
	'-Wlogical-op',
	'-Wmissing-include-dirs',
	'-Wold-style-definition',
	'-Wpointer-arith',
	'-Winit-self',
	'-Wstrict-prototypes',
	'-Wimplicit-fallthrough=2',
	'-Wendif-labels',
	'-Wstrict-aliasing=2',
	'-Woverflow',
	'-Wmissing-prototypes',
	'-Walloca',

	'-Wno-missing-braces',
	'-Wno-missing-field-initializers',
	'-Wno-unused-parameter',
]), language: 'c')

fs = import('fs')

# Strip relative path prefixes from the code if possible, otherwise hide them.
relative_dir = fs.relative_to(meson.current_source_dir(), meson.global_build_root()) + '/'
if cc.has_argument('-fmacro-prefix-map=/prefix/to/hide=')
	add_project_arguments(
		'-fmacro-prefix-map=@0@='.format(relative_dir),
		language: 'c',
	)
else
	add_project_arguments(
		'-D_WLR_REL_SRC_DIR="@0@"'.format(relative_dir),
		language: 'c',
	)
endif

# Update features based on options
backends = get_option('backends')
features = {
	'drm-backend': backends.contains('drm') or backends.contains('auto'),
	'RDP-backend': backends.contains('RDP'),
	'libinput-backend': backends.contains('libinput') or backends.contains('auto'),
	'xwayland': get_option('xwayland').enabled(),
	'gles2-renderer': get_option('renderers').contains('gles2') or get_option('renderers').contains('auto'),
	'vulkan-renderer': get_option('renderers').contains('vulkan'),
	'gbm-allocator': get_option('allocators').contains('gbm') or get_option('allocators').contains('auto'),
	'udmabuf-allocator': get_option('allocators').contains('udmabuf'),
	'session': get_option('session').enabled(),
	'color-management': get_option('color-management').enabled(),
}
internal_features = {
	'xcb-errors': get_option('xcb-errors').enabled(),
	'egl': false,
	'libliftoff': get_option('libliftoff').enabled(),
}
internal_config = configuration_data()
internal_config.set('WLR_HAS_RDP_BACKEND', features['RDP-backend'].to_int())

wayland_kwargs = {
	'version': '>=1.23.1',
	'fallback': 'wayland',
	'default_options': [
		'tests=false',
		'documentation=false',
	],
}
wayland_server = dependency('wayland-server',
	kwargs: wayland_kwargs,
)

drm = dependency('libdrm',
	version: '>=2.4.122',
	fallback: 'libdrm',
	default_options: [
		'auto_features=disabled',
		'tests=false',
	],
)
xkbcommon = dependency(
	'xkbcommon',
	fallback: 'libxkbcommon',
	default_options: [
		'enable-tools=false',
		'enable-x11=false',
		'enable-docs=false',
		'enable-xkbregistry=false',
	],
)
pixman = dependency('pixman-1',
	version: '>=0.43.0',
	fallback: 'pixman',
	default_options: ['werror=false'],
)
math = cc.find_library('m')
rt = cc.find_library('rt')

# Add FreeRDP and WinPR dependencies for RDP-backend
freerdp2 = dependency('freerdp2', required: features['RDP-backend'])
winpr2 = dependency('winpr2', required: features['RDP-backend'])

wlr_files = []
wlr_deps = [
	wayland_server,
	drm,
	xkbcommon,
	pixman,
	math,
	rt,
]

# Include RDP backend source if enabled
if features['RDP-backend']
	wlr_deps += [freerdp2, winpr2]
	wlr_files += ['backend/RDP/backend.c']
endif

subdir('protocol')
subdir('render')

subdir('backend')
subdir('types')
subdir('util')
subdir('xcursor')
subdir('xwayland')

subdir('include')

# Generate config.h
config_h = configure_file(
	output: 'config.h',
	configuration: internal_config,
	install: true,
	install_dir: '@0@-@1@.@2@/wlr'.format(meson.project_name(), version_major, version_minor)
)

wlr_inc = include_directories('include')

symbols_file = 'wlroots.syms'
symbols_flag = '-Wl,--version-script,@0@/@1@'.format(meson.current_source_dir(), symbols_file)
lib_wlr = library(
	versioned_name, wlr_files,
	dependencies: wlr_deps,
	include_directories: [wlr_inc],
	install: true,
	link_args: symbols_flag,
	link_depends: symbols_file,
)

wlr_vars = {}
foreach name, have : features
	wlr_vars += { 'have_' + name.underscorify(): have.to_string() }
endforeach

wlroots = declare_dependency(
	link_with: lib_wlr,
	dependencies: wlr_deps,
	include_directories: wlr_inc,
	variables: wlr_vars,
)

meson.override_dependency(versioned_name, wlroots)

summary(features + internal_features, bool_yn: true)

if get_option('examples')
	subdir('examples')
	subdir('tinywl')
endif

pkgconfig = import('pkgconfig')
pkgconfig.generate(
	lib_wlr,
	name: 'wlroots',
	description: 'Wayland compositor library',
	subdirs: versioned_name,
	url: 'https://gitlab.freedesktop.org/wlroots/wlroots',
	variables: wlr_vars,
)