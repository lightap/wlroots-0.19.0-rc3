#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wayland-server-core.h>
#include <wlr/util/log.h>
#include <wlr/xwayland.h>
#include "config.h"
#include "sockets.h"

static void safe_close(int fd) {
	if (fd >= 0) {
		close(fd);
	}
}

noreturn static void exec_xwayland(struct wlr_xwayland_server *server,
		int notify_fd) {
	if (!set_cloexec(server->x_fd[0], false) ||
			!set_cloexec(server->x_fd[1], false) ||
			!set_cloexec(server->wl_fd[1], false)) {
		wlr_log(WLR_ERROR, "Failed to unset CLOEXEC on FD");
		_exit(EXIT_FAILURE);
	}
	if (server->options.enable_wm && !set_cloexec(server->wm_fd[1], false)) {
		wlr_log(WLR_ERROR, "Failed to unset CLOEXEC on FD");
		_exit(EXIT_FAILURE);
	}

	char *argv[64] = {0};
	size_t i = 0;

	char listenfd0[16], listenfd1[16], displayfd[16];
	snprintf(listenfd0, sizeof(listenfd0), "%d", server->x_fd[0]);
	snprintf(listenfd1, sizeof(listenfd1), "%d", server->x_fd[1]);
	snprintf(displayfd, sizeof(displayfd), "%d", notify_fd);

	argv[i++] = "Xwayland";
	argv[i++] = server->display_name;
	argv[i++] = "-rootless";
	argv[i++] = "-core";

	argv[i++] = "-terminate";
#if HAVE_XWAYLAND_TERMINATE_DELAY
	char terminate_delay[16];
	if (server->options.terminate_delay > 0) {
		snprintf(terminate_delay, sizeof(terminate_delay), "%d",
			server->options.terminate_delay);
		argv[i++] = terminate_delay;
	}
#endif

#if HAVE_XWAYLAND_LISTENFD
	argv[i++] = "-listenfd";
	argv[i++] = listenfd0;
	argv[i++] = "-listenfd";
	argv[i++] = listenfd1;
#else
	argv[i++] = "-listen";
	argv[i++] = listenfd0;
	argv[i++] = "-listen";
	argv[i++] = listenfd1;
#endif
	argv[i++] = "-displayfd";
	argv[i++] = displayfd;

	char wmfd[16];
	if (server->options.enable_wm) {
		snprintf(wmfd, sizeof(wmfd), "%d", server->wm_fd[1]);
		argv[i++] = "-wm";
		argv[i++] = wmfd;
	}

#if HAVE_XWAYLAND_NO_TOUCH_POINTER_EMULATION
	if (server->options.no_touch_pointer_emulation) {
		argv[i++] = "-noTouchPointerEmulation";
	}
#else
	server->options.no_touch_pointer_emulation = false;
#endif

#if HAVE_XWAYLAND_FORCE_XRANDR_EMULATION
	if (server->options.force_xrandr_emulation) {
		argv[i++] = "-force-xrandr-emulation";
	}
#else
	server->options.force_xrandr_emulation = false;
#endif

	argv[i++] = NULL;

	assert(i <= sizeof(argv) / sizeof(argv[0]));

	char wayland_socket_str[16];
	snprintf(wayland_socket_str, sizeof(wayland_socket_str), "%d", server->wl_fd[1]);
	setenv("WAYLAND_SOCKET", wayland_socket_str, true);

	wlr_log(WLR_INFO, "Starting Xwayland on :%d", server->display);

	// Closes stdout/stderr depending on log verbosity
	enum wlr_log_importance verbosity = wlr_log_get_verbosity();
	int devnull = open("/dev/null", O_WRONLY | O_CREAT | O_CLOEXEC, 0666);
	if (devnull < 0) {
		wlr_log_errno(WLR_ERROR, "XWayland: failed to open /dev/null");
		_exit(EXIT_FAILURE);
	}
	if (verbosity < WLR_INFO) {
		dup2(devnull, STDOUT_FILENO);
	}
	if (verbosity < WLR_ERROR) {
		dup2(devnull, STDERR_FILENO);
	}

	const char *xwayland_path = getenv("WLR_XWAYLAND");
	if (xwayland_path) {
		wlr_log(WLR_INFO, "Using Xwayland binary '%s' due to WLR_XWAYLAND",
			xwayland_path);
	} else {
		xwayland_path = XWAYLAND_PATH;
	}

	// This returns if and only if the call fails
	execvp(xwayland_path, argv);

	wlr_log_errno(WLR_ERROR, "failed to exec %s", xwayland_path);
	close(devnull);
	_exit(EXIT_FAILURE);
}

