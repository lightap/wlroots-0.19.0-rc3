/*
 * This an unstable interface of wlroots. No guarantees are made regarding the
 * future consistency of this API.
 */
#ifndef WLR_USE_UNSTABLE
#error "Add -DWLR_USE_UNSTABLE to enable unstable wlroots features"
#endif

#ifndef WLR_TYPES_WLR_XDG_FOREIGN_V1_H
#define WLR_TYPES_WLR_XDG_FOREIGN_V1_H

#include <wayland-server-core.h>
#include <wlr/types/wlr_xdg_foreign_registry.h>

struct wlr_xdg_foreign_v1 {
	struct {
		struct wl_global *global;
		struct wl_list objects; // wlr_xdg_exported_v1.link or wlr_xdg_imported_v1.link
	} exporter, importer;

	struct wlr_xdg_foreign_registry *registry;

	struct {
		struct wl_signal destroy;
	} events;

	void *data;

	struct {
		struct wl_listener foreign_registry_destroy;
		struct wl_listener display_destroy;
	} WLR_PRIVATE;
};

struct wlr_xdg_exported_v1 {
	struct wlr_xdg_foreign_exported base;

	struct wl_resource *resource;
	struct wl_list link; // wlr_xdg_foreign_v1.exporter.objects

	struct {
		struct wl_listener xdg_toplevel_destroy;
	} WLR_PRIVATE;
};

struct wlr_xdg_imported_v1 {
	struct wlr_xdg_foreign_exported *exported;

	struct wl_resource *resource;
	struct wl_list link; // wlr_xdg_foreign_v1.importer.objects
	struct wl_list children;

	struct {
		struct wl_listener exported_destroyed;
	} WLR_PRIVATE;
};

struct wlr_xdg_imported_child_v1 {
	struct wlr_xdg_imported_v1 *imported;
	struct wlr_xdg_toplevel *toplevel;

	struct wl_list link; // wlr_xdg_imported_v1.children

	struct {
		struct wl_listener xdg_toplevel_destroy;
		struct wl_listener xdg_toplevel_set_parent;
	} WLR_PRIVATE;
};

struct wlr_xdg_foreign_v1 *wlr_xdg_foreign_v1_create(
		struct wl_display *display, struct wlr_xdg_foreign_registry *registry);

#endif
