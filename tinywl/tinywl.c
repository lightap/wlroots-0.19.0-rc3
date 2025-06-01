#include <wlr/types/wlr_xdg_shell.h>
#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/backend/RDP.h>
#include <wlr/types/wlr_xdg_shell.h>

#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/log.h>
#include <xkbcommon/xkbcommon.h>
#include <wlr/render/gles2.h>
#include <wlr/render/drm_format_set.h>
#include <wayland-server-protocol.h>
#include <drm_fourcc.h>
#include <string.h>
#include <EGL/egl.h>
#include <wlr/render/egl.h>
#include <wlr/render/gles2.h>
#include <wlr/render/egl.h>

#include <wlr/backend/RDP.h>
#include <wlr/render/gles2.h>     // For wlr_gles2_renderer_create_surfaceless, wlr_gles2_renderer_create_with_egl
#include <wlr/render/wlr_renderer.h> // For wlr_renderer_begin, wlr_renderer_end etc.
#include <wlr/render/egl.h>    
#include <GLES3/gl31.h> // For OpenGL ES 3.1 constants
#include <wlr/render/gles2.h>

#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/box.h> // For wlr_matrix_project_box, also includes matrix.h
#include <wlr/util/log.h> // For WLR_ERRORARNING
#include <freerdp/freerdp.h> // For freerdp_peer and rdpContext
 // For wlr_rdp_backend
#include <wlr/types/wlr_seat.h> // For wlr_seat_pointer_notify_*
#include <linux/input-event-codes.h> // For BTN_LEFT, BTN_RIGHT, BTN_MIDDLE
#include <wlr/render/wlr_renderer.h> // For renderer functions
#include <wlr/types/wlr_matrix.h>   // For wlr_matrix_project_box
#include <wlr/types/wlr_scene.h>    // For scene graph functions
#include <wlr/render/gles2.h>       // For GLES2-specific functions
#include <wlr/render/pass.h>        // For wlr_render_pass API
#include <GLES3/gl31.h>
#include <time.h>

#ifndef RENDER_EGL_H
#define RENDER_EGL_H

#include <wlr/render/egl.h>

#include <wlr/render/wlr_renderer.h> // For wlr_renderer functions
      // For wlr_matrix_project_box
#include <wlr/types/wlr_scene.h>    // For scene graph functions
#include <GLES3/gl31.h>             // For OpenGL ES 3.1

struct wlr_egl {
    EGLDisplay display;
    EGLContext context;
    EGLDeviceEXT device; // may be EGL_NO_DEVICE_EXT
    struct gbm_device *gbm_device;

    struct {
        // Display extensions
        bool KHR_image_base;
        bool EXT_image_dma_buf_import;
        bool EXT_image_dma_buf_import_modifiers;
        bool IMG_context_priority;
        bool EXT_create_context_robustness;

        // Device extensions
        bool EXT_device_drm;
        bool EXT_device_drm_render_node;

        // Client extensions
        bool EXT_device_query;
        bool KHR_platform_gbm;
        bool EXT_platform_device;
        bool KHR_display_reference;
    } exts;

    struct {
        PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT;
        PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
        PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR;
        PFNEGLQUERYDMABUFFORMATSEXTPROC eglQueryDmaBufFormatsEXT;
        PFNEGLQUERYDMABUFMODIFIERSEXTPROC eglQueryDmaBufModifiersEXT;
        PFNEGLDEBUGMESSAGECONTROLKHRPROC eglDebugMessageControlKHR;
        PFNEGLQUERYDISPLAYATTRIBEXTPROC eglQueryDisplayAttribEXT;
        PFNEGLQUERYDEVICESTRINGEXTPROC eglQueryDeviceStringEXT;
        PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT;
        PFNEGLCREATESYNCKHRPROC eglCreateSyncKHR;
        PFNEGLDESTROYSYNCKHRPROC eglDestroySyncKHR;
        PFNEGLDUPNATIVEFENCEFDANDROIDPROC eglDupNativeFenceFDANDROID;
        PFNEGLWAITSYNCKHRPROC eglWaitSyncKHR;
    } procs;

    bool has_modifiers;
    struct wlr_drm_format_set dmabuf_texture_formats;
    struct wlr_drm_format_set dmabuf_render_formats;
};

struct wlr_egl_context {
    EGLDisplay display;
    EGLContext context;
    EGLSurface draw_surface;
    EGLSurface read_surface;
};

/**
 * Initializes an EGL context for the given DRM FD.
 *
 * Will attempt to load all possibly required API functions.
 */
struct wlr_egl *wlr_egl_create_with_drm_fd(int drm_fd);

/**
 * Frees all related EGL resources, makes the context not-current and
 * unbinds a bound wayland display.
 */
void wlr_egl_destroy(struct wlr_egl *egl);

/**
 * Creates an EGL image from the given dmabuf attributes. Check usability
 * of the dmabuf with wlr_egl_check_import_dmabuf once first.
 */
EGLImageKHR wlr_egl_create_image_from_dmabuf(struct wlr_egl *egl,
    struct wlr_dmabuf_attributes *attributes, bool *external_only);

/**
 * Get DMA-BUF formats suitable for sampling usage.
 */
const struct wlr_drm_format_set *wlr_egl_get_dmabuf_texture_formats(
    struct wlr_egl *egl);
/**
 * Get DMA-BUF formats suitable for rendering usage.
 */
const struct wlr_drm_format_set *wlr_egl_get_dmabuf_render_formats(
    struct wlr_egl *egl);

/**
 * Destroys an EGL image created with the given wlr_egl.
 */
bool wlr_egl_destroy_image(struct wlr_egl *egl, EGLImageKHR image);

int wlr_egl_dup_drm_fd(struct wlr_egl *egl);

/**
 * Restore EGL context that was previously saved using wlr_egl_save_current().
 */
bool wlr_egl_restore_context(struct wlr_egl_context *context);

/**
 * Make the EGL context current.
 *
 * The old EGL context is saved. Callers are expected to clear the current
 * context when they are done by calling wlr_egl_restore_context().
 */
bool wlr_egl_make_current(struct wlr_egl *egl, struct wlr_egl_context *save_context);

bool wlr_egl_unset_current(struct wlr_egl *egl);

EGLSyncKHR wlr_egl_create_sync(struct wlr_egl *egl, int fence_fd);

void wlr_egl_destroy_sync(struct wlr_egl *egl, EGLSyncKHR sync);

int wlr_egl_dup_fence_fd(struct wlr_egl *egl, EGLSyncKHR sync);

bool wlr_egl_wait_sync(struct wlr_egl *egl, EGLSyncKHR sync);

#endif

// Add at the top of your tinywl.c file
struct wlr_renderer *wlr_gles2_renderer_create_surfaceless(void);

/* RDP backend access */
struct wlr_rdp_backend {
    struct wlr_backend backend;
    freerdp_peer *global_rdp_peer;
    bool started;
    /* Other fields may exist, but we only need these */
};

#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_xdg_shell.h> // THIS IS KEY for wlr_xdg_surface_get_geometry
// Add other wlr/types/ as needed: scene, output_layout, cursor, seat, etc.
#include <wlr/types/wlr_scene.h>


#include <wlr/backend.h>
#include <wlr/render/wlr_renderer.h> // THIS IS KEY for struct wlr_render_texture_options
#include <wlr/render/allocator.h>
#include <wlr/render/egl.h>
#include <wlr/render/gles2.h>

#include <drm_fourcc.h>

/* Structures */
enum tinywl_cursor_mode {
    TINYWL_CURSOR_PASSTHROUGH,
    TINYWL_CURSOR_MOVE,
    TINYWL_CURSOR_RESIZE,
};



struct tinywl_server {
    struct wl_display *wl_display;
    struct wlr_backend *backend;
    struct wlr_renderer *renderer;
    struct wlr_allocator *allocator;
    struct wlr_scene *scene;
    struct wlr_scene_output_layout *scene_layout;

    struct wlr_xdg_shell *xdg_shell;
    struct wl_listener new_xdg_toplevel;
    struct wl_listener new_xdg_popup;
    struct wl_list toplevels;

    struct wlr_cursor *cursor;
    struct wlr_xcursor_manager *cursor_mgr;
    struct wl_listener cursor_motion;
    struct wl_listener cursor_motion_absolute;
    struct wl_listener cursor_button;
    struct wl_listener cursor_axis;
    struct wl_listener cursor_frame;

struct wl_listener pointer_motion;
    struct wlr_seat *seat;
    struct wl_listener new_input;
    struct wl_listener request_cursor;
    struct wl_listener request_set_selection;
    struct wl_list keyboards;
    enum tinywl_cursor_mode cursor_mode;
    struct tinywl_toplevel *grabbed_toplevel;
    double grab_x, grab_y;
    struct wlr_box grab_geobox;
    uint32_t resize_edges;

    struct wlr_output_layout *output_layout;
    struct wl_list outputs;
    struct wl_listener new_output;
      EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
   EGLSurface egl_surface;
 // For XDG clients
 // list of tinywl_output.link
    struct wl_list views;   // <<< THIS IS CRUCIAL: list of tinywl_view.link
    struct wl_list popups;        // List of tinywl_popup objects
    struct wl_listener new_popup; // Listener for new xdg_popup events

     GLuint shader_program; // Flame shader for buffers/animations
    GLint flame_shader_mvp_loc;
    GLint flame_shader_tex_loc;
    GLint flame_shader_time_loc;
    GLint flame_shader_res_loc;

    // New: Rectangle Shader
    GLuint rect_shader_program;
    GLint rect_shader_mvp_loc;
    GLint rect_shader_color_loc;
    GLint rect_shader_time_loc;
    GLint rect_shader_resolution_loc; // New: for iResolution

    // New: Panel Shader
    GLuint panel_shader_program;
    GLint panel_shader_mvp_loc;
    GLint panel_shader_time_loc;
    GLint panel_shader_base_color_loc; // u_panel_base_color
    GLint panel_shader_resolution_loc;

     // ADD THESE UNIFORM LOCATIONS FOR THE PANEL PREVIEW
    GLint panel_shader_preview_tex_loc;
    GLint panel_shader_is_preview_active_loc;
    GLint panel_shader_preview_rect_loc;
    GLint panel_shader_preview_tex_transform_loc;

    // Pointer to the top panel's scene node for easy identification
    struct wlr_scene_node *top_panel_node;

     // Custom shader program for animations
    GLint scale_uniform;  // Location of the scale uniform in the shader

    // For shared quad rendering
    GLuint quad_vao;
    GLuint quad_vbo;
    GLuint quad_ibo;

  
};

/* Updated tinywl_output struct to store timer */
struct tinywl_output {
    struct wl_list link;
    struct tinywl_server *server;
    struct wlr_output *wlr_output;
    struct wl_listener frame;
    struct wl_listener request_state;
    struct wl_listener destroy;
    struct wl_event_source *timer; /* Added for timer access */
    struct wlr_scene_output *scene_output;
    bool rendering; 
    struct wlr_texture *cached_texture;
};

enum tinywl_toplevel_type {
    TINYWL_TOPLEVEL_XDG,
    TINYWL_TOPLEVEL_CUSTOM,
};

struct tinywl_toplevel {
    struct wl_list link;
    struct tinywl_server *server;
    union {
        struct wlr_xdg_toplevel *xdg_toplevel;
        struct wlr_scene_rect *custom_rect;
    };

    // This 'scene_tree' should be the *main frame* for the window,
    // including decorations AND client content area.
    struct wlr_scene_tree *scene_tree; // Overall window frame

    // This will point to the scene tree specifically for the client's content
    // (i.e., the tree returned by wlr_scene_xdg_surface_create for the base xdg_surface)
    struct wlr_scene_tree *client_xdg_scene_tree; // Tree for xdg_toplevel->base

    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener commit;
    struct wl_listener destroy;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    struct wl_listener request_maximize;
    struct wl_listener request_fullscreen;
    struct wlr_texture *cached_texture;
    uint32_t last_commit_seq;
    bool needs_redraw;
    struct wlr_texture *custom_texture;
bool mapped;
    // Animation state
    bool is_animating;
    float scale;          // Current scale factor (1.0 = normal size)
    float target_scale;   // Target scale for animation (e.g., 1.0 for zoom in, 0.0 for minimize)
    float animation_start; // Animation start time (in seconds)
    float animation_duration; // Duration in seconds (e.g., 0.3 for 300ms)
 bool pending_destroy;
enum tinywl_toplevel_type type;
};





struct tinywl_keyboard {
    struct wl_list link;
    struct tinywl_server *server;
    struct wlr_keyboard *wlr_keyboard;
    struct wl_listener modifiers;
    struct wl_listener key;
    struct wl_listener destroy;
};

struct tinywl_view {
    struct tinywl_server *server;
    struct wlr_xdg_surface *xdg_surface; // Or wlr_layer_surface_v1, etc.
    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener destroy;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    // You also need a commit listener on xdg_surface->surface->events.commit
    // to know when to ask for a frame render.
    struct wl_listener surface_commit;

    bool mapped;
    int x, y; // Position on the output
    struct wl_list link; // For server.views
};

struct tinywl_popup {
    struct wl_listener map;       // Listener for surface map events
    struct wl_listener unmap;     // Listener for surface unmap events
    struct wl_listener destroy;   // Listener for popup destroy
    struct wl_listener commit;    // Listener for surface commit
    struct wlr_xdg_popup *xdg_popup; // The underlying xdg_popup
    struct tinywl_server *server;    // Reference to the compositor
    struct wlr_scene_tree *scene_tree; // Scene node for rendering
    struct wl_list link;          // Link for list management
};

struct shader_uniform_spec {
    const char *name;       // Name of the uniform in GLSL
    GLint *location_ptr;  // Pointer to where the GLint location should be stored
};


/* Function declarations */
//struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display, struct wlr_egl *egl);
//struct wlr_renderer *wlr_gles2_renderer_create_surfaceless(void);
struct wlr_allocator *wlr_rdp_allocator_create(struct wlr_renderer *renderer);
void rdp_transmit_surface(struct wlr_buffer *buffer);
//freerdp_peer *get_global_rdp_peer(void);
struct wlr_egl *setup_surfaceless_egl(struct tinywl_server *server) ;
void cleanup_egl(struct tinywl_server *server);
static void process_cursor_motion(struct tinywl_server *server, uint32_t time);
void check_scene_bypass_issue(struct wlr_scene_output *scene_output, struct wlr_output *output) ;
void debug_scene_rendering(struct wlr_scene *scene, struct wlr_output *output) ;
void debug_scene_tree(struct wlr_scene *scene, struct wlr_output *output);
void desktop_background(struct tinywl_server *server) ;

 float get_monotonic_time_seconds_as_float(void) ;
static void scene_buffer_iterator(struct wlr_scene_buffer *scene_buffer, int sx, int sy, void *user_data); // If not already declared



// Function to compile a shader
static GLuint compile_shader(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log_buf[512];
        glGetShaderInfoLog(shader, sizeof(log_buf), NULL, log_buf);
        wlr_log(WLR_ERROR, "Shader compilation failed: %s", log_buf);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}



static bool create_generic_shader_program(
    struct wlr_renderer *renderer,
    const char *shader_name_for_log,
    const char *vertex_src,
    const char *fragment_src,
    GLuint *program_id_out,
    struct shader_uniform_spec *uniforms_to_get,
    int num_uniforms_to_get
) {
    if (!renderer || !program_id_out || !vertex_src || !fragment_src) {
        wlr_log(WLR_ERROR, "Invalid NULL arguments to create_generic_shader_program for %s", shader_name_for_log);
        if (program_id_out) *program_id_out = 0;
        return false;
    }
    if (num_uniforms_to_get > 0 && !uniforms_to_get) {
        wlr_log(WLR_ERROR, "uniforms_to_get is NULL but num_uniforms_to_get is > 0 for %s", shader_name_for_log);
        *program_id_out = 0;
        return false;
    }

    *program_id_out = 0; // Initialize output

    struct wlr_egl *egl = wlr_gles2_renderer_get_egl(renderer);
    if (!egl) {
        wlr_log(WLR_ERROR, "Failed to get EGL context for %s shader", shader_name_for_log);
        return false;
    }

    struct wlr_egl_context saved_egl_ctx;
    // Save the current EGL context and make ours current
    if (!wlr_egl_make_current(egl, &saved_egl_ctx)) {
        wlr_log(WLR_ERROR, "Failed to make EGL context current for %s shader", shader_name_for_log);
        return false;
    }

    GLenum gl_err;
    while ((gl_err = glGetError()) != GL_NO_ERROR) {
        wlr_log(WLR_DEBUG, "%s: Clearing pre-existing GL error: 0x%x", shader_name_for_log, gl_err);
    }

    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_src);
    if (!vs) {
        wlr_log(WLR_ERROR, "Vertex shader compilation failed for %s", shader_name_for_log);
        wlr_egl_restore_context(&saved_egl_ctx);
        return false;
    }

    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
    if (!fs) {
        wlr_log(WLR_ERROR, "Fragment shader compilation failed for %s", shader_name_for_log);
        glDeleteShader(vs);
        wlr_egl_restore_context(&saved_egl_ctx);
        return false;
    }

    GLuint program = glCreateProgram();
    if (program == 0) {
        wlr_log(WLR_ERROR, "glCreateProgram failed for %s: GL error 0x%x", shader_name_for_log, glGetError());
        glDeleteShader(vs);
        glDeleteShader(fs);
        wlr_egl_restore_context(&saved_egl_ctx);
        return false;
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Shaders can be deleted after linking
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (!link_status) {
        char log_buf[1024];
        glGetProgramInfoLog(program, sizeof(log_buf), NULL, log_buf);
        wlr_log(WLR_ERROR, "%s shader program linking failed: %s", shader_name_for_log, log_buf);
        glDeleteProgram(program);
        wlr_egl_restore_context(&saved_egl_ctx);
        return false;
    }

    wlr_log(WLR_INFO, "%s shader program created successfully: ID=%u", shader_name_for_log, program);
    *program_id_out = program;

    // Get uniform locations if requested
    if (num_uniforms_to_get > 0) {
        glUseProgram(program); // Program must be active to query uniform locations
        for (int i = 0; i < num_uniforms_to_get; ++i) {
            if (uniforms_to_get[i].name && uniforms_to_get[i].location_ptr) {
                *(uniforms_to_get[i].location_ptr) = glGetUniformLocation(program, uniforms_to_get[i].name);
                if (*(uniforms_to_get[i].location_ptr) == -1) {
                    wlr_log(WLR_ERROR, "%s shader: Uniform '%s' not found (or optimized out). Location: %d",
                            shader_name_for_log, uniforms_to_get[i].name, *(uniforms_to_get[i].location_ptr));
                } else {
                    wlr_log(WLR_DEBUG, "%s shader: Uniform '%s' found at location %d.",
                            shader_name_for_log, uniforms_to_get[i].name, *(uniforms_to_get[i].location_ptr));
                }
            } else {
                wlr_log(WLR_ERROR, "%s shader: Invalid uniform_spec at index %d (null name or ptr).", shader_name_for_log, i);
            }
        }
        glUseProgram(0); // Unbind program after getting locations
    }

    // Restore the previous EGL context
    if (!wlr_egl_restore_context(&saved_egl_ctx)) {
        wlr_log(WLR_ERROR, "Failed to restore previous EGL context after %s shader creation.", shader_name_for_log);
        // This isn't necessarily fatal for the shader creation itself, but good to know.
    }

    return true;
}



/* Function implementations */
static void server_destroy(struct tinywl_server *server) {
    /* Remove all listeners */
    wl_list_remove(&server->new_xdg_toplevel.link);
    wl_list_remove(&server->new_xdg_popup.link);
    wl_list_remove(&server->cursor_motion.link);
    wl_list_remove(&server->cursor_motion_absolute.link);
    wl_list_remove(&server->cursor_button.link);
    wl_list_remove(&server->cursor_axis.link);
    wl_list_remove(&server->cursor_frame.link);
    wl_list_remove(&server->new_input.link);
    wl_list_remove(&server->request_cursor.link);
    wl_list_remove(&server->request_set_selection.link);
    wl_list_remove(&server->new_output.link);

    /* Clean up toplevels */
    struct tinywl_toplevel *toplevel, *toplevel_tmp;
    wl_list_for_each_safe(toplevel, toplevel_tmp, &server->toplevels, link) {
        wl_list_remove(&toplevel->link);
        wl_list_remove(&toplevel->map.link);
        wl_list_remove(&toplevel->unmap.link);
        wl_list_remove(&toplevel->commit.link);
        wl_list_remove(&toplevel->destroy.link);
        wl_list_remove(&toplevel->request_move.link);
        wl_list_remove(&toplevel->request_resize.link);
        wl_list_remove(&toplevel->request_maximize.link);
        wl_list_remove(&toplevel->request_fullscreen.link);
        free(toplevel);
    }

    /* Clean up outputs */
    struct tinywl_output *output, *output_tmp;
    wl_list_for_each_safe(output, output_tmp, &server->outputs, link) {
        wl_list_remove(&output->link);
        wl_list_remove(&output->frame.link);
        wl_list_remove(&output->request_state.link);
        wl_list_remove(&output->destroy.link);
        free(output);
    }

    /* Clean up keyboards */
    struct tinywl_keyboard *kb, *kb_tmp;
    wl_list_for_each_safe(kb, kb_tmp, &server->keyboards, link) {
        wl_list_remove(&kb->link);
        wl_list_remove(&kb->modifiers.link);
        wl_list_remove(&kb->key.link);
        wl_list_remove(&kb->destroy.link);
        free(kb);
    }

    /* Destroy resources */
    if (server->scene) {
        wlr_scene_node_destroy(&server->scene->tree.node);
    }
    if (server->cursor_mgr) {
        wlr_xcursor_manager_destroy(server->cursor_mgr);
    }
    if (server->cursor) {
        wlr_cursor_destroy(server->cursor);
    }
    if (server->allocator) {
        wlr_allocator_destroy(server->allocator);
    }
    /* Temporary workaround for renderer buffer list corruption */
    if (server->renderer) {
        wlr_log(WLR_INFO, "Skipping renderer destruction due to potential buffer list corruption");
        // wlr_renderer_destroy(server.renderer); // Commented out to avoid segfault
    }
    if (server->backend) {
        wlr_backend_destroy(server->backend);
    }
    if (server->seat) {
        wlr_seat_destroy(server->seat);
    }
    if (server->wl_display) {
        wl_display_destroy(server->wl_display);
    }

  

     struct wlr_egl *egl_destroy = NULL;
    if (server->renderer) { // Need renderer to get EGL context
        egl_destroy = wlr_gles2_renderer_get_egl(server->renderer);
    }

    if (egl_destroy && wlr_egl_make_current(egl_destroy, NULL)) {
        if (server->shader_program) {
            glDeleteProgram(server->shader_program);
            server->shader_program = 0;
        }
        if (server->quad_vao) {
            glDeleteVertexArrays(1, &server->quad_vao);
            server->quad_vao = 0;
        }
        if (server->quad_vbo) {
            glDeleteBuffers(1, &server->quad_vbo);
            server->quad_vbo = 0;
        }
        if (server->quad_ibo) {
            glDeleteBuffers(1, &server->quad_ibo);
            server->quad_ibo = 0;
        }
        wlr_egl_unset_current(egl_destroy);
    } else if (server->shader_program || server->quad_vao || server->quad_vbo || server->quad_ibo) {
        wlr_log(WLR_ERROR, "Could not make EGL context current to delete GL resources in server_destroy");
    }

       if (egl_destroy && wlr_egl_make_current(egl_destroy, NULL)) {
        if (server->shader_program) { // Flame shader
            glDeleteProgram(server->shader_program);
            server->shader_program = 0;
        }
        if (server->rect_shader_program) { // New: Rect shader
            glDeleteProgram(server->rect_shader_program);
            server->rect_shader_program = 0;
        }
        // ... delete VAO, VBO, IBO ...
        wlr_egl_unset_current(egl_destroy);
    }
}