static void server_finish_process(struct wlr_xwayland_server *server) {
	if (!server || server->display == -1) {
		return;
	}

	if (server->x_fd_read_event[0]) {
		wl_event_source_remove(server->x_fd_read_event[0]);
		wl_event_source_remove(server->x_fd_read_event[1]);

		server->x_fd_read_event[0] = server->x_fd_read_event[1] = NULL;
	}

	if (server->client) {
		wl_list_remove(&server->client_destroy.link);
		wl_client_destroy(server->client);
	}
	if (server->pipe_source) {
		wl_event_source_remove(server->pipe_source);
	}

	safe_close(server->wl_fd[0]);
	safe_close(server->wl_fd[1]);
	safe_close(server->wm_fd[0]);
	safe_close(server->wm_fd[1]);
	memset(server, 0, offsetof(struct wlr_xwayland_server, display));
	server->wl_fd[0] = server->wl_fd[1] = -1;
	server->wm_fd[0] = server->wm_fd[1] = -1;

	/* We do not kill the Xwayland process, it dies to broken pipe
	 * after we close our side of the wm/wl fds. This is more reliable
	 * than trying to kill something that might no longer be Xwayland.
	 */
}

static void server_finish_display(struct wlr_xwayland_server *server) {
	if (!server) {
		return;
	}

	wl_list_remove(&server->display_destroy.link);
	wl_list_init(&server->display_destroy.link);

	if (server->display == -1) {
		return;
	}

	safe_close(server->x_fd[0]);
	safe_close(server->x_fd[1]);
	server->x_fd[0] = server->x_fd[1] = -1;

	unlink_display_sockets(server->display);
	server->display = -1;
	server->display_name[0] = '\0';
}

static bool server_start(struct wlr_xwayland_server *server);
static bool server_start_lazy(struct wlr_xwayland_server *server);

static void handle_client_destroy(struct wl_listener *listener, void *data) {
	struct wlr_xwayland_server *server =
		wl_container_of(listener, server, client_destroy);

	if (server->pipe_source) {
		// Xwayland failed to start, let the readiness handler deal with it
		return;
	}

	// Don't call client destroy: it's being destroyed already
	server->client = NULL;
	wl_list_remove(&server->client_destroy.link);

	server_finish_process(server);

	if (time(NULL) - server->server_start > 5) {
		if (server->options.lazy) {
			wlr_log(WLR_INFO, "Restarting Xwayland (lazy)");
			server_start_lazy(server);
		} else  {
			wlr_log(WLR_INFO, "Restarting Xwayland");
			server_start(server);
		}
	}
}

static void handle_display_destroy(struct wl_listener *listener, void *data) {
	struct wlr_xwayland_server *server =
		wl_container_of(listener, server, display_destroy);

	// Don't call client destroy: the display is being destroyed, it's too late
	if (server->client) {
		server->client = NULL;
		wl_list_remove(&server->client_destroy.link);
	}

	wlr_xwayland_server_destroy(server);
}

