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

#include <wlr/types/wlr_xdg_decoration_v1.h>


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
#include <string.h> // For memcpy
#include <math.h>   // For round
#include <wlr/types/wlr_matrix.h>
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

#include <wlr/render/egl.h>

#include <wlr/render/wlr_renderer.h> // For wlr_renderer functions
      // For wlr_matrix_project_box
#include <wlr/types/wlr_scene.h>    // For scene graph functions
#include <GLES3/gl31.h>             // For OpenGL ES 3.1

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

#define PROTOCOL_DECORATION_MODE_NONE 0
#define PROTOCOL_DECORATION_MODE_SERVER_SIDE 1
#define PROTOCOL_DECORATION_MODE_CLIENT_SIDE 2


#ifndef RENDER_EGL_H
#define RENDER_EGL_H


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



struct render_data {
    struct wlr_render_pass *pass;
    struct wlr_renderer *renderer;
    GLuint shader_program;
       GLint scale_uniform_loc; // Renamed to avoid conflict
    struct tinywl_server *server;
    struct wlr_output *output; // Add output for projection matrix
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
    GLint rect_shader_rect_res_loc;  

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


// NEW: Shader for SSDs
    GLuint ssd_shader_program;
    GLint ssd_shader_mvp_loc;
    GLint ssd_shader_color_loc;
    GLint ssd_shader_time_loc;                  // NEW for SSD shader
    GLint ssd_shader_rect_pixel_dimensions_loc; // Was already there, now used as 

    struct wlr_scene_node *main_background_node;

    GLuint back_shader_program;
    GLint back_shader_mvp_loc;
    GLint back_shader_time_loc;
    GLint back_shader_base_color_loc; // u_panel_base_color
    GLint back_shader_resolution_loc;
    GLint back_shader_tex_loc ;
    GLint back_shader_res_loc;

    // Pointer to the top panel's scene node for easy identification
    struct wlr_scene_node *top_panel_node;

     // Custom shader program for animations
    GLint scale_uniform;  // Location of the scale uniform in the shader

    // For shared quad rendering
    GLuint quad_vao;
    GLuint quad_vbo;
    GLuint quad_ibo;


 

    struct wlr_xdg_decoration_manager_v1 *xdg_decoration_manager;
struct wl_listener xdg_decoration_new;
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

struct tinywl_decoration { // Your struct for SSD scene rects
    struct tinywl_toplevel *toplevel;
    struct wlr_scene_rect *title_bar;
    struct wlr_scene_rect *border_top;
     bool enabled;
 
        struct wlr_scene_rect *border_left;
        struct wlr_scene_rect *border_right;
        struct wlr_scene_rect *border_bottom;



};

#define TITLE_BAR_HEIGHT 30
#define BORDER_WIDTH 4

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

struct wlr_xdg_toplevel_decoration_v1 *decoration;
    struct wl_listener decoration_destroy;
    struct wl_listener decoration_request_mode;

     struct tinywl_decoration ssd; 

     struct wl_listener decoration_destroy_listener;
     struct wl_listener decoration_request_mode_listener;

     // Decoration fields
     bool ssd_pending_enable;
    
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

// Forward declarations (ensure these are before server_new_xdg_decoration)
static void decoration_handle_destroy(struct wl_listener *listener, void *data);
static void decoration_handle_request_mode(struct wl_listener *listener, void *data);
// Helper function to update decoration geometry
static void update_decoration_geometry(struct tinywl_toplevel *toplevel);
 void ensure_ssd_enabled(struct tinywl_toplevel *toplevel);

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
    wlr_cursor_attach_input_device(server->cursor, device); // This is good, adds device to cursor

    // The wlr_cursor itself will emit motion events that aggregate all attached inputs.
    // You are already listening to server->cursor->events.motion in main().
    // So, you might not even need to listen to individual pointer->events.motion here.

    // If you *do* want to listen to individual raw pointer events:
    struct wlr_pointer *pointer = wlr_pointer_from_input_device(device);
    if (pointer) {
        // If you absolutely need a per-device listener that calls server_pointer_motion,
        // then server_pointer_motion CANNOT use wl_container_of with server.pointer_motion.
        // It would need a different way to get the 'server' instance.

        // For now, let's assume the listener on server.cursor->events.motion in main()
        // is sufficient for cursor movement. We can remove the direct listener on
        // pointer->events.motion to avoid the conflict if server.cursor handles aggregation.

        wlr_log(WLR_DEBUG, "Pointer device %p attached to cursor. Cursor will aggregate its events.", device);
        wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default"); // Set cursor image for this device if needed
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


// You can replace or augment desktop_toplevel_at.
// This new function tries to find the toplevel owner of any scene node.
static struct tinywl_toplevel *get_toplevel_from_scene_node(struct wlr_scene_node *node) {
    if (!node) {
        return NULL;
    }
    // Traverse up the scene graph to find a node that has a tinywl_toplevel in its data field
    // and ensure that node is the toplevel->scene_tree.
    struct wlr_scene_tree *tree = node->parent;
    while (tree != NULL) {
        if (tree->node.data != NULL) {
            struct tinywl_toplevel *toplevel_candidate = tree->node.data;
            // Verify this is indeed the main scene_tree of the toplevel
            if (toplevel_candidate->scene_tree == tree) {
                return toplevel_candidate;
            }
        }
        tree = tree->node.parent;
    }
    // If the node itself is a toplevel's scene_tree
    if (node->type == WLR_SCENE_NODE_TREE && node->data != NULL) {
        struct tinywl_toplevel *toplevel_candidate = node->data;
        if (toplevel_candidate->scene_tree == (struct wlr_scene_tree *)node) {
            return toplevel_candidate;
        }
    }
    return NULL;
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


static void process_cursor_move(struct tinywl_server *server) { // This is called when server->cursor_mode == TINYWL_CURSOR_MOVE
    struct tinywl_toplevel *toplevel = server->grabbed_toplevel;
    if (!toplevel) return; // Should not happen in move mode

    // New position for the toplevel's scene_tree
    int new_x = server->cursor->x - server->grab_x;
    int new_y = server->cursor->y - server->grab_y;

    wlr_scene_node_set_position(&toplevel->scene_tree->node, new_x, new_y);
    wlr_log(WLR_DEBUG, "Moving toplevel to (%d, %d)", new_x, new_y);
}

// --- Modified process_cursor_resize ---
static void process_cursor_resize(struct tinywl_server *server) {
    struct tinywl_toplevel *toplevel = server->grabbed_toplevel;
    if (!toplevel || toplevel->type != TINYWL_TOPLEVEL_XDG || !toplevel->xdg_toplevel) {
        return;
    }

    // border_x and border_y are the new target screen coordinates for the
    // *content edges* being dragged, based on initial grab offset.
    double border_x_target = server->cursor->x - server->grab_x;
    double border_y_target = server->cursor->y - server->grab_y;

    // Initialize new content dimensions/positions with the initial grab_geobox (content screen geo)
    int new_content_left_screen = server->grab_geobox.x;
    int new_content_top_screen = server->grab_geobox.y;
    int new_content_width = server->grab_geobox.width;
    int new_content_height = server->grab_geobox.height;

    if (server->resize_edges & WLR_EDGE_TOP) {
        new_content_top_screen = border_y_target;
        new_content_height = (server->grab_geobox.y + server->grab_geobox.height) - new_content_top_screen;
    } else if (server->resize_edges & WLR_EDGE_BOTTOM) {
        new_content_height = border_y_target - new_content_top_screen;
    }

    if (server->resize_edges & WLR_EDGE_LEFT) {
        new_content_left_screen = border_x_target;
        new_content_width = (server->grab_geobox.x + server->grab_geobox.width) - new_content_left_screen;
    } else if (server->resize_edges & WLR_EDGE_RIGHT) {
        new_content_width = border_x_target - new_content_left_screen;
    }

    // Add minimum size constraints for content
    int min_width = 50;  // Minimum content width
    int min_height = 30; // Minimum content height
    if (new_content_width < min_width) {
        if (server->resize_edges & WLR_EDGE_LEFT) {
            new_content_left_screen -= (min_width - new_content_width);
        }
        new_content_width = min_width;
    }
    if (new_content_height < min_height) {
         if (server->resize_edges & WLR_EDGE_TOP) {
            new_content_top_screen -= (min_height - new_content_height);
        }
        new_content_height = min_height;
    }

    // Determine new position for the scene_tree (top-left of entire decorated window)
    int scene_tree_new_x = new_content_left_screen;
    int scene_tree_new_y = new_content_top_screen;

    if (toplevel->ssd.enabled) {
        scene_tree_new_x -= BORDER_WIDTH;
        scene_tree_new_y -= TITLE_BAR_HEIGHT;
    }
    // else: scene_tree_new_x/y are already correct as content_left/top if no SSD

    wlr_scene_node_set_position(&toplevel->scene_tree->node, scene_tree_new_x, scene_tree_new_y);
    wlr_xdg_toplevel_set_size(toplevel->xdg_toplevel, new_content_width, new_content_height);

    // update_decoration_geometry will be called on the next commit from the client
    // which will correctly size and position SSDs around the new content size,
    // and position client_xdg_scene_tree within the main scene_tree.
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
    // THIS listener *IS* server.cursor_motion from the struct tinywl_server
    struct tinywl_server *server = wl_container_of(listener, server, cursor_motion); // Correct use of server.cursor_motion
    struct wlr_pointer_motion_event *event = data;

    // The 'dev' field in wlr_pointer_motion_event from wlr_cursor might be NULL
    // if the motion was synthetic (e.g. warp). Use server->cursor->active_pointer_device if needed.
    // For wlr_cursor_move, the device argument is optional if you just want to move based on deltas.
    wlr_cursor_move(server->cursor, NULL /* or &event->pointer->base if from a specific device */,
                    event->delta_x, event->delta_y);
    process_cursor_motion(server, event->time_msec);
}

static void server_cursor_motion_absolute(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_motion_absolute);
    struct wlr_pointer_motion_absolute_event *event = data;
    wlr_cursor_warp_absolute(server->cursor, &event->pointer->base, event->x, event->y);
    process_cursor_motion(server, event->time_msec);
}



// --- Modified server_cursor_button ---
static void server_cursor_button(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, cursor_button);
    struct wlr_pointer_button_event *event = data;
    struct wlr_seat *seat = server->seat;

    wlr_log(WLR_DEBUG, "Button: button=%u, state=%s, time=%u, cursor_mode=%d",
            event->button, event->state == WL_POINTER_BUTTON_STATE_PRESSED ? "pressed" : "released",
            event->time_msec, server->cursor_mode);

    double sx_at_node, sy_at_node; // Renamed to avoid conflict with sx, sy for surface
    struct wlr_scene_node *scene_node_at_cursor = wlr_scene_node_at(
        &server->scene->tree.node, server->cursor->x, server->cursor->y, &sx_at_node, &sy_at_node);

    struct tinywl_toplevel *toplevel = NULL;
    if (scene_node_at_cursor) {
        toplevel = get_toplevel_from_scene_node(scene_node_at_cursor);
    }

    if (event->state == WL_POINTER_BUTTON_STATE_RELEASED) {
        if (server->cursor_mode == TINYWL_CURSOR_MOVE || server->cursor_mode == TINYWL_CURSOR_RESIZE) {
            server->cursor_mode = TINYWL_CURSOR_PASSTHROUGH;
            server->grabbed_toplevel = NULL;
            wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
            wlr_log(WLR_DEBUG, "Ended interactive move/resize.");
        }
        wlr_seat_pointer_notify_button(seat, event->time_msec, event->button, event->state);
    } else if (event->state == WL_POINTER_BUTTON_STATE_PRESSED) {
        bool button_handled_by_compositor = false;

        if (toplevel && toplevel->type == TINYWL_TOPLEVEL_XDG && toplevel->xdg_toplevel && toplevel->ssd.enabled && event->button == BTN_LEFT) {
            uint32_t current_resize_edges = 0;
            const char *cursor_to_set = NULL;

            if (scene_node_at_cursor == &toplevel->ssd.title_bar->node) {
                focus_toplevel(toplevel);
                server->cursor_mode = TINYWL_CURSOR_MOVE;
                server->grabbed_toplevel = toplevel;
                server->grab_x = server->cursor->x - toplevel->scene_tree->node.x;
                server->grab_y = server->cursor->y - toplevel->scene_tree->node.y;
                wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "grabbing");
                button_handled_by_compositor = true;
            } else if (toplevel->ssd.border_left && scene_node_at_cursor == &toplevel->ssd.border_left->node) {
                current_resize_edges = WLR_EDGE_LEFT;
                cursor_to_set = "left_side";
            } else if (toplevel->ssd.border_right && scene_node_at_cursor == &toplevel->ssd.border_right->node) {
                current_resize_edges = WLR_EDGE_RIGHT;
                cursor_to_set = "right_side";
            } else if (toplevel->ssd.border_bottom && scene_node_at_cursor == &toplevel->ssd.border_bottom->node) {
                current_resize_edges = WLR_EDGE_BOTTOM;
                cursor_to_set = "bottom_side";
            }
            // TODO: Add corner checks if desired, e.g.:
            // else if (cursor near bottom-left corner of SSD) {
            //    current_resize_edges = WLR_EDGE_BOTTOM | WLR_EDGE_LEFT;
            //    cursor_to_set = "bottom_left_corner";
            // }

            if (current_resize_edges != 0) {
                focus_toplevel(toplevel);
                server->cursor_mode = TINYWL_CURSOR_RESIZE;
                server->grabbed_toplevel = toplevel;
                server->resize_edges = current_resize_edges;

                // Store grab_geobox as the current *content* geometry in screen coordinates
                struct wlr_box *client_geo = &toplevel->xdg_toplevel->base->geometry; // Usually 0,0 relative to its own surface
                // Get scene_tree position (top-left of whole decorated window)
                int scene_tree_x = toplevel->scene_tree->node.x;
                int scene_tree_y = toplevel->scene_tree->node.y;

                // Calculate content top-left in screen coordinates
                server->grab_geobox.x = scene_tree_x + BORDER_WIDTH;
                server->grab_geobox.y = scene_tree_y + TITLE_BAR_HEIGHT;
                server->grab_geobox.width = toplevel->xdg_toplevel->current.width;  // Use current size
                server->grab_geobox.height = toplevel->xdg_toplevel->current.height; // Use current size

                // Store grab_x/y as offset from cursor to the *corresponding content edge*
                if (current_resize_edges & WLR_EDGE_LEFT) {
                    server->grab_x = server->cursor->x - server->grab_geobox.x;
                }
                if (current_resize_edges & WLR_EDGE_RIGHT) {
                    server->grab_x = server->cursor->x - (server->grab_geobox.x + server->grab_geobox.width);
                }
                // No top edge SSD resize for now
                if (current_resize_edges & WLR_EDGE_BOTTOM) {
                    server->grab_y = server->cursor->y - (server->grab_geobox.y + server->grab_geobox.height);
                }
                
                if (cursor_to_set) {
                    wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, cursor_to_set);
                }
                wlr_log(WLR_INFO, "Grabbed toplevel '%s' for RESIZE (edges: %u). Grab box (content screen): %d,%d %dx%d. Grab offset x:%.1f y:%.1f",
                    toplevel->xdg_toplevel->title ? toplevel->xdg_toplevel->title : "N/A",
                    current_resize_edges, server->grab_geobox.x, server->grab_geobox.y,
                    server->grab_geobox.width, server->grab_geobox.height, server->grab_x, server->grab_y);
                button_handled_by_compositor = true;
            }
        }