static void focus_toplevel(struct tinywl_toplevel *toplevel) {
    if (toplevel == NULL) {
        wlr_log(WLR_ERROR, "focus_toplevel: No toplevel to focus");
        return;
    }
    struct tinywl_server *server = toplevel->server;
    if (!server) { // Defensive
        wlr_log(WLR_ERROR, "focus_toplevel: toplevel->server is NULL!");
        return;
    }
    struct wlr_seat *seat = server->seat;
    if (!seat) { // Defensive
        wlr_log(WLR_ERROR, "focus_toplevel: server->seat is NULL!");
        return;
    }

    // Ensure xdg_toplevel and its base surface are valid before dereferencing
    if (!toplevel->xdg_toplevel || !toplevel->xdg_toplevel->base || !toplevel->xdg_toplevel->base->surface) {
        wlr_log(WLR_ERROR, "focus_toplevel: toplevel xdg structures or surface are NULL! xdg_toplevel=%p", (void*)toplevel->xdg_toplevel);
        if (toplevel->xdg_toplevel && !toplevel->xdg_toplevel->base) wlr_log(WLR_ERROR, "  base is NULL");
        else if (toplevel->xdg_toplevel && toplevel->xdg_toplevel->base && !toplevel->xdg_toplevel->base->surface) wlr_log(WLR_ERROR, "  surface is NULL");
        return;
    }
    struct wlr_surface *surface = toplevel->xdg_toplevel->base->surface;

    struct wlr_surface *prev_surface = seat->keyboard_state.focused_surface;

    if (prev_surface == surface) {
        wlr_log(WLR_DEBUG, "Surface %p already focused, skipping", surface); // Changed to DEBUG
        return;
    }

    if (prev_surface) {
        struct wlr_xdg_toplevel *prev_toplevel =
            wlr_xdg_toplevel_try_from_wlr_surface(prev_surface);
        if (prev_toplevel != NULL) {
            wlr_xdg_toplevel_set_activated(prev_toplevel, false);
            wlr_log(WLR_DEBUG, "Deactivated previous toplevel %p", prev_toplevel); // Changed to DEBUG
        }
    }

    if (!toplevel->scene_tree) { // Defensive
        wlr_log(WLR_ERROR, "focus_toplevel: toplevel->scene_tree is NULL!");
        return;
    }
    wlr_scene_node_raise_to_top(&toplevel->scene_tree->node);
    wl_list_remove(&toplevel->link);
    wl_list_insert(&server->toplevels, &toplevel->link);
    wlr_xdg_toplevel_set_activated(toplevel->xdg_toplevel, true);
    wlr_log(WLR_INFO, "Focused toplevel %p, title='%s'", toplevel, // Changed to INFO
            toplevel->xdg_toplevel->title ? toplevel->xdg_toplevel->title : "(null)");

    struct wlr_keyboard *keyboard = wlr_seat_get_keyboard(seat);
    // CRITICAL CHECK: Ensure keyboard exists before trying to use it for notify_enter
    if (keyboard) {
        if (surface) { // Surface should be valid from checks above
            wlr_log(WLR_INFO, "focus_toplevel: Attempting to notify seat of keyboard focus on surface %p", surface);
            wlr_seat_keyboard_notify_enter(seat, surface,
                keyboard->keycodes, keyboard->num_keycodes, &keyboard->modifiers);
            wlr_log(WLR_INFO, "focus_toplevel: Notified seat of keyboard focus on surface %p", surface);
            if (server->wl_display) { // Defensive
                 wl_display_flush_clients(server->wl_display);
            } else {
                 wlr_log(WLR_ERROR, "focus_toplevel: server->wl_display is NULL during flush_clients!");
            }
        } else {
             wlr_log(WLR_ERROR, "focus_toplevel: Surface became NULL before notify_enter (should not happen).");
        }
    } else {
        wlr_log(WLR_ERROR, "focus_toplevel: No keyboard attached to seat %s. Cannot notify keyboard enter.", seat->name);
        // If there's no keyboard, we might still want to set pointer focus if applicable,
        // but for keyboard focus, we can't proceed.
        // Consider if wlr_seat_keyboard_clear_focus(seat); is appropriate if keyboard becomes NULL
        // or if focus should only be set if a keyboard is present.
        // For now, just logging is fine to confirm if this is the path.
    }
}

static void keyboard_handle_modifiers(struct wl_listener *listener, void *data) {
    struct tinywl_keyboard *keyboard = wl_container_of(listener, keyboard, modifiers);
    wlr_seat_set_keyboard(keyboard->server->seat, keyboard->wlr_keyboard);
    wlr_seat_keyboard_notify_modifiers(keyboard->server->seat,
        &keyboard->wlr_keyboard->modifiers);
}

static bool handle_keybinding(struct tinywl_server *server, xkb_keysym_t sym) {
    switch (sym) {
    case XKB_KEY_Escape:
        wl_display_terminate(server->wl_display);
        break;
    case XKB_KEY_F1:
        if (wl_list_length(&server->toplevels) < 2) {
            break;
        }
        struct tinywl_toplevel *next_toplevel =
            wl_container_of(server->toplevels.prev, next_toplevel, link);
        focus_toplevel(next_toplevel);
        break;
    default:
        return false;
    }
    return true;
}

static void keyboard_handle_key(struct wl_listener *listener, void *data) {
    struct tinywl_keyboard *keyboard = wl_container_of(listener, keyboard, key);
    struct tinywl_server *server = keyboard->server;
    struct wlr_keyboard_key_event *event = data;
    struct wlr_seat *seat = server->seat;
    
    // Log key event details with timestamp to track potential duplicates
    wlr_log(WLR_DEBUG, "Key event: keycode=%u, state=%s, time=%u",
            event->keycode, event->state == WL_KEYBOARD_KEY_STATE_PRESSED ? "pressed" : "released",
            event->time_msec);
    
    // Track duplicate events - if we see same keycode and state with similar timestamp, ignore it
    static uint32_t last_keycode = 0;
    static uint32_t last_state = 0;
    static uint32_t last_time = 0;
    
    // Check for duplicate events (same key, same state, within 5ms)
    if (last_keycode == event->keycode && 
        last_state == event->state &&
        event->time_msec > 0 && 
        last_time > 0 &&
        (event->time_msec - last_time < 5 || last_time - event->time_msec < 5)) {
        wlr_log(WLR_ERROR, "Ignoring duplicate key event: keycode=%u, state=%u, time=%u (prev=%u)",
                event->keycode, event->state, event->time_msec, last_time);
        return;
    }
    
    // Store current event details for duplicate detection
    last_keycode = event->keycode;
    last_state = event->state;
    last_time = event->time_msec;

    // Ignore release events at startup with no focused surface and no prior press
    if (event->state == WL_KEYBOARD_KEY_STATE_RELEASED && !seat->keyboard_state.focused_surface) {
        // Note: XKB keycodes are offset by 8 from evdev keycodes
        xkb_keycode_t xkb_keycode = event->keycode + 8;
        struct xkb_state *state = keyboard->wlr_keyboard->xkb_state;
        if (xkb_state_key_get_level(state, xkb_keycode, 0) == 0) {
            wlr_log(WLR_ERROR, "Ignoring phantom release for keycode=%u (no prior press, no focused surface)",
                    event->keycode);
            return;
        }
    }
    
    // Convert keycode to XKB keycode (Wayland keycodes are offset by 8)
    uint32_t keycode = event->keycode + 8;
    const xkb_keysym_t *syms;
    int nsyms = xkb_state_key_get_syms(keyboard->wlr_keyboard->xkb_state, keycode, &syms);
    
    if (nsyms <= 0) {
        wlr_log(WLR_ERROR, "No keysyms found for keycode %u (raw=%u)", keycode, event->keycode);
    } else {
        // Only log detailed keysym info at debug level to reduce noise
        if (wlr_log_get_verbosity() >= WLR_DEBUG) {
            for (int i = 0; i < nsyms; i++) {
                char buf[32];
                xkb_keysym_get_name(syms[i], buf, sizeof(buf));
                wlr_log(WLR_DEBUG, "Keysym %d: %s (0x%x)", i, buf, syms[i]);
            }
        }
    }
    
    // Handle keybindings for Alt+key combinations
    bool handled = false;
    uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard->wlr_keyboard);
    if ((modifiers & WLR_MODIFIER_ALT) && event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
        for (int i = 0; i < nsyms; i++) {
            handled = handle_keybinding(server, syms[i]);
            if (handled) break;
        }
    }
    
    // Forward key event to the focused client if not handled
    if (!handled) {
        // Set the keyboard once at initialization or focus change, not on every key event
        static struct wlr_keyboard *last_keyboard = NULL;
        if (last_keyboard != keyboard->wlr_keyboard) {
            wlr_seat_set_keyboard(seat, keyboard->wlr_keyboard);
            last_keyboard = keyboard->wlr_keyboard;
        }
        
        // Only notify if there's a focused surface
        if (seat->keyboard_state.focused_surface) {
            wlr_seat_keyboard_notify_key(seat, event->time_msec, event->keycode, event->state);
            wlr_log(WLR_DEBUG, "Key sent to focused surface %p", seat->keyboard_state.focused_surface);
        } else {
            wlr_log(WLR_DEBUG, "No focused surface for key event");
        }
    }
}

static void keyboard_handle_destroy(struct wl_listener *listener, void *data) {
    struct tinywl_keyboard *keyboard = wl_container_of(listener, keyboard, destroy);
    wl_list_remove(&keyboard->modifiers.link);
    wl_list_remove(&keyboard->key.link);
    wl_list_remove(&keyboard->destroy.link);
    wl_list_remove(&keyboard->link);
    free(keyboard);
}

static void server_new_keyboard(struct tinywl_server *server, struct wlr_input_device *device) {
    struct wlr_keyboard *wlr_keyboard = wlr_keyboard_from_input_device(device);
    if (!wlr_keyboard) {
        wlr_log(WLR_ERROR, "Failed to get wlr_keyboard from input device %p", device);
        return;
    }

    struct tinywl_keyboard *keyboard = calloc(1, sizeof(*keyboard));
    if (!keyboard) {
        wlr_log(WLR_ERROR, "Failed to allocate tinywl_keyboard");
        return;
    }
    keyboard->server = server;
    keyboard->wlr_keyboard = wlr_keyboard;

    // Initialize XKB context and keymap
    struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    if (!context) {
        wlr_log(WLR_ERROR, "Failed to create XKB context");
        free(keyboard);
        return;
    }

    // Use a more specific keymap to align with RDP client expectations
    struct xkb_rule_names rules = {0};
    rules.rules = "evdev";
    rules.model = "pc105";
    rules.layout = "gb";
    rules.variant = "";
    rules.options = "";
    struct xkb_keymap *keymap = xkb_keymap_new_from_names(context, &rules, XKB_KEYMAP_COMPILE_NO_FLAGS);
    if (!keymap) {
        wlr_log(WLR_ERROR, "Failed to create XKB keymap");
        xkb_context_unref(context);
        free(keyboard);
        return;
    }

    if (!wlr_keyboard_set_keymap(wlr_keyboard, keymap)) {
        wlr_log(WLR_ERROR, "Failed to set keymap on wlr_keyboard");
        xkb_keymap_unref(keymap);
        xkb_context_unref(context);
        free(keyboard);
        return;
    }
    xkb_keymap_unref(keymap);
    xkb_context_unref(context);

    // Set repeat info
    wlr_keyboard_set_repeat_info(wlr_keyboard, 25, 600);
    wlr_log(WLR_ERROR, "Keyboard %p initialized with keymap and repeat info", wlr_keyboard);

    // Add event listeners
    keyboard->modifiers.notify = keyboard_handle_modifiers;
    wl_signal_add(&wlr_keyboard->events.modifiers, &keyboard->modifiers);
    keyboard->key.notify = keyboard_handle_key;
    wl_signal_add(&wlr_keyboard->events.key, &keyboard->key);
    keyboard->destroy.notify = keyboard_handle_destroy;
    wl_signal_add(&device->events.destroy, &keyboard->destroy);

    // Set keyboard on seat
    wlr_seat_set_keyboard(server->seat, wlr_keyboard);
    wlr_log(WLR_ERROR, "Keyboard %p set on seat %s", wlr_keyboard, server->seat->name);

    wl_list_insert(&server->keyboards, &keyboard->link);
    wlr_log(WLR_ERROR, "Keyboard %p added to server keyboards list", wlr_keyboard);

    // Update seat capabilities
    wlr_seat_set_capabilities(server->seat, WL_SEAT_CAPABILITY_KEYBOARD);
    wlr_log(WLR_ERROR, "Set seat capabilities: WL_SEAT_CAPABILITY_KEYBOARD");
}

static void server_pointer_motion(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, pointer_motion);
    struct wlr_pointer_motion_event *event = data;

    if (!server->cursor || !server->output_layout) {
        wlr_log(WLR_ERROR, "Cursor or output layout is null during motion event");
        return;
    }

    wlr_cursor_move(server->cursor, &event->pointer->base, event->delta_x, event->delta_y);
    wlr_log(WLR_DEBUG, "Pointer motion: cursor moved to (%f, %f)", server->cursor->x, server->cursor->y);
    process_cursor_motion(server, event->time_msec);
}

static void server_new_pointer(struct tinywl_server *server, struct wlr_input_device *device) {
    if (device->type != WLR_INPUT_DEVICE_POINTER) {
        wlr_log(WLR_ERROR, "Device %p is not a pointer", device);
        return;
    }
    wlr_cursor_attach_input_device(server->cursor, device);
    struct wlr_pointer *pointer = wlr_pointer_from_input_device(device);
    if (pointer) {
        static struct wl_listener pointer_motion_listener;
        pointer_motion_listener.notify = server_pointer_motion;
        wl_signal_add(&pointer->events.motion, &pointer_motion_listener);
        wlr_log(WLR_DEBUG, "Attached motion listener for pointer %p", pointer);
        // Set default cursor image
        wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
        wlr_log(WLR_INFO, "Set default cursor image for device %p", device);
    } else {
        wlr_log(WLR_ERROR, "Failed to get wlr_pointer from device %p", device);
    }
}

static void server_new_input(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, new_input);
    struct wlr_input_device *device = data;
    switch (device->type) {
    case WLR_INPUT_DEVICE_KEYBOARD:
        server_new_keyboard(server, device);
        break;
    case WLR_INPUT_DEVICE_POINTER:
        server_new_pointer(server, device);
        break;
    default:
        break;
    }
    uint32_t caps = WL_SEAT_CAPABILITY_POINTER;
    if (!wl_list_empty(&server->keyboards)) {
        caps |= WL_SEAT_CAPABILITY_KEYBOARD;
    }
    wlr_seat_set_capabilities(server->seat, caps);
}

static void seat_request_cursor(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, request_cursor);
    struct wlr_seat_pointer_request_set_cursor_event *event = data;
    struct wlr_seat_client *focused_client = server->seat->pointer_state.focused_client;
    if (focused_client == event->seat_client) {
        wlr_cursor_set_surface(server->cursor, event->surface, event->hotspot_x, event->hotspot_y);
    }
}

static void seat_request_set_selection(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, request_set_selection);
    struct wlr_seat_request_set_selection_event *event = data;
    wlr_seat_set_selection(server->seat, event->source, event->serial);
}



static struct tinywl_toplevel *desktop_toplevel_at(
        struct tinywl_server *server, double lx, double ly,
        struct wlr_surface **surface, double *sx, double *sy) {
    /* This returns the topmost node in the scene at the given layout coords.
     * We only care about surface nodes as we are specifically looking for a
     * surface in the surface tree of a tinywl_toplevel. */
    struct wlr_scene_node *node = wlr_scene_node_at(
        &server->scene->tree.node, lx, ly, sx, sy);
    if (node == NULL || node->type != WLR_SCENE_NODE_BUFFER) {
        return NULL;
    }
    struct wlr_scene_buffer *scene_buffer = wlr_scene_buffer_from_node(node);
    struct wlr_scene_surface *scene_surface =
        wlr_scene_surface_try_from_buffer(scene_buffer);
    if (!scene_surface) {
        return NULL;
    }

    *surface = scene_surface->surface;
    /* Find the node corresponding to the tinywl_toplevel at the root of this
     * surface tree, it is the only one for which we set the data field. */
    struct wlr_scene_tree *tree = node->parent;
    while (tree != NULL && tree->node.data == NULL) {
        tree = tree->node.parent;
    }
    return tree->node.data;
}

///////////////////////////////////////////////////


static void process_cursor_move(struct tinywl_server *server) {
    struct tinywl_toplevel *toplevel = server->grabbed_toplevel;
    wlr_scene_node_set_position(&toplevel->scene_tree->node,
        server->cursor->x - server->grab_x, server->cursor->y - server->grab_y);
}

static void process_cursor_resize(struct tinywl_server *server) {
    struct tinywl_toplevel *toplevel = server->grabbed_toplevel;
    double border_x = server->cursor->x - server->grab_x;
    double border_y = server->cursor->y - server->grab_y;
    int new_left = server->grab_geobox.x;
    int new_right = server->grab_geobox.x + server->grab_geobox.width;
    int new_top = server->grab_geobox.y;
    int new_bottom = server->grab_geobox.y + server->grab_geobox.height;

    if (server->resize_edges & WLR_EDGE_TOP) {
        new_top = border_y;
        if (new_top >= new_bottom) {
            new_top = new_bottom - 1;
        }
    } else if (server->resize_edges & WLR_EDGE_BOTTOM) {
        new_bottom = border_y;
        if (new_bottom <= new_top) {
            new_bottom = new_top + 1;
        }
    }
    if (server->resize_edges & WLR_EDGE_LEFT) {
        new_left = border_x;
        if (new_left >= new_right) {
            new_left = new_right - 1;
        }
    } else if (server->resize_edges & WLR_EDGE_RIGHT) {
        new_right = border_x;
        if (new_right <= new_left) {
            new_right = new_left + 1;
        }
    }

    struct wlr_box *geo_box = &toplevel->xdg_toplevel->base->geometry;
    wlr_scene_node_set_position(&toplevel->scene_tree->node,
        new_left - geo_box->x, new_top - geo_box->y);

    int new_width = new_right - new_left;
    int new_height = new_bottom - new_top;
    wlr_xdg_toplevel_set_size(toplevel->xdg_toplevel, new_width, new_height);
}


// Add this helper function to improve popup handling
static void ensure_popup_responsiveness(struct tinywl_server *server, struct wlr_surface *surface) {
    if (!surface) return;
    
    struct wlr_xdg_surface *xdg_surface = wlr_xdg_surface_try_from_wlr_surface(surface);
    if (!xdg_surface || xdg_surface->role != WLR_XDG_SURFACE_ROLE_POPUP) {
        return;
    }
    
    // Ensure popup gets frame callbacks
    if (surface->current.buffer) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        wlr_surface_send_frame_done(surface, &now);
    }
    
    // Schedule configure if needed
    if (!xdg_surface->configured) {
        wlr_xdg_surface_schedule_configure(xdg_surface);
    }
}
/////////////////////////////////////////////////////////////
static void process_cursor_motion(struct tinywl_server *server, uint32_t time) {
    if (server->cursor_mode == TINYWL_CURSOR_MOVE) {
        process_cursor_move(server);
        return;
    } else if (server->cursor_mode == TINYWL_CURSOR_RESIZE) {
        process_cursor_resize(server);
        return;
    }
    
    double sx, sy;
    struct wlr_seat *seat = server->seat;
    struct wlr_surface *surface = NULL;
    wlr_log(WLR_DEBUG, "[PROCESS_CURSOR_MOTION] Cursor at: (%.2f, %.2f)", server->cursor->x, server->cursor->y);
    
    struct tinywl_toplevel *toplevel = desktop_toplevel_at(server,
            server->cursor->x, server->cursor->y, &surface, &sx, &sy);
    
    wlr_log(WLR_DEBUG, "[PROCESS_CURSOR_MOTION] Surface: %p, Surface coords: (%.2f, %.2f), Toplevel: %p",
            surface, sx, sy, toplevel);
    
    if (!toplevel) {
        wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
    }
    
    if (surface) {
        ensure_popup_responsiveness(server, surface);
        wlr_seat_pointer_notify_enter(seat, surface, sx, sy);
        wlr_seat_pointer_notify_motion(seat, time, sx, sy);
    } else {
        wlr_seat_pointer_clear_focus(seat);
    }
}


static void server_cursor_motion(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_motion);
    struct wlr_pointer_motion_event *event = data;
    wlr_cursor_move(server->cursor, &event->pointer->base, event->delta_x, event->delta_y);
    process_cursor_motion(server, event->time_msec);
}

static void server_cursor_motion_absolute(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_motion_absolute);
    struct wlr_pointer_motion_absolute_event *event = data;
    wlr_cursor_warp_absolute(server->cursor, &event->pointer->base, event->x, event->y);
    process_cursor_motion(server, event->time_msec);
}



static void server_cursor_button(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_button);
    struct wlr_pointer_button_event *event = data;

    // Log event for debugging
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    uint64_t now_ms = now.tv_sec * 1000 + now.tv_nsec / 1000000;
    wlr_log(WLR_DEBUG, "Button: button=%u, state=%s, event_time=%u, now=%lu, cursor_mode=%d",
            event->button, event->state == WL_POINTER_BUTTON_STATE_PRESSED ? "pressed" : "released",
            event->time_msec, now_ms, server->cursor_mode);

    // Find surface under cursor
    double sx, sy;
    struct wlr_surface *surface = NULL;
    struct tinywl_toplevel *toplevel = desktop_toplevel_at(server,
            server->cursor->x, server->cursor->y, &surface, &sx, &sy);

    // Update pointer focus
    if (surface) {
        wlr_seat_pointer_notify_enter(server->seat, surface, sx, sy);
        wlr_seat_pointer_notify_motion(server->seat, event->time_msec, sx, sy);
    } else {
        wlr_seat_pointer_clear_focus(server->seat);
    }

    // Notify button event to clients
    wlr_seat_pointer_notify_button(server->seat, event->time_msec, event->button, event->state);

    // Handle button press
    if (event->state == WL_POINTER_BUTTON_STATE_PRESSED) {
        if (toplevel && surface && toplevel->xdg_toplevel) {
            bool is_popup_surface = false;
            struct wlr_xdg_surface *xdg_surface = wlr_xdg_surface_try_from_wlr_surface(surface);
            if (xdg_surface && xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
                is_popup_surface = true;
            }
            if (!is_popup_surface && server->cursor_mode == TINYWL_CURSOR_PASSTHROUGH) {
                focus_toplevel(toplevel);

                // Initiate drag or resize based on keyboard modifiers
                bool alt_pressed = wlr_keyboard_get_modifiers(server->seat->keyboard_state.keyboard) & WLR_MODIFIER_ALT;
                bool ctrl_pressed = wlr_keyboard_get_modifiers(server->seat->keyboard_state.keyboard) & WLR_MODIFIER_CTRL;
                if (event->button == BTN_LEFT && alt_pressed) {
                    server->cursor_mode = TINYWL_CURSOR_MOVE;
                    server->grabbed_toplevel = toplevel;
                    server->grab_x = server->cursor->x;
                    server->grab_y = server->cursor->y;
                    // Initialize grab_geobox with cursor position and surface dimensions
                    int width = 800, height = 600; // Default size
                    if (surface && surface->current.width > 0 && surface->current.height > 0) {
                        width = surface->current.width;
                        height = surface->current.height;
                    }
                    server->grab_geobox = (struct wlr_box){
                        .x = (int)server->cursor->x,
                        .y = (int)server->cursor->y,
                        .width = width,
                        .height = height
                    };
                    wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "grab");
                    wlr_log(WLR_DEBUG, "Started window move: toplevel=%p, geobox=[%d,%d,%d,%d]",
                            toplevel, server->grab_geobox.x, server->grab_geobox.y,
                            server->grab_geobox.width, server->grab_geobox.height);
                } else if (event->button == BTN_LEFT && ctrl_pressed) {
                    server->cursor_mode = TINYWL_CURSOR_RESIZE;
                    server->grabbed_toplevel = toplevel;
                    server->grab_x = server->cursor->x;
                    server->grab_y = server->cursor->y;
                    // Initialize grab_geobox with cursor position and surface dimensions
                    int width = 800, height = 600; // Default size
                    if (surface && surface->current.width > 0 && surface->current.height > 0) {
                        width = surface->current.width;
                        height = surface->current.height;
                    }
                    server->grab_geobox = (struct wlr_box){
                        .x = (int)server->cursor->x,
                        .y = (int)server->cursor->y,
                        .width = width,
                        .height = height
                    };
                    server->resize_edges = WLR_EDGE_BOTTOM | WLR_EDGE_RIGHT;
                    wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "resize");
                    wlr_log(WLR_DEBUG, "Started window resize: toplevel=%p, geobox=[%d,%d,%d,%d], edges=%u",
                            toplevel, server->grab_geobox.x, server->grab_geobox.y,
                            server->grab_geobox.width, server->grab_geobox.height, server->resize_edges);
                }
            }
        }
    }
    // Handle button release
    else if (event->state == WL_POINTER_BUTTON_STATE_RELEASED) {
        // Reset cursor mode and grab state
        if (server->cursor_mode == TINYWL_CURSOR_MOVE || server->cursor_mode == TINYWL_CURSOR_RESIZE) {
            server->cursor_mode = TINYWL_CURSOR_PASSTHROUGH;
            server->grabbed_toplevel = NULL;
            server->grab_x = 0;
            server->grab_y = 0;
            server->grab_geobox = (struct wlr_box){0};
            server->resize_edges = 0;
            wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
            wlr_log(WLR_DEBUG, "Reset cursor mode to PASSTHROUGH on button release");
        }
    }

    // Handle popup surface frame updates
    if (surface) {
        struct wlr_xdg_surface *xdg_surface = wlr_xdg_surface_try_from_wlr_surface(surface);
        if (xdg_surface && xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
            struct tinywl_output *output;
            wl_list_for_each(output, &server->outputs, link) {
                if (output->wlr_output && output->wlr_output->enabled) {
                    wlr_output_schedule_frame(output->wlr_output);
                }
            }
            if (xdg_surface->surface && !xdg_surface->surface->current.buffer) {
                struct timespec now;
                clock_gettime(CLOCK_MONOTONIC, &now);
                wlr_surface_send_frame_done(xdg_surface->surface, &now);
            }
        }
    }

    // Schedule frame for all outputs
    struct tinywl_output *output;
    wl_list_for_each(output, &server->outputs, link) {
        if (output->wlr_output && output->wlr_output->enabled) {
            wlr_output_schedule_frame(output->wlr_output);
        }
    }
}

static void server_cursor_axis(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_axis);
    struct wlr_pointer_axis_event *event = data;
    wlr_seat_pointer_notify_axis(server->seat, event->time_msec, event->orientation,
        event->delta, event->delta_discrete, event->source, event->relative_direction);
}

static void server_cursor_frame(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_frame);
    wlr_seat_pointer_notify_frame(server->seat);
}


static void popup_map(struct wl_listener *listener, void *data) {
    struct tinywl_popup *popup = wl_container_of(listener, popup, map);
    
    // Enable the popup in the scene
    wlr_scene_node_set_enabled(&popup->scene_tree->node, true);
    
    // Send the ack_configure immediately to complete the handshake
    if (popup->xdg_popup && popup->xdg_popup->base) {
        struct wlr_xdg_surface *xdg_surface = popup->xdg_popup->base;
        if (xdg_surface->configured) {
            // Send ack to complete popup configuration
            wlr_xdg_surface_schedule_configure(xdg_surface);
        }
    }
    
    // Schedule frames on all outputs to ensure popup is displayed
    struct tinywl_server *server = popup->server;
    if (server) {
        struct tinywl_output *output;
        wl_list_for_each(output, &server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
            }
        }
    }
    
    wlr_log(WLR_DEBUG, "Popup mapped and configured: %p", popup->xdg_popup);
}

static void popup_unmap(struct wl_listener *listener, void *data) {
    struct tinywl_popup *popup = wl_container_of(listener, popup, unmap);
    
    // Disable the popup in the scene
    wlr_scene_node_set_enabled(&popup->scene_tree->node, false);
    
    // Schedule frames to update display
    struct tinywl_server *server = popup->server;
    if (server) {
        struct tinywl_output *output;
        wl_list_for_each(output, &server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
            }
        }
    }
    
    wlr_log(WLR_DEBUG, "Popup unmapped: %p", popup->xdg_popup);
}

static void popup_destroy(struct wl_listener *listener, void *data) {
    struct tinywl_popup *popup = wl_container_of(listener, popup, destroy);
    
    wlr_log(WLR_DEBUG, "Popup destroyed: %p", popup->xdg_popup);
    
    // Clean up listeners
    wl_list_remove(&popup->map.link);
    wl_list_remove(&popup->unmap.link);
    wl_list_remove(&popup->destroy.link);
    wl_list_remove(&popup->commit.link);
    wl_list_remove(&popup->link);
    
    // Destroy scene node
    if (popup->scene_tree) {
        wlr_scene_node_destroy(&popup->scene_tree->node);
    }
    
    // Schedule frames to update display after popup destruction
    struct tinywl_server *server = popup->server;
    if (server) {
        struct tinywl_output *output;
        wl_list_for_each(output, &server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
            }
        }
    }
    
    free(popup);
}




static void popup_commit(struct wl_listener *listener, void *data) {
    struct tinywl_popup *popup = wl_container_of(listener, popup, commit);
    
    if (!popup->xdg_popup || !popup->xdg_popup->base) {
        return;
    }

    struct wlr_xdg_surface *xdg_surface = popup->xdg_popup->base;
    struct wlr_surface *surface = xdg_surface->surface;
    
    wlr_log(WLR_DEBUG, "Popup commit: %p (initial: %d, configured: %d, mapped: %d)", 
            popup->xdg_popup, xdg_surface->initial_commit, xdg_surface->configured, surface ? surface->mapped : 0);
    
    // Handle configuration properly with immediate response for GTK
    if (xdg_surface->initial_commit) {
        // For initial commit, configure immediately
        wlr_xdg_surface_schedule_configure(xdg_surface);
        wlr_log(WLR_DEBUG, "Scheduled initial configure for popup: %p", popup->xdg_popup);
    } else if (!xdg_surface->configured) {
        // Configure unconfigured popups immediately
        wlr_xdg_surface_schedule_configure(xdg_surface);
        wlr_log(WLR_DEBUG, "Scheduled configure for unconfigured popup: %p", popup->xdg_popup);
    }
    
    // CRITICAL: Always send frame_done for popup commits to prevent GTK freezing
    if (surface && surface->current.buffer) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        wlr_surface_send_frame_done(surface, &now);
    }
    
    // Schedule frames on all outputs
    struct tinywl_server *server = popup->server;
    if (server) {
        struct tinywl_output *output;
        wl_list_for_each(output, &server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
            }
        }
    }
    
    // If this is a commit after configure, the popup should be ready
    if (xdg_surface->configured && surface && surface->mapped) {
        wlr_log(WLR_DEBUG, "Popup ready and mapped: %p", popup->xdg_popup);
    }
}


// Ensure you have these includes (or equivalent for your project structure)
#include <time.h>
#include <wlr/types/wlr_compositor.h> // For wlr_surface_send_frame_done
#include <wlr/types/wlr_output_damage.h> // For wlr_output_damage_add_whole (optional but good)


#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/box.h>
#include <wlr/util/transform.h>
#include <wlr/util/log.h>
#include <wlr/render/wlr_renderer.h>


#include <pixman.h>
#include <time.h>
#include <pthread.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_matrix.h> 
#include <wlr/types/wlr_matrix.h>


struct render_data {
    struct wlr_render_pass *pass;
    struct wlr_renderer *renderer;
    GLuint shader_program;
       GLint scale_uniform_loc; // Renamed to avoid conflict
    struct tinywl_server *server;
    struct wlr_output *output; // Add output for projection matrix
};

#include <wlr/types/wlr_matrix.h>

// ... (struct render_data definition) ...

#include <string.h> // For memcpy
#include <math.h>   // For round
#include <wlr/types/wlr_matrix.h>
// Ensure other necessary includes are present at the top of your file

// struct render_data should be defined as before:
/*
struct render_data {
    struct wlr_render_pass *pass;
    struct wlr_renderer *renderer;
    GLuint shader_program;
    // GLint scale_uniform_loc; // If you had a separate scale uniform in shader
    struct tinywl_server *server;
    struct wlr_output *output;
};
*/
#include <string.h> // For memcpy
#include <math.h>   // For round
#include <wlr/types/wlr_matrix.h>

// struct render_data definition...

#include <string.h> // For memcpy
#include <math.h>   // For round
#include <wlr/types/wlr_matrix.h>


#include <string.h> // For memcpy
#include <math.h>   // For round, fmaxf, fminf
#include <time.h>   // For clock_gettime

float get_monotonic_time_seconds_as_float(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
        wlr_log_errno(WLR_ERROR, "clock_gettime failed");
        return 0.0f;
    }
    return (float)ts.tv_sec + (float)ts.tv_nsec / 1e9f;
}

#include <string.h> // For memcpy
#include <math.h>   // For round, fmaxf, fminf
#include <time.h>   // For clock_gettime


// Helper (ensure it's defined or declared before these functions)
// static float get_monotonic_time_seconds_as_float() { /* ... */ }
#include <GLES2/gl2.h>
#include <wlr/render/gles2.h>
#include <wlr/types/wlr_matrix.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/util/log.h>
#include <string.h>
#include <stdlib.h>








// STEP 5: Alternative test - Create rectangle differently
void desktop_background(struct tinywl_server *server) {
    wlr_log(WLR_INFO, "=== ALTERNATIVE RECTANGLE TEST ===");
    
    // Method 1: Direct color array
    float red_color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    struct wlr_scene_rect *rect1 = wlr_scene_rect_create(&server->scene->tree, 1024, 768, red_color);
    if (rect1) {
        wlr_scene_node_set_position(&rect1->node, 0, 0);
        wlr_scene_node_set_enabled(&rect1->node, true);
        wlr_log(WLR_INFO, "Alternative rect 1 created at (0,0)");
    }
    
    
    
    
}


void desktop_panel(struct tinywl_server *server) {
    int panel_height=60;
    wlr_log(WLR_INFO, "=== DESKTOP PANEL TEST ===");
    float panel_tint_color[4] = {0.2f, 0.8f, 0.3f, 0.9f}; // Give it a distinct tint
    struct wlr_scene_rect *panel_srect = wlr_scene_rect_create(
        &server->scene->tree,
        1024, // Assuming output width
        panel_height,   // Panel height
        panel_tint_color
    );
    if (panel_srect) {
        server->top_panel_node = &panel_srect->node; // CRITICAL: Store the node
        wlr_scene_node_set_position(server->top_panel_node, 0, 768-panel_height); // Position at top
        wlr_scene_node_set_enabled(server->top_panel_node, true);
        wlr_scene_node_raise_to_top(server->top_panel_node); // Ensure it's on top
        wlr_log(WLR_INFO, "Desktop panel rect created and assigned to server->top_panel_node: %p at (0,0)", server->top_panel_node);
    } else {
        wlr_log(WLR_ERROR, "Failed to create desktop_panel rect.");
        server->top_panel_node = NULL; // Ensure it's NULL if creation fails
    }
}


// ISSUE 5: Debug logging to verify scene tree state
void debug_scene_tree(struct wlr_scene *scene, struct wlr_output *output) {
    wlr_log(WLR_INFO, "[DEBUG] Scene tree state for output %s:", output->name);
    
    struct wlr_scene_node *node;
    int node_count = 0;
    
    wl_list_for_each(node, &scene->tree.children, link) {
        node_count++;
        const char *type_name = "UNKNOWN";
        
        // Use correct enum values for wlroots 0.19
        switch (node->type) {
            case WLR_SCENE_NODE_TREE:
                type_name = "TREE";
                break;
            case WLR_SCENE_NODE_RECT:
                type_name = "RECT";
                break;
            case WLR_SCENE_NODE_BUFFER:
                type_name = "BUFFER";
                break;
            default:
                type_name = "UNKNOWN";
                break;
        }
        
        struct tinywl_toplevel *tl = node->data;
        const char *custom_type = "NONE";
        if (tl) {
            custom_type = (tl->type == TINYWL_TOPLEVEL_CUSTOM) ? "CUSTOM" : "XDG";
        }
        
        wlr_log(WLR_INFO, "[DEBUG] Node %d: type=%s, enabled=%d, custom_type=%s, pos=(%d,%d)",
                node_count, type_name, node->enabled, custom_type, node->x, node->y);
        
        // If it's a rect node, show color info
        if (node->type == WLR_SCENE_NODE_RECT) {
            struct wlr_scene_rect *rect = wlr_scene_rect_from_node(node);
            wlr_log(WLR_INFO, "[DEBUG] Rect: %dx%d, color=(%.2f,%.2f,%.2f,%.2f)",
                    rect->width, rect->height, 
                    rect->color[0], rect->color[1], rect->color[2], rect->color[3]);
        }
    }
    
    wlr_log(WLR_INFO, "[DEBUG] Total nodes: %d", node_count);
}

