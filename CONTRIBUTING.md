# Contributing to wlroots

Contributing just involves sending a merge request. You will probably be more
successful with your contribution if you visit [#wlroots on Libera Chat]
upfront and discuss your plans.

Note: rules are made to be broken. Adjust or ignore any/all of these as you see
fit, but be prepared to justify it to your peers.

## Forking

New GitLab accounts may not have the permission to fork repositories. You will
need to [file a user verification request] to get this permission. See the
[freedesktop wiki] for more details.

The fork must be marked as public to allow CI to run.

## Merge Requests

If you already have your own merge request habits, feel free to use them. If you
don't, however, allow me to make a suggestion: feature branches pulled from
upstream. Try this:

1. Fork wlroots
2. `git clone git@gitlab.freedesktop.org:<username>/wlroots.git && cd wlroots`
3. `git remote add upstream https://gitlab.freedesktop.org/wlroots/wlroots.git`

You only need to do this once. You're never going to use your fork's master
branch. Instead, when you start working on a feature, do this:

1. `git fetch upstream`
2. `git checkout -b add-so-and-so-feature upstream/master`
3. Add and commit your changes
4. `git push -u origin add-so-and-so-feature`
5. Make a merge request from your feature branch

When you submit your merge request, your commit log should do most of the talking
when it comes to describing your changes and their motivation. In addition to
this, your merge request's comments will ideally include a test plan that the
reviewers can use to (1) demonstrate the problem on master, if applicable and
(2) verify that the problem no longer exists with your changes applied (or that
your new features work correctly). Document all of the edge cases you're aware
of so we can adequately test them - then verify the test plan yourself before
submitting.

## Commit Log

Unlike many projects using GitHub and GitLab, wlroots has a [linear, "recipe"
style] history. This means that every commit should be small, digestible,
stand-alone, and functional. Rather than a purely chronological commit history
like this:

```
doc: final docs for view transforms
fix tests when disabled, redo broken doc formatting
better transformed-view iteration (thanks Hannah!)
try to catch more cases in tests
tests: add new spline test
fix compilation on splines
doc: notes on reticulating splines
compositor: add spline reticulation for view transforms
```

We aim to have a clean history which only reflects the final state, broken up
into functional groupings:

```
compositor: add spline reticulation for view transforms
compositor: new iterator for view transforms
tests: add view-transform correctness tests
doc: fix formatting for view transforms
```

This ensures that the final patch series only contains the final state,
without the changes and missteps taken along the development process. A linear
history eases reviewing, cherry-picking and reverting changes.

If you aren't comfortable with manipulating the Git history, have a look at
[git-rebase.io].

## Commit Messages

Please strive to write good commit messages. Here's some guidelines to follow:

The first line should be limited to 50 characters and should be a sentence that
completes the thought [When applied, this commit will...] *"Implement
cmd_move"* or *"Improve performance of arrange_windows on ARM"* or similar.

The subsequent lines should be separated from the subject line by a single
blank line, and include optional details. In this you can give justification
for the change, [reference issues], or explain some of the subtler
details of your patch. This is important because when someone finds a line of
code they don't understand later, they can use the `git blame` command to find
out what the author was thinking when they wrote it. It's also easier to review
your merge requests if they're separated into logical commits that have good
commit messages and justify themselves in the extended commit description.

As a good rule of thumb, anything you might put into the merge request
description on GitLab is probably fair game for going into the extended commit
message as well.

See [How to Write a Git Commit Message] for more details.

## Code Review

When your changes are submitted for review, one or more core committers will
look over them. Smaller changes might be merged with little fanfare, but larger
changes will typically see review from several people. Be prepared to receive
some feedback - you may be asked to make changes to your work. Our code review
process is:

1. **Triage** the merge request. Do the commit messages make sense? Is a test
   plan necessary and/or present? Add anyone as reviewers that you think should
   be there (using the relevant GitLab feature, if you have the permissions, or
   with an @mention if necessary).
2. **Review** the code. Look for code style violations, naming convention
   violations, buffer overflows, memory leaks, logic errors, non-portable code
   (including GNU-isms), etc. For significant changes to the public API, loop in
   a couple more people for discussion.
3. **Execute** the test plan, if present.
4. **Merge** the merge request when all reviewers approve.
5. **File** follow-up tickets if appropriate.

## Code of Conduct

Note that as a project hosted on freedesktop.org, wlroots follows its
[Code of Conduct], based on the Contributor Covenant. Please conduct yourself
in a respectful and civilized manner when communicating with community members
on IRC and bug tracker.

## Style Reference

wlroots is written in C with a style similar to the [kernel style], but with a
few notable differences.

Try to keep your code conforming to C11 and POSIX as much as possible, and do
not use GNU extensions.

### Brackets

Brackets always go on the same line, including in functions.
Always include brackets for if/while/for, even if it's a single statement.
```c
void function(void) {
	if (condition1) {
		do_thing1();
	}

	if (condition2) {
		do_thing2();
	} else {
		do_thing3();
	}
}
```

### Indentation

Indentations are a single tab.

For long lines that need to be broken, the continuation line should be indented
with an additional tab.
If the line being broken is opening a new block (functions, if, while, etc.),
the continuation line should be indented with two tabs, so they can't be
misread as being part of the block.
```c
really_long_function(argument1, argument2, ...,
	argument3, argument4);

if (condition1 && condition2 && ...
		condition3 && condition4) {
	do_thing();
}
```

Try to break the line in the place which you think is the most appropriate.

### Line Length

Try to keep your lines under 100 columns, but you can break this rule if it
improves readability. Don't break lines indiscriminately, try to find nice
breaking points so your code is easy to read.

### Names

Global function and type names should be prefixed with `wlr_submodule_` (e.g.
`struct wlr_output`, `wlr_output_set_cursor`).  For static functions and
types local to a file, the names chosen aren't as important.  Local function
names shouldn't have a `wlr_` prefix.

For include guards, use the header's filename relative to include.  Uppercase
all of the characters, and replace any invalid characters with an underscore.

### Construction/Destruction Functions

Functions that are responsible for constructing objects should take one of the
two following forms:

* `init`: for functions which accept a pointer to a pre-allocated object (e.g.
a member of a struct) and initialize it. Such functions must zero out the
memory before initializing it to avoid leaving unset fields.
* `create`: for functions which allocate the memory for an object, initialize
it, and return a pointer. Such functions should allocate the memory with
`calloc()` to avoid leaving unset fields.

Likewise, functions that are responsible for destructing objects should take
one of the two following forms:

* `finish`: for functions which accept a pointer to an object and deinitialize
it. If a finished object isn't destroyed but kept for future use, it must be
reinitialized to be used again.
* `destroy`: for functions which accept a pointer to an object, deinitialize
it, and free the memory. Such functions should always be able to accept a NULL
pointer.

If the object has signals, the destructor function must assert that their
listener lists are empty.

```c
void wlr_thing_init(struct wlr_thing *thing) {
	*thing = (struct wlr_thing){
		// ...
	};

	wl_signal_init(&thing->events.destroy);
	wl_signal_init(&thing->events.foo);
}

void wlr_thing_finish(struct wlr_thing *thing) {
	wl_signal_emit_mutable(&thing->events.destroy, NULL);

	assert(wl_list_empty(&thing->events.destroy.listener_list));
	assert(wl_list_empty(&thing->events.foo.listener_list));
}
```

### Error Codes

For functions not returning a value, they should return a (stdbool.h) bool to
indicate whether they succeeded or not.

### Macros

Try to keep the use of macros to a minimum, especially if a function can do the
job.  If you do need to use them, try to keep them close to where they're being
used and `#undef` them after.

### Documentation

* Documentation comments for declarations start with `/**` and end with `*/`.
* Cross-reference other declarations by ending function names with `()`, and
  writing `struct`, `union`, `enum` or `typedef` before types.
* Document fields which can be NULL with a `// may be NULL` comment, optionally
  with more details describing when this can happen.
* Document the bits of a bitfield with a `// enum bar` comment.
* Document the `data` argument of a `struct wl_signal` with a `// struct foo`
  comment.
* Document the contents and container of a `struct wl_list` with a
  `// content.link` and `// container.list` comment.

### Private fields

Wrap private fields of public structures with `struct { … } WLR_PRIVATE`. This
ensures that compositor authors don't use them by accident. Within wlroots
`WLR_PRIVATE` is expanded to nothing, so private fields are accessed in the same
way as public ones.

### Safety

* Avoid string manipulation functions which don't take the size of the
  destination buffer as input: for instance, prefer `snprintf` over `sprintf`.
* Avoid repeating type names in `sizeof()` where possible. For instance, prefer
  `ptr = calloc(1, sizeof(*ptr))` over `ptr = calloc(1, sizeof(struct foo))`.
* Prefer `*ptr = (struct foo){0}` over `memset(ptr, 0, sizeof(*ptr))`.
* Prefer `*foo = *bar` over `memcpy(foo, bar, sizeof(*foo))`.

### Example

```c
struct wlr_backend *wlr_backend_autocreate(struct wl_display *display) {
	struct wlr_backend *backend;
	if (getenv("WAYLAND_DISPLAY") || getenv("_WAYLAND_DISPLAY")) {
		backend = attempt_wl_backend(display);
		if (backend) {
			return backend;
		}
	}

	const char *RDP_display = getenv("DISPLAY");
	if (RDP_display) {
		return wlr_RDP_backend_create(display, RDP_display);
	}

	// Attempt DRM+libinput

	struct wlr_session *session = wlr_session_create(display);
	if (!session) {
		wlr_log(WLR_ERROR, "Failed to start a DRM session");
		return NULL;
	}

	int gpu = wlr_session_find_gpu(session);
	if (gpu == -1) {
		wlr_log(WLR_ERROR, "Failed to open DRM device");
		goto error_session;
	}

	backend = wlr_multi_backend_create(session);
	if (!backend) {
		goto error_gpu;
	}

	struct wlr_backend *libinput = wlr_libinput_backend_create(display, session);
	if (!libinput) {
		goto error_multi;
	}

	struct wlr_backend *drm = wlr_drm_backend_create(display, session, gpu);
	if (!drm) {
		goto error_libinput;
	}

	wlr_multi_backend_add(backend, libinput);
	wlr_multi_backend_add(backend, drm);
	return backend;

error_libinput:
	wlr_backend_destroy(libinput);
error_multi:
	wlr_backend_destroy(backend);
error_gpu:
	wlr_session_close_file(session, gpu);
error_session:
	wlr_session_destroy(session);
	return NULL;
}
```

## Wayland protocol implementation

Each protocol generally lives in a file with the same name, usually containing
at least one struct for each interface in the protocol. For instance,
`xdg_shell` lives in `types/wlr_xdg_shell.h` and has a `wlr_xdg_surface` struct.

### Globals

Global interfaces generally have public constructors and destructors. Their
struct has a field holding the `wl_global` itself, a destroy signal and a
`wl_display` destroy listener. Example:

```c
struct wlr_compositor {
	struct wl_global *global;
	…

	struct {
		struct wl_signal new_surface;
		struct wl_signal destroy;
	} events;

	struct {
		struct wl_listener display_destroy;
	} WLR_PRIVATE;
};
```

When the destructor is called, it should emit the destroy signal, remove the
display destroy listener, destroy the `wl_global` and then destroy the struct.
The destructor can assume all clients and resources have been already
destroyed.

### Resources

Resources are the representation of Wayland objects on the compositor side. They
generally have an associated struct, called the _object struct_, stored in their
`user_data` field.

Object structs can be retrieved from resources via `wl_resource_get_data`. To
prevent bad casts, a safe helper function checking the type of the resource is
used:

```c
static const struct wl_surface_interface surface_impl;

struct wlr_surface *wlr_surface_from_resource(struct wl_resource *resource) {
	assert(wl_resource_instance_of(resource, &wl_surface_interface,
		&surface_impl));
	return wl_resource_get_user_data(resource);
}
```

If a pointer to a `wl_resource` is stored, a resource destroy handler needs to
be registered to clean it up. libwayland will automatically destroy resources
in an arbitrary order when a client is disconnected, the compositor must handle
this correctly.

### Destroying resources

Object structs should only be destroyed when their resource is destroyed, ie.
in the resource destroy handler (set with `wl_resource_set_implementation`).

- If the object has a destructor request: the request handler should just call
  `wl_resource_destroy` and do nothing else. The compositor must not destroy
  resources on its own outside the destructor request handler.
- If the protocol specifies that an object is destroyed when an event is sent:
  it's the only case where the compositor is allowed to send the event and then
  call `wl_resource_destroy`. An example of this is `wl_callback`.

### Inert resources

Some resources can become inert in situations described in the protocol or when
the compositor decides to get rid of them. All requests made to inert resources
should be ignored, except the destructor. This is achieved by:

1. When the resource becomes inert: destroy the object struct and call
   `wl_resource_set_user_data(resource, NULL)`. Do not destroy the resource.
2. For each request made to a resource that can be inert: add a NULL check to
   ignore the request if the resource is inert.
3. When the client calls the destructor request on the resource: call
   `wl_resource_destroy(resource)` as usual.
4. When the resource is destroyed, if the resource isn't inert, destroy the
   object struct.

Example:

```c
// Handles the destroy request
static void subsurface_handle_destroy(struct wl_client *client,
		struct wl_resource *resource) {
	wl_resource_destroy(resource);
}

// Handles a regular request
static void subsurface_set_position(struct wl_client *client,
		struct wl_resource *resource, int32_t x, int32_t y) {
	struct wlr_subsurface *subsurface = subsurface_from_resource(resource);
	if (subsurface == NULL) {
		return;
	}

	…
}

// Destroys the wlr_subsurface struct
static void subsurface_destroy(struct wlr_subsurface *subsurface) {
	if (subsurface == NULL) {
		return;
	}

	…

	wl_resource_set_user_data(subsurface->resource, NULL);
	free(subsurface);
}

// Resource destroy listener
static void subsurface_handle_resource_destroy(struct wl_resource *resource) {
	struct wlr_subsurface *subsurface = subsurface_from_resource(resource);
	subsurface_destroy(subsurface);
}

// Makes the resource inert
static void subsurface_handle_surface_destroy(struct wl_listener *listener,
		void *data) {
	struct wlr_subsurface *subsurface =
		wl_container_of(listener, subsurface, surface_destroy);
	subsurface_destroy(subsurface);
}
```

[#wlroots on Libera Chat]: https://web.libera.chat/gamja/?channels=#wlroots
[file a user verification request]: https://gitlab.freedesktop.org/freedesktop/freedesktop/-/issues/new?issuable_template=User%20verification
[freedesktop wiki]: https://gitlab.freedesktop.org/freedesktop/freedesktop/-/wikis/home
[linear, "recipe" style]: https://www.bitsnbites.eu/git-history-work-log-vs-recipe/
[git-rebase.io]: https://git-rebase.io/
[reference issues]: https://docs.gitlab.com/ee/user/project/issues/managing_issues.html#closing-issues-automatically
[Code of Conduct]: https://www.freedesktop.org/wiki/CodeOfConduct/
[How to Write a Git Commit Message]: https://chris.beams.io/posts/git-commit/
[kernel style]: https://www.kernel.org/doc/Documentation/process/coding-style.rst
