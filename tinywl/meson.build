executable(
	'tinywl',
	['tinywl.c', protocols_server_header['xdg-shell'], '../backend/RDP/backend.c'],  # Adjusted path
	dependencies: [
		wlroots,
		dependency('libpng'),
		dependency('wayland-server'),
		dependency('xkbcommon'),
		dependency('pixman-1'),
		dependency('libdrm'),
		dependency('glesv2'),
		meson.get_compiler('c').find_library('freerdp2'),
		dependency('winpr2'),
	],
	include_directories: [
		include_directories('../include'),
		include_directories('../backend/RDP'),  # Adjusted for backend/RDP/backend.h
	]
)