// scene_buffer_iterator with preview rendering logic and detailed logging
static void scene_buffer_iterator(struct wlr_scene_buffer *scene_buffer,
                                 int sx, int sy,
                                 void *user_data) {
    struct render_data *rdata = user_data;
    struct wlr_renderer *renderer = rdata->renderer;
    struct wlr_output *output = rdata->output;
    struct tinywl_server *server = rdata->server;
    struct wlr_gles2_texture_attribs tex_attribs;
    const char *output_name_log = output ? output->name : "UNKNOWN_OUTPUT";

    // Basic validation
    if (!rdata || !server || !renderer || !output) {
        wlr_log(WLR_ERROR, "[SCENE_ITERATOR_FATAL:%s] Invalid render_data, server, renderer, or output.", output_name_log);
        return;
    }
    if (!scene_buffer) {
        wlr_log(WLR_ERROR, "[SCENE_ITERATOR_FATAL:%s] scene_buffer is NULL.", output_name_log);
        return;
    }
    if (!scene_buffer->node.enabled) {
        wlr_log(WLR_DEBUG, "[SCENE_ITERATOR:%s] scene_buffer %p node not enabled, skipping.", output_name_log, (void*)scene_buffer);
        return;
    }
    if (output->width == 0 || output->height == 0) {
        wlr_log(WLR_ERROR, "[SCENE_ITERATOR:%s] Output has zero width/height.", output_name_log);
        return;
    }
    if (!scene_buffer->buffer) {
        wlr_log(WLR_DEBUG, "[SCENE_ITERATOR:%s] scene_buffer %p has NULL wlr_buffer, skipping.", output_name_log, (void*)scene_buffer);
        return;
    }

    // --- Toplevel identification (simplified like second version) ---
    struct tinywl_toplevel *toplevel = NULL;
    struct wlr_scene_node *node_iter = &scene_buffer->node;
    while (node_iter) {
        if (node_iter->data) {
            struct tinywl_toplevel *temp_ptl = node_iter->data;
            if (temp_ptl && temp_ptl->server == server && temp_ptl->scene_tree == (struct wlr_scene_tree*)node_iter) {
                toplevel = temp_ptl;
                break;
            }
        }
        if (!node_iter->parent) break;
        node_iter = &node_iter->parent->node;
    }

    float anim_scale_factor = 1.0f;
    char title_buffer[128] = "NO_TL_ITER";
    const char* tl_title_for_log = "UNKNOWN_OR_NOT_TOPLEVEL_CONTENT";

    // Surface validation like second version
    struct wlr_scene_surface *current_scene_surface = wlr_scene_surface_try_from_buffer(scene_buffer);
    struct wlr_surface *surface_to_render = current_scene_surface ? current_scene_surface->surface : NULL;
    if (!surface_to_render) return;

    // --- Animation Scaling (same logic but with debugging like second version) ---
    if (toplevel) {
        if (toplevel->xdg_toplevel && toplevel->xdg_toplevel->title) {
            snprintf(title_buffer, sizeof(title_buffer), "%s", toplevel->xdg_toplevel->title);
            tl_title_for_log = title_buffer;
        } else {
            snprintf(title_buffer, sizeof(title_buffer), "Ptr:%p (XDG maybe NULL)", toplevel);
            tl_title_for_log = "TOPLEVEL_NO_TITLE";
        }

        wlr_log(WLR_ERROR, "[ITERATOR DEBUG:%s] Processing TL ('%s' Ptr:%p). Anim:%d, Scale:%.3f, Target:%.3f, StartT:%.3f, PendingD:%d, NodeEn:%d",
               output_name_log, title_buffer, toplevel,
               toplevel->is_animating, toplevel->scale, toplevel->target_scale,
               toplevel->animation_start, toplevel->pending_destroy,
               (toplevel->scene_tree ? toplevel->scene_tree->node.enabled : -1));

        if (toplevel->is_animating) {
            struct timespec now_anim;
            clock_gettime(CLOCK_MONOTONIC, &now_anim);
            float current_time_anim = now_anim.tv_sec + now_anim.tv_nsec / 1e9f;
            float elapsed = current_time_anim - toplevel->animation_start;
            if (elapsed < 0.0f) elapsed = 0.0f;
            float t = 0.0f;
            if (toplevel->animation_duration > 1e-5f) {
                t = elapsed / toplevel->animation_duration;
            } else if (elapsed > 0) {
                t = 1.0f;
            }

            if (t >= 1.0f) {
                t = 1.0f;
                anim_scale_factor = toplevel->target_scale;
                if (toplevel->target_scale == 0.0f && !toplevel->pending_destroy) {
                    toplevel->scale = 0.0f;
                    wlr_log(WLR_ERROR, "[ITERATOR DEBUG:%s] '%s' close anim reached t>=1. Scale=0.0.", output_name_log, title_buffer);
                    wlr_output_schedule_frame(output);
                } else {
                    toplevel->is_animating = false;
                    toplevel->scale = toplevel->target_scale;
                }
            } else {
                anim_scale_factor = toplevel->scale + (toplevel->target_scale - toplevel->scale) * t;
            }

            if (toplevel->is_animating) {
                wlr_output_schedule_frame(output);
            }
            wlr_log(WLR_ERROR, "[ITERATOR DEBUG:%s] '%s': t=%.3f, anim_scale_factor=%.3f",
                   output_name_log, title_buffer, t, anim_scale_factor);
        } else {
            anim_scale_factor = toplevel->scale;
            if (toplevel->target_scale == 0.0f && toplevel->scale <= 0.001f && !toplevel->pending_destroy) {
                wlr_log(WLR_ERROR, "[ITERATOR DEBUG:%s] '%s' static at scale 0.0.", output_name_log, title_buffer);
                wlr_output_schedule_frame(output);
            }
        }
    }

    if (anim_scale_factor < 0.001f && !(toplevel && toplevel->target_scale == 0.0f && !toplevel->pending_destroy)) {
        anim_scale_factor = 0.001f;
    }

    // --- Main Window Rendering (using second version's approach) ---
    struct wlr_texture *texture = wlr_texture_from_buffer(renderer, scene_buffer->buffer);
    if (!texture) {
        wlr_log(WLR_ERROR, "[SCENE_ITERATOR:%s] Failed to create wlr_texture from buffer %p for '%s'.",
                output_name_log, (void*)scene_buffer->buffer, tl_title_for_log);
        return;
    }
    wlr_gles2_texture_get_attribs(texture, &tex_attribs);

    struct wlr_box main_render_box = {
        .x = (int)round((double)sx + (double)texture->width * (1.0 - (double)anim_scale_factor) / 2.0),
        .y = (int)round((double)sy + (double)texture->height * (1.0 - (double)anim_scale_factor) / 2.0),
        .width = (int)round((double)texture->width * (double)anim_scale_factor),
        .height = (int)round((double)texture->height * (double)anim_scale_factor),
    };

    if (main_render_box.width <= 0 || main_render_box.height <= 0) {
        if (toplevel && !toplevel->pending_destroy && toplevel->target_scale == 0.0f) {
            wlr_log(WLR_ERROR, "[ITERATOR DEBUG:%s] '%s' scaled to zero W/H.", output_name_log, title_buffer);
            wlr_output_schedule_frame(output);
        }
        wlr_texture_destroy(texture);
        return;
    }

    // Calculate MVP matrix using second version's simpler approach
    float main_mvp[9];
    wlr_matrix_identity(main_mvp);
    float box_scale_x = (float)main_render_box.width * (2.0f / output->width);
    float box_scale_y = (float)main_render_box.height * (-2.0f / output->height);
    float box_translate_x = ((float)main_render_box.x / output->width) * 2.0f - 1.0f;
    float box_translate_y = ((float)main_render_box.y / output->height) * -2.0f + 1.0f;

    // Handle output transforms like second version
    if (output->transform == WL_OUTPUT_TRANSFORM_FLIPPED_180) {
        main_mvp[0] = -box_scale_x; main_mvp[4] = -box_scale_y; main_mvp[6] = -box_translate_x; main_mvp[7] = -box_translate_y;
        main_mvp[1] = main_mvp[2] = main_mvp[3] = main_mvp[5] = 0.0f; main_mvp[8] = 1.0f;
    } else if (output->transform == WL_OUTPUT_TRANSFORM_NORMAL) {
        main_mvp[0] = box_scale_x; main_mvp[4] = box_scale_y; main_mvp[6] = box_translate_x; main_mvp[7] = box_translate_y;
        main_mvp[1] = main_mvp[2] = main_mvp[3] = main_mvp[5] = 0.0f; main_mvp[8] = 1.0f;
    } else {
        float temp_mvp[9] = {box_scale_x, 0.0f, 0.0f, 0.0f, box_scale_y, 0.0f, box_translate_x, box_translate_y, 1.0f};
        float transform_matrix[9];
        wlr_matrix_transform(transform_matrix, output->transform);
        wlr_matrix_multiply(main_mvp, transform_matrix, temp_mvp);
    }

    // Handle buffer transforms like second version
    if (scene_buffer->transform != WL_OUTPUT_TRANSFORM_NORMAL) {
        float temp_mvp_buffer[9];
        memcpy(temp_mvp_buffer, main_mvp, sizeof(main_mvp));
        float buffer_matrix[9];
        wlr_matrix_transform(buffer_matrix, scene_buffer->transform);
        wlr_matrix_multiply(main_mvp, temp_mvp_buffer, buffer_matrix);
    }

    // Set uniforms using glGetUniformLocation like second version
    GLint mvp_loc = glGetUniformLocation(server->shader_program, "mvp");
    if (mvp_loc != -1) glUniformMatrix3fv(mvp_loc, 1, GL_FALSE, main_mvp);

    GLint tex_loc = glGetUniformLocation(server->shader_program, "texture_sampler_uniform");
    if (tex_loc != -1) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(tex_attribs.target, tex_attribs.tex);
        glTexParameteri(tex_attribs.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(tex_attribs.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(tex_attribs.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(tex_attribs.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glUniform1i(tex_loc, 0);
    }

    // Set time uniform like second version
    struct timespec now_shader_time_iter_draw;
    clock_gettime(CLOCK_MONOTONIC, &now_shader_time_iter_draw);
    float time_value_iter_draw = now_shader_time_iter_draw.tv_sec + now_shader_time_iter_draw.tv_nsec / 1e9f;
    GLint time_loc_iter_draw = glGetUniformLocation(server->shader_program, "time");
    if (time_loc_iter_draw != -1) glUniform1f(time_loc_iter_draw, time_value_iter_draw);

    // Set resolution uniform like second version
    float resolution_iter_draw[2] = {(float)output->width, (float)output->height};
    GLint resolution_loc_iter_draw = glGetUniformLocation(server->shader_program, "iResolution");
    if (resolution_loc_iter_draw != -1) glUniform2fv(resolution_loc_iter_draw, 1, resolution_iter_draw);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    wlr_log(WLR_DEBUG, "[SCENE_ITERATOR:%s] Drew main window for '%s'", output_name_log, tl_title_for_log);

    // --- Preview Rendering (kept from first version) ---
    if (toplevel && server->top_panel_node && server->top_panel_node->enabled &&
        texture) { // Ensure texture is still valid

        bool should_preview_this_toplevel = false;
        // Example: Preview only the toplevel whose title is "simple-egl"
        if (toplevel->xdg_toplevel && toplevel->xdg_toplevel->title &&
            strcmp(toplevel->xdg_toplevel->title, "simple-egl") == 0) {
            should_preview_this_toplevel = true;
        }

        if (should_preview_this_toplevel) {
            wlr_log(WLR_INFO, "[SCENE_ITERATOR_PREVIEW:%s] Rendering preview for '%s' onto panel.", output_name_log, tl_title_for_log);
            struct wlr_scene_rect *panel_srect = wlr_scene_rect_from_node(server->top_panel_node);

            if (panel_srect) {
                int panel_screen_x = server->top_panel_node->x;
                int panel_screen_y = server->top_panel_node->y;

                float preview_x_offset_in_panel = 20.0f;
                float preview_y_padding_in_panel = 5.0f;
                float preview_height_pixels = (float)panel_srect->height - 2.0f * preview_y_padding_in_panel;
                if (preview_height_pixels < 1.0f) preview_height_pixels = 1.0f;

                float aspect_ratio = (texture->width > 0 && texture->height > 0) ? (float)texture->width / (float)texture->height : 1.0f;
                float preview_width_pixels = preview_height_pixels * aspect_ratio;
                if (preview_width_pixels < 1.0f) preview_width_pixels = 1.0f;

                if (preview_x_offset_in_panel + preview_width_pixels > panel_srect->width - 5.0f) {
                    preview_width_pixels = panel_srect->width - 5.0f - preview_x_offset_in_panel;
                    if (preview_width_pixels < 1.0f) preview_width_pixels = 0;
                }

                struct wlr_box preview_box_on_screen = {
                    .x = (int)round((double)panel_screen_x + preview_x_offset_in_panel),
                    .y = (int)round((double)panel_screen_y + preview_y_padding_in_panel),
                    .width = (int)round(preview_width_pixels),
                    .height = (int)round(preview_height_pixels),
                };

                if (preview_box_on_screen.width > 0 && preview_box_on_screen.height > 0) {
                    float preview_mvp[9];
                    float p_box_scale_x = (float)preview_box_on_screen.width * (2.0f / output->width);
                    float p_box_scale_y = (float)preview_box_on_screen.height * (-2.0f / output->height);
                    float p_box_translate_x = ((float)preview_box_on_screen.x / output->width) * 2.0f - 1.0f;
                    float p_box_translate_y = ((float)preview_box_on_screen.y / output->height) * -2.0f + 1.0f;
                    float p_model_view[9] = { p_box_scale_x, 0.0f, 0.0f, 0.0f, p_box_scale_y, 0.0f, p_box_translate_x, p_box_translate_y, 1.0f };

                    memcpy(preview_mvp, p_model_view, sizeof(p_model_view));
                    
                    // Apply output transform to the preview MVP
                    if (output->transform != WL_OUTPUT_TRANSFORM_NORMAL) {
                        float temp_preview_mvp[9];
                        memcpy(temp_preview_mvp, preview_mvp, sizeof(preview_mvp));
                        float output_transform_matrix[9];
                        wlr_matrix_identity(output_transform_matrix);
                        wlr_matrix_transform(output_transform_matrix, output->transform);
                        wlr_matrix_multiply(preview_mvp, output_transform_matrix, temp_preview_mvp);
                    }

                    // Re-set uniforms for the preview draw call
                    GLint preview_mvp_loc = glGetUniformLocation(server->shader_program, "mvp");
                    if (preview_mvp_loc != -1) glUniformMatrix3fv(preview_mvp_loc, 1, GL_FALSE, preview_mvp);
                    
                    GLint preview_tex_loc = glGetUniformLocation(server->shader_program, "texture_sampler_uniform");
                    if (preview_tex_loc != -1) {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(tex_attribs.target, tex_attribs.tex);
                        glUniform1i(preview_tex_loc, 0);
                    }

                    // Set iResolution specifically for the preview's dimensions
                    GLint preview_res_loc = glGetUniformLocation(server->shader_program, "iResolution");
                    if (preview_res_loc != -1) {
                        float preview_res_vec[2] = {(float)preview_box_on_screen.width, (float)preview_box_on_screen.height};
                        glUniform2fv(preview_res_loc, 1, preview_res_vec);
                    }

                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    wlr_log(WLR_DEBUG, "[SCENE_ITERATOR_PREVIEW:%s] Drew preview for '%s' at %d,%d %dx%d",
                           output_name_log, tl_title_for_log,
                           preview_box_on_screen.x, preview_box_on_screen.y,
                           preview_box_on_screen.width, preview_box_on_screen.height);

                    // Restore iResolution for subsequent main window rendering
                    if (preview_res_loc != -1) {
                         float main_res_vec[2] = {(float)output->width, (float)output->height};
                         glUniform2fv(preview_res_loc, 1, main_res_vec);
                    }
                }
            } else {
                wlr_log(WLR_ERROR, "[SCENE_ITERATOR_PREVIEW:%s] Panel node is not a wlr_scene_rect, cannot get dimensions.", output_name_log);
            }
        }
    }

    // Cleanup like second version
    if (tex_loc != -1) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(tex_attribs.target, 0);
    }
    wlr_texture_destroy(texture);
}

#include <pthread.h>
#include <pixman.h>
#include <string.h>

static pthread_mutex_t rdp_transmit_mutex = PTHREAD_MUTEX_INITIALIZER;


struct scene_diagnostics_data {
    struct wlr_output *output;
    int buffer_count;
};



#include <pthread.h>
#include <pixman.h>
#include <string.h> // For memcpy if you use it, not directly in this function now



// STEP 2: Add this debug function to check scene state during rendering
void debug_scene_rendering(struct wlr_scene *scene, struct wlr_output *output) {
    wlr_log(WLR_INFO, "[RENDER_DEBUG:%s] === SCENE RENDERING DEBUG ===", output->name);
    
    int total_nodes = 0, enabled_nodes = 0, rect_nodes = 0;
    struct wlr_scene_node *node;
    
    wl_list_for_each(node, &scene->tree.children, link) {
        total_nodes++;
        if (node->enabled) enabled_nodes++;
        
        const char *type_str = "UNKNOWN";
        switch (node->type) {
            case WLR_SCENE_NODE_TREE: type_str = "TREE"; break;
            case WLR_SCENE_NODE_RECT: 
                type_str = "RECT"; 
                rect_nodes++;
                // Detailed rect info
                struct wlr_scene_rect *rect = wlr_scene_rect_from_node(node);
                wlr_log(WLR_INFO, "[RENDER_DEBUG:%s] RECT found: %dx%d at (%d,%d), enabled=%d, color=(%.2f,%.2f,%.2f,%.2f)",
                        output->name, rect->width, rect->height, node->x, node->y, node->enabled,
                        rect->color[0], rect->color[1], rect->color[2], rect->color[3]);
                break;
            case WLR_SCENE_NODE_BUFFER: type_str = "BUFFER"; break;
        }
        
        wlr_log(WLR_INFO, "[RENDER_DEBUG:%s] Node: type=%s, pos=(%d,%d), enabled=%d", 
                output->name, type_str, node->x, node->y, node->enabled);
    }
    
    wlr_log(WLR_INFO, "[RENDER_DEBUG:%s] Summary: total=%d, enabled=%d, rects=%d", 
            output->name, total_nodes, enabled_nodes, rect_nodes);
    wlr_log(WLR_INFO, "[RENDER_DEBUG:%s] === SCENE RENDERING DEBUG END ===", output->name);
}

// STEP 3: Check if you're bypassing scene rendering entirely
// Add this to your output_frame function RIGHT AFTER the scene_output check:
void check_scene_bypass_issue(struct wlr_scene_output *scene_output, struct wlr_output *output) {
    wlr_log(WLR_INFO, "[BYPASS_CHECK:%s] Scene output: %p", output->name, (void*)scene_output);
    
    // Check if you're using wlr_scene_output_commit properly
    // This is CRITICAL - if you're not using this, scene rects won't render
    
    // In your output_frame, you should be doing:
    // bool scene_commit_result = wlr_scene_output_commit(scene_output, NULL);
    // wlr_log(WLR_INFO, "[BYPASS_CHECK:%s] Scene commit result: %d", output->name, scene_commit_result);
    
    wlr_log(WLR_INFO, "[BYPASS_CHECK:%s] If you see this but no rectangles, you might be bypassing scene rendering", output->name);
}

static void render_rect_node(struct wlr_scene_node *node, void *user_data) {
     struct render_data *rdata = user_data; // rdata still useful for output, server
    struct wlr_output *output = rdata->output;
    struct tinywl_server *server = rdata->server;

    if (node->type != WLR_SCENE_NODE_RECT || !node->enabled) {
        return;
    }
    struct wlr_scene_rect *scene_rect = wlr_scene_rect_from_node(node);

    int sx = node->x; // Assuming these are layout coordinates for the current output
    int sy = node->y;

    wlr_log(WLR_INFO, "[RECT_NODE_RENDER:%s] Rendering RECT %dx%d at layout_pos(%d,%d) with DEDICATED RECT SHADER. Color (%.2f, %.2f, %.2f, %.2f)",
            output->name, scene_rect->width, scene_rect->height, sx, sy,
            scene_rect->color[0], scene_rect->color[1], scene_rect->color[2], scene_rect->color[3]);

    if (output->width == 0 || output->height == 0) return;
    if (server->rect_shader_program == 0) {
        wlr_log(WLR_ERROR, "Rect shader program is 0, cannot render rect.");
        return;
    }

    // Activate the rectangle shader
    glUseProgram(server->rect_shader_program);
    // VAO (server->quad_vao) is assumed to be bound by output_frame

    float mvp[9];
    wlr_matrix_identity(mvp);

    float box_scale_x = (float)scene_rect->width * (2.0f / output->width);
    float box_scale_y = (float)scene_rect->height * (-2.0f / output->height);
    float box_translate_x = ((float)sx / output->width) * 2.0f - 1.0f;
    float box_translate_y = ((float)sy / output->height) * -2.0f + 1.0f;

    float base_mvp[9] = {
        box_scale_x, 0.0f, 0.0f,
        0.0f, box_scale_y, 0.0f,
        box_translate_x, box_translate_y, 1.0f
    };

    if (output->transform != WL_OUTPUT_TRANSFORM_NORMAL) {
        float output_transform_matrix[9];
        wlr_matrix_identity(output_transform_matrix);
        wlr_matrix_transform(output_transform_matrix, output->transform);
        wlr_matrix_multiply(mvp, output_transform_matrix, base_mvp);
    } else {
        memcpy(mvp, base_mvp, sizeof(base_mvp));
    }

    // Set uniforms for the RECT shader
    if (server->rect_shader_mvp_loc != -1) {
        glUniformMatrix3fv(server->rect_shader_mvp_loc, 1, GL_FALSE, mvp);
    } else {
        wlr_log(WLR_ERROR, "Rect shader MVP uniform location is -1!");
    }

    if (server->rect_shader_color_loc != -1) {
        // scene_rect->color is float[4] {r,g,b,a}
        glUniform4fv(server->rect_shader_color_loc, 1, scene_rect->color);
    } else {
        wlr_log(WLR_ERROR, "Rect shader color uniform location is -1!");
    }

     if (server->rect_shader_time_loc != -1) {
        struct timespec current_render_time_spec;
        clock_gettime(CLOCK_MONOTONIC, &current_render_time_spec);
        float time_value_sec = (float)current_render_time_spec.tv_sec +
                               (float)current_render_time_spec.tv_nsec / 1e9f;

        // Optional: Log the time value being set, less frequently
        static float last_logged_rect_time_val = 0.0f; // Static to persist across calls
        if (fabs(time_value_sec - last_logged_rect_time_val) > 0.5f || last_logged_rect_time_val == 0.0f) {
            wlr_log(WLR_INFO, "[render_rect_node] Setting 'time' (loc %d) to %.3f for rect on output %s",
                server->rect_shader_time_loc, time_value_sec, output->name);
            last_logged_rect_time_val = time_value_sec;
        }

        glUniform1f(server->rect_shader_time_loc, time_value_sec); // *** THIS IS THE ACTUAL SETTING OF THE UNIFORM ***
        GLenum err_after_time_uniform = glGetError();
        if (err_after_time_uniform != GL_NO_ERROR) {
            wlr_log(WLR_ERROR, "[render_rect_node] GL error AFTER glUniform1f(time): 0x%x", err_after_time_uniform);
        }
    } else {
        // This log is correct: if rect_shader_time_loc is -1, we can't set it.
        // This indicates an issue in create_rect_shader_program or the shader source.
        wlr_log(WLR_ERROR, "[render_rect_node] Rect shader 'time' uniform location is -1! Cannot set time.");
    }

    if (server->rect_shader_resolution_loc != -1) {
        float resolution_vec[2] = {(float)output->width, (float)output->height};
        glUniform2fv(server->rect_shader_resolution_loc, 1, resolution_vec);
    } else {
        // Log error if you haven't already in create_rect_shader_program
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Do NOT call glUseProgram(0) here; output_frame will switch or unbind later.
}

static void render_panel_node(struct wlr_scene_node *node, void *user_data) {
    struct render_data *rdata = user_data;
    struct wlr_output *output = rdata->output;
    struct tinywl_server *server = rdata->server;

    if (node->type != WLR_SCENE_NODE_RECT || !node->enabled) {
        return;
    }
    struct wlr_scene_rect *panel_srect = wlr_scene_rect_from_node(node);

    if (output->width == 0 || output->height == 0) {
        static struct wlr_output *last_logged_zero_dim_output = NULL;
        if (last_logged_zero_dim_output != output) {
            wlr_log(WLR_ERROR, "[render_panel_node] Output %s has zero width/height.", output->name);
            last_logged_zero_dim_output = output;
        }
        return;
    }
    if (server->panel_shader_program == 0) {
        static bool panel_shader_is_zero_logged = false;
        if (!panel_shader_is_zero_logged) {
            wlr_log(WLR_ERROR, "[render_panel_node] Panel shader program is 0.");
            panel_shader_is_zero_logged = true;
        }
        return;
    }

    glUseProgram(server->panel_shader_program);
    GLenum err_after_use_program = glGetError();
    if (err_after_use_program != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "[render_panel_node] GL error AFTER glUseProgram(panel_shader: %u): 0x%x",
                server->panel_shader_program, err_after_use_program);
        return;
    }

    // --- Standard Panel Uniforms (MVP, time, iResolution for panel's own shader effects) ---
    float mvp_for_panel_geometry[9];
    int sx = node->x;
    int sy = node->y;
    float box_scale_x = (float)panel_srect->width * (2.0f / output->width);
    float box_scale_y = (float)panel_srect->height * (-2.0f / output->height);
    float box_translate_x = ((float)sx / output->width) * 2.0f - 1.0f;
    float box_translate_y = ((float)sy / output->height) * -2.0f + 1.0f;

    float base_mvp[9] = {
        box_scale_x, 0.0f, 0.0f,
        0.0f, box_scale_y, 0.0f,
        box_translate_x, box_translate_y, 1.0f
    };
    if (output->transform != WL_OUTPUT_TRANSFORM_NORMAL) {
        float output_transform_matrix[9];
        wlr_matrix_identity(output_transform_matrix);
        wlr_matrix_transform(output_transform_matrix, output->transform);
        wlr_matrix_multiply(mvp_for_panel_geometry, output_transform_matrix, base_mvp);
    } else {
        memcpy(mvp_for_panel_geometry, base_mvp, sizeof(base_mvp));
    }

    if (server->panel_shader_mvp_loc != -1) {
        glUniformMatrix3fv(server->panel_shader_mvp_loc, 1, GL_FALSE, mvp_for_panel_geometry);
    } else {
         wlr_log(WLR_ERROR, "[render_panel_node] Panel shader 'mvp' uniform location is -1.");
    }

    if (server->panel_shader_time_loc != -1) {
        glUniform1f(server->panel_shader_time_loc, get_monotonic_time_seconds_as_float());
    } else {
        static bool panel_time_loc_missing_logged = false;
        if (!panel_time_loc_missing_logged) {
            wlr_log(WLR_INFO, "[render_panel_node] Panel shader 'time' uniform location is -1 (or shader doesn't use it).");
            panel_time_loc_missing_logged = true;
        }
    }

    if (server->panel_shader_resolution_loc != -1) {
        float panel_resolution_vec[2] = {(float)output->width, (float)output->height};
        glUniform2fv(server->panel_shader_resolution_loc, 1, panel_resolution_vec);
    } else {
        static bool panel_res_loc_missing_logged = false;
        if (!panel_res_loc_missing_logged) {
            wlr_log(WLR_INFO, "[render_panel_node] Panel shader 'iResolution' uniform location is -1 (or shader doesn't use it).");
            panel_res_loc_missing_logged = true;
        }
    }
    // --- End Standard Panel Uniforms ---

    // --- Preview-Specific Uniform Setup (using cached_texture strategy) ---
    struct tinywl_toplevel *preview_toplevel_candidate = NULL;
    struct wlr_texture *texture_for_preview = NULL;
    struct wlr_gles2_texture_attribs preview_gl_tex_attribs = {0}; // Initialize
    bool is_preview_active_flag = false; // Default to false

    float calculated_norm_x = 0.f, calculated_norm_y = 0.f, calculated_norm_w = 0.f, calculated_norm_h = 0.f;
    float calculated_preview_tex_transform_matrix[9];
    wlr_matrix_identity(calculated_preview_tex_transform_matrix); // Initialize to identity

    wlr_log(WLR_DEBUG, "[PNODE_PREVIEW] Finding preview toplevel to use its cached_texture. Total toplevels: %d", wl_list_length(&server->toplevels));

    if (!wl_list_empty(&server->toplevels)) {
        struct tinywl_toplevel *iter_tl;
        wl_list_for_each_reverse(iter_tl, &server->toplevels, link) {
            const char *title = (iter_tl->xdg_toplevel && iter_tl->xdg_toplevel->title) ? iter_tl->xdg_toplevel->title : "N/A";
            wlr_log(WLR_DEBUG, "[PNODE_PREVIEW_ITER] Checking TL: %p (Title: %s) for cached_texture: %p",
                    (void*)iter_tl, title, (void*)iter_tl->cached_texture);

            if (iter_tl->xdg_toplevel &&
                iter_tl->scene_tree && iter_tl->scene_tree->node.enabled &&
                iter_tl->scale > 0.01f && !iter_tl->pending_destroy &&
                iter_tl->cached_texture != NULL) {
                
                wlr_log(WLR_INFO, "[PNODE_PREVIEW_ITER] TL '%s': Using its cached_texture %p (Size: %dx%d).",
                        title, (void*)iter_tl->cached_texture,
                        iter_tl->cached_texture->width, iter_tl->cached_texture->height);
                preview_toplevel_candidate = iter_tl;
                texture_for_preview = iter_tl->cached_texture;
                break; 
            }
        }
    }

    if (preview_toplevel_candidate && texture_for_preview) {
        wlr_log(WLR_INFO, "[PNODE_PREVIEW] Preparing to use TL '%s's cached_texture %p for preview.",
            preview_toplevel_candidate->xdg_toplevel->title, (void*)texture_for_preview);

        is_preview_active_flag = true; 
        wlr_gles2_texture_get_attribs(texture_for_preview, &preview_gl_tex_attribs);

        glActiveTexture(GL_TEXTURE1); 
        glBindTexture(preview_gl_tex_attribs.target, preview_gl_tex_attribs.tex);
        glTexParameteri(preview_gl_tex_attribs.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(preview_gl_tex_attribs.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(preview_gl_tex_attribs.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(preview_gl_tex_attribs.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GLenum gl_err_texbind = glGetError();
        if(gl_err_texbind != GL_NO_ERROR) {
            wlr_log(WLR_ERROR, "[PNODE_PREVIEW_GL_ERROR] Error after binding/setting params for preview texture unit 1: 0x%x", gl_err_texbind);
            is_preview_active_flag = false;
        }

        if (is_preview_active_flag && server->panel_shader_preview_tex_loc != -1) {
            glUniform1i(server->panel_shader_preview_tex_loc, 1); 
             wlr_log(WLR_INFO, "[PNODE_PREVIEW] Set u_previewTexture (loc %d) to unit 1. GL Tex ID: %u",
                    server->panel_shader_preview_tex_loc, preview_gl_tex_attribs.tex);
        } else if(is_preview_active_flag) { 
             wlr_log(WLR_ERROR, "[PNODE_PREVIEW] u_previewTexture location is -1! Cannot set sampler.");
             is_preview_active_flag = false;
        }

        if (is_preview_active_flag) { 
            float preview_pixel_height = (float)panel_srect->height - 10.0f; 
            if (preview_pixel_height < 1.0f) preview_pixel_height = 1.0f;
            float aspect = (texture_for_preview->width > 0 && texture_for_preview->height > 0) ?
                           (float)texture_for_preview->width / (float)texture_for_preview->height : 1.0f;
            float preview_pixel_width = preview_pixel_height * aspect;
            if (preview_pixel_width < 1.0f) preview_pixel_width = 1.0f;
            float preview_pixel_x_start = 20.0f;

            if (panel_srect->width > 0 && panel_srect->height > 0) {
                calculated_norm_x = preview_pixel_x_start / (float)panel_srect->width;
                calculated_norm_y = 5.0f / (float)panel_srect->height; 
                calculated_norm_w = preview_pixel_width / (float)panel_srect->width;
                calculated_norm_h = preview_pixel_height / (float)panel_srect->height;
                calculated_norm_w = fmaxf(0.0f, fminf(calculated_norm_w, 1.0f - calculated_norm_x)); 
                calculated_norm_h = fmaxf(0.0f, fminf(calculated_norm_h, 1.0f - calculated_norm_y)); 
                
                if (calculated_norm_w > 0.001f && calculated_norm_h > 0.001f && server->panel_shader_preview_rect_loc != -1) {
                     // Value setting will happen below, after all is_preview_active_flag checks
                } else {
                    wlr_log(WLR_ERROR, "[PNODE_PREVIEW] Cannot set u_previewRect or rect too small. norm_w=%.3f, norm_h=%.3f, loc=%d. Panel Dims: %dx%d",
                            calculated_norm_w, calculated_norm_h, server->panel_shader_preview_rect_loc, panel_srect->width, panel_srect->height);
                    is_preview_active_flag = false; 
                }
            } else {
                 wlr_log(WLR_ERROR, "[PNODE_PREVIEW] Panel has zero dimensions (%dx%d), cannot calculate u_previewRect.", panel_srect->width, panel_srect->height);
                 is_preview_active_flag = false;
            }
        }
            
        if (is_preview_active_flag) { 
            calculated_preview_tex_transform_matrix[4] = -1.0f; 
            calculated_preview_tex_transform_matrix[7] =  1.0f; 
            if (server->panel_shader_preview_tex_transform_loc == -1) {
                 wlr_log(WLR_ERROR, "[PNODE_PREVIEW] u_previewTexTransform location is -1! Cannot set tex transform.");
                 // Potentially set is_preview_active_flag = false; if this transform is critical
            }
        }
    } else { 
        wlr_log(WLR_DEBUG, "[PNODE_PREVIEW] No suitable toplevel with a cached_texture found, or texture_for_preview is NULL.");
        is_preview_active_flag = false;
    }

    // Set u_isPreviewActive now that all conditions have been checked
    if (server->panel_shader_is_preview_active_loc != -1) {
        glUniform1i(server->panel_shader_is_preview_active_loc, is_preview_active_flag ? 1 : 0);
        wlr_log(WLR_INFO, "[PNODE_PREVIEW] FINAL u_isPreviewActive (loc %d) set to: %s",
                server->panel_shader_is_preview_active_loc, is_preview_active_flag ? "TRUE (1)" : "FALSE (0)");
    } else {
        wlr_log(WLR_ERROR, "[PNODE_PREVIEW] u_isPreviewActive uniform location is -1!");
    }

    // Set the other preview uniforms ONLY IF preview is active
    if (is_preview_active_flag) {
        if (server->panel_shader_preview_rect_loc != -1) {
            float rect_data[4] = {calculated_norm_x, calculated_norm_y, calculated_norm_w, calculated_norm_h};
            glUniform4fv(server->panel_shader_preview_rect_loc, 1, rect_data);
            wlr_log(WLR_INFO, "[PNODE_PREVIEW] Actually set u_previewRect (loc %d) to: (%.3f, %.3f, %.3f, %.3f)",
                    server->panel_shader_preview_rect_loc, calculated_norm_x, calculated_norm_y, calculated_norm_w, calculated_norm_h);
        }
        if (server->panel_shader_preview_tex_transform_loc != -1) {
            glUniformMatrix3fv(server->panel_shader_preview_tex_transform_loc, 1, GL_FALSE, calculated_preview_tex_transform_matrix);
            wlr_log(WLR_INFO, "[PNODE_PREVIEW] Actually set u_previewTexTransform (loc %d).", server->panel_shader_preview_tex_transform_loc);
        }
    }


    GLenum err_before_draw = glGetError();
    if (err_before_draw != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "[PNODE_PRE_DRAW_GL_ERROR] GL error BEFORE panel draw: 0x%x", err_before_draw);
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    GLenum err_after_draw = glGetError();
    if (err_after_draw != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "[PNODE_POST_DRAW_GL_ERROR] GL error AFTER panel draw: 0x%x", err_after_draw);
    }

    if (is_preview_active_flag && texture_for_preview) { 
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(preview_gl_tex_attribs.target, 0); 
        glActiveTexture(GL_TEXTURE0); 
        // DO NOT destroy `texture_for_preview` here (it's cached)
    }
}

static void output_frame(struct wl_listener *listener, void *data) {
    struct tinywl_output *output_wrapper = wl_container_of(listener, output_wrapper, frame);
    struct wlr_output *wlr_output = output_wrapper->wlr_output;
    struct tinywl_server *server = output_wrapper->server;
    struct wlr_scene *scene = server->scene;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    if (!wlr_output->enabled || !wlr_output->renderer || !wlr_output->allocator) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] Output not ready.", wlr_output->name);
        struct wlr_scene_output *scene_output_early_exit = wlr_scene_get_scene_output(scene, wlr_output);
        if (scene_output_early_exit) wlr_scene_output_send_frame_done(scene_output_early_exit, &now);
        return;
    }

    struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(scene, wlr_output);
    if (!scene_output) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] No scene_output.", wlr_output->name);
        return;
    }

    debug_scene_rendering(scene, wlr_output);

    struct wlr_output_state state;
    wlr_output_state_init(&state);

    struct wlr_scene_output_state_options opts = {0};
    bool has_damage = wlr_scene_output_build_state(scene_output, &state, &opts);

    if (!has_damage && !(state.committed & WLR_OUTPUT_STATE_BUFFER)) {
        wlr_log(WLR_DEBUG, "[OUTPUT_FRAME:%s] No damage or buffer, skipping render.", wlr_output->name);
        wlr_output_state_finish(&state);
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

    pixman_region32_t full_damage_region;
    pixman_region32_init_rect(&full_damage_region, 0, 0, wlr_output->width, wlr_output->height);
    wlr_output_state_set_damage(&state, &full_damage_region);
    pixman_region32_fini(&full_damage_region);

    struct wlr_render_pass *pass = wlr_output_begin_render_pass(wlr_output, &state, NULL);
    if (!pass) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] wlr_output_begin_render_pass failed.", wlr_output->name);
        wlr_output_state_finish(&state);
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, wlr_output->width, wlr_output->height);
    glScissor(0, 0, wlr_output->width, wlr_output->height);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    // Important: Choose the correct blend func. If your shaders output PREMULTIPLIED ALPHA:
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    // If your shaders output STRAIGHT (NON-PREMULTIPLIED) ALPHA:
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    if (server->quad_vao == 0) {
         wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] Quad VAO is zero! Skipping custom draw.", wlr_output->name);
    } else {
        glBindVertexArray(server->quad_vao);

        struct render_data rdata = {
            .renderer = server->renderer,
            .server = server,
            .output = wlr_output,
            .pass = pass // Though pass is not directly used by custom GL calls in iterators
        };

       

        // --- Render Generic Rectangles (middle layer) ---
        wlr_log(WLR_DEBUG, "[OUTPUT_FRAME:%s] Iterating scene children for custom rects (non-panel).", wlr_output->name);
        struct wlr_scene_node *child_node;
        wl_list_for_each(child_node, &scene->tree.children, link) {
            if (!child_node->enabled) {
                continue;
            }

            // Skip panel nodes in this pass - we'll render them later
            if (child_node == server->top_panel_node) {
                continue;
            }

            if (child_node->type == WLR_SCENE_NODE_RECT) {
                if (server->rect_shader_program != 0) {
                    wlr_log(WLR_DEBUG, "[OUTPUT_FRAME] Rendering generic RECT node %p with rect_shader (Melt, ID: %u).", child_node, server->rect_shader_program);
                    render_rect_node(child_node, &rdata);
                } else {
                    wlr_log(WLR_ERROR, "[OUTPUT_FRAME] server->rect_shader_program is 0, cannot render generic rect.");
                }
            } else if (child_node->type == WLR_SCENE_NODE_TREE && child_node->enabled) {
                struct wlr_scene_node *sub_child_node;
                struct wlr_scene_tree *sub_tree = wlr_scene_tree_from_node(child_node);
                wl_list_for_each(sub_child_node, &sub_tree->children, link) {
                    if (!sub_child_node->enabled) continue;
                    
                    // Skip panel nodes in subtrees too
                    if (sub_child_node == server->top_panel_node) {
                        continue;
                    }
                    
                    if (sub_child_node->type == WLR_SCENE_NODE_RECT) {
                        if (server->rect_shader_program != 0) {
                            wlr_log(WLR_DEBUG, "[OUTPUT_FRAME] Rendering generic RECT node %p (in subtree) with rect_shader (Melt, ID: %u).", sub_child_node, server->rect_shader_program);
                            render_rect_node(sub_child_node, &rdata);
                        } else {
                            wlr_log(WLR_ERROR, "[OUTPUT_FRAME] server->rect_shader_program is 0 for generic rect in subtree!");
                        }
                    }
                }
            }
        }

      

        // --- Render Panel Nodes LAST (foreground layer) ---
        wlr_log(WLR_DEBUG, "[OUTPUT_FRAME:%s] Rendering panel nodes on top.", wlr_output->name);
        wl_list_for_each(child_node, &scene->tree.children, link) {
            if (!child_node->enabled) {
                continue;
            }

            if (child_node == server->top_panel_node) {
                if (server->panel_shader_program != 0) {
                    wlr_log(WLR_DEBUG, "[OUTPUT_FRAME] Rendering TOP PANEL node %p with panel_shader (ID: %u) ON TOP.", child_node, server->panel_shader_program);
                    render_panel_node(child_node, &rdata);
                } else {
                    wlr_log(WLR_ERROR, "[OUTPUT_FRAME] server->panel_shader_program is 0, cannot render top panel.");
                }
            } else if (child_node->type == WLR_SCENE_NODE_TREE && child_node->enabled) {
                struct wlr_scene_node *sub_child_node;
                struct wlr_scene_tree *sub_tree = wlr_scene_tree_from_node(child_node);
                wl_list_for_each(sub_child_node, &sub_tree->children, link) {
                    if (!sub_child_node->enabled) continue;
                    if (sub_child_node == server->top_panel_node) {
                         if (server->panel_shader_program != 0) {
                            wlr_log(WLR_DEBUG, "[OUTPUT_FRAME] Rendering TOP PANEL node %p (in subtree) with panel_shader (ID: %u) ON TOP.", sub_child_node, server->panel_shader_program);
                            render_panel_node(sub_child_node, &rdata);
                        } else {
                             wlr_log(WLR_ERROR, "[OUTPUT_FRAME] server->panel_shader_program is 0 for top_panel_node in subtree!");
                        }
                    }
                }
            }
        }

           // --- Render Client Window Buffers FIRST (background layer) ---
        if (server->shader_program != 0) { // This is the flame shader for client windows
            glUseProgram(server->shader_program); // Ensure flame shader is active

            wlr_log(WLR_DEBUG, "[OUTPUT_FRAME:%s] Iterating for BUFFERS (client windows, using flame shader ID %u)",
                    wlr_output->name, server->shader_program);
            wlr_scene_node_for_each_buffer(&scene->tree.node, scene_buffer_iterator, &rdata);
        } else {
            wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] Flame shader program (server->shader_program) is 0, skipping buffer rendering.", wlr_output->name);
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }

    glDisable(GL_SCISSOR_TEST);

    if (!wlr_render_pass_submit(pass)) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] wlr_render_pass_submit failed.", wlr_output->name);
    }

    GLenum gl_err = glGetError();
    if (gl_err != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] GL error after render pass submit: 0x%x", wlr_output->name, gl_err);
    }

    if (state.committed & WLR_OUTPUT_STATE_BUFFER && state.buffer &&
        state.buffer->width > 0 && state.buffer->height > 0) {
        pthread_mutex_lock(&rdp_transmit_mutex);
        struct wlr_buffer *locked_buffer = wlr_buffer_lock(state.buffer);
        if (locked_buffer) {
            rdp_transmit_surface(locked_buffer);
            wlr_buffer_unlock(locked_buffer);
        } else {
            wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] Failed to lock buffer for RDP transmission.", wlr_output->name);
        }
        pthread_mutex_unlock(&rdp_transmit_mutex);
    } else {
        wlr_log(WLR_DEBUG, "[OUTPUT_FRAME:%s] No buffer to transmit or buffer invalid.", wlr_output->name);
    }

    wlr_output_state_finish(&state);
    wlr_scene_output_send_frame_done(scene_output, &now);

    // // TEMPORARY: Force continuous rendering for animation debugging
    // if (wlr_output && wlr_output->enabled &&
    //     ( (server->panel_shader_program != 0 && server->panel_shader_time_loc != -1) ||
    //       (server->rect_shader_program != 0 && server->rect_shader_time_loc != -1) ||
    //       (server->shader_program != 0 && server->flame_shader_time_loc != -1) ) ) {
    //      wlr_log(WLR_DEBUG, "[OUTPUT_FRAME %s] Scheduling next frame due to active shader animation.", wlr_output->name);
    //      wlr_output_schedule_frame(wlr_output);
    // }
}
static void output_request_state(struct wl_listener *listener, void *data) {
    struct tinywl_output *output = wl_container_of(listener, output, request_state);
    const struct wlr_output_event_request_state *event = data;
    wlr_output_commit_state(output->wlr_output, event->state);
}

