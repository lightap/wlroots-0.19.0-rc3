PKG_CONFIG?=pkg-config
WAYLAND_PROTOCOLS!=$(PKG_CONFIG) --variable=pkgdatadir wayland-protocols
WAYLAND_SCANNER!=$(PKG_CONFIG) --variable=wayland_scanner wayland-scanner
# Define PKG_CONFIG_PATH to include custom paths and system paths
PKG_CONFIG_PATH?=/usr/local/lib/pkgconfig:/usr/lib/x86_64-linux-gnu/pkgconfig:$$PKG_CONFIG_PATH
# Check if required pkg-config packages are available
CHECK_PKG_CONFIG=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) --exists $(1) && echo yes || echo no)
# Ensure all required packages are found
REQUIRED_PKGS=wlroots wayland-server xkbcommon pixman-1 libdrm glesv2 libpng freerdp2 winpr2
$(foreach pkg,$(REQUIRED_PKGS),$(if $(call CHECK_PKG_CONFIG,$(pkg)),,\
	$(error Package $(pkg) not found in PKG_CONFIG_PATH=$(PKG_CONFIG_PATH))))
# Define LIBS with all required libraries, manually adding -lEGL
LIBS=\
	$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) --libs wlroots) \
	$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) --libs wayland-server) \
	$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) --libs xkbcommon) \
	$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) --libs pixman-1) \
	$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) --libs libdrm) \
	$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) --libs glesv2) \
	$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) --libs libpng) \
	$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) --libs freerdp2) \
	$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) --libs winpr2) \
	-lEGL -lm
# Define CFLAGS with all required includes
CFLAGS_PKG_CONFIG=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) --cflags wlroots wayland-server xkbcommon pixman-1 libdrm glesv2 libpng freerdp2 winpr2)
CFLAGS+=$(CFLAGS_PKG_CONFIG) -I. -Iinclude -DWLR_USE_UNSTABLE -g -Werror
LDFLAGS+=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) --libs-only-L wlroots wayland-server xkbcommon pixman-1 libdrm glesv2 libpng freerdp2 winpr2)

all: tinywl

# Generate xdg-shell protocol header
xdg-shell-protocol.h:
	$(WAYLAND_SCANNER) server-header \
		$(WAYLAND_PROTOCOLS)/stable/xdg-shell/xdg-shell.xml $@

tinywl.o: tinywl.c xdg-shell-protocol.h
	$(CC) -c $< $(CFLAGS) -o $@

backend_RDP_backend.o: ../backend/RDP/backend.c
	$(CC) -c $< $(CFLAGS) -o $@

# Just compile tinywl and the backend without the renderer
tinywl: tinywl.o backend_RDP_backend.o
	$(CC) $^ $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@

clean:
	rm -f tinywl tinywl.o backend_RDP_backend.o xdg-shell-protocol.h

.PHONY: all clean