        if (!button_handled_by_compositor) {
            struct wlr_surface *surface_under_cursor = NULL;
            double sx_surf, sy_surf; // For surface-local coordinates
            if (scene_node_at_cursor && scene_node_at_cursor->type == WLR_SCENE_NODE_BUFFER) {
                struct wlr_scene_buffer *sbuf = wlr_scene_buffer_from_node(scene_node_at_cursor);
                struct wlr_scene_surface *ssurf = wlr_scene_surface_try_from_buffer(sbuf);
                if (ssurf) {
                    surface_under_cursor = ssurf->surface;
                    // sx_at_node, sy_at_node are already surface-local from wlr_scene_node_at
                    sx_surf = sx_at_node;
                    sy_surf = sy_at_node;
                }
            }

            if (surface_under_cursor) {
                if (toplevel) {
                    focus_toplevel(toplevel);
                }
                wlr_seat_pointer_notify_enter(seat, surface_under_cursor, sx_surf, sy_surf);
                wlr_seat_pointer_notify_button(seat, event->time_msec, event->button, event->state);
            } else {
                wlr_seat_pointer_clear_focus(seat);
                 if (seat->keyboard_state.focused_surface) {
                     wlr_seat_keyboard_clear_focus(seat);
                 }
            }
        }
    }

    struct tinywl_output *output_iter;
    wl_list_for_each(output_iter, &server->outputs, link) {
        if (output_iter->wlr_output && output_iter->wlr_output->enabled) {
            wlr_output_schedule_frame(output_iter->wlr_output);
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








float get_monotonic_time_seconds_as_float(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
        wlr_log_errno(WLR_ERROR, "clock_gettime failed");
        return 0.0f;
    }
    return (float)ts.tv_sec + (float)ts.tv_nsec / 1e9f;
}











void desktop_background(struct tinywl_server *server) {
    wlr_log(WLR_INFO, "=== DESKTOP BACKGROUND (MELT SHADER) ===");
    float bg_color[4] = {1.0f, 0.0f, 0.0f, 1.0f}; // Base color for the Melt shader
    struct wlr_scene_rect *bg_rect = wlr_scene_rect_create(&server->scene->tree, 1024, 768, bg_color);
    if (bg_rect) {
        server->main_background_node = &bg_rect->node; // Store the pointer
        wlr_scene_node_set_position(&bg_rect->node, 0, 0);
        wlr_scene_node_set_enabled(&bg_rect->node, true);
        wlr_log(WLR_INFO, "Main background rect created at (0,0), node: %p", (void*)server->main_background_node);
    } else {
        server->main_background_node = NULL;
        wlr_log(WLR_ERROR, "Failed to create main background rect.");
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

// scene_buffer_iterator for MAIN WINDOWS ONLY (using windowback Shader)
static void scene_buffer_iterator_main_window_only(
        struct wlr_scene_buffer *scene_buffer,
        int sx, int sy,
        void *user_data) {

    struct render_data *rdata = user_data;
    struct wlr_renderer *renderer = rdata->renderer;
    struct wlr_output *output = rdata->output;
    struct tinywl_server *server = rdata->server;
    struct wlr_gles2_texture_attribs tex_attribs;
    const char *output_name_log = output ? output->name : "UNKNOWN_OUTPUT_MAIN_ITER";

    // --- Basic Validation ---
    if (!rdata || !server || !renderer || !output) {
        wlr_log(WLR_ERROR, "[SITER_MAIN_ONLY:%s] Invalid render_data, server, renderer, or output.", output_name_log);
        return;
    }
    if (!scene_buffer) {
        wlr_log(WLR_ERROR, "[SITER_MAIN_ONLY:%s] scene_buffer is NULL.", output_name_log);
        return;
    }
    if (!scene_buffer->node.enabled) {
        // This buffer node is part of the scene but currently disabled, skip rendering.
        // This is normal, e.g., for an unmapped window.
        return;
    }
    if (output->width == 0 || output->height == 0) {
        wlr_log(WLR_ERROR, "[SITER_MAIN_ONLY:%s] Output has zero width/height.", output_name_log);
        return;
    }
    if (!scene_buffer->buffer) {
        wlr_log(WLR_DEBUG, "[SITER_MAIN_ONLY:%s] scene_buffer %p has NULL wlr_buffer, skipping.", output_name_log, (void*)scene_buffer);
        return;
    }

    // --- Find Associated Toplevel for Animation ---
    struct tinywl_toplevel *toplevel_for_anim = NULL;
    struct wlr_scene_node *current_node_for_owner_search = &scene_buffer->node;
    int depth = 0;
    while(current_node_for_owner_search && depth < 10) { // Depth limit for safety
        if (current_node_for_owner_search->data != NULL) {
            struct tinywl_toplevel *potential_tl = current_node_for_owner_search->data;
            struct tinywl_toplevel *check_tl_in_list;
            bool is_known_and_correct_toplevel = false;
            wl_list_for_each(check_tl_in_list, &server->toplevels, link) {
                if (check_tl_in_list == potential_tl) {
                    // Check if this is the toplevel's main scene tree node
                    if (potential_tl->scene_tree == (struct wlr_scene_tree*)current_node_for_owner_search) {
                        is_known_and_correct_toplevel = true;
                        break;
                    }
                }
            }
            if (is_known_and_correct_toplevel) {
                toplevel_for_anim = potential_tl;
                break;
            }
        }
        if (!current_node_for_owner_search->parent) break;
        current_node_for_owner_search = &current_node_for_owner_search->parent->node;
        depth++;
    }

    const char* tl_title_for_log = "UNKNOWN/NON-TOPLEVEL";
    if (toplevel_for_anim && toplevel_for_anim->xdg_toplevel && toplevel_for_anim->xdg_toplevel->title) {
        tl_title_for_log = toplevel_for_anim->xdg_toplevel->title;
    } else if (toplevel_for_anim) {
        tl_title_for_log = "TOPLEVEL_NO_TITLE";
    }
    wlr_log(WLR_DEBUG, "[SITER_MAIN_ONLY:%s] Processing buffer for '%s' (toplevel_for_anim: %p)",
            output_name_log, tl_title_for_log, (void*)toplevel_for_anim);


    // --- Animation Scaling ---
    float anim_scale_factor = 1.0f;
    if (toplevel_for_anim) {
        if (toplevel_for_anim->is_animating) {
            struct timespec now_anim;
            clock_gettime(CLOCK_MONOTONIC, &now_anim);
            float current_time_anim = now_anim.tv_sec + now_anim.tv_nsec / 1e9f;
            float elapsed = current_time_anim - toplevel_for_anim->animation_start;
            if (elapsed < 0.0f) elapsed = 0.0f;
            float t = 0.0f;
            if (toplevel_for_anim->animation_duration > 1e-5f) {
                t = elapsed / toplevel_for_anim->animation_duration;
            } else if (elapsed > 0) { t = 1.0f; }

            if (t >= 1.0f) {
                t = 1.0f;
                anim_scale_factor = toplevel_for_anim->target_scale;
                if (toplevel_for_anim->target_scale == 0.0f && !toplevel_for_anim->pending_destroy) {
                    toplevel_for_anim->scale = 0.0f;
                    wlr_output_schedule_frame(output);
                } else {
                    toplevel_for_anim->is_animating = false;
                    toplevel_for_anim->scale = toplevel_for_anim->target_scale;
                }
            } else {
                anim_scale_factor = toplevel_for_anim->scale + (toplevel_for_anim->target_scale - toplevel_for_anim->scale) * t;
            }
            if (toplevel_for_anim->is_animating) {
                wlr_output_schedule_frame(output);
            }
        } else {
            anim_scale_factor = toplevel_for_anim->scale;
            if (toplevel_for_anim->target_scale == 0.0f && toplevel_for_anim->scale <= 0.001f && !toplevel_for_anim->pending_destroy) {
                wlr_output_schedule_frame(output);
            }
        }
        // wlr_log(WLR_DEBUG, "[SITER_MAIN_ONLY:%s] TL '%s' anim_scale_factor: %.3f", output_name_log, tl_title_for_log, anim_scale_factor);
    }
    if (anim_scale_factor < 0.001f && !(toplevel_for_anim && toplevel_for_anim->target_scale == 0.0f && !toplevel_for_anim->pending_destroy)) {
        anim_scale_factor = 0.001f; // Prevent fully zero scale unless intended for minimize
    }


    // --- Main Window Rendering ---
    struct wlr_texture *texture = wlr_texture_from_buffer(renderer, scene_buffer->buffer);
    if (!texture) {
        wlr_log(WLR_ERROR, "[SITER_MAIN_ONLY:%s] Failed to create wlr_texture from buffer %p for '%s'.",
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
        if (toplevel_for_anim && !toplevel_for_anim->pending_destroy && toplevel_for_anim->target_scale == 0.0f) {
            // This is fine for a minimized window.
        } else {
            wlr_log(WLR_DEBUG, "[SITER_MAIN_ONLY:%s] Scaled dimensions for '%s' are zero/negative (%dx%d) with scale %.2f. Skipping draw.",
                   output_name_log, tl_title_for_log, main_render_box.width, main_render_box.height, anim_scale_factor);
        }
        wlr_texture_destroy(texture);
        return;
    }

    // Calculate MVP matrix exactly like first version
    float main_mvp[9];
    wlr_matrix_identity(main_mvp);
    float box_scale_x = (float)main_render_box.width * (2.0f / output->width);
    float box_scale_y = (float)main_render_box.height * (-2.0f / output->height);
    float box_translate_x = ((float)main_render_box.x / output->width) * 2.0f - 1.0f;
    float box_translate_y = ((float)main_render_box.y / output->height) * -2.0f + 1.0f;

    // Handle output transforms like first version
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

    // Handle buffer transforms like first version
    if (scene_buffer->transform != WL_OUTPUT_TRANSFORM_NORMAL) {
        float temp_mvp_buffer[9];
        memcpy(temp_mvp_buffer, main_mvp, sizeof(main_mvp));
        float buffer_matrix[9];
        wlr_matrix_transform(buffer_matrix, scene_buffer->transform);
        wlr_matrix_multiply(main_mvp, temp_mvp_buffer, buffer_matrix);
    }

   if (server->back_shader_mvp_loc != -1) { // Use windowback's MVP location
        glUniformMatrix3fv(server->back_shader_mvp_loc, 1, GL_FALSE, main_mvp);
    }
    if (server->back_shader_tex_loc != -1) { // Use windowback's texture sampler location
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(tex_attribs.target, tex_attribs.tex);
        glTexParameteri(tex_attribs.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(tex_attribs.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(tex_attribs.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(tex_attribs.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glUniform1i(server->back_shader_tex_loc, 0);
    }
    if (server->back_shader_time_loc != -1) { // Use windowback's time location
        glUniform1f(server->back_shader_time_loc, get_monotonic_time_seconds_as_float());
    }
    if (server->back_shader_res_loc != -1) { // Use windowback's iResolution location
        float res_array[2] = {(float)main_render_box.width, (float)main_render_box.height};
        glUniform2fv(server->back_shader_res_loc, 1, res_array);
    }
    // Draw the main window content
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    wlr_log(WLR_DEBUG, "[SITER_MAIN_ONLY:%s] Drew main window for '%s'", output_name_log, tl_title_for_log);

    // Cleanup for this iteration
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(tex_attribs.target, 0);
    wlr_texture_destroy(texture);
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


    // --- Preview Rendering (modified for sequential positioning) ---
if (toplevel && server->top_panel_node && server->top_panel_node->enabled &&
    texture) { // Ensure texture is still valid

    bool should_preview_this_toplevel = true;
    
    if (should_preview_this_toplevel) {
        wlr_log(WLR_INFO, "[SCENE_ITERATOR_PREVIEW:%s] Rendering preview for '%s' onto panel.", output_name_log, tl_title_for_log);
        struct wlr_scene_rect *panel_srect = wlr_scene_rect_from_node(server->top_panel_node);

        if (panel_srect) {
            int panel_screen_x = server->top_panel_node->x;
            int panel_screen_y = server->top_panel_node->y;

            // Calculate this toplevel's index in the list
            int toplevel_index = 0;
            struct tinywl_toplevel *current_tl;
            wl_list_for_each(current_tl, &server->toplevels, link) {
                if (current_tl == toplevel) {
                    break;
                }
                toplevel_index++;
            }

            // Position each preview at 20-pixel intervals: 20, 40, 60, etc.
            float preview_x_offset_in_panel = 20.0f + (toplevel_index * 100.0f);
            float preview_y_padding_in_panel = 5.0f;
            float preview_height_pixels = (float)panel_srect->height - 2.0f * preview_y_padding_in_panel;
            if (preview_height_pixels < 1.0f) preview_height_pixels = 1.0f;

            float aspect_ratio = (texture->width > 0 && texture->height > 0) ? (float)texture->width / (float)texture->height : 1.0f;
            float preview_width_pixels = preview_height_pixels * aspect_ratio;
            if (preview_width_pixels < 1.0f) preview_width_pixels = 1.0f;

            // Check if preview fits within panel bounds
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
                // Rest of the preview rendering code remains the same...
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
                wlr_log(WLR_DEBUG, "[SCENE_ITERATOR_PREVIEW:%s] Drew preview #%d for '%s' at %d,%d %dx%d",
                       output_name_log, toplevel_index, tl_title_for_log,
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



static pthread_mutex_t rdp_transmit_mutex = PTHREAD_MUTEX_INITIALIZER;


struct scene_diagnostics_data {
    struct wlr_output *output;
    int buffer_count;
};






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
    struct render_data *rdata = user_data;
    struct wlr_output *output = rdata->output;
    struct tinywl_server *server = rdata->server;

    if (node->type != WLR_SCENE_NODE_RECT || !node->enabled) {
        return;
    }
    struct wlr_scene_rect *scene_rect = wlr_scene_rect_from_node(node);

    int render_sx, render_sy;
    if (!wlr_scene_node_coords(node, &render_sx, &render_sy)) {
        wlr_log(WLR_DEBUG, "[RENDER_RECT_NODE:%s] wlr_scene_node_coords failed for node %p. Skipping.",
                output->name, (void*)node);
        return;
    }

    if (output->width == 0 || output->height == 0) return;

    GLuint program_to_use;
    GLint mvp_loc = -1, color_loc = -1, time_loc = -1, output_res_loc = -1, rect_dim_loc = -1;
    // GLint current_ssd_rect_pixel_dims_loc = -1; // Not needed for this simple pattern

    bool is_main_background = (server->main_background_node == node);

    if (is_main_background) {
        program_to_use = server->rect_shader_program; // "Melt"
        mvp_loc = server->rect_shader_mvp_loc;
        color_loc = server->rect_shader_color_loc;
        time_loc = server->rect_shader_time_loc;
        output_res_loc = server->rect_shader_resolution_loc;
        rect_dim_loc = server->rect_shader_rect_res_loc; // If your Melt shader still uses this
    } else { // SSD
        program_to_use = server->ssd_shader_program;
        mvp_loc = server->ssd_shader_mvp_loc;
        color_loc = server->ssd_shader_color_loc;
        time_loc = server->ssd_shader_time_loc; 
        // No other uniforms needed for this simple SSD shader
    }

    if (program_to_use == 0) {
        wlr_log(WLR_ERROR, "Cannot render rect %dx%d, shader program ID is 0 for purpose %s.",
                scene_rect->width, scene_rect->height, is_main_background ? "BG" : "SSD");
        return;
    }
    glUseProgram(program_to_use);

    float mvp[9];
    wlr_matrix_identity(mvp);
    float box_scale_x = (float)scene_rect->width * (2.0f / output->width);
    float box_scale_y = (float)scene_rect->height * (-2.0f / output->height);
    float box_translate_x = ((float)render_sx / output->width) * 2.0f - 1.0f;
    float box_translate_y = ((float)render_sy / output->height) * -2.0f + 1.0f;

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

   if (mvp_loc != -1) glUniformMatrix3fv(mvp_loc, 1, GL_FALSE, mvp);


    // Set u_color (used by both Melt and current SSD shader for alpha)
    if (color_loc != -1) {
        // Log to confirm alpha for SSDs, especially
        if (!is_main_background) {
             wlr_log(WLR_ERROR, "[SSD_RENDER_UCOLOR] Node %p, Program %u: Setting u_color (loc %d) to (%.2f, %.2f, %.2f, %.2f)",
                (void*)node, program_to_use, color_loc,
                scene_rect->color[0], scene_rect->color[1], scene_rect->color[2], scene_rect->color[3]);
        }
        glUniform4fv(color_loc, 1, scene_rect->color);
    }

    // Set time (used by Melt and current SSD shader)
    if (time_loc != -1) {
        float current_time_value = get_monotonic_time_seconds_as_float();
        // Log to confirm time is being set for SSDs
        if (!is_main_background) {
             wlr_log(WLR_ERROR, "[SSD_RENDER_TIME] Node %p, Program %u: Setting time (loc %d) to %.2f",
                (void*)node, program_to_use, time_loc, current_time_value);
        }
        glUniform1f(time_loc, current_time_value);
    } else {
         if (!is_main_background) { // Log if time_loc is -1 specifically for SSD path
             wlr_log(WLR_ERROR, "[SSD_RENDER_TIME_FAIL] Node %p, Program %u: time_loc is -1 for SSD shader.", (void*)node, program_to_use);
         }
    }
    
   
    // No other uniforms to set for the simple SSD shader beyond mvp and u_color

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

    // debug_scene_rendering(scene, wlr_output); // Keep for debugging if needed

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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
            .pass = pass
        };

        // --- Render Background, SSDs, and Panel Rects ---
        // This simplified loop iterates all rects. render_rect_node differentiates.
        // The panel is handled separately by render_panel_node.
        wlr_log(WLR_DEBUG, "[OUTPUT_FRAME:%s] Iterating for ALL RECTS (background, SSDs).", wlr_output->name);
        struct wlr_scene_node *iter_node_lvl1; // Nodes directly under scene root
        wl_list_for_each(iter_node_lvl1, &scene->tree.children, link) {
            if (!iter_node_lvl1->enabled) {
                continue;
            }

            if (iter_node_lvl1 == server->top_panel_node) {
                // Skip panel here, render it specifically later
                continue;
            }

            if (iter_node_lvl1->type == WLR_SCENE_NODE_RECT) {
                // This is the main background or other top-level rects
                render_rect_node(iter_node_lvl1, &rdata);
            } else if (iter_node_lvl1->type == WLR_SCENE_NODE_TREE) {
                // This is a toplevel's scene_tree. Iterate its children for SSD rects.
                struct tinywl_toplevel *toplevel_ptr = iter_node_lvl1->data;
                if (toplevel_ptr && toplevel_ptr->ssd.enabled) {
                    struct wlr_scene_tree *toplevel_s_tree = wlr_scene_tree_from_node(iter_node_lvl1);
                    struct wlr_scene_node *ssd_node_candidate;
                    wl_list_for_each(ssd_node_candidate, &toplevel_s_tree->children, link) {
                        if (ssd_node_candidate->enabled && ssd_node_candidate->type == WLR_SCENE_NODE_RECT) {
                            // Check if this node is one of the SSD parts
                            // This check is important to not accidentally re-render client content area if it was a rect
                            if ( (toplevel_ptr->ssd.title_bar && ssd_node_candidate == &toplevel_ptr->ssd.title_bar->node) ||
                                 (toplevel_ptr->ssd.border_left && ssd_node_candidate == &toplevel_ptr->ssd.border_left->node) ||
                                 (toplevel_ptr->ssd.border_right && ssd_node_candidate == &toplevel_ptr->ssd.border_right->node) ||
                                 (toplevel_ptr->ssd.border_bottom && ssd_node_candidate == &toplevel_ptr->ssd.border_bottom->node) ) {
                                render_rect_node(ssd_node_candidate, &rdata);
                            }
                        }
                    }
                }
            }
        }

        // --- Render Panel Node ---
        if (server->top_panel_node && server->top_panel_node->enabled) {
            if (server->panel_shader_program != 0) {
                wlr_log(WLR_DEBUG, "[OUTPUT_FRAME:%s] Rendering TOP PANEL node %p with panel_shader (ID: %u).",
                        wlr_output->name, (void*)server->top_panel_node, server->panel_shader_program);
                render_panel_node(server->top_panel_node, &rdata);
            } else {
                wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] Panel shader program is 0, cannot render panel.", wlr_output->name);
            }
        }

        // --- Render Client Window Buffers (using back_shader_program for background effect) ---
   /*     if (server->back_shader_program != 0) {
            glUseProgram(server->back_shader_program);
            wlr_log(WLR_DEBUG, "[OUTPUT_FRAME:%s] Iterating for BUFFERS (client content with back_shader ID %u)",
                    wlr_output->name, server->ssd_shader_program);
            wlr_scene_node_for_each_buffer(&scene->tree.node, scene_buffer_iterator_main_window_only, &rdata);
        } else {
            wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] Client background effect shader (back_shader_program) is 0.", wlr_output->name);
        }
*/
        // --- Render Client Window Buffers (using main shader_program for foreground effect like "Flame") ---
        if (server->shader_program != 0) {
            glUseProgram(server->shader_program);
            wlr_log(WLR_DEBUG, "[OUTPUT_FRAME:%s] Iterating for BUFFERS (client content with flame_shader ID %u)",
                    wlr_output->name, server->shader_program);
            wlr_scene_node_for_each_buffer(&scene->tree.node, scene_buffer_iterator, &rdata);
        } else {
            wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] Client flame shader (shader_program) is 0.", wlr_output->name);
        }

        glBindVertexArray(0);
        glUseProgram(0); // Unbind any active program
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

// Helper function to disable/destroy SSDs
static void ensure_ssd_disabled(struct tinywl_toplevel *toplevel) {
    const char *title = (toplevel->xdg_toplevel && toplevel->xdg_toplevel->title) ? toplevel->xdg_toplevel->title : "N/A";
    wlr_log(WLR_INFO, "[SSD_HELPER] Enter ensure_ssd_disabled for toplevel %s. Current ssd.enabled: %d",
            title, toplevel->ssd.enabled);

    if (!toplevel->ssd.enabled) {
        wlr_log(WLR_DEBUG, "[SSD_HELPER] SSDs already disabled for %s", title);
        return;
    }
    wlr_log(WLR_INFO, "[SSD_HELPER] Disabling and destroying SSDs for %s.", title);

    if (toplevel->ssd.title_bar) { wlr_scene_node_destroy(&toplevel->ssd.title_bar->node); toplevel->ssd.title_bar = NULL; }
    if (toplevel->ssd.border_left) { wlr_scene_node_destroy(&toplevel->ssd.border_left->node); toplevel->ssd.border_left = NULL; }
    if (toplevel->ssd.border_right) { wlr_scene_node_destroy(&toplevel->ssd.border_right->node); toplevel->ssd.border_right = NULL; }
    if (toplevel->ssd.border_bottom) { wlr_scene_node_destroy(&toplevel->ssd.border_bottom->node); toplevel->ssd.border_bottom = NULL; }
    
    toplevel->ssd.enabled = false;

    if (toplevel->client_xdg_scene_tree) {
        wlr_log(WLR_INFO, "[SSD_HELPER] Resetting client_xdg_scene_tree position for %s to (0,0).", title);
        wlr_scene_node_set_position(&toplevel->client_xdg_scene_tree->node, 0, 0);
    }
}

static void update_decoration_geometry(struct tinywl_toplevel *toplevel) {
    const char *title = (toplevel && toplevel->xdg_toplevel && toplevel->xdg_toplevel->title) ?
                        toplevel->xdg_toplevel->title : "N/A";

    if (!toplevel || !toplevel->client_xdg_scene_tree ||
        !toplevel->xdg_toplevel || !toplevel->xdg_toplevel->base ||
        !toplevel->xdg_toplevel->base->surface) {
        wlr_log(WLR_DEBUG, "[UPDATE_DECO:%s] Pre-conditions not met.", title);
        return;
    }

    if (!toplevel->ssd.enabled) {
        wlr_log(WLR_DEBUG, "[UPDATE_DECO:%s] SSDs not enabled. Client content at (0,0).", title);
        wlr_scene_node_set_position(&toplevel->client_xdg_scene_tree->node, 0, 0);
        if (toplevel->ssd.title_bar) wlr_scene_node_set_enabled(&toplevel->ssd.title_bar->node, false);
        if (toplevel->ssd.border_left) wlr_scene_node_set_enabled(&toplevel->ssd.border_left->node, false);
        if (toplevel->ssd.border_right) wlr_scene_node_set_enabled(&toplevel->ssd.border_right->node, false);
        if (toplevel->ssd.border_bottom) wlr_scene_node_set_enabled(&toplevel->ssd.border_bottom->node, false);
        return;
    }

    struct wlr_xdg_toplevel *xdg_toplevel = toplevel->xdg_toplevel;
    struct wlr_scene_node *client_content_node = &toplevel->client_xdg_scene_tree->node;

    // Get the actual dimensions of the client's content scene node.
    // This usually comes from the wlr_scene_buffer associated with the client's surface.
    int actual_content_width = 0;
    int actual_content_height = 0;

    // The client_xdg_scene_tree is a wlr_scene_surface. Its first child should be the wlr_scene_buffer.
    if (!wl_list_empty(&client_content_node->parent->children)) { // client_content_node is the wlr_scene_xdg_surface tree
        struct wlr_scene_node *buffer_node_candidate = wl_container_of(toplevel->client_xdg_scene_tree->children.next, buffer_node_candidate, link);

        if (buffer_node_candidate && buffer_node_candidate->type == WLR_SCENE_NODE_BUFFER) {
            struct wlr_scene_buffer *scene_buffer = wlr_scene_buffer_from_node(buffer_node_candidate);
            if (scene_buffer && scene_buffer->buffer) {
                actual_content_width = scene_buffer->buffer->width;
                actual_content_height = scene_buffer->buffer->height;
                 wlr_log(WLR_DEBUG, "[UPDATE_DECO:%s] Using scene_buffer dimensions: %dx%d", title, actual_content_width, actual_content_height);
            } else if (scene_buffer && scene_buffer->dst_width > 0 && scene_buffer->dst_height > 0) {
                // Fallback if buffer isn't present yet but dst dimensions are set
                actual_content_width = scene_buffer->dst_width;
                actual_content_height = scene_buffer->dst_height;
                wlr_log(WLR_DEBUG, "[UPDATE_DECO:%s] Using scene_buffer->dst_width/height: %dx%d", title, actual_content_width, actual_content_height);
            }
        }
    }

    // Fallback to xdg_toplevel->current if scene buffer dimensions are not available/valid
    if (actual_content_width <= 0 || actual_content_height <= 0) {
        wlr_log(WLR_DEBUG, "[UPDATE_DECO:%s] Scene buffer dimensions not available or zero, falling back to xdg_toplevel->current (%dx%d).",
                title, xdg_toplevel->current.width, xdg_toplevel->current.height);
        actual_content_width = xdg_toplevel->current.width;
        actual_content_height = xdg_toplevel->current.height;
    }


    if (actual_content_width <= 0 || actual_content_height <= 0) {
        wlr_log(WLR_DEBUG, "[UPDATE_DECO:%s] Final effective content size is %dx%d. Hiding SSDs. Client at (0,0).",
                title, actual_content_width, actual_content_height);
        if (toplevel->ssd.title_bar) wlr_scene_node_set_enabled(&toplevel->ssd.title_bar->node, false);
        if (toplevel->ssd.border_left) wlr_scene_node_set_enabled(&toplevel->ssd.border_left->node, false);
        if (toplevel->ssd.border_right) wlr_scene_node_set_enabled(&toplevel->ssd.border_right->node, false);
        if (toplevel->ssd.border_bottom) wlr_scene_node_set_enabled(&toplevel->ssd.border_bottom->node, false);
        wlr_scene_node_set_position(&toplevel->client_xdg_scene_tree->node, 0, 0);
        return;
    }

    wlr_log(WLR_INFO, "[UPDATE_DECO:%s] Effective content geometry for SSD sizing: %dx%d (from scene/current)",
            title, actual_content_width, actual_content_height);

    if (!toplevel->ssd.title_bar || !toplevel->ssd.border_left ||
        !toplevel->ssd.border_right || !toplevel->ssd.border_bottom) {
        wlr_log(WLR_ERROR, "[UPDATE_DECO:%s] SSD rects NULL. Re-enabling.", title);
        ensure_ssd_enabled(toplevel); // This will call update_decoration_geometry again
        return; // Avoid continuing if re-enable is needed, it will recall this func.
    }
    
    wlr_scene_node_set_enabled(&toplevel->ssd.title_bar->node, true);
    wlr_scene_node_set_enabled(&toplevel->ssd.border_left->node, true);
    wlr_scene_node_set_enabled(&toplevel->ssd.border_right->node, true);
    wlr_scene_node_set_enabled(&toplevel->ssd.border_bottom->node, true);

    int ssd_total_width = actual_content_width + 2 * BORDER_WIDTH;

    wlr_scene_rect_set_size(toplevel->ssd.title_bar, ssd_total_width, TITLE_BAR_HEIGHT);
    wlr_scene_node_set_position(&toplevel->ssd.title_bar->node, 0, 0);

    wlr_scene_rect_set_size(toplevel->ssd.border_left, BORDER_WIDTH, actual_content_height);
    wlr_scene_node_set_position(&toplevel->ssd.border_left->node, 0, TITLE_BAR_HEIGHT);

    wlr_scene_rect_set_size(toplevel->ssd.border_right, BORDER_WIDTH, actual_content_height);
    wlr_scene_node_set_position(&toplevel->ssd.border_right->node, BORDER_WIDTH + actual_content_width, TITLE_BAR_HEIGHT);

    wlr_scene_rect_set_size(toplevel->ssd.border_bottom, ssd_total_width, BORDER_WIDTH);
    wlr_scene_node_set_position(&toplevel->ssd.border_bottom->node, 0, TITLE_BAR_HEIGHT + actual_content_height);

    wlr_log(WLR_INFO, "[UPDATE_DECO:%s] Positioning client_xdg_scene_tree at (%d, %d).",
            title, BORDER_WIDTH, TITLE_BAR_HEIGHT);
    wlr_scene_node_set_position(&toplevel->client_xdg_scene_tree->node, BORDER_WIDTH, TITLE_BAR_HEIGHT);
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
    struct wlr_xdg_toplevel *xdg_toplevel = toplevel->xdg_toplevel;
    struct wlr_xdg_surface *xdg_surface_for_toplevel = xdg_toplevel->base; // Alias for clarity
    const char *title = (xdg_toplevel->title) ? xdg_toplevel->title : "N/A";

    wlr_log(WLR_INFO, "[MAP:%s] Mapped. Current XDG configured content size: %dx%d",
            title, xdg_toplevel->current.width, xdg_toplevel->current.height);
    toplevel->mapped = true;

    int target_content_width = 800;
    int target_content_height = 600;
    bool size_from_client_geom = false;

    struct wlr_box client_geom = xdg_surface_for_toplevel->geometry;
    wlr_log(WLR_DEBUG, "[MAP:%s] Client's current xdg_surface->geometry: %dx%d at %d,%d",
            title, client_geom.width, client_geom.height, client_geom.x, client_geom.y);

    if (client_geom.width > 0 && client_geom.height > 0) {
        target_content_width = client_geom.width;
        target_content_height = client_geom.height;
        size_from_client_geom = true;
    }

    bool intend_server_decorations = false;
    if (toplevel->decoration) {
        if (toplevel->decoration->current.mode == WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE ||
            toplevel->decoration->pending.mode == WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE) {
            intend_server_decorations = true;
        }
    } else {
        intend_server_decorations = true;
    }

    if (size_from_client_geom && intend_server_decorations) {
        int derived_content_width = client_geom.width - (2 * BORDER_WIDTH);
        int derived_content_height = client_geom.height - TITLE_BAR_HEIGHT - BORDER_WIDTH;

        if (derived_content_width >= 50 && derived_content_height >= 50) {
            target_content_width = derived_content_width;
            target_content_height = derived_content_height;
            wlr_log(WLR_INFO, "[MAP:%s] Client suggested total %dx%d. Server intends SSDs. Derived target CONTENT size: %dx%d.",
                    title, client_geom.width, client_geom.height, target_content_width, target_content_height);
        } else {
            wlr_log(WLR_INFO, "[MAP:%s] Client suggested total %dx%d. Derived SSD content size %dx%d too small. Using client's suggestion as content or default.",
                    title, client_geom.width, client_geom.height, derived_content_width, derived_content_height);
            // If derived is too small, we either use client_geom directly as content or a larger default
            // For now, if derived is too small, let's revert to client_geom as content, assuming client might not want SSDs yet.
            target_content_width = client_geom.width; // Revert
            target_content_height = client_geom.height; // Revert
        }
    } else if (size_from_client_geom) {
         wlr_log(WLR_INFO, "[MAP:%s] Using client-suggested geometry directly as content (no SSDs intended yet): %dx%d",
                title, target_content_width, target_content_height);
    } else {
        wlr_log(WLR_INFO, "[MAP:%s] No valid client geometry, using default content size: %dx%d.",
                title, target_content_width, target_content_height);
    }
    
    if ((xdg_toplevel->current.width == 0 && xdg_toplevel->current.height == 0) ||
        xdg_toplevel->current.width != target_content_width ||
        xdg_toplevel->current.height != target_content_height) {
        wlr_log(WLR_INFO, "[MAP:%s] Setting XDG toplevel CONTENT size to %dx%d.", title, target_content_width, target_content_height);
        wlr_xdg_toplevel_set_size(xdg_toplevel, target_content_width, target_content_height);
    } else {
        wlr_log(WLR_INFO, "[MAP:%s] XDG toplevel content size already %dx%d, not re-setting.", title, xdg_toplevel->current.width, xdg_toplevel->current.height);
        if (toplevel->ssd_pending_enable && xdg_surface_for_toplevel->initialized) {
             wlr_log(WLR_DEBUG, "[MAP:%s] Size matches but ssd_pending_enable, scheduling configure.", title);
            wlr_xdg_surface_schedule_configure(xdg_surface_for_toplevel);
        }
    }

    focus_toplevel(toplevel);
}


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





// Helper function to create a scene rect
static struct wlr_scene_rect *create_decoration_rect(
    struct wlr_scene_tree *parent,
    int width, int height, int x, int y, float color[4]) {
    struct wlr_scene_rect *rect = wlr_scene_rect_create(parent, width, height, color);
    if (rect) {
        wlr_scene_node_set_position(&rect->node, x, y);
        wlr_scene_node_set_enabled(&rect->node, true); // Initially enabled
    } else {
        wlr_log(WLR_ERROR, "Failed to create decoration rect");
    }
    return rect;
}







static void xdg_toplevel_commit(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, commit);
    struct wlr_xdg_toplevel *xdg_toplevel = toplevel->xdg_toplevel;
    struct wlr_xdg_surface *xdg_surface = xdg_toplevel->base;
    struct wlr_surface *surface = xdg_surface->surface;
    struct tinywl_server *server = toplevel->server;
    const char *title = (xdg_toplevel->title) ? xdg_toplevel->title : "N/A";

    if (!surface) {
        wlr_log(WLR_ERROR, "[COMMIT:%s] Surface pointer was NULL!", title);
        return;
    }

    enum wlr_xdg_toplevel_decoration_v1_mode mode_at_commit_entry = WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_NONE;
    if (toplevel->decoration) {
        mode_at_commit_entry = toplevel->decoration->current.mode;
    }

    wlr_log(WLR_INFO, "[COMMIT:%s] START - Mapped:%d, Init:%d, XDGConf:%d, SSD_EN:%d, SSD_PEND:%d. DecoCurr:%d, DecoPend:%d. Buf:%p(%dx%d), Seq:%u. XDGCurrentSize:%dx%d",
            title, surface->mapped, xdg_surface->initialized, xdg_surface->configured,
            toplevel->ssd.enabled, toplevel->ssd_pending_enable,
            mode_at_commit_entry,
            toplevel->decoration ? toplevel->decoration->pending.mode : -1,
            (void*)surface->current.buffer, surface->current.buffer ? surface->current.buffer->width : 0, surface->current.buffer ? surface->current.buffer->height : 0,
            surface->current.seq, xdg_toplevel->current.width, xdg_toplevel->current.height);

    if (surface->current.buffer) {
        if (!(toplevel->cached_texture && toplevel->last_commit_seq == surface->current.seq)) {
            if (toplevel->cached_texture) {
                wlr_texture_destroy(toplevel->cached_texture);
            }
            toplevel->cached_texture = wlr_texture_from_buffer(server->renderer, surface->current.buffer);
            toplevel->last_commit_seq = surface->current.seq;
        }
    } else {
        if (toplevel->cached_texture) {
            wlr_texture_destroy(toplevel->cached_texture);
            toplevel->cached_texture = NULL;
        }
        toplevel->last_commit_seq = surface->current.seq;
    }

    bool needs_surface_configure = false;
    enum wlr_xdg_toplevel_decoration_v1_mode server_preferred_mode = WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE;

    if (xdg_surface->initial_commit) {
        needs_surface_configure = true;
        if (xdg_toplevel->current.width == 0 && xdg_toplevel->current.height == 0) {
            wlr_xdg_toplevel_set_size(xdg_toplevel, 0, 0);
        }
    }

    if (toplevel->decoration && toplevel->ssd_pending_enable) {
        if (xdg_surface->initialized || xdg_surface->initial_commit) {
            enum wlr_xdg_toplevel_decoration_v1_mode current_actual_mode = toplevel->decoration->current.mode;
            enum wlr_xdg_toplevel_decoration_v1_mode pending_protocol_mode = toplevel->decoration->pending.mode;

            if (current_actual_mode != server_preferred_mode ||
                (pending_protocol_mode != WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_NONE && pending_protocol_mode != server_preferred_mode)) {
                wlr_log(WLR_INFO, "[COMMIT:%s] ssd_pending_enable=true. Current deco mode: %d, Pending deco mode: %d. Server setting preferred mode: %d.",
                        title, current_actual_mode, pending_protocol_mode, server_preferred_mode);
                wlr_xdg_toplevel_decoration_v1_set_mode(toplevel->decoration, server_preferred_mode);
                needs_surface_configure = true;
            }
            toplevel->ssd_pending_enable = false;
        } else {
            wlr_log(WLR_INFO, "[COMMIT:%s] ssd_pending_enable=true, but surface not initialized and not initial_commit. Deferring mode set logic.", title);
        }
    }

    int content_width = xdg_toplevel->current.width;
    int content_height = xdg_toplevel->current.height;

    enum wlr_xdg_toplevel_decoration_v1_mode effective_mode_for_decision = WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_NONE;
    if (toplevel->decoration) {
        effective_mode_for_decision = (toplevel->decoration->pending.mode != WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_NONE) ?
                                      toplevel->decoration->pending.mode : toplevel->decoration->current.mode;
    }

    if (effective_mode_for_decision == server_preferred_mode) {
        if (content_width > 0 && content_height > 0) {
            if (!toplevel->ssd.enabled) {
                wlr_log(WLR_INFO, "[COMMIT:%s] Effective mode is SERVER_SIDE (%d) & content %dx%d. Enabling SSDs.",
                        title, effective_mode_for_decision, content_width, content_height);
                ensure_ssd_enabled(toplevel);
                if (toplevel->ssd.enabled) {
                    needs_surface_configure = true;
                }
            }
        } else {
            if (toplevel->ssd.enabled) {
                wlr_log(WLR_INFO, "[COMMIT:%s] Effective mode is SERVER_SIDE (%d), but content size %dx%d is 0. Disabling SSDs.",
                        title, effective_mode_for_decision, content_width, content_height);
                ensure_ssd_disabled(toplevel);
                needs_surface_configure = true;
            }
        }
    } else {
        if (toplevel->ssd.enabled) {
            wlr_log(WLR_INFO, "[COMMIT:%s] Effective mode is %d (NOT SERVER_SIDE). Disabling SSDs.",
                    title, effective_mode_for_decision);
            ensure_ssd_disabled(toplevel);
            needs_surface_configure = true;
        }
    }

    if (toplevel->ssd.enabled) {
        update_decoration_geometry(toplevel);
    }

    if (needs_surface_configure || (xdg_surface->initialized && !xdg_surface->configured) ) {
        wlr_log(WLR_INFO, "[COMMIT:%s] Scheduling configure for xdg_surface. NeedsConfFlag:%d, InitAndNotYetXDGConfigured:%d",
                title, needs_surface_configure, (xdg_surface->initialized && !xdg_surface->configured));
        uint32_t serial = wlr_xdg_surface_schedule_configure(xdg_surface);
        wlr_log(WLR_DEBUG, "[COMMIT:%s] Scheduled xdg_surface configure with serial %u.", title, serial);
    }

    if (surface->mapped) {
        struct tinywl_output *output_iter;
        wl_list_for_each(output_iter, &server->outputs, link) {
            if (output_iter->wlr_output && output_iter->wlr_output->enabled) {
                wlr_output_schedule_frame(output_iter->wlr_output);
            }
        }
    }

    enum wlr_xdg_toplevel_decoration_v1_mode mode_at_commit_exit = WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_NONE;
    if (toplevel->decoration) {
        mode_at_commit_exit = toplevel->decoration->current.mode;
    }

    wlr_log(WLR_INFO, "[COMMIT:%s] END - Mapped:%d, Init:%d, Conf:%d, SSD_EN:%d, DECO_CURR_MODE_EXIT:%d, Seq:%u. XDG_Size(current):%dx%d. Surface Buffer: %p (%dx%d)",
            title, surface->mapped, xdg_surface->initialized, xdg_surface->configured,
            toplevel->ssd.enabled, mode_at_commit_exit, surface->current.seq,
            xdg_toplevel->current.width, xdg_toplevel->current.height,
            (void*)surface->current.buffer,
            surface->current.buffer ? surface->current.buffer->width : 0,
            surface->current.buffer ? surface->current.buffer->height : 0);
}


static void decoration_handle_destroy(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel =
        wl_container_of(listener, toplevel, decoration_destroy_listener);

    if (!toplevel) return;
    struct wlr_xdg_toplevel *xdg_toplevel = toplevel->xdg_toplevel;

    wlr_log(WLR_INFO, "[DECO_OBJ_DESTROY] For toplevel %s, decoration %p is being destroyed.",
            xdg_toplevel->title ? xdg_toplevel->title : "N/A",
            (void*)toplevel->decoration);

    wl_list_remove(&toplevel->decoration_destroy_listener.link);
    if (toplevel->decoration_request_mode_listener.link.next) {
        wl_list_remove(&toplevel->decoration_request_mode_listener.link);
    }
    toplevel->decoration = NULL;
    toplevel->ssd_pending_enable = false;

    ensure_ssd_disabled(toplevel);

    if (xdg_toplevel->base->initialized) {
        wlr_log(WLR_INFO, "[DECO_OBJ_DESTROY] Toplevel %s: Scheduling configure for main xdg_surface.",
                xdg_toplevel->title);
        wlr_xdg_surface_schedule_configure(xdg_toplevel->base);
    }
}

// Modified xdg_toplevel_destroy to clean up SSD
static void xdg_toplevel_destroy(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, destroy);
    const char *title = toplevel->xdg_toplevel && toplevel->xdg_toplevel->title ? toplevel->xdg_toplevel->title : "N/A";

    wlr_log(WLR_INFO, "[DESTROY:%s] Destroying toplevel %p", title, (void*)toplevel);

    // Clean up cached texture
    if (toplevel->cached_texture) {
        wlr_texture_destroy(toplevel->cached_texture);
        toplevel->cached_texture = NULL;
    }

    // Clean up decoration listeners
    if (toplevel->decoration) {
        wl_list_remove(&toplevel->decoration_destroy_listener.link);
        wl_list_remove(&toplevel->decoration_request_mode_listener.link);
        toplevel->decoration = NULL;
    }

    // Remove other listeners
    wl_list_remove(&toplevel->map.link);
    wl_list_remove(&toplevel->unmap.link);
    wl_list_remove(&toplevel->commit.link);
    wl_list_remove(&toplevel->destroy.link);
    wl_list_remove(&toplevel->request_move.link);
    wl_list_remove(&toplevel->request_resize.link);
    wl_list_remove(&toplevel->request_maximize.link);
    wl_list_remove(&toplevel->request_fullscreen.link);

    // Destroy scene tree (includes SSD elements)
    if (toplevel->scene_tree) {
        wlr_scene_node_destroy(&toplevel->scene_tree->node);
        toplevel->scene_tree = NULL;
        toplevel->client_xdg_scene_tree = NULL;
        toplevel->ssd.title_bar = NULL;
        toplevel->ssd.border_left = NULL;
        toplevel->ssd.border_right = NULL;
        toplevel->ssd.border_bottom = NULL;
    }

    wl_list_remove(&toplevel->link);
    free(toplevel);
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



static void server_new_xdg_toplevel(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, new_xdg_toplevel);
    struct wlr_xdg_toplevel *xdg_toplevel = data;

    wlr_log(WLR_INFO, "New XDG Toplevel (app_id: %s, title: %s): %p",
            xdg_toplevel->app_id ? xdg_toplevel->app_id : "N/A",
            xdg_toplevel->title ? xdg_toplevel->title : "N/A",
            (void*)xdg_toplevel);

    if (!xdg_toplevel || !xdg_toplevel->base || !xdg_toplevel->base->surface) {
        wlr_log(WLR_ERROR, "server_new_xdg_toplevel: Invalid xdg_toplevel or base components.");
        if (xdg_toplevel && xdg_toplevel->resource) {
            wl_resource_post_no_memory(xdg_toplevel->resource);
        }
        return;
    }

    struct tinywl_toplevel *toplevel = calloc(1, sizeof(struct tinywl_toplevel));
    if (!toplevel) {
        wlr_log(WLR_ERROR, "server_new_xdg_toplevel: Failed to allocate tinywl_toplevel");
        wl_resource_post_no_memory(xdg_toplevel->resource);
        return;
    }

    toplevel->server = server;
    toplevel->xdg_toplevel = xdg_toplevel;
    wl_list_init(&toplevel->link);
    toplevel->scale = 1.0f;
    toplevel->target_scale = 1.0f;
    toplevel->is_animating = false;

    toplevel->decoration = NULL;
    toplevel->ssd.enabled = false;
    toplevel->ssd_pending_enable = false; // Initialize to false
    toplevel->ssd.title_bar = NULL;
    toplevel->ssd.border_left = NULL;
    toplevel->ssd.border_right = NULL;
    toplevel->ssd.border_bottom = NULL;

    xdg_toplevel->base->data = toplevel;

    toplevel->scene_tree = wlr_scene_tree_create(&server->scene->tree);
    if (!toplevel->scene_tree) {
        wlr_log(WLR_ERROR, "Failed to create scene_tree for %p", (void*)xdg_toplevel);
        free(toplevel);
        xdg_toplevel->base->data = NULL;
        wl_resource_post_no_memory(xdg_toplevel->resource);
        return;
    }
    toplevel->scene_tree->node.data = toplevel;

    toplevel->client_xdg_scene_tree = wlr_scene_xdg_surface_create(
        toplevel->scene_tree, xdg_toplevel->base);
    if (!toplevel->client_xdg_scene_tree) {
        wlr_log(WLR_ERROR, "Failed to create client_xdg_scene_tree for %p", (void*)xdg_toplevel);
        wlr_scene_node_destroy(&toplevel->scene_tree->node);
        free(toplevel);
        xdg_toplevel->base->data = NULL;
        wl_resource_post_no_memory(xdg_toplevel->resource);
        return;
    }

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

    wl_list_init(&toplevel->decoration_destroy_listener.link);
    wl_list_init(&toplevel->decoration_request_mode_listener.link);

    wlr_log(WLR_INFO, "Toplevel %p (title: %s) wrapper created. SSDs NOT enabled. Waiting for xdg-decoration.",
            (void*)toplevel, xdg_toplevel->title ? xdg_toplevel->title : "N/A");
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


void ensure_ssd_enabled(struct tinywl_toplevel *toplevel) {
    const char *title = (toplevel->xdg_toplevel && toplevel->xdg_toplevel->title) ? toplevel->xdg_toplevel->title : "N/A";
    wlr_log(WLR_INFO, "[SSD_HELPER] Enter ensure_ssd_enabled for toplevel %s. Current ssd.enabled: %d",
            title, toplevel->ssd.enabled);

    if (toplevel->ssd.enabled) {
        wlr_log(WLR_DEBUG, "[SSD_HELPER] SSDs already enabled for toplevel %s. Ensuring geometry update.", title);
        update_decoration_geometry(toplevel); // Call update to be sure
        return;
    }

    wlr_log(WLR_INFO, "[SSD_HELPER] Enabling SSDs and creating decoration rects for toplevel %s.", title);

    // Ensure old ones are cleared if somehow they exist while enabled is false
    if (toplevel->ssd.title_bar) { wlr_scene_node_destroy(&toplevel->ssd.title_bar->node); toplevel->ssd.title_bar = NULL; }
    if (toplevel->ssd.border_left) { wlr_scene_node_destroy(&toplevel->ssd.border_left->node); toplevel->ssd.border_left = NULL; }
    if (toplevel->ssd.border_right) { wlr_scene_node_destroy(&toplevel->ssd.border_right->node); toplevel->ssd.border_right = NULL; }
    if (toplevel->ssd.border_bottom) { wlr_scene_node_destroy(&toplevel->ssd.border_bottom->node); toplevel->ssd.border_bottom = NULL; }


    float title_bar_color[4] = {0.0f, 0.0f, 0.8f, 0.7f}; // Slightly transparent
    float border_color[4]    = {0.1f, 0.1f, 0.1f, 0.7f}; // Slightly transparent

    toplevel->ssd.title_bar    = create_decoration_rect(toplevel->scene_tree, 0, 0, 0, 0, title_bar_color);
    toplevel->ssd.border_left  = create_decoration_rect(toplevel->scene_tree, 0, 0, 0, 0, border_color);
    toplevel->ssd.border_right = create_decoration_rect(toplevel->scene_tree, 0, 0, 0, 0, border_color);
    toplevel->ssd.border_bottom= create_decoration_rect(toplevel->scene_tree, 0, 0, 0, 0, border_color);

    if (!toplevel->ssd.title_bar || !toplevel->ssd.border_left ||
        !toplevel->ssd.border_right || !toplevel->ssd.border_bottom) {
        wlr_log(WLR_ERROR, "[SSD_HELPER] FAILED TO CREATE SOME SSD ELEMENTS for %s", title);
        // Clean up any successfully created ones
        if (toplevel->ssd.title_bar) { wlr_scene_node_destroy(&toplevel->ssd.title_bar->node); toplevel->ssd.title_bar = NULL; }
        if (toplevel->ssd.border_left) { wlr_scene_node_destroy(&toplevel->ssd.border_left->node); toplevel->ssd.border_left = NULL; }
        if (toplevel->ssd.border_right) { wlr_scene_node_destroy(&toplevel->ssd.border_right->node); toplevel->ssd.border_right = NULL; }
        if (toplevel->ssd.border_bottom) { wlr_scene_node_destroy(&toplevel->ssd.border_bottom->node); toplevel->ssd.border_bottom = NULL; }
        toplevel->ssd.enabled = false;
    } else {
        toplevel->ssd.enabled = true;
        wlr_log(WLR_INFO, "[SSD_HELPER] SSDs successfully created for %s. title_bar: %p, left: %p, right: %p, bottom: %p",
                title, (void*)toplevel->ssd.title_bar, (void*)toplevel->ssd.border_left,
                (void*)toplevel->ssd.border_right, (void*)toplevel->ssd.border_bottom);

        // Initial positioning of client content within the scene_tree (which is the overall frame)
        if (toplevel->client_xdg_scene_tree) {
            wlr_log(WLR_INFO, "[SSD_HELPER] Positioning client_xdg_scene_tree for %s to (%d, %d) relative to scene_tree.",
                    title, BORDER_WIDTH, TITLE_BAR_HEIGHT);
            wlr_scene_node_set_position(&toplevel->client_xdg_scene_tree->node, BORDER_WIDTH, TITLE_BAR_HEIGHT);
        }
        update_decoration_geometry(toplevel); // This will size/position everything correctly
    }
}





static void server_new_xdg_decoration(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, xdg_decoration_new);
    struct wlr_xdg_toplevel_decoration_v1 *decoration = data;
    struct wlr_xdg_toplevel *xdg_toplevel = decoration->toplevel;
    struct tinywl_toplevel *toplevel = xdg_toplevel->base->data; // xdg_surface is xdg_toplevel->base

    const char *title_for_log = xdg_toplevel->title ? xdg_toplevel->title : "N/A";

    wlr_log(WLR_INFO, "[DECO_MGR] New decoration object %p for toplevel '%s'. Initial client requested mode: %d",
            (void*)decoration, title_for_log, decoration->requested_mode);

    if (!toplevel) {
        wlr_log(WLR_ERROR, "[DECO_MGR] No tinywl_toplevel for new decoration on '%s'.", title_for_log);
        return;
    }

    if (toplevel->decoration && toplevel->decoration != decoration) {
        wlr_log(WLR_INFO, "[DECO_MGR] Replacing old decoration for toplevel '%s'", title_for_log);
        wl_list_remove(&toplevel->decoration_destroy_listener.link);
        wl_list_remove(&toplevel->decoration_request_mode_listener.link);
    }

    toplevel->decoration = decoration;
    toplevel->decoration_destroy_listener.notify = decoration_handle_destroy;
    wl_signal_add(&decoration->events.destroy, &toplevel->decoration_destroy_listener);
    toplevel->decoration_request_mode_listener.notify = decoration_handle_request_mode;
    wl_signal_add(&decoration->events.request_mode, &toplevel->decoration_request_mode_listener);

    // CRITICAL CHANGE: Do NOT call wlr_xdg_toplevel_decoration_v1_set_mode() here.
    // Only set the flag. The commit handler will call set_mode when the surface is initialized.
    toplevel->ssd_pending_enable = true;
    wlr_log(WLR_INFO, "[DECO_MGR] Toplevel '%s': Associated decoration. Marked ssd_pending_enable = true. Actual mode set and surface configure will occur on commit.", title_for_log);

    // If the main xdg_surface IS ALREADY initialized, we can schedule a configure for it.
    // This helps if the decoration is created *after* the surface's initial commit.
    // The commit handler will then see ssd_pending_enable and call set_mode.
    if (xdg_toplevel->base->initialized) {
        wlr_log(WLR_DEBUG, "[DECO_MGR] Toplevel '%s' xdg_surface is already initialized. Scheduling configure for xdg_surface to process pending decoration.", title_for_log);
        wlr_xdg_surface_schedule_configure(xdg_toplevel->base);
    }
}

static void decoration_handle_request_mode(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel =
        wl_container_of(listener, toplevel, decoration_request_mode_listener);
    struct wlr_xdg_toplevel_decoration_v1 *decoration = data;
    struct wlr_xdg_toplevel *xdg_toplevel = toplevel->xdg_toplevel;
    struct wlr_xdg_surface *xdg_surface = xdg_toplevel->base;

    const char *title = (xdg_toplevel && xdg_toplevel->title) ?
                        xdg_toplevel->title : "N/A";

    wlr_log(WLR_INFO, "[DECO_REQUEST_MODE:%s] Client requested mode: %d. Surface initialized: %d",
            title, decoration->requested_mode, xdg_surface->initialized);

    // CRITICAL CHANGE: Do NOT call wlr_xdg_toplevel_decoration_v1_set_mode() here directly
    // if the surface might not be initialized.
    // Set the flag, and let the commit handler call set_mode.
    toplevel->ssd_pending_enable = true;
    wlr_log(WLR_INFO, "[DECO_REQUEST_MODE:%s] Marked ssd_pending_enable = true. Mode set will be handled in commit.", title);

    // If the surface IS ALREADY initialized, schedule a configure for it.
    // The commit handler will then see ssd_pending_enable and call set_mode.
    if (xdg_surface->initialized) {
        wlr_log(WLR_DEBUG, "[DECO_REQUEST_MODE:%s] Surface is initialized. Scheduling configure for xdg_surface to process pending decoration.", title);
        wlr_xdg_surface_schedule_configure(xdg_surface);
    }
}








static void handle_xdg_decoration_new(struct wl_listener *listener, void *data) {
    struct tinywl_server *server = wl_container_of(listener, server, xdg_decoration_new);
    struct wlr_xdg_toplevel_decoration_v1 *decoration = data;
    
    // Force client-side decorations (no server decorations)
    wlr_xdg_toplevel_decoration_v1_set_mode(decoration, 
        WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);
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
"    vec2 adjusted_uv = vec2(uv.x, 1.0 - uv.y * 1.2 + 0.3);\n"
"    vec2 p = adjusted_uv * dist;\n"
"    p += sin(p.yx * 8.0 + vec2(0.2, -0.3) * iTime) * 0.04;\n"
"    p += sin(p.yx * 16.0 + vec2(0.6, 0.1) * iTime) * 0.01;\n"
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





const char *back_vertex_shader_src =
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

const char *back_fragment_shader_src =   
    "/* Octgrams shader converted to panel format, with curved corners using v_texcoord */\n"
    "#version 300 es\n"
    "precision mediump float;\n"
    "\n"
    "in vec2 v_texcoord; // IMPORTANT: This MUST be correctly supplied for the panel's quad\n"
    "out vec4 frag_color;\n"
    "\n"
    "uniform float time;\n"
    "uniform vec2 iResolution; // Resolution of the viewport/target this shader draws to\n"
    "\n"
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
    "// Helper function for Signed Distance to a 2D Rounded Box\n"
    "// p: point to sample in pixel coordinates\n"
    "// b: half-extents of the box in pixel coordinates\n"
    "// r: corner radius in pixels (constant regardless of aspect ratio)\n"
    "float sdfRoundBox2D(vec2 p, vec2 b, float r) {\n"
    "    r = min(r, min(b.x, b.y)); // Clamp radius\n"
    "    vec2 q = abs(p) - b + r;\n"
    "    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;\n"
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
    "void main() {\n"
    "    // Scene rendering part (uses gl_FragCoord to map to the effect space)\n"
    "    vec2 fragCoord_for_scene = gl_FragCoord.xy;\n"
    "    float iTime = time;\n"
    "    \n"
    "    vec2 p_scene = (fragCoord_for_scene * 2.0 - iResolution.xy) / min(iResolution.x, iResolution.y);\n"
    "    vec3 ro = vec3(0.0, -0.2, iTime * 4.0);\n"
    "    vec3 ray = normalize(vec3(p_scene, 1.5));\n"
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
    "    // --- Rounded Corners Masking using pixel-perfect dimensions ---\n"
    "    // Convert v_texcoord [0,1] to pixel coordinates relative to center\n"
    "    vec2 pixel_pos = (v_texcoord - 0.5) * iResolution;\n"
    "\n"
    "    // Half-extents of the rectangle in pixels\n"
    "    vec2 box_half_extents_pixels = iResolution * 0.5;\n"
    "    \n"
    "    // Corner radius in pixels (constant circular radius regardless of aspect ratio)\n"
    "    float corner_radius_pixels = 20.0; // Adjust this value for desired corner size\n"
    "\n"
    "    // Calculate signed distance to the rounded rectangle edge in pixels\n"
    "    float dist_to_edge = sdfRoundBox2D(pixel_pos, box_half_extents_pixels, corner_radius_pixels);\n"
    "\n"
    "    // Antialiasing width in pixels (typically 1-2 pixels for smooth edges)\n"
    "    float aa_pixels = 1.5;\n"
    "    \n"
    "    // Create alpha mask: 1.0 inside, 0.0 outside, smooth transition at the edge\n"
    "    // Using step function to ensure corners are completely transparent\n"
    "    float shape_alpha_mask;\n"
    "    if (dist_to_edge > aa_pixels * 0.5) {\n"
    "        shape_alpha_mask = 0.0; // Completely outside - fully transparent\n"
    "    } else if (dist_to_edge < -aa_pixels * 0.5) {\n"
    "        shape_alpha_mask = 1.0; // Completely inside - fully opaque\n"
    "    } else {\n"
    "        // Smooth transition zone\n"
    "        shape_alpha_mask = 1.0 - smoothstep(-aa_pixels * 0.5, aa_pixels * 0.5, dist_to_edge);\n"
    "    }\n"
    "    \n"
    "    shape_alpha_mask = clamp(shape_alpha_mask, 0.0, 1.0);\n"
    "\n"
    "    float original_alpha = 0.6; // Original alpha of the content\n"
    "    float final_alpha = original_alpha * shape_alpha_mask;\n"
    "    // --- End Rounded Corners Masking ---\n"
    "\n"
    "    frag_color = vec4(col, final_alpha).bgra;\n"
    "}";

static const char *ssd_vertex_shader_src =
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


static const char *ssd_fragment_shader_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "\n"
    "// Uniforms from the original context\n"
    "uniform vec4 u_color;   // Base tint and alpha from the compositor\n"
    "uniform float time;     // Time uniform for animation (used as iTime)\n"
    "uniform mat3 mvp;       // MVP matrix (kept as requested, not used by this shader's core logic)\n"
    "\n"
    "out vec4 frag_color;    // Output fragment color\n"
    "\n"
    "// --- Start of Inigo Quilez's shader code (adapted) ---\n"
    "// Copyright Inigo Quilez, 2013 - https://iquilezles.org/\n"
    "// I am the sole copyright owner of this Work.\n"
    "// You cannot host, display, distribute or share this Work neither\n"
    "// as it is or altered, here on Shadertoy or anywhere else, in any\n"
    "// form including physical and digital. You cannot use this Work in any\n"
    "// commercial or non-commercial product, website or project. You cannot\n"
    "// sell this Work and you cannot mint an NFTs of it or train a neural\n"
    "// network with it without permission. I share this Work for educational\n"
    "// purposes, and you can link to it, through an URL, proper attribution\n"
    "// and unmodified screenshot, as part of your educational material. If\n"
    "// these conditions are too restrictive please contact me and we'll\n"
    "// definitely work it out.\n"
    "\n"
    "// See here for a tutorial on how to make this:\n"
    "//\n"
    "// https://iquilezles.org/articles/warp\n"
    "\n"
    "//====================================================================\n"
    "\n"
    "const mat2 m = mat2( 0.80,  0.60, -0.60,  0.80 );\n"
    "\n"
    "float noise( in vec2 p )\n"
    "{\n"
    "   return sin(p.x)*sin(p.y);\n"
    "}\n"
    "\n"
    "float fbm4( vec2 p )\n"
    "{\n"
    "    float f = 0.0;\n"
    "    f += 0.5000*noise( p ); p = m*p*2.02;\n"
    "    f += 0.2500*noise( p ); p = m*p*2.03;\n"
    "    f += 0.1250*noise( p ); p = m*p*2.01;\n"
    "    f += 0.0625*noise( p );\n"
    "    return f/0.9375;\n"
    "}\n"
    "\n"
    "float fbm6( vec2 p )\n"
    "{\n"
    "    float f = 0.0;\n"
    "    f += 0.500000*(0.5+0.5*noise( p )); p = m*p*2.02;\n"
    "    f += 0.250000*(0.5+0.5*noise( p )); p = m*p*2.03;\n"
    "    f += 0.125000*(0.5+0.5*noise( p )); p = m*p*2.01;\n"
    "    f += 0.062500*(0.5+0.5*noise( p )); p = m*p*2.04;\n"
    "    f += 0.031250*(0.5+0.5*noise( p )); p = m*p*2.01;\n"
    "    f += 0.015625*(0.5+0.5*noise( p ));\n"
    "    return f/0.96875;\n"
    "}\n"
    "\n"
    "vec2 fbm4_2( vec2 p )\n"
    "{\n"
    "    return vec2(fbm4(p), fbm4(p+vec2(7.8)));\n"
    "}\n"
    "\n"
    "vec2 fbm6_2( vec2 p )\n"
    "{\n"
    "    return vec2(fbm6(p+vec2(16.8)), fbm6(p+vec2(11.5)));\n"
    "}\n"
    "\n"
    "//====================================================================\n"
    "\n"
    "// Input 'time' (original uniform) is used instead of 'iTime' from Shadertoy\n"
    "float func( vec2 q, out vec4 ron )\n"
    "{\n"
    "    q += 0.03*sin( vec2(0.27,0.23)*time*10.0 + length(q)*vec2(4.1,4.3));\n"
    "\n"
    "   vec2 o = fbm4_2( 0.9*q );\n"
    "\n"
    "    o += 0.04*sin( vec2(0.12,0.14)*time*10.0 + length(o));\n"
    "\n"
    "    vec2 n = fbm6_2( 3.0*o );\n"
    "\n"
    "   ron = vec4( o, n );\n"
    "\n"
    "    float f = 0.5 + 0.5*fbm4( 1.8*q + 6.0*n );\n"
    "\n"
    "    return mix( f, f*f*f*3.5, f*abs(n.x) );\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "    // Using a hardcoded resolution, similar to iResolution in Shadertoy.\n"
    "    // The original shader also used a hardcoded vec2 R = vec2(1920.0, 1080.0);\n"
    "    // This value should be adjusted if your target resolution is different.\n"
    "    vec2 iResolution = vec2(1920.0, 1080.0);\n"
    "\n"
    "    // Calculate normalized coordinates 'p' from gl_FragCoord.xy (pixel coordinates)\n"
    "    // This matches the Shadertoy convention: vec2 p = (2.0*fragCoord-iResolution.xy)/iResolution.y;\n"
    "    vec2 p = (2.0*gl_FragCoord.xy - iResolution.xy) / iResolution.y;\n"
    "    float e = 2.0 / iResolution.y; // Small epsilon for derivatives, based on resolution\n"
    "\n"
    "    vec4 on = vec4(0.0); // Will store intermediate values 'o' and 'n' from func\n"
    "    float f = func(p, on); // Call the main pattern generation function\n"
    "\n"
    "   vec3 col = vec3(0.0);\n"
    "    col = mix( vec3(0.2,0.1,0.4), vec3(0.3,0.05,0.05), f );\n"
    "    col = mix( col, vec3(0.9,0.9,0.9), dot(on.zw,on.zw) ); // on.zw corresponds to 'n'\n"
    "    col = mix( col, vec3(0.0,0.0,0.9), 0.2 + 0.5*on.y*on.y ); // on.y corresponds to 'o.y'\n"
    "    col = mix( col, vec3(0.9,0.2,0.2), 0.5*smoothstep(1.2,1.3,abs(on.z)+abs(on.w)) ); // on.z, on.w are n.x, n.y\n"
    "    col = clamp( col*f*2.0, 0.0, 1.0 );\n"
    "    \n"
    "#if 0\n"
    "    // GPU derivatives - bad quality, but fast\n"
    "    // Uses iResolution.x and iResolution.y for scaling derivatives\n"
    "   vec3 nor = normalize( vec3( dFdx(f)*iResolution.x*10.0, 6.0, dFdy(f)*iResolution.y*10.0 ) );\n"
    "#else\n"
    "    // Manual derivatives - better quality, but slower\n"
    "    vec4 kk; // Dummy 'out' variable for func calls\n"
    "   vec3 nor = normalize( vec3( func(p+vec2(e,0.0),kk)-f, \n"
    "                                2.0*e, \n"
    "                                func(p+vec2(0.0,e),kk)-f ) );\n"
    "#endif\n"
    "\n"
    "    vec3 lig = normalize( vec3( 0.9, 0.2, -0.4 ) );\n"
    "    float dif = clamp( 0.3+0.7*dot( nor, lig ), 0.0, 1.0 );\n"
    "    vec3 lin = vec3(0.70,0.90,0.95)*(nor.y*0.5+0.5) + vec3(0.15,0.10,0.05)*dif;\n"
    "    col *= 1.2*lin;\n"
    "   col = 1.0 - col; // Invert colors\n"
    "   col = 1.1*col*col; // Apply contrast/gamma like effect\n"
    "    \n"
    "    // Use alpha from u_color uniform, as in the original shader structure\n"
    "    float final_alpha = u_color.a > 0.0 ? u_color.a : 1.0;\n"
    "    \n"
    "    frag_color = vec4( col, 0.9 ); // Set the final fragment color\n"
    "}\n"
    "// --- End of Inigo Quilez's shader code (adapted) ---\n";

// ... (rest of your includes and global definitions) ...

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
        {"iResolution", &server.rect_shader_resolution_loc},
        {"u_rectResolution", &server.rect_shader_rect_res_loc} 
    };
     if (!create_generic_shader_program(server.renderer, "MeltRectShader",
                                     rect_vertex_shader_src,
                                     rect_fragment_shader_src, // Use the modified one from above
                                     &server.rect_shader_program,
                                     melt_rect_uniforms, sizeof(melt_rect_uniforms) / sizeof(melt_rect_uniforms[0]))) {
        wlr_log(WLR_ERROR, "Failed to create 'Melt' rect shader program.");
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

  

     struct shader_uniform_spec back_uniforms[] = {
        {"mvp",                       &server.back_shader_mvp_loc},
        {"texture_sampler_uniform",   &server.back_shader_tex_loc}, // If FS uses it
        {"time",                      &server.back_shader_time_loc},      // If FS uses it
        {"iResolution",               &server.back_shader_res_loc}       // If FS uses it
        // Add any other uniforms your back_fragment_shader_src DEFINES
        // e.g., {"u_some_color", &server.back_shader_some_other_uniform_loc}
    };

    if (!create_generic_shader_program(server.renderer, "backgroundShader", // Log name
                                     back_vertex_shader_src,  // Your chosen VS for window backgrounds
                                     back_fragment_shader_src, // Your chosen FS for window backgrounds
                                     &server.back_shader_program, // Store program ID here
                                     back_uniforms,
                                     sizeof(back_uniforms) / sizeof(back_uniforms[0]))) {
        wlr_log(WLR_ERROR, "Failed to create Window Background shader program.");
        server_destroy(&server); return 1;
    }


// In main()
 // --- Create SSD Shader (Checkerboard) ---
    struct shader_uniform_spec ssd_shader_uniforms[] = {
        {"mvp", &server.ssd_shader_mvp_loc},
        {"u_color", &server.ssd_shader_color_loc},
        // These are not used by the checkerboard shader, so their locations will be -1.
        // This is fine as render_rect_node doesn't try to set them for SSDs.
        {"time", &server.ssd_shader_time_loc},
        {"iResolution", &server.ssd_shader_rect_pixel_dimensions_loc}
    };

    // Use the correctly named global shader sources
    if (!create_generic_shader_program(server.renderer, "SSDCheckerboardShader", // Updated log name
                                     ssd_vertex_shader_src,  // Use the renamed global VS
                                     ssd_fragment_shader_src, // Use the renamed global FS
                                     &server.ssd_shader_program,
                                     ssd_shader_uniforms,
                                     sizeof(ssd_shader_uniforms) / sizeof(ssd_shader_uniforms[0]))) {
        wlr_log(WLR_ERROR, "Failed to create SSD Checkerboard shader program.");
        server_destroy(&server); return 1;
    }
    // Updated log message to be more accurate for the checkerboard shader
    wlr_log(WLR_INFO, "SSDCheckerboardShader created (ID: %u). MVP@%d, Color@%d. (Unused: Time@%d, iResolution@%d)",
            server.ssd_shader_program,
            server.ssd_shader_mvp_loc,
            server.ssd_shader_color_loc,
            server.ssd_shader_time_loc, // This will likely be -1
            server.ssd_shader_rect_pixel_dimensions_loc); 
wlr_log(WLR_INFO, "SSDOctagramShader created (ID: %u). MVP@%d, Color@%d, Time@%d, RectDims(iRes)@%d",
        server.ssd_shader_program,
        server.ssd_shader_mvp_loc,
        server.ssd_shader_color_loc,
        server.ssd_shader_time_loc,
        server.ssd_shader_rect_pixel_dimensions_loc);
wlr_log(WLR_INFO, "SSDMultiColorRoundedShader created (ID: %u). MVP@%d, Color@%d, RectDims@%d",
        server.ssd_shader_program, server.ssd_shader_mvp_loc, server.ssd_shader_color_loc, server.ssd_shader_rect_pixel_dimensions_loc);
wlr_log(WLR_INFO, "SSDColorShader created (ID: %u). MVP@%d, Color@%d",
        server.ssd_shader_program, server.ssd_shader_mvp_loc, server.ssd_shader_color_loc);
    // Add a log to confirm locations (optional but helpful)
    wlr_log(WLR_INFO, "backgroundShader created (ID: %u). MVP@%d, Tex@%d, Time@%d, iRes@%d",
            server.back_shader_program,
            server.back_shader_mvp_loc,
            server.back_shader_tex_loc,
            server.back_shader_time_loc,
            server.back_shader_res_loc);
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

  // ...
server.xdg_shell = wlr_xdg_shell_create(server.wl_display, 3);
server.new_xdg_toplevel.notify = server_new_xdg_toplevel; // KEEP THIS
wl_signal_add(&server.xdg_shell->events.new_toplevel, &server.new_xdg_toplevel);
// ...

server.xdg_decoration_manager = wlr_xdg_decoration_manager_v1_create(server.wl_display);
if (!server.xdg_decoration_manager) {
    wlr_log(WLR_ERROR, "Failed to create XDG decoration manager v1");
    return 1;
}
server.xdg_decoration_new.notify = server_new_xdg_decoration; // Ensure this points to your callback
wl_signal_add(&server.xdg_decoration_manager->events.new_toplevel_decoration,
              &server.xdg_decoration_new); // Ensure this is the correct listener struct member
wlr_log(WLR_ERROR, "!!!!!!!!!! XDG DECORATION LISTENER ADDED (main) for manager %p, listener %p, notify %p",
    (void*)server.xdg_decoration_manager, (void*)&server.xdg_decoration_new, (void*)server_new_xdg_decoration);
// ...
 

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