static void output_destroy(struct wl_listener *listener, void *data) {
    struct tinywl_output *output = wl_container_of(listener, output, destroy);
    wl_list_remove(&output->frame.link);
    wl_list_remove(&output->request_state.link);
    wl_list_remove(&output->destroy.link);
    wl_list_remove(&output->link);
    free(output);
}



/* Updated server_new_output without damage */

static void server_new_output(struct wl_listener *listener, void *data) {
    struct tinywl_server *server =
        wl_container_of(listener, server, new_output);
    struct wlr_output *wlr_output = data;

    struct tinywl_output *output_wrapper_iter;
    wl_list_for_each(output_wrapper_iter, &server->outputs, link) {
        if (output_wrapper_iter->wlr_output == wlr_output) {
            wlr_log(WLR_INFO, "server_new_output: Output %s (ptr %p) already processed, skipping.",
                    wlr_output->name ? wlr_output->name : "(null)", (void*)wlr_output);
            return;
        }
    }

    wlr_output_init_render(wlr_output, server->allocator, server->renderer);

    struct wlr_output_state state;
    wlr_output_state_init(&state);
    wlr_output_state_set_enabled(&state, true);

    struct wlr_output_mode *mode = wlr_output_preferred_mode(wlr_output);
    if (mode != NULL) {
        wlr_output_state_set_mode(&state, mode);
    }

    wlr_log(WLR_ERROR, "SERVER_NEW_OUTPUT: About to set transform. WL_OUTPUT_TRANSFORM_FLIPPED_180 is defined as: %d.", WL_OUTPUT_TRANSFORM_FLIPPED_180);
  //  wlr_output_state_set_transform(&state, WL_OUTPUT_TRANSFORM_FLIPPED_180);

    if (!wlr_output_commit_state(wlr_output, &state)) {
        wlr_log(WLR_ERROR, "Failed to commit output state for %s", wlr_output->name);
        wlr_output_state_finish(&state);
        return;
    }
    wlr_output_state_finish(&state);

    wlr_log(WLR_ERROR, "SERVER_NEW_OUTPUT: After commit, actual wlr_output->transform is: %d.", wlr_output->transform);
    wlr_log(WLR_ERROR, "SERVER_NEW_OUTPUT: WL_OUTPUT_TRANSFORM_NORMAL is %d", WL_OUTPUT_TRANSFORM_NORMAL);
    wlr_log(WLR_ERROR, "SERVER_NEW_OUTPUT: WL_OUTPUT_TRANSFORM_90 is %d", WL_OUTPUT_TRANSFORM_90);
    wlr_log(WLR_ERROR, "SERVER_NEW_OUTPUT: WL_OUTPUT_TRANSFORM_180 is %d", WL_OUTPUT_TRANSFORM_180);
    wlr_log(WLR_ERROR, "SERVER_NEW_OUTPUT: WL_OUTPUT_TRANSFORM_270 is %d", WL_OUTPUT_TRANSFORM_270);
    wlr_log(WLR_ERROR, "SERVER_NEW_OUTPUT: WL_OUTPUT_TRANSFORM_FLIPPED is %d", WL_OUTPUT_TRANSFORM_FLIPPED);
    wlr_log(WLR_ERROR, "SERVER_NEW_OUTPUT: WL_OUTPUT_TRANSFORM_FLIPPED_90 is %d", WL_OUTPUT_TRANSFORM_FLIPPED_90);
    wlr_log(WLR_ERROR, "SERVER_NEW_OUTPUT: WL_OUTPUT_TRANSFORM_FLIPPED_180 is %d (This is the one we used for set_transform)", WL_OUTPUT_TRANSFORM_FLIPPED_180);
    wlr_log(WLR_ERROR, "SERVER_NEW_OUTPUT: WL_OUTPUT_TRANSFORM_FLIPPED_270 is %d", WL_OUTPUT_TRANSFORM_FLIPPED_270);


    struct tinywl_output *output = calloc(1, sizeof(*output));
    if (!output) {
        wlr_log(WLR_ERROR, "Failed to allocate tinywl_output for %s", wlr_output->name);
        return;
    }
    output->wlr_output = wlr_output;
    output->server = server;

    output->frame.notify = output_frame;
    wl_signal_add(&wlr_output->events.frame, &output->frame);

    output->request_state.notify = output_request_state;
    wl_signal_add(&wlr_output->events.request_state, &output->request_state);

    output->destroy.notify = output_destroy;
    wl_signal_add(&wlr_output->events.destroy, &output->destroy);

    wl_list_insert(&server->outputs, &output->link);

    struct wlr_output_layout_output *l_output = wlr_output_layout_add_auto(server->output_layout,
        wlr_output);
    if (!l_output) {
         wlr_log(WLR_ERROR, "Failed to add output %s to layout", wlr_output->name);
         wl_list_remove(&output->frame.link);
         wl_list_remove(&output->request_state.link);
         wl_list_remove(&output->destroy.link);
         wl_list_remove(&output->link);
         free(output);
         return;
    }

    struct wlr_scene_output *scene_output = wlr_scene_output_create(server->scene, wlr_output);
    if (!scene_output) {
        wlr_log(WLR_ERROR, "Failed to create scene output for %s", wlr_output->name);
        wlr_output_layout_remove(server->output_layout, wlr_output);
        wl_list_remove(&output->frame.link);
        wl_list_remove(&output->request_state.link);
        wl_list_remove(&output->destroy.link);
        wl_list_remove(&output->link);
        free(output);
        return;
    }
    output->scene_output = scene_output;

    wlr_scene_output_layout_add_output(server->scene_layout, l_output, scene_output);
    
    wlr_output_schedule_frame(output->wlr_output);
}