static int xserver_handle_ready(int fd, uint32_t mask, void *data) {
	struct wlr_xwayland_server *server = data;

	if (mask & WL_EVENT_READABLE) {
		/* Xwayland writes to the pipe twice, so if we close it too early
		 * it's possible the second write will fail and Xwayland shuts down.
		 * Make sure we read until end of line marker to avoid this.
		 */
		char buf[64];
		ssize_t n = read(fd, buf, sizeof(buf));
		if (n < 0 && errno != EINTR) {
			/* Clear mask to signal start failure after reaping child */
			wlr_log_errno(WLR_ERROR, "read from Xwayland display_fd failed");
			mask = 0;
		} else if (n <= 0 || buf[n-1] != '\n') {
			/* Returning 1 here means recheck and call us again if required. */
			return 1;
		}
	}

	while (waitpid(server->pid, NULL, 0) < 0) {
		if (errno == EINTR) {
			continue;
		}

		/* If some application has installed a SIGCHLD handler, they
		 * may race and waitpid() on our child, which will cause this
		 * waitpid() to fail. We have a signal from the
		 * notify pipe that things are ready, so this waitpid() is only
		 * to prevent zombies, which will have already been reaped by
		 * the application's SIGCHLD handler.
		 */
		if (errno == ECHILD) {
			break;
		}
		wlr_log_errno(WLR_ERROR, "waitpid for Xwayland fork failed");
		goto error;
	}
	/* Xwayland will only write on the fd once it has finished its
	 * initial setup. Getting an event here without READABLE means
	 * the server end failed.
	 */
	if (!(mask & WL_EVENT_READABLE)) {
		assert(mask & WL_EVENT_HANGUP);
		wlr_log(WLR_ERROR, "Xwayland startup failed, not setting up xwm");
		goto error;
	}
	wlr_log(WLR_DEBUG, "Xserver is ready");

	close(fd);
	wl_event_source_remove(server->pipe_source);
	server->pipe_source = NULL;
	server->ready = true;

	struct wlr_xwayland_server_ready_event event = {
		.server = server,
		.wm_fd = server->wm_fd[0],
	};
	wl_signal_emit_mutable(&server->events.ready, &event);

	/* We removed the source, so don't need recheck */
	return 0;

error:
	/* clean up */
	close(fd);
	server_finish_process(server);
	server_finish_display(server);
	return 0;
}

static bool server_start_display(struct wlr_xwayland_server *server,
		struct wl_display *wl_display) {
	server->display_destroy.notify = handle_display_destroy;
	wl_display_add_destroy_listener(wl_display, &server->display_destroy);

	server->display = open_display_sockets(server->x_fd);
	if (server->display < 0) {
		server_finish_display(server);
		return false;
	}

	snprintf(server->display_name, sizeof(server->display_name),
		":%d", server->display);
	return true;
}

static bool server_start(struct wlr_xwayland_server *server) {
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, server->wl_fd) != 0) {
		wlr_log_errno(WLR_ERROR, "socketpair failed");
		server_finish_process(server);
		return false;
	}
	if (!set_cloexec(server->wl_fd[0], true) ||
			!set_cloexec(server->wl_fd[1], true)) {
		wlr_log(WLR_ERROR, "Failed to set O_CLOEXEC on socket");
		server_finish_process(server);
		return false;
	}
	if (server->options.enable_wm) {
		if (socketpair(AF_UNIX, SOCK_STREAM, 0, server->wm_fd) != 0) {
			wlr_log_errno(WLR_ERROR, "socketpair failed");
			server_finish_process(server);
			return false;
		}
		if (!set_cloexec(server->wm_fd[0], true) ||
				!set_cloexec(server->wm_fd[1], true)) {
			wlr_log(WLR_ERROR, "Failed to set O_CLOEXEC on socket");
			server_finish_process(server);
			return false;
		}
	}

	server->server_start = time(NULL);

	server->client = wl_client_create(server->wl_display, server->wl_fd[0]);
	if (!server->client) {
		wlr_log_errno(WLR_ERROR, "wl_client_create failed");
		server_finish_process(server);
		return false;
	}

	server->wl_fd[0] = -1; /* not ours anymore */

	server->client_destroy.notify = handle_client_destroy;
	wl_client_add_destroy_listener(server->client, &server->client_destroy);

	int notify_fd[2];
	if (pipe(notify_fd) == -1) {
		wlr_log_errno(WLR_ERROR, "pipe failed");
		server_finish_process(server);
		return false;
	}
	if (!set_cloexec(notify_fd[0], true)) {
		wlr_log(WLR_ERROR, "Failed to set CLOEXEC on FD");
		close(notify_fd[0]);
		close(notify_fd[1]);
		server_finish_process(server);
		return false;
	}

	struct wl_event_loop *loop = wl_display_get_event_loop(server->wl_display);
	server->pipe_source = wl_event_loop_add_fd(loop, notify_fd[0],
		WL_EVENT_READABLE, xserver_handle_ready, server);

	wl_signal_emit_mutable(&server->events.start, NULL);

	server->pid = fork();
	if (server->pid < 0) {
		wlr_log_errno(WLR_ERROR, "fork failed");
		close(notify_fd[0]);
		close(notify_fd[1]);
		server_finish_process(server);
		return false;
	} else if (server->pid == 0) {
		pid_t pid = fork();
		if (pid < 0) {
			wlr_log_errno(WLR_ERROR, "second fork failed");
			_exit(EXIT_FAILURE);
		} else if (pid == 0) {
			exec_xwayland(server, notify_fd[1]);
		}

		_exit(EXIT_SUCCESS);
	}

	/* close child fds */
	/* remain managing x sockets for lazy start */
	close(notify_fd[1]);
	close(server->wl_fd[1]);
	safe_close(server->wm_fd[1]);
	server->wl_fd[1] = server->wm_fd[1] = -1;

	return true;
}

static int xwayland_socket_connected(int fd, uint32_t mask, void *data) {
	struct wlr_xwayland_server *server = data;

	wl_event_source_remove(server->x_fd_read_event[0]);
	wl_event_source_remove(server->x_fd_read_event[1]);
	server->x_fd_read_event[0] = server->x_fd_read_event[1] = NULL;

	server_start(server);

	return 0;
}

static bool server_start_lazy(struct wlr_xwayland_server *server) {
	struct wl_event_loop *loop = wl_display_get_event_loop(server->wl_display);

	if (!(server->x_fd_read_event[0] = wl_event_loop_add_fd(loop, server->x_fd[0],
				WL_EVENT_READABLE, xwayland_socket_connected, server))) {
		return false;
	}

	if (!(server->x_fd_read_event[1] = wl_event_loop_add_fd(loop, server->x_fd[1],
				WL_EVENT_READABLE, xwayland_socket_connected, server))) {
		wl_event_source_remove(server->x_fd_read_event[0]);
		server->x_fd_read_event[0] = NULL;
		return false;
	}

	return true;
}

static void handle_idle(void *data) {
	struct wlr_xwayland_server *server = data;
	server->idle_source = NULL;
	server_start(server);
}

void wlr_xwayland_server_destroy(struct wlr_xwayland_server *server) {
	if (!server) {
		return;
	}

	if (server->idle_source != NULL) {
		wl_event_source_remove(server->idle_source);
	}
	server_finish_process(server);
	server_finish_display(server);

	wl_signal_emit_mutable(&server->events.destroy, NULL);

	assert(wl_list_empty(&server->events.start.listener_list));
	assert(wl_list_empty(&server->events.ready.listener_list));
	assert(wl_list_empty(&server->events.destroy.listener_list));

	free(server);
}

struct wlr_xwayland_server *wlr_xwayland_server_create(
		struct wl_display *wl_display,
		struct wlr_xwayland_server_options *options) {
	if (!getenv("WLR_XWAYLAND") && access(XWAYLAND_PATH, X_OK) != 0) {
		wlr_log(WLR_ERROR, "Cannot find Xwayland binary \"%s\"", XWAYLAND_PATH);
		return NULL;
	}

	struct wlr_xwayland_server *server = calloc(1, sizeof(*server));
	if (!server) {
		return NULL;
	}

	server->wl_display = wl_display;
	server->options = *options;

#if !HAVE_XWAYLAND_TERMINATE_DELAY
	server->options.terminate_delay = 0;
#endif

	server->x_fd[0] = server->x_fd[1] = -1;
	server->wl_fd[0] = server->wl_fd[1] = -1;
	server->wm_fd[0] = server->wm_fd[1] = -1;

	wl_signal_init(&server->events.start);
	wl_signal_init(&server->events.ready);
	wl_signal_init(&server->events.destroy);

	if (!server_start_display(server, wl_display)) {
		goto error_alloc;
	}

	if (server->options.lazy) {
		if (!server_start_lazy(server)) {
			goto error_display;
		}
	} else {
		struct wl_event_loop *loop = wl_display_get_event_loop(wl_display);
		server->idle_source = wl_event_loop_add_idle(loop, handle_idle, server);
		if (server->idle_source == NULL) {
			goto error_display;
		}
	}

	return server;

error_display:
	server_finish_display(server);
error_alloc:
	free(server);
	return NULL;
}