static void xdg_toplevel_map(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, map);
    wlr_log(WLR_ERROR, "!!!!!!!!!! XDG_TOPLEVEL_MAP CALLED (CUSTOM PATH ACTIVE) for toplevel %p (xdg: %p, title: '%s') !!!!!!!!!!",
            (void*)toplevel,
            (void*)toplevel->xdg_toplevel,
            toplevel->xdg_toplevel && toplevel->xdg_toplevel->title ? toplevel->xdg_toplevel->title : "N/A");

    if (!toplevel || !toplevel->xdg_toplevel) {
        wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Invalid toplevel=%p or xdg_toplevel=%p",
                toplevel, toplevel ? toplevel->xdg_toplevel : NULL);
        return;
    }

    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 1");

    if (toplevel->scene_tree) {
        wlr_scene_node_set_enabled(&toplevel->scene_tree->node, true);
        wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Toplevel %p scene_tree node enabled.", toplevel);
    } else {
        wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Toplevel %p has NO scene_tree!", toplevel);
    }

    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 2");

    toplevel->is_animating = true;
    toplevel->scale = 0.0f;
    toplevel->target_scale = 1.0f;
    toplevel->animation_duration = 0.3f;

    struct timespec now_map_time;
    if (clock_gettime(CLOCK_MONOTONIC, &now_map_time) == -1) {
        wlr_log_errno(WLR_ERROR, "XDG_TOPLEVEL_MAP: clock_gettime failed");
    }

    toplevel->animation_start = now_map_time.tv_sec + now_map_time.tv_nsec / 1e9f;

    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 9 (animation_start set to %f)", toplevel->animation_start);

    // Remove and insert into toplevels list
    wl_list_remove(&toplevel->link);
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 10 (after wl_list_remove)");

    wl_list_insert(&toplevel->server->toplevels, &toplevel->link);
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 11 (after wl_list_insert)");
    // Skip wl_list_length for now, or add defensive check
    if (toplevel->server->toplevels.next && toplevel->server->toplevels.prev) {
        wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Toplevel %p added to server->toplevels.", toplevel);
    } else {
        wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: server->toplevels appears uninitialized!");
    }

    focus_toplevel(toplevel);
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 12 (after focus_toplevel)");

    if (toplevel->xdg_toplevel && toplevel->xdg_toplevel->base) {
        wlr_log(WLR_ERROR, "!!!!!!!!!! XDG_TOPLEVEL_MAP: Scheduling configure for xdg_surface %p (configured: %d) !!!!!!!!!!",
                (void*)toplevel->xdg_toplevel->base, toplevel->xdg_toplevel->base->configured);
        wlr_xdg_surface_schedule_configure(toplevel->xdg_toplevel->base);
    }

    struct wlr_surface *surface = toplevel->xdg_toplevel && toplevel->xdg_toplevel->base ?
                                 toplevel->xdg_toplevel->base->surface : NULL;

    if (surface && surface->mapped) {
        struct timespec frame_done_now;
        clock_gettime(CLOCK_MONOTONIC, &frame_done_now);
        wlr_surface_send_frame_done(surface, &frame_done_now);

        struct tinywl_output *output;
        wl_list_for_each(output, &toplevel->server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
            }
        }
    } else {
        wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Surface not mapped or no surface: %p", (void*)surface);
    }

    wlr_log(WLR_ERROR, "!!!!!!!!!! XDG_TOPLEVEL_MAP FINISHED for toplevel %p !!!!!!!!!!", (void*)toplevel);
}
/*
static void xdg_toplevel_map(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, map);
    wlr_log(WLR_ERROR, "!!!!!!!!!! XDG_TOPLEVEL_MAP CALLED (CUSTOM PATH ACTIVE) for toplevel %p (xdg: %p, title: '%s') !!!!!!!!!!",
            (void*)toplevel,
            (void*)toplevel->xdg_toplevel,
            (toplevel->xdg_toplevel && toplevel->xdg_toplevel->title) ? toplevel->xdg_toplevel->title : "N/A");

    if (!toplevel) { // Ultra defensive
        wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: toplevel pointer is NULL at start!");
        return;
    }
    if (!toplevel->xdg_toplevel) { // Ultra defensive
        wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: toplevel->xdg_toplevel is NULL!");
        return;
    }


    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 1"); // CHECKPOINT 1

    if (toplevel->scene_tree) {
        wlr_scene_node_set_enabled(&toplevel->scene_tree->node, true);
        wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Toplevel %p scene_tree node explicitly enabled on map.", toplevel); // This was your line 2025
    } else {
        wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Toplevel %p has NO scene_tree on map!", toplevel);
    }

    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 2 (after scene_tree enable)");

    toplevel->is_animating = true;
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 3 (is_animating set)");

    toplevel->scale = 0.0f;
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 4 (scale set)");

    toplevel->target_scale = 1.0f;
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 5 (target_scale set)");

    toplevel->animation_duration = 0.3f;
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 6 (animation_duration set)");

    struct timespec now_map_time; // Renamed to avoid any conflict if 'now' is global somewhere (unlikely)
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 7 (before clock_gettime)");

    if (clock_gettime(CLOCK_MONOTONIC, &now_map_time) == -1) {
        wlr_log_errno(WLR_ERROR, "XDG_TOPLEVEL_MAP: clock_gettime failed");
        // Decide how to handle, maybe don't start animation or use a fallback
    }
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 8 (after clock_gettime)");

    toplevel->animation_start = now_map_time.tv_sec + now_map_time.tv_nsec / 1e9f; // Added 'f' for float division
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 9 (animation_start set to %f)", toplevel->animation_start);

    wl_list_remove(&toplevel->link);
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 10 (after wl_list_remove)");

    wl_list_insert(&toplevel->server->toplevels, &toplevel->link);
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Toplevel %p ADDED to server->toplevels list (CUSTOM PATH ACTIVE). List length: %d",
            toplevel, wl_list_length(&toplevel->server->toplevels));
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 11 (after wl_list_insert)");

    focus_toplevel(toplevel);
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 12 (after focus_toplevel)");

    if (toplevel->xdg_toplevel && toplevel->xdg_toplevel->base) {
        wlr_log(WLR_ERROR, "!!!!!!!!!! XDG_TOPLEVEL_MAP: Explicitly scheduling configure for xdg_surface %p (configured: %d) !!!!!!!!!!",
                (void*)toplevel->xdg_toplevel->base,
                toplevel->xdg_toplevel->base->configured);
        wlr_xdg_surface_schedule_configure(toplevel->xdg_toplevel->base);
    }
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 13 (after schedule_configure block)");


    struct wlr_surface *surface = NULL;
    if (toplevel->xdg_toplevel && toplevel->xdg_toplevel->base) {
        surface = toplevel->xdg_toplevel->base->surface;
    }
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 14 (surface pointer set)");


    if (surface && surface->mapped) {
        struct timespec frame_done_now_map; // Renamed
        clock_gettime(CLOCK_MONOTONIC, &frame_done_now_map);
        wlr_surface_send_frame_done(surface, &frame_done_now_map);
        wlr_log(WLR_DEBUG, "XDG_TOPLEVEL_MAP: Sent frame_done for surface %p", (void*)surface);

        struct tinywl_output *output;
        wl_list_for_each(output, &toplevel->server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
                wlr_log(WLR_DEBUG, "XDG_TOPLEVEL_MAP: Scheduled frame for output %s", output->wlr_output->name);
            }
        }
    } else {
        wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Toplevel %p underlying surface not mapped or no surface after map logic (surface ptr: %p)",
                toplevel, (void*)surface);
    }
    wlr_log(WLR_ERROR, "XDG_TOPLEVEL_MAP: Checkpoint 15 (before final log)");
    wlr_log(WLR_ERROR, "!!!!!!!!!! XDG_TOPLEVEL_MAP FINISHED (CUSTOM PATH ACTIVE) for toplevel %p !!!!!!!!!!", (void*)toplevel);
}*/

#include <time.h> // For clock_gettime, ensure this is included



#include <time.h> // For clock_gettime, ensure this is included
#include <math.h>   // For fmaxf, fminf





static void xdg_toplevel_unmap(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel_wrapper = wl_container_of(listener, toplevel_wrapper, unmap);
    if (!toplevel_wrapper || !toplevel_wrapper->xdg_toplevel) return;
    const char *title = toplevel_wrapper->xdg_toplevel->title ? toplevel_wrapper->xdg_toplevel->title : "N/A";

    wlr_log(WLR_ERROR, "[UNMAP SUPER DEBUG RECT:%s Ptr:%p] Event received.", title, toplevel_wrapper);

    // For this test, unmap does very little. Destroy will handle the debug rect.
    toplevel_wrapper->is_animating = false;
    toplevel_wrapper->pending_destroy = false; // It won't be destroyed by iterator

    if (toplevel_wrapper->scene_tree && !toplevel_wrapper->scene_tree->node.enabled) {
        wlr_scene_node_set_enabled(&toplevel_wrapper->scene_tree->node, true);
    }
    struct tinywl_output *out;
    wl_list_for_each(out, &toplevel_wrapper->server->outputs, link) { wlr_output_schedule_frame(out->wlr_output); }
}

static void xdg_toplevel_commit(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, commit);
    struct wlr_surface *surface = toplevel->xdg_toplevel->base->surface;
    struct tinywl_server *server = toplevel->server;
    const char *title = (toplevel->xdg_toplevel && toplevel->xdg_toplevel->title) ? toplevel->xdg_toplevel->title : "N/A";

    if (!surface) {
        wlr_log(WLR_ERROR, "[COMMIT:%s] Surface pointer was NULL!", title);
        return;
    }

    wlr_log(WLR_INFO, "[COMMIT:%s] Surface %p (mapped: %d) commit event. Current buffer: %p (size %dx%d), Seq: %u. Cached tex before: %p, Last cached seq: %u",
            title, (void*)surface, surface->mapped,
            (void*)surface->current.buffer,
            surface->current.buffer ? surface->current.buffer->width : 0,
            surface->current.buffer ? surface->current.buffer->height : 0,
            surface->current.seq,
            (void*)toplevel->cached_texture,
            toplevel->last_commit_seq);

    if (surface->current.buffer) {
        // A buffer is present in the current state of the surface for this commit.
        // Let's try to cache/update the texture.
        if (toplevel->cached_texture && toplevel->last_commit_seq == surface->current.seq) {
            // Buffer is current, and seq matches our cached texture's seq. Assume texture is still valid.
            wlr_log(WLR_DEBUG, "[COMMIT:%s] Current buffer %p and seq %u match cached. No texture update.",
                title, (void*)surface->current.buffer, surface->current.seq);
        } else {
            // Either no cached texture, or sequence number changed, indicating new content.
            if (toplevel->cached_texture) {
                wlr_log(WLR_DEBUG, "[COMMIT:%s] Destroying old cached_texture %p (old seq %u, new seq %u).",
                        title, (void*)toplevel->cached_texture, toplevel->last_commit_seq, surface->current.seq);
                wlr_texture_destroy(toplevel->cached_texture);
                toplevel->cached_texture = NULL;
            }

            wlr_log(WLR_INFO, "[COMMIT:%s] Attempting to create new cached_texture from current.buffer %p.",
                    title, (void*)surface->current.buffer);
            toplevel->cached_texture = wlr_texture_from_buffer(server->renderer, surface->current.buffer);

            if (toplevel->cached_texture) {
                wlr_log(WLR_INFO, "[COMMIT:%s] Successfully cached new texture %p (from buffer %p, size %dx%d). New last_commit_seq: %u",
                        title, (void*)toplevel->cached_texture, (void*)surface->current.buffer,
                        toplevel->cached_texture->width, toplevel->cached_texture->height, surface->current.seq);
                toplevel->last_commit_seq = surface->current.seq;
            } else {
                wlr_log(WLR_ERROR, "[COMMIT:%s] Failed to create texture from current.buffer %p for caching. last_commit_seq remains %u.",
                        title, (void*)surface->current.buffer, toplevel->last_commit_seq);
                // Consider what last_commit_seq should be if caching fails. Keeping old might be misleading.
                // Perhaps set to a value indicating failure or current seq anyway.
                toplevel->last_commit_seq = surface->current.seq; // Update seq even on failure to prevent retrying this exact failed buffer.
            }
        }
    } else {
        // surface->current.buffer is NULL for this commit.
        // This means the client committed without a buffer, or detached its buffer.
        if (toplevel->cached_texture) {
            wlr_log(WLR_INFO, "[COMMIT:%s] Current buffer is NULL. Destroying stale cached_texture %p (was for seq %u).",
                    title, (void*)toplevel->cached_texture, toplevel->last_commit_seq);
            wlr_texture_destroy(toplevel->cached_texture);
            toplevel->cached_texture = NULL;
        }
        toplevel->last_commit_seq = surface->current.seq; // Update seq to reflect this bufferless state.
    }

    // Handle initial commit and configuration
    if (toplevel->xdg_toplevel->base->initial_commit) {
        wlr_xdg_toplevel_set_size(toplevel->xdg_toplevel, 0, 0);
        if (!toplevel->xdg_toplevel->base->configured) {
             wlr_xdg_surface_schedule_configure(toplevel->xdg_toplevel->base);
             wlr_log(WLR_DEBUG, "[COMMIT:%s] Scheduled configure for initial commit.", title);
        }
    }

    if (surface->mapped) {
        struct tinywl_output *output_iter;
        wl_list_for_each(output_iter, &server->outputs, link) {
            if (output_iter->wlr_output && output_iter->wlr_output->enabled) {
                wlr_output_schedule_frame(output_iter->wlr_output);
            }
        }
    }
}






static void xdg_toplevel_destroy(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel_wrapper = wl_container_of(listener, toplevel_wrapper, destroy);
    struct wlr_xdg_toplevel *wlroots_xdg_toplevel = toplevel_wrapper ? toplevel_wrapper->xdg_toplevel : NULL;
    const char *title_for_log = "N/A_DESTROY";
    if (wlroots_xdg_toplevel && wlroots_xdg_toplevel->title) {
        title_for_log = wlroots_xdg_toplevel->title;
    } else if (toplevel_wrapper) {
        // If title isn't available, use pointer for logging
        char ptr_buf[32];
        snprintf(ptr_buf, sizeof(ptr_buf), "TL_WRAPPER:%p", (void*)toplevel_wrapper);
        title_for_log = ptr_buf; // Not ideal but better than N/A if title is gone
    }


    wlr_log(WLR_INFO, "[DESTROY:%s] Listener called. Wrapper Ptr:%p.", title_for_log, (void*)toplevel_wrapper);

    if (!toplevel_wrapper) {
        // ... (existing early exit for null wrapper) ...
        return;
    }

    // Destroy cached_texture FIRST, as it might be tied to the surface/renderer
    if (toplevel_wrapper->cached_texture) {
        wlr_log(WLR_INFO, "[DESTROY:%s] Destroying cached_texture %p.",
                title_for_log, (void*)toplevel_wrapper->cached_texture);
        wlr_texture_destroy(toplevel_wrapper->cached_texture);
        toplevel_wrapper->cached_texture = NULL;
    }

    // Remove listeners from the wlroots XDG toplevel resource if it still exists
    if (wlroots_xdg_toplevel) { // Check if xdg_toplevel pointer in wrapper is still valid
        wlr_log(WLR_DEBUG, "[DESTROY:%s] Removing listeners from wlr_xdg_toplevel %p.", title_for_log, (void*)wlroots_xdg_toplevel);
        wl_list_remove(&toplevel_wrapper->map.link);
        wl_list_remove(&toplevel_wrapper->unmap.link);
        wl_list_remove(&toplevel_wrapper->commit.link);
        // Ensure this destroy listener itself is removed
        // This check is important: only remove if 'listener' is indeed our 'destroy' member
        if (listener == &toplevel_wrapper->destroy) {
             wl_list_remove(&toplevel_wrapper->destroy.link);
        } else {
             // If somehow another listener triggered this, still remove the official one
             wl_list_remove(&toplevel_wrapper->destroy.link);
             wlr_log(WLR_ERROR, "[DESTROY:%s] Called from unexpected listener, but removed destroy listener anyway.", title_for_log);
        }
        wl_list_remove(&toplevel_wrapper->request_move.link);
        wl_list_remove(&toplevel_wrapper->request_resize.link);
        wl_list_remove(&toplevel_wrapper->request_maximize.link);
        wl_list_remove(&toplevel_wrapper->request_fullscreen.link);
    } else {
         wlr_log(WLR_INFO, "[DESTROY:%s] Wrapper's xdg_toplevel was already NULL. Listeners might have been removed prior.", title_for_log);
         // Still ensure this listener's link is cleaned if it's the one being called
         if (listener == &toplevel_wrapper->destroy) {
              wl_list_remove(&toplevel_wrapper->destroy.link);
         }
    }


    // Scene node destruction
    if (toplevel_wrapper->scene_tree) {
        wlr_log(WLR_DEBUG, "[DESTROY:%s] Destroying scene_tree node %p.", title_for_log, (void*)&toplevel_wrapper->scene_tree->node);
        wlr_scene_node_destroy(&toplevel_wrapper->scene_tree->node); // This also destroys client_xdg_scene_tree if it's a child
        toplevel_wrapper->scene_tree = NULL;
        toplevel_wrapper->client_xdg_scene_tree = NULL; // It's destroyed as part of scene_tree
    }

    // Remove from server's list
    wl_list_remove(&toplevel_wrapper->link);
    wlr_log(WLR_DEBUG, "[DESTROY:%s] Removed toplevel wrapper from server list.", title_for_log);


    // If this destroy is called directly (e.g. from server_destroy), we'd free here.
    // If it's from the xdg_toplevel->events.destroy signal, wlroots handles the xdg_toplevel.
    // The wrapper struct should be freed.
    // The KEEP_ALIVE logic made this complicated. For a normal destroy:
    // free(toplevel_wrapper);
    // toplevel_wrapper = NULL; // Good practice if pointer is reused, though not here.
    // For your KEEP_ALIVE debug, you skip the free.
    // For now, let's assume the "KEEP_ALIVE" implies you are not freeing toplevel_wrapper here.

    wlr_log(WLR_INFO, "[DESTROY:%s] Finished processing destroy for wrapper %p.", title_for_log, (void*)toplevel_wrapper);
    // Nullify the xdg_toplevel pointer in the wrapper, as the wlroots resource is gone (or going).
    // This prevents use-after-free if the wrapper struct persists.
    toplevel_wrapper->xdg_toplevel = NULL;
}
static void begin_interactive(struct tinywl_toplevel *toplevel, enum tinywl_cursor_mode mode, uint32_t edges) {
    struct tinywl_server *server = toplevel->server;
    server->grabbed_toplevel = toplevel;
    server->cursor_mode = mode;

    if (mode == TINYWL_CURSOR_MOVE) {
        server->grab_x = server->cursor->x - toplevel->scene_tree->node.x;
        server->grab_y = server->cursor->y - toplevel->scene_tree->node.y;
    } else {
        struct wlr_box *geo_box = &toplevel->xdg_toplevel->base->geometry;
        double border_x = (toplevel->scene_tree->node.x + geo_box->x) +
            ((edges & WLR_EDGE_RIGHT) ? geo_box->width : 0);
        double border_y = (toplevel->scene_tree->node.y + geo_box->y) +
            ((edges & WLR_EDGE_BOTTOM) ? geo_box->height : 0);
        server->grab_x = server->cursor->x - border_x;
        server->grab_y = server->cursor->y - border_y;
        server->grab_geobox = *geo_box;
        server->grab_geobox.x += toplevel->scene_tree->node.x;
        server->grab_geobox.y += toplevel->scene_tree->node.y;
        server->resize_edges = edges;
    }
}

static void xdg_toplevel_request_move(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, request_move);
    begin_interactive(toplevel, TINYWL_CURSOR_MOVE, 0);
}

static void xdg_toplevel_request_resize(struct wl_listener *listener, void *data) {
    struct wlr_xdg_toplevel_resize_event *event = data;
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, request_resize);
    begin_interactive(toplevel, TINYWL_CURSOR_RESIZE, event->edges);
}

static void xdg_toplevel_request_maximize(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, request_maximize);
    if (toplevel->xdg_toplevel->base->initialized) {
        wlr_xdg_surface_schedule_configure(toplevel->xdg_toplevel->base);
    }
}

static void xdg_toplevel_request_fullscreen(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, request_fullscreen);
    if (toplevel->xdg_toplevel->base->initialized) {
        wlr_xdg_surface_schedule_configure(toplevel->xdg_toplevel->base);
    }
}
/*
static void server_new_xdg_toplevel(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, new_xdg_toplevel);
    struct wlr_xdg_toplevel *xdg_toplevel = data;
    wlr_log(WLR_ERROR, "!!!!!!!!!! SERVER_NEW_XDG_TOPLEVEL CALLED for xdg_toplevel %p (app_id: %s, title: %s) !!!!!!!!!!",
            (void*)xdg_toplevel,
            xdg_toplevel->app_id ? xdg_toplevel->app_id : "N/A",
            xdg_toplevel->title ? xdg_toplevel->title : "N/A");

    struct tinywl_toplevel *toplevel = calloc(1, sizeof(*toplevel));
    if (!toplevel) {
        wlr_log(WLR_ERROR, "SERVER_NEW_XDG_TOPLEVEL: Failed to allocate tinywl_toplevel");
        return; // Early exit
    }
    wl_list_init(&toplevel->link); 
    toplevel->server = server;
    toplevel->xdg_toplevel = xdg_toplevel;
    toplevel->scale = 1.0f; // Default scale
    toplevel->target_scale = 1.0f;
    toplevel->is_animating = false;

wl_signal_init(&xdg_toplevel->resource->destroy_signal);

    toplevel->scene_tree = wlr_scene_tree_create(&server->scene->tree);
    if (!toplevel->scene_tree) {
        wlr_log(WLR_ERROR, "SERVER_NEW_XDG_TOPLEVEL: Failed to create scene_tree (frame) for toplevel");
        free(toplevel);
        return; // Early exit
    }
    toplevel->scene_tree->node.data = toplevel;
    xdg_toplevel->base->data = toplevel;

    toplevel->client_xdg_scene_tree = wlr_scene_xdg_surface_create(toplevel->scene_tree, xdg_toplevel->base);
    if (!toplevel->client_xdg_scene_tree) {
        wlr_log(WLR_ERROR, "SERVER_NEW_XDG_TOPLEVEL: Failed to create client_xdg_scene_tree for toplevel");
        wlr_scene_node_destroy(&toplevel->scene_tree->node);
        free(toplevel);
        return; // Early exit
    }

    wlr_scene_node_set_position(&toplevel->client_xdg_scene_tree->node, 0, 0);

    toplevel->map.notify = xdg_toplevel_map;
    wl_signal_add(&xdg_toplevel->base->surface->events.map, &toplevel->map);
    toplevel->unmap.notify = xdg_toplevel_unmap;
    wl_signal_add(&xdg_toplevel->base->surface->events.unmap, &toplevel->unmap);
    toplevel->commit.notify = xdg_toplevel_commit;
    wl_signal_add(&xdg_toplevel->base->surface->events.commit, &toplevel->commit);
    toplevel->destroy.notify = xdg_toplevel_destroy;
    wl_signal_add(&xdg_toplevel->events.destroy, &toplevel->destroy);
    toplevel->request_move.notify = xdg_toplevel_request_move;
    wl_signal_add(&xdg_toplevel->events.request_move, &toplevel->request_move);
    toplevel->request_resize.notify = xdg_toplevel_request_resize;
    wl_signal_add(&xdg_toplevel->events.request_resize, &toplevel->request_resize);
    toplevel->request_maximize.notify = xdg_toplevel_request_maximize;
    wl_signal_add(&xdg_toplevel->events.request_maximize, &toplevel->request_maximize);
    toplevel->request_fullscreen.notify = xdg_toplevel_request_fullscreen;
    wl_signal_add(&xdg_toplevel->events.request_fullscreen, &toplevel->request_fullscreen);

    // Crucially, the toplevel is added to the list *in xdg_toplevel_map*
    // This is generally okay, but let's confirm map is called.
    wlr_log(WLR_ERROR, "!!!!!!!!!! SERVER_NEW_XDG_TOPLEVEL FINISHED SUCCESSFULLY for xdg_toplevel %p. Toplevel struct %p created. WAITING FOR MAP. !!!!!!!!!!", (void*)xdg_toplevel, (void*)toplevel);
}*/

static void server_new_xdg_toplevel(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, new_xdg_toplevel);
    struct wlr_xdg_toplevel *xdg_toplevel = data;
    wlr_log(WLR_ERROR, "!!!!!!!!!! SERVER_NEW_XDG_TOPLEVEL CALLED for xdg_toplevel %p (app_id: %s, title: %s) !!!!!!!!!!",
            (void*)xdg_toplevel,
            xdg_toplevel->app_id ? xdg_toplevel->app_id : "N/A",
            xdg_toplevel->title ? xdg_toplevel->title : "N/A");

    // Validate input to prevent null pointer dereferences
    if (!server || !xdg_toplevel || !xdg_toplevel->base || !xdg_toplevel->base->surface) {
        wlr_log(WLR_ERROR, "SERVER_NEW_XDG_TOPLEVEL: Invalid server or xdg_toplevel");
        return;
    }

    // Allocate toplevel with zero-initialized memory
    struct tinywl_toplevel *toplevel = calloc(1, sizeof(*toplevel));
    if (!toplevel) {
        wlr_log(WLR_ERROR, "SERVER_NEW_XDG_TOPLEVEL: Failed to allocate tinywl_toplevel");
        wl_resource_post_no_memory(xdg_toplevel->resource);
        return;
    }

    // Initialize toplevel fields
    toplevel->server = server;
    toplevel->xdg_toplevel = xdg_toplevel;
    wl_list_init(&toplevel->link); // Initialize list link for server->toplevels
    toplevel->scale = 1.0f; // Default scale
    toplevel->target_scale = 1.0f;
    toplevel->is_animating = false;
    wlr_log(WLR_DEBUG, "SERVER_NEW_XDG_TOPLEVEL: Toplevel %p allocated", toplevel);

    // Create scene tree for the toplevel
    toplevel->scene_tree = wlr_scene_tree_create(&server->scene->tree);
    if (!toplevel->scene_tree) {
        wlr_log(WLR_ERROR, "SERVER_NEW_XDG_TOPLEVEL: Failed to create scene_tree for toplevel");
        free(toplevel);
        wl_resource_post_no_memory(xdg_toplevel->resource);
        return;
    }
    toplevel->scene_tree->node.data = toplevel;
    xdg_toplevel->base->data = toplevel;
    wlr_log(WLR_DEBUG, "SERVER_NEW_XDG_TOPLEVEL: Scene tree %p created", toplevel->scene_tree);

    // Create client XDG surface scene node
    toplevel->client_xdg_scene_tree = wlr_scene_xdg_surface_create(toplevel->scene_tree, xdg_toplevel->base);
    if (!toplevel->client_xdg_scene_tree) {
        toplevel->client_xdg_scene_tree->node.data = toplevel; 
        wlr_log(WLR_ERROR, "SERVER_NEW_XDG_TOPLEVEL: Failed to create client_xdg_scene_tree");
        wlr_scene_node_destroy(&toplevel->scene_tree->node);
        free(toplevel);
        wl_resource_post_no_memory(xdg_toplevel->resource);
        return;
    }
    wlr_scene_node_set_position(&toplevel->client_xdg_scene_tree->node, 0, 0);
    wlr_log(WLR_DEBUG, "SERVER_NEW_XDG_TOPLEVEL: Client XDG scene tree %p created", toplevel->client_xdg_scene_tree);

    // Initialize listener structures
    toplevel->map = (struct wl_listener){ .notify = xdg_toplevel_map };
    toplevel->unmap = (struct wl_listener){ .notify = xdg_toplevel_unmap };
    toplevel->commit = (struct wl_listener){ .notify = xdg_toplevel_commit };
    toplevel->destroy = (struct wl_listener){ .notify = xdg_toplevel_destroy };
    toplevel->request_move = (struct wl_listener){ .notify = xdg_toplevel_request_move };
    toplevel->request_resize = (struct wl_listener){ .notify = xdg_toplevel_request_resize };
    toplevel->request_maximize = (struct wl_listener){ .notify = xdg_toplevel_request_maximize };
    toplevel->request_fullscreen = (struct wl_listener){ .notify = xdg_toplevel_request_fullscreen };

    // Add listeners safely
    wl_signal_add(&xdg_toplevel->base->surface->events.map, &toplevel->map);
    wl_signal_add(&xdg_toplevel->base->surface->events.unmap, &toplevel->unmap);
    wl_signal_add(&xdg_toplevel->base->surface->events.commit, &toplevel->commit);
    wl_signal_add(&xdg_toplevel->events.destroy, &toplevel->destroy);
    wl_signal_add(&xdg_toplevel->events.request_move, &toplevel->request_move);
    wl_signal_add(&xdg_toplevel->events.request_resize, &toplevel->request_resize);
    wl_signal_add(&xdg_toplevel->events.request_maximize, &toplevel->request_maximize);
    wl_signal_add(&xdg_toplevel->events.request_fullscreen, &toplevel->request_fullscreen);
    wlr_log(WLR_DEBUG, "SERVER_NEW_XDG_TOPLEVEL: Listeners added for toplevel %p", toplevel);

    // Do not reinitialize xdg_toplevel->resource->destroy_signal; it's already initialized by wlroots
    // Adding the destroy listener is sufficient

    // Log completion; toplevel is added to server->toplevels in xdg_toplevel_map
    wlr_log(WLR_ERROR, "!!!!!!!!!! SERVER_NEW_XDG_TOPLEVEL FINISHED SUCCESSFULLY for xdg_toplevel %p. Toplevel struct %p created. WAITING FOR MAP. !!!!!!!!!!",
            (void*)xdg_toplevel, (void*)toplevel);
}



static void server_new_xdg_popup(struct wl_listener *listener, void *data) {
    if (!listener || !data) {
        wlr_log(WLR_ERROR, "Invalid listener=%p or data=%p in server_new_xdg_popup", listener, data);
        return;
    }

    struct tinywl_server *server = wl_container_of(listener, server, new_xdg_popup);
    if (!server || !server->wl_display || !server->xdg_shell || !server->scene) {
        wlr_log(WLR_ERROR, "Invalid server state: server=%p, display=%p, xdg_shell=%p, scene=%p",
                server, server ? server->wl_display : NULL, server ? server->xdg_shell : NULL, server ? server->scene : NULL);
        return;
    }

    struct wlr_xdg_popup *xdg_popup = data;
    wlr_log(WLR_INFO, "New XDG popup: %p, parent=%p, surface=%p, initial_commit=%d",
            xdg_popup, xdg_popup->parent, xdg_popup->base ? xdg_popup->base->surface : NULL,
            xdg_popup->base ? xdg_popup->base->initial_commit : 0);

    // Validate popup surface and role
    if (!xdg_popup->base || !xdg_popup->base->surface || xdg_popup->base->role != WLR_XDG_SURFACE_ROLE_POPUP) {
        wlr_log(WLR_ERROR, "Invalid popup: base=%p, surface=%p, role=%d",
                xdg_popup->base, xdg_popup->base ? xdg_popup->base->surface : NULL,
                xdg_popup->base ? (int)xdg_popup->base->role : -1);
        return;
    }

    // Validate parent surface
    struct wlr_xdg_surface *parent = wlr_xdg_surface_try_from_wlr_surface(xdg_popup->parent);
    if (!parent || !parent->surface) {
        wlr_log(WLR_ERROR, "No valid parent XDG surface: parent=%p, parent->surface=%p",
                parent, parent ? parent->surface : NULL);
        return;
    }

    // Ensure parent is a toplevel (popups cannot have popup parents)
    if (parent->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
        wlr_log(WLR_ERROR, "Parent surface is not a toplevel: role=%d", parent->role);
        return;
    }

    // Validate parent scene tree
    struct wlr_scene_tree *parent_tree = parent->data;
    if (!parent_tree || parent_tree->node.type != WLR_SCENE_NODE_TREE) {
        wlr_log(WLR_ERROR, "Invalid parent scene tree: parent_tree=%p, type=%d",
                parent_tree, parent_tree ? (int)parent_tree->node.type : -1);
        return;
    }

    // Check parent surface state
    if (!parent->surface->mapped || !wlr_surface_has_buffer(parent->surface)) {
        wlr_log(WLR_ERROR, "Parent surface not ready: mapped=%d, has_buffer=%d",
                parent->surface->mapped, wlr_surface_has_buffer(parent->surface));
        return; // Defer until parent is ready
    }

    // Allocate popup structure
    struct tinywl_popup *popup = calloc(1, sizeof(struct tinywl_popup));
    if (!popup) {
        wlr_log(WLR_ERROR, "Failed to allocate tinywl_popup");
        return;
    }

    popup->xdg_popup = xdg_popup;
    popup->server = server;

    // Create scene tree for popup
    popup->scene_tree = wlr_scene_xdg_surface_create(parent_tree, xdg_popup->base);
    if (!popup->scene_tree) {
        wlr_log(WLR_ERROR, "Failed to create scene tree for popup: xdg_surface=%p", xdg_popup->base);
        free(popup);
        return;
    }

    // Set data pointers
    popup->scene_tree->node.data = popup;
    xdg_popup->base->data = popup; // Store popup for consistency with toplevels

    // Set up event listeners
    popup->map.notify = popup_map;
    wl_signal_add(&xdg_popup->base->surface->events.map, &popup->map);
    popup->unmap.notify = popup_unmap;
    wl_signal_add(&xdg_popup->base->surface->events.unmap, &popup->unmap);
    popup->destroy.notify = popup_destroy;
    wl_signal_add(&xdg_popup->events.destroy, &popup->destroy);
    popup->commit.notify = popup_commit;
    wl_signal_add(&xdg_popup->base->surface->events.commit, &popup->commit);

    // Add to server's popup list
    wl_list_insert(&server->popups, &popup->link);
    wlr_log(WLR_DEBUG, "Popup created: %p, parent=%p, scene_tree=%p, xdg_surface=%p",
            xdg_popup, parent->surface, popup->scene_tree, xdg_popup->base);

    // Schedule configure if needed
    if (xdg_popup->base->initial_commit) {
        wlr_xdg_surface_schedule_configure(xdg_popup->base);
        wlr_log(WLR_DEBUG, "Scheduled configure for popup %p", xdg_popup);
    }

    // Schedule frame
    struct tinywl_output *output;
    wl_list_for_each(output, &server->outputs, link) {
        if (output->wlr_output && output->wlr_output->enabled) {
            wlr_output_schedule_frame(output->wlr_output);
        }
    }
}


#ifndef EGL_PLATFORM_SURFACELESS_MESA
#define EGL_PLATFORM_SURFACELESS_MESA 0x31DD
#endif
#ifndef EGL_PLATFORM_SURFACELESS_MESA
#define EGL_PLATFORM_SURFACELESS_MESA 0x31DD
#endif

#ifndef EGL_PLATFORM_SURFACELESS_MESA
#define EGL_PLATFORM_SURFACELESS_MESA 0x31DD
#endif


void cleanup_egl(struct tinywl_server *server) {
    printf("Cleaning up EGL resources\n");
    if (server->egl_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(server->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (server->egl_surface != EGL_NO_SURFACE) {
            eglDestroySurface(server->egl_display, server->egl_surface);
            server->egl_surface = EGL_NO_SURFACE;
        }
        if (server->egl_context != EGL_NO_CONTEXT) {
            eglDestroyContext(server->egl_display, server->egl_context);
            server->egl_context = EGL_NO_CONTEXT;
        }
        eglTerminate(server->egl_display);
        server->egl_display = EGL_NO_DISPLAY;
    }
}


/* Updated main function */
int main(int argc, char *argv[]) {
    


// Vertex shader for simple colored rectangles
static const char *rect_vertex_shader_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 position;      // Vertex positions (0 to 1 for unit quad)\n"
    "in vec2 texcoord;      // Vertex texture/local coordinates (also 0 to 1 if VAO is set up for it)\n"
    "out vec2 v_texcoord;   // Pass local quad coordinates to fragment shader (for vignette)\n"
    "uniform mat3 mvp;\n"
    "void main() {\n"
    "    vec3 pos_transformed = mvp * vec3(position, 1.0);\n"
    "    gl_Position = vec4(pos_transformed.xy, 0.0, 1.0);\n"
    "    v_texcoord = texcoord; // Pass the VAO's texcoord attribute\n"
    "}\n";

static const char *rect_fragment_shader_src =
    "/* Creative Commons Licence Attribution-NonCommercial-ShareAlike \n"
    "phreax/jiagual 2025 \n"
    "Variation of https://www.shadertoy.com/view/tfG3zt\n"
    "             https://www.shadertoy.com/view/3XjXzK\n"
    "Inspired by Xor's recent volumetric shaders\n"
    "https://www.shadertoy.com/view/tXlXDX\n"
    "*/\n"
    "#version 300 es\n"
    "precision mediump float;\n"
    "\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "\n"
    "uniform float time;\n"
    "uniform vec4 u_color;\n"
    "\n"
    "#define PALETTE 6\n"
    "\n"
    "mat2 rot(float x) {return mat2(cos(x), -sin(x), sin(x), cos(x));}\n"
    "vec3 pal(float x) {return .5+.5*cos(6.28*x-vec3(5,4,1));}\n"
    "\n"
    "vec3 getPal(int palette, float x) {\n"
    "    return pal(x);\n"
    "}\n"
    "\n"
    "#define SIN(x) sin(x)\n"
    "\n"
    "void main() {\n"
    "    vec2 fragCoord = gl_FragCoord.xy;\n"
    "    vec2 iResolution = vec2(1024.0, 768.0);\n"
    "    float iTime = time * 0.5;\n"
    "    \n"
    "    vec2 uv = (fragCoord - .5*iResolution.xy)/min(iResolution.x, iResolution.y);\n"
    "    float tt = iTime*.5;\n"
    "    \n"
    "    uv.xy *= mix(.8, 1.2, SIN(-tt+5.*length(uv.xy)));\n"
    "    \n"
    "    vec3 col = vec3(0);\n"
    "    vec3 rd = vec3(uv, 1);\n"
    "    vec3 p = vec3(0);\n"
    "    float t = .1;\n"
    "    \n"
    "    for(float i=0.; i<120.; i++) {\n"
    "        vec3 p = t*rd + rd;\n"
    "        \n"
    "        float r = length(p);\n"
    "        \n"
    "        float z = p.z;\n"
    "        p.xy *= rot(p.z*.75);\n"
    "      \n"
    "        // log spherical coords\n"
    "        p = vec3(log(r)*.5,\n"
    "            acos(p.z / r),\n"
    "            atan(p.y, p.x));\n"
    "        p = abs(p)-mix(.1, .5, SIN(.2*tt));\n"
    "    \n"
    "        for(float j=0.; j < 3.; j++) {     \n"
    "            float  a= exp(j)/exp2(j);\n"
    "            p += cos(2.*p.yzx*a + .5*tt - length(p.xy)*9.)/a; \n"
    "        }\n"
    "        \n"
    "        float d = 0.007 + abs((exp2(1.4*p)-vec3(0,1.+.7*SIN(tt),0)).y-1.)/14.;\n"
    "        float k = t*.7 +length(p)*.1 - .2*tt + z*.1;\n"
    "        vec3 c = getPal(PALETTE, k);\n"
    "        c = mix(c, c*vec3(0.922,0.973,0.725), SIN(z*.5));\n"
    "        col += c*1e-3/d;       \n"
    "        t += d/4.;\n"
    "    }\n"
    "    \n"
    "    // add glow in the center\n"
    "    float gl = exp(-17.*length(uv.xy));\n"
    "    col += .4*mix(vec3(0.361,0.957,1.000), vec3(0.847,1.000,0.561), SIN(gl*2.-tt))*pow(gl*11., 1.);\n"
    "    \n"
    "    // tone mapping & gamma correction\n"
    "    col *= tanh(col*.1);\n"
    "    col = pow(col, vec3(.45));\n"
    "    \n"
    "    // Apply user color as subtle accent\n"
    "    col = mix(col, col * u_color.rgb, 0.1);\n"
    "    \n"
    "    frag_color = vec4(col, u_color.a).bgra;\n"
    "}\n";


const char *vertex_shader_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 position;\n"
    "in vec2 texcoord;\n" 
    "out vec2 v_texcoord;\n" 
    "uniform mat3 mvp;\n"
    "void main() {\n"
    "    vec3 pos_transformed = mvp * vec3(position, 1.0);\n"
    "    gl_Position = vec4(pos_transformed.xy, 0.0, 1.0);\n"
    "    v_texcoord = position;\n" 
    "}\n";

const char *fragment_shader_src = 
"#version 300 es\n"
"precision mediump float;\n"
"in vec2 v_texcoord;\n"
"out vec4 frag_color;\n"
"uniform sampler2D texture_sampler_uniform;\n"
"uniform float time;\n"
"uniform vec2 iResolution;\n"
"\n"
"float rand(vec2 n) {\n"
"    return fract(sin(cos(dot(n, vec2(12.9898, 12.1414)))) * 83758.5453);\n"
"}\n"
"\n"
"float noise(vec2 n) {\n"
"    const vec2 d = vec2(0.0, 1.0);\n"
"    vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));\n"
"    return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);\n"
"}\n"
"\n"
"float fbm(vec2 n) {\n"
"    float total = 0.0, amplitude = 1.0;\n"
"    for (int i = 0; i < 5; i++) {\n"
"        total += noise(n) * amplitude;\n"
"        n += n * 1.7;\n"
"        amplitude *= 0.67;\n"
"    }\n"
"    return total;\n"
"}\n"
"\n"
"void main() {\n"
"    float alpha_threshold = 0.9;\n"
"    vec2 uv = v_texcoord;\n"
"    \n"
"    // Sample original texture\n"
"    vec4 tex_color = texture2D(texture_sampler_uniform, uv);\n"
"    \n"
"    // Preserve transparent areas\n"
"    if(tex_color.a <= alpha_threshold) {\n"
"        frag_color = vec4(0.0, 0.0, 0.0, 0.0).bgra;\n"
"        return;\n"
"    }\n"
"    \n"
"    // Normalize time\n"
"    float iTime = mod(time, 100.0);\n"
"    \n"
"    // Color palette from Remix 2 with white\n"
"    const vec3 c1 = vec3(0.5, 0.0, 0.1); // Red\n"
"    const vec3 c2 = vec3(0.9, 0.1, 0.0); // Orange\n"
"    const vec3 c3 = vec3(0.2, 0.1, 0.7); // Subtle blue\n"
"    const vec3 c4 = vec3(1.0, 0.9, 0.1); // Yellow\n"
"    const vec3 c7 = vec3(1.0, 1.0, 1.0); // White for highlights\n"
"    \n"
"    // Speed and shift\n"
"    vec2 speed = vec2(0.1, 0.9);\n"
"    float shift = 1.327 + sin(iTime * 2.0) / 2.4;\n"
"    float dist = 3.5 - sin(iTime * 0.4) / 1.89;\n"
"    \n"
"    // Adjust UV to position flames (base at bottom, tips at top)\n"
"    vec2 adjusted_uv = vec2(uv.x, 1.0 - uv.y * 1.2 + 0.2);\n"
"    vec2 p = adjusted_uv * dist;\n"
"    p += sin(p.yx * 4.0 + vec2(0.2, -0.3) * iTime) * 0.04;\n"
"    p += sin(p.yx * 8.0 + vec2(0.6, 0.1) * iTime) * 0.01;\n"
"    p.x -= iTime / 1.1;\n"
"    \n"
"    // Multiple FBM layers for base flames\n"
"    float q = fbm(p - iTime * 0.3 + 1.0 * sin(iTime + 0.5) / 2.0);\n"
"    float qb = fbm(p - iTime * 0.4 + 0.1 * cos(iTime) / 2.0);\n"
"    float q2 = fbm(p - iTime * 0.44 - 5.0 * cos(iTime) / 2.0) - 6.0;\n"
"    float q3 = fbm(p - iTime * 0.9 - 10.0 * cos(iTime) / 15.0) - 4.0;\n"
"    float q4 = fbm(p - iTime * 1.4 - 20.0 * sin(iTime) / 14.0) + 2.0;\n"
"    q = (q + qb - 0.4 * q2 - 2.0 * q3 + 0.6 * q4) / 3.8;\n"
"    \n"
"    // Top flame layer for taller flames\n"
"    vec2 top_flame_p = vec2(adjusted_uv.x * 5.0 + sin(iTime * 0.8) * 0.2, (adjusted_uv.y + iTime * 1.2) * 16.0);\n"
"    float top_flame_q = fbm(top_flame_p - iTime * 0.3 + 1.0 * sin(iTime + 0.5) / 2.0);\n"
"    float top_flame_qb = fbm(top_flame_p - iTime * 0.4 + 0.1 * cos(iTime) / 2.0);\n"
"    float top_flame_q2 = fbm(top_flame_p - iTime * 0.44 - 5.0 * cos(iTime) / 2.0) - 6.0;\n"
"    float top_flame_q3 = fbm(top_flame_p - iTime * 0.9 - 10.0 * cos(iTime) / 15.0) - 4.0;\n"
"    float top_flame_q4 = fbm(top_flame_p - iTime * 1.4 - 20.0 * sin(iTime) / 14.0) + 2.0;\n"
"    float top_flame_noise = (top_flame_q + top_flame_qb - 0.4 * top_flame_q2 - 2.0 * top_flame_q3 + 0.6 * top_flame_q4) / 3.8;\n"
"    top_flame_noise = clamp(top_flame_noise * 1.2, 0.0, 1.0);\n"
"    \n"
"    // Flame shapes\n"
"    float dist_from_fire_line = adjusted_uv.y;\n"
"    float flame_shape = smoothstep(0.0, 0.3, dist_from_fire_line);\n"
"    float top_flame_shape = smoothstep(0.0, 1.0, dist_from_fire_line);\n"
"    \n"
"    // Flame intensities\n"
"    vec2 r = vec2(fbm(p + q / 2.0 + iTime * speed.x - p.x - p.y), fbm(p + q - iTime * speed.y));\n"
"    float flame_intensity = q * flame_shape;\n"
"    float flame_intensity_smooth = smoothstep(0.1, 0.95, flame_intensity);\n"
"    float top_flame_intensity = top_flame_noise * top_flame_shape;\n"
"    top_flame_intensity *= (1.0 + sin(iTime * 2.0 + adjusted_uv.x * 5.0) * 0.3);\n"
"    float top_flame_intensity_smooth = smoothstep(0.15, 0.95, top_flame_intensity);\n"
"    \n"
"    // Color blending\n"
"    vec3 fire_color = mix(c1, c2, fbm(p + r));\n"
"    fire_color = mix(c3, c4, r.x);\n"
"    fire_color = vec3(1.0 / pow(fire_color + 1.61, vec3(4.0)));\n"
"    fire_color = mix(fire_color, c7, pow(flame_intensity, 2.0) * 0.1);\n"
"    fire_color = vec3(1.0, 0.2, 0.05) / pow((r.y + r.y) * max(0.0, adjusted_uv.y) + 0.1, 4.0);\n"
"    fire_color += tex_color.xyz * 0.01 * pow((r.y + r.y) * 0.65, 5.0) * mix(vec3(0.9, 0.4, 0.3), vec3(0.7, 0.5, 0.2), adjusted_uv.y);\n"
"    fire_color = fire_color / (10.0 + max(vec3(0.0), fire_color));\n"
"    \n"
"    // Top flame color\n"
"    vec2 top_r = vec2(fbm(top_flame_p + top_flame_q / 2.0 + iTime * speed.x - top_flame_p.x - top_flame_p.y), fbm(top_flame_p + top_flame_q - iTime * speed.y));\n"
"    vec3 top_fire_color = mix(c2, c4, fbm(top_flame_p + top_r));\n"
"    top_fire_color += mix(c3, c4, top_r.x) * 0.5;\n"
"    top_fire_color = vec3(1.0 / pow(top_fire_color + 1.61, vec3(4.0)));\n"
"    top_fire_color = mix(top_fire_color, c7, pow(top_flame_intensity, 2.0) * 0.95);\n"
"    top_fire_color = top_fire_color / (1.0 + max(vec3(0.0), top_fire_color));\n"
"    \n"
"    // Flickering\n"
"    float flicker = fbm(vec2(iTime * 10.0 + adjusted_uv.x * 8.0, iTime * 7.0)) * 0.4 + 0.6;\n"
"    \n"
"    // Blend with background\n"
"    vec3 final_color = mix(vec3(0.0), fire_color, flame_intensity_smooth);\n"
"    final_color = mix(final_color, top_fire_color, top_flame_intensity_smooth * 0.01);\n"
"    float final_alpha = max(flame_intensity_smooth, top_flame_intensity_smooth);\n"
"    \n"
"    // Apply texture overlay at 60% opacity\n"
"    final_color = mix(final_color, tex_color.xyz, 0.2);\n"
"    \n"
"    // Make low-intensity areas transparent\n"
"    if(final_alpha < 0.1) {\n"
"        frag_color = vec4(0.0, 0.0, 0.0, 0.0).bgra;\n"
"        return;\n"
"    }\n"
"    \n"
"    frag_color = vec4(final_color*4.0, final_alpha).bgra;\n"
"}";   



// Vertex shader for simple colored rectangles
static const char *panel_vertex_shader_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 position;      // Vertex positions (0 to 1 for unit quad)\n"
    "in vec2 texcoord;      // Vertex texture/local coordinates (also 0 to 1 if VAO is set up for it)\n"
    "out vec2 v_texcoord;   // Pass local quad coordinates to fragment shader (for vignette)\n"
    "uniform mat3 mvp;\n"
    "void main() {\n"
    "    vec3 pos_transformed = mvp * vec3(position, 1.0);\n"
    "    gl_Position = vec4(pos_transformed.xy, 0.0, 1.0);\n"
    "    v_texcoord = texcoord; // Pass the VAO's texcoord attribute\n"
    "}\n";

static const char *panel_fragment_shader_src = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "\n"
    "in vec2 v_texcoord; // Normalized UV for the panel quad itself (0,0 to 1,1)\n"
    "out vec4 frag_color;\n"
    "\n"
    "// Time and resolution uniforms for octgrams animation\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "\n"
    "// Uniforms for the preview functionality\n"
    "uniform sampler2D u_previewTexture;     // The window texture for the preview\n"
    "uniform bool u_isPreviewActive;         // Flag: is there a preview to draw?\n"
    "uniform vec4 u_previewRect;           // Preview position and size on panel: x, y, w, h (normalized 0-1 relative to panel's own UVs)\n"
    "uniform mat3 u_previewTexTransform;   // Transform for sampling the preview texture\n"
    "\n"
    "// Octgrams animation variables and functions\n"
    "float gTime = 0.0;\n"
    "const float REPEAT = 5.0;\n"
    "\n"
    "mat2 rot(float a) {\n"
    "    float c = cos(a), s = sin(a);\n"
    "    return mat2(c,s,-s,c);\n"
    "}\n"
    "\n"
    "float sdBox( vec3 p, vec3 b ) {\n"
    "    vec3 q = abs(p) - b;\n"
    "    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);\n"
    "}\n"
    "\n"
    "float box(vec3 pos, float scale) {\n"
    "    pos *= scale;\n"
    "    float base = sdBox(pos, vec3(.4,.4,.1)) /1.5;\n"
    "    pos.xy *= 5.0;\n"
    "    pos.y -= 3.5;\n"
    "    pos.xy *= rot(.75);\n"
    "    float result = -base;\n"
    "    return result;\n"
    "}\n"
    "\n"
    "float box_set(vec3 pos, float iTime) {\n"
    "    vec3 pos_origin = pos;\n"
    "    pos = pos_origin;\n"
    "    pos.y += sin(gTime * 0.4) * 2.5;\n"
    "    pos.xy *= rot(.8);\n"
    "    float box1 = box(pos,2.0 - abs(sin(gTime * 0.4)) * 1.5);\n"
    "    pos = pos_origin;\n"
    "    pos.y -=sin(gTime * 0.4) * 2.5;\n"
    "    pos.xy *= rot(.8);\n"
    "    float box2 = box(pos,2.0 - abs(sin(gTime * 0.4)) * 1.5);\n"
    "    pos = pos_origin;\n"
    "    pos.x +=sin(gTime * 0.4) * 2.5;\n"
    "    pos.xy *= rot(.8);\n"
    "    float box3 = box(pos,2.0 - abs(sin(gTime * 0.4)) * 1.5);\n"
    "    pos = pos_origin;\n"
    "    pos.x -=sin(gTime * 0.4) * 2.5;\n"
    "    pos.xy *= rot(.8);\n"
    "    float box4 = box(pos,2.0 - abs(sin(gTime * 0.4)) * 1.5);\n"
    "    pos = pos_origin;\n"
    "    pos.xy *= rot(.8);\n"
    "    float box5 = box(pos,.5) * 6.0;\n"
    "    pos = pos_origin;\n"
    "    float box6 = box(pos,.5) * 6.0;\n"
    "    float result = max(max(max(max(max(box1,box2),box3),box4),box5),box6);\n"
    "    return result;\n"
    "}\n"
    "\n"
    "float map(vec3 pos, float iTime) {\n"
    "    vec3 pos_origin = pos;\n"
    "    float box_set1 = box_set(pos, iTime);\n"
    "    return box_set1;\n"
    "}\n"
    "\n"
    "vec4 getOctgramsColor() {\n"
    "    vec2 fragCoord = gl_FragCoord.xy;\n"
    "    float iTime = time;\n"
    "    \n"
    "    vec2 p = (fragCoord.xy * 2.0 - iResolution.xy) / min(iResolution.x, iResolution.y);\n"
    "    vec3 ro = vec3(0.0, -0.2, iTime * 4.0);\n"
    "    vec3 ray = normalize(vec3(p, 1.5));\n"
    "    ray.xy = ray.xy * rot(sin(iTime * .03) * 5.0);\n"
    "    ray.yz = ray.yz * rot(sin(iTime * .05) * .2);\n"
    "    float t = 0.1;\n"
    "    vec3 col = vec3(0.0);\n"
    "    float ac = 0.0;\n"
    "    \n"
    "    for (int i = 0; i < 99; i++){\n"
    "        vec3 pos = ro + ray * t;\n"
    "        pos = mod(pos-2.0, 4.0) -2.0;\n"
    "        gTime = iTime -float(i) * 0.01;\n"
    "        \n"
    "        float d = map(pos, iTime);\n"
    "        d = max(abs(d), 0.01);\n"
    "        ac += exp(-d*23.0);\n"
    "        t += d* 0.55;\n"
    "    }\n"
    "    \n"
    "    col = vec3(ac * 0.02);\n"
    "    col +=vec3(0.0,0.2 * abs(sin(iTime)),0.5 + sin(iTime) * 0.2);\n"
    "    \n"
    "    return vec4(col, 0.6);\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    // Start with the octgrams animation as the background\n"
    "    vec4 background_color = getOctgramsColor();\n"
    "    vec4 final_pixel_color = background_color;\n"
    "\n"
    "    // Layer the preview on top if active\n"
    "    if (u_isPreviewActive) {\n"
    "        // Check if we're inside the preview rectangle\n"
    "        bool is_inside_preview_x = (v_texcoord.x >= u_previewRect.x && v_texcoord.x < (u_previewRect.x + u_previewRect.z));\n"
    "        bool is_inside_preview_y = (v_texcoord.y >= u_previewRect.y && v_texcoord.y < (u_previewRect.y + u_previewRect.w));\n"
    "        \n"
    "        if (is_inside_preview_x && is_inside_preview_y) {\n"
    "            // We're inside the preview area - sample the preview texture\n"
    "            vec3 transformed_uv_homogeneous = u_previewTexTransform * vec3(v_texcoord, 1.0);\n"
    "            vec2 final_sample_uv = transformed_uv_homogeneous.xy;\n"
    "            \n"
    "            vec4 preview_sample = texture(u_previewTexture, final_sample_uv);\n"
    "            \n"
    "            if (preview_sample.a > 0.05) {\n"
    "                // Blend the preview over the background\n"
    "                final_pixel_color = mix(background_color, preview_sample, preview_sample.a);\n"
    "            }\n"
    "        }\n"
    "    }\n"
    "\n"
    "    frag_color = final_pixel_color.bgra;\n"
    "}";


    setenv("MESA_VK_VERSION_OVERRIDE", "1.2", 1);
    setenv("MESA_LOADER_DRIVER_OVERRIDE", "zink", 1);
   // setenv("GALLIUM_DRIVER", "zink", 1);
   // setenv("GALLIUM_DRIVER", "d3d12", 1); 
    setenv("ZINK_DEBUG", "nofp64,nofast_color_clear", 1);
    setenv("VK_DRIVER_FILES", "/usr/share/vulkan/icd.d/vulkan_icd.json", 1);
    setenv("ZINK_DESCRIPTORS", "lazy", 1);
    setenv("ZINK_NO_TIMELINES", "1", 1);
    setenv("ZINK_NO_DECOMPRESS", "1", 1);

    printf("Starting compositor with surfaceless EGL display\n");

    // Print environment variables
    printf("VK_DRIVER_FILES=%s\n", getenv("VK_DRIVER_FILES"));
    printf("MESA_LOADER_DRIVER_OVERRIDE=%s\n", getenv("MESA_LOADER_DRIVER_OVERRIDE"));
    printf("GALLIUM_DRIVER=%s\n", getenv("GALLIUM_DRIVER"));
    wlr_log_init(WLR_DEBUG, NULL);
    char *startup_cmd = NULL;

    int c;
    while ((c = getopt(argc, argv, "s:h")) != -1) {
        switch (c) {
        case 's':
            startup_cmd = optarg;
            break;
        default:
            printf("Usage: %s [-s startup command]\n", argv[0]);
            return 0;
        }
    }
    if (optind < argc) {
        printf("Usage: %s [-s startup command]\n", argv[0]);
        return 0;
    }

    struct tinywl_server server = {0};

// Initialize all lists
    wl_list_init(&server.toplevels);
    wl_list_init(&server.outputs);
    wl_list_init(&server.keyboards);
    wl_list_init(&server.popups);

    server.wl_display = wl_display_create();
    if (!server.wl_display) {
        wlr_log(WLR_ERROR, "Cannot create Wayland display");
        return 1;
    }


server.seat = wlr_seat_create(server.wl_display, "seat0");
    if (!server.seat) {
        wlr_log(WLR_ERROR, "Failed to create seat");
        wl_display_destroy(server.wl_display);
        return 1;
    }
    server.request_cursor.notify = seat_request_cursor;
    wl_signal_add(&server.seat->events.request_set_cursor, &server.request_cursor);
    server.request_set_selection.notify = seat_request_set_selection;
    wl_signal_add(&server.seat->events.request_set_selection, &server.request_set_selection);
    wlr_log(WLR_INFO, "Compositor seat 'seat0' created: %p", (void*)server.seat);


wlr_log(WLR_INFO, "Setting compositor display");
wlr_backend_set_compositor_display(server.wl_display);
    const char *backends_env = getenv("WLR_BACKENDS");
    if (backends_env && strcmp(backends_env, "RDP") == 0) {
        wlr_log(WLR_INFO, "Creating RDP backend");
//        server.backend = wlr_RDP_backend_create(server.wl_display);


 setenv("WLR_BACKENDS", "RDP",1);

server.backend = wlr_RDP_backend_create(server.wl_display);
  if (!server.backend) {
            wlr_log(WLR_ERROR, "Failed to create RDP backend");
            wlr_seat_destroy(server.seat); 
            wl_display_destroy(server.wl_display);
            return 1;
        }
        // ***** SET THE GLOBAL SEAT FOR RDP BACKEND *****
        wlr_RDP_backend_set_compositor_seat(server.seat); 
        wlr_log(WLR_INFO, "RDP Backend: Compositor seat '%s' (%p) set.", server.seat->name, (void*)server.seat);

    } else {
        wlr_log(WLR_INFO, "Autocreating backend (not RDP).");
        server.backend = wlr_backend_autocreate(wl_display_get_event_loop(server.wl_display), NULL);
        if (!server.backend) {
            wlr_log(WLR_ERROR, "Failed to autocreate backend");
            wlr_seat_destroy(server.seat); 
            wl_display_destroy(server.wl_display);
            return 1;
        }
    }

    server.renderer = wlr_gles2_renderer_create_surfaceless();
    if (server.renderer == NULL) {
        wlr_log(WLR_ERROR, "failed to create wlr_renderer");
        return 1;
    }

  

const char *vendor = (const char *)glGetString(GL_VENDOR);
    const char *renderer = (const char *)glGetString(GL_RENDERER);
    const char *version = (const char *)glGetString(GL_VERSION);
    const char *shading_lang = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);

    wlr_log(WLR_INFO, "GL_VENDOR: %s", vendor);
    wlr_log(WLR_INFO, "GL_RENDERER: %s", renderer);
    wlr_log(WLR_INFO, "GL_VERSION: %s", version);
    wlr_log(WLR_INFO, "GL_SHADING_LANGUAGE_VERSION: %s", shading_lang);


    wlr_renderer_init_wl_display(server.renderer, server.wl_display);



 // --- Create Flame Shader ---
    struct shader_uniform_spec flame_uniforms[] = {
        {"mvp", &server.flame_shader_mvp_loc},
        {"texture_sampler_uniform", &server.flame_shader_tex_loc},
        {"time", &server.flame_shader_time_loc},
        {"iResolution", &server.flame_shader_res_loc}
    };
    if (!create_generic_shader_program(server.renderer, "FlameShader",
                                     vertex_shader_src, fragment_shader_src, // Use your actual flame shader sources
                                     &server.shader_program,
                                     flame_uniforms, sizeof(flame_uniforms) / sizeof(flame_uniforms[0]))) {
        wlr_log(WLR_ERROR, "Failed to create flame shader program.");
        server_destroy(&server); return 1;
    }

    // --- Create "70s Melt" Rectangle Shader ---
    struct shader_uniform_spec melt_rect_uniforms[] = {
        {"mvp", &server.rect_shader_mvp_loc},
        {"u_color", &server.rect_shader_color_loc},
        {"time", &server.rect_shader_time_loc},
        {"iResolution", &server.rect_shader_resolution_loc}
    };
    if (!create_generic_shader_program(server.renderer, "MeltRectShader",
                                     rect_vertex_shader_src, // Your 70s melt VS
                                     rect_fragment_shader_src, // Your 70s melt FS
                                     &server.rect_shader_program,
                                     melt_rect_uniforms, sizeof(melt_rect_uniforms) / sizeof(melt_rect_uniforms[0]))) {
        wlr_log(WLR_ERROR, "Failed to create '70s Melt' rect shader program.");
        server_destroy(&server); return 1;
    }

    // --- Create Panel Shader (Stripes) ---
    struct shader_uniform_spec panel_uniforms[] = {
        {"mvp", &server.panel_shader_mvp_loc},                       // For mat3
        {"time", &server.panel_shader_time_loc},                     // For float
        {"iResolution", &server.panel_shader_resolution_loc},        // For vec2
        {"u_previewTexture", &server.panel_shader_preview_tex_loc},  // For sampler2D (int)
        {"u_isPreviewActive", &server.panel_shader_is_preview_active_loc}, // For bool (int)
        {"u_previewRect", &server.panel_shader_preview_rect_loc},          // For vec4
        {"u_previewTexTransform", &server.panel_shader_preview_tex_transform_loc} // For mat3
    };
    if (!create_generic_shader_program(server.renderer, "PanelStripeShader",
                                     panel_vertex_shader_src, panel_fragment_shader_src,
                                     &server.panel_shader_program,
                                     panel_uniforms, sizeof(panel_uniforms) / sizeof(panel_uniforms[0]))) {
        wlr_log(WLR_ERROR, "Failed to create panel stripe shader program.");
        server_destroy(&server); return 1;
    }

    // --- VAO Setup ---
    // Ensure EGL context is current for VAO operations.
    // create_generic_shader_program restores the context, so make it current again.
    struct wlr_egl *egl_main = wlr_gles2_renderer_get_egl(server.renderer);
    if (wlr_egl_make_current(egl_main, NULL)) {
        // Your existing VAO setup code...
        // When setting attribute pointers, you need a program active that uses those attributes
        // OR use layout(location=N) in GLSL and fixed locations in glVertexAttribPointer.
        // Since you get locations by name, make one program active:
        glUseProgram(server.shader_program); // or rect_shader_program, or panel_shader_program
                                           // assuming "position" and "texcoord" are common attribute names.

        GLint pos_loc = glGetAttribLocation(server.shader_program, "position");
        if (pos_loc != -1) {
            glBindBuffer(GL_ARRAY_BUFFER, server.quad_vbo);
            glEnableVertexAttribArray(pos_loc);
            glVertexAttribPointer(pos_loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
        }
        GLint tex_loc = glGetAttribLocation(server.shader_program, "texcoord");
        if (tex_loc != -1) {
            glBindBuffer(GL_ARRAY_BUFFER, server.quad_vbo);
            glEnableVertexAttribArray(tex_loc);
            glVertexAttribPointer(tex_loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
        }
        glUseProgram(0);
        glBindVertexArray(0);
        wlr_egl_unset_current(egl_main);
    } else {
         wlr_log(WLR_ERROR, "Failed to make EGL context current for VAO setup.");
         server_destroy(&server); return 1;
    }
    GLenum err_main;
    if (wlr_egl_make_current(egl_main, NULL)) {
        while ((err_main = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main A (after make_current): 0x%x", err_main); }

        const GLfloat verts[] = { 
            0.0f, 0.0f, 0.0f, 1.0f, 
            1.0f, 0.0f, 1.0f, 1.0f, 
            0.0f, 1.0f, 0.0f, 0.0f, 
            1.0f, 1.0f, 1.0f, 0.0f, 
        };
        const GLuint indices[] = {0, 1, 2, 1, 3, 2}; 

        glGenVertexArrays(1, &server.quad_vao);
        while ((err_main = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main B (after GenVertexArrays): 0x%x", err_main); }
        glBindVertexArray(server.quad_vao);
        wlr_log(WLR_DEBUG, "MAIN_VAO_SETUP: VAO ID: %d bound", server.quad_vao);
        while ((err_main = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main C (after BindVertexArray): 0x%x", err_main); }

        glGenBuffers(1, &server.quad_vbo);
        while ((err_main = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main D (after GenBuffers VBO): 0x%x", err_main); }
        glBindBuffer(GL_ARRAY_BUFFER, server.quad_vbo); 
        wlr_log(WLR_DEBUG, "MAIN_VAO_SETUP: VBO ID: %d bound to GL_ARRAY_BUFFER", server.quad_vbo);
        while ((err_main = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main E (after BindBuffer VBO): 0x%x", err_main); }
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        while ((err_main = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main F (after BufferData VBO): 0x%x", err_main); }

        glGenBuffers(1, &server.quad_ibo);
        while ((err_main = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main G (after GenBuffers IBO): 0x%x", err_main); }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, server.quad_ibo); 
        wlr_log(WLR_DEBUG, "MAIN_VAO_SETUP: IBO ID: %d bound to GL_ELEMENT_ARRAY_BUFFER (associated with VAO %d)", server.quad_ibo, server.quad_vao);
        while ((err_main = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main H (after BindBuffer IBO): 0x%x", err_main); }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        while ((err_main = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main I (after BufferData IBO): 0x%x", err_main); }

        GLint pos_attrib = glGetAttribLocation(server.shader_program, "position");
        wlr_log(WLR_DEBUG, "MAIN_VAO_SETUP: 'position' attrib location: %d", pos_attrib);
        if (pos_attrib != -1) {
            glEnableVertexAttribArray(pos_attrib);
            glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
        } else { wlr_log(WLR_ERROR, "Position attribute NOT FOUND"); }
        while ((err_main = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main J (after position attrib setup): 0x%x", err_main); }

        GLint texcoord_attrib = glGetAttribLocation(server.shader_program, "texcoord");
        wlr_log(WLR_DEBUG, "MAIN_VAO_SETUP: 'texcoord' attrib location: %d", texcoord_attrib);
        if (texcoord_attrib != -1) { 
            glEnableVertexAttribArray(texcoord_attrib);
            glVertexAttribPointer(texcoord_attrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
        } else { wlr_log(WLR_INFO, "Texcoord attribute NOT FOUND (OK for solid color shader but VS still declares it)."); }
        while ((err_main = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main K (after texcoord attrib setup): 0x%x", err_main); }

        glBindVertexArray(0); 
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
        wlr_egl_unset_current(egl_main);
        while ((err_main = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error after VAO setup in main (unbinding): 0x%x", err_main); }
    } else { 
        wlr_log(WLR_ERROR, "Failed to make EGL context current for quad VBO setup in main");
        server_destroy(&server);
        // return 1; // Or handle error appropriately
    }
    

    server.allocator = wlr_allocator_autocreate(server.backend, server.renderer);
    if (!server.allocator) {
        wlr_log(WLR_ERROR, "Failed to create allocator");
        server_destroy(&server);
        return 1;
    }


 
    wlr_compositor_create(server.wl_display, 5, server.renderer);
    wlr_subcompositor_create(server.wl_display);
    wlr_data_device_manager_create(server.wl_display);

    server.output_layout = wlr_output_layout_create(server.wl_display);
    wl_list_init(&server.outputs);
    server.new_output.notify = server_new_output;
    wl_signal_add(&server.backend->events.new_output, &server.new_output);

   

 server.scene = wlr_scene_create();
    server.scene_layout = wlr_scene_attach_output_layout(server.scene, server.output_layout);
   
    wl_list_init(&server.toplevels);
    wl_list_init(&server.popups);

    server.xdg_shell = wlr_xdg_shell_create(server.wl_display, 3);
    server.new_xdg_toplevel.notify = server_new_xdg_toplevel;
    wl_signal_add(&server.xdg_shell->events.new_toplevel, &server.new_xdg_toplevel);
    server.new_xdg_popup.notify = server_new_xdg_popup;
    wl_signal_add(&server.xdg_shell->events.new_popup, &server.new_xdg_popup);



 


    server.cursor = wlr_cursor_create();
    wlr_cursor_attach_output_layout(server.cursor, server.output_layout);
    server.cursor_mgr = wlr_xcursor_manager_create(NULL, 24);
    wlr_xcursor_manager_load(server.cursor_mgr, 1);

    server.cursor_mode = TINYWL_CURSOR_PASSTHROUGH;
    server.cursor_motion.notify = server_cursor_motion;
    wl_signal_add(&server.cursor->events.motion, &server.cursor_motion);
    server.cursor_motion_absolute.notify = server_cursor_motion_absolute;
    wl_signal_add(&server.cursor->events.motion_absolute, &server.cursor_motion_absolute);
    server.cursor_button.notify = server_cursor_button;
    wl_signal_add(&server.cursor->events.button, &server.cursor_button);
    server.cursor_axis.notify = server_cursor_axis;
    wl_signal_add(&server.cursor->events.axis, &server.cursor_axis);
    server.cursor_frame.notify = server_cursor_frame;
    wl_signal_add(&server.cursor->events.frame, &server.cursor_frame);

    wl_list_init(&server.keyboards);
    server.new_input.notify = server_new_input;
    wl_signal_add(&server.backend->events.new_input, &server.new_input);
 

desktop_background(&server);
desktop_panel(&server);

    const char *socket = wl_display_add_socket_auto(server.wl_display);
    if (!socket) {
        wlr_log(WLR_ERROR, "Unable to create wayland socket");
        wlr_seat_destroy(server.seat);
        wlr_xcursor_manager_destroy(server.cursor_mgr);
        wlr_cursor_destroy(server.cursor);
        wlr_output_layout_destroy(server.output_layout);
        wlr_allocator_destroy(server.allocator);
        wlr_renderer_destroy(server.renderer);
        wlr_backend_destroy(server.backend);
        wl_display_destroy(server.wl_display);
        return 1;
    }

    if (!wlr_backend_start(server.backend)) {
        wlr_log(WLR_ERROR, "Failed to start backend");
        server_destroy(&server);
        return 1;
    }

    setenv("WAYLAND_DISPLAY", socket, true);
    if (startup_cmd) {
        if (fork() == 0) {
            execl("/bin/sh", "/bin/sh", "-c", startup_cmd, (void *)NULL);
        }
    }



    wlr_log(WLR_INFO, "Running Wayland compositor on WAYLAND_DISPLAY=%s", socket);



    wl_display_run(server.wl_display);

    wl_display_destroy_clients(server.wl_display);
    server_destroy(&server);
    return 0;
}