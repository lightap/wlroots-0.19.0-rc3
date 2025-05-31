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

    GLuint shader_program; // Custom shader program for animations
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
};

struct tinywl_toplevel {
    struct wl_list link;
    struct tinywl_server *server;
    struct wlr_xdg_toplevel *xdg_toplevel;

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

    // Animation state
    bool is_animating;
    float scale;          // Current scale factor (1.0 = normal size)
    float target_scale;   // Target scale for animation (e.g., 1.0 for zoom in, 0.0 for minimize)
    float animation_start; // Animation start time (in seconds)
    float animation_duration; // Duration in seconds (e.g., 0.3 for 300ms)
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

/* Function declarations */
//struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display, struct wlr_egl *egl);
//struct wlr_renderer *wlr_gles2_renderer_create_surfaceless(void);
struct wlr_allocator *wlr_rdp_allocator_create(struct wlr_renderer *renderer);
void rdp_transmit_surface(struct wlr_buffer *buffer);
//freerdp_peer *get_global_rdp_peer(void);
struct wlr_egl *setup_surfaceless_egl(struct tinywl_server *server) ;
void cleanup_egl(struct tinywl_server *server);
static void process_cursor_motion(struct tinywl_server *server, uint32_t time);

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

static bool create_shader_program(struct tinywl_server *server) {
    struct wlr_egl *egl = wlr_gles2_renderer_get_egl(server->renderer);
    GLenum err;
    if (!egl) {
        wlr_log(WLR_ERROR, "No EGL context available for renderer");
        return false;
    }

    if (!wlr_egl_make_current(egl, NULL)) {
        wlr_log(WLR_ERROR, "Failed to make EGL context current for shader creation");
        return false;
    }
    while ((err = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "OpenGL Error before shader creation: 0x%x", err); }

    const char *vendor = (const char *)glGetString(GL_VENDOR);
    const char *renderer_str = (const char *)glGetString(GL_RENDERER); 
    const char *version = (const char *)glGetString(GL_VERSION);
    const char *shading_lang = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    wlr_log(WLR_INFO, "GL_VENDOR: %s", vendor ? vendor : "unknown");
    wlr_log(WLR_INFO, "GL_RENDERER: %s", renderer_str ? renderer_str : "unknown");
    wlr_log(WLR_INFO, "GL_VERSION: %s", version ? version : "unknown");
    wlr_log(WLR_INFO, "GL_SHADING_LANGUAGE_VERSION: %s", shading_lang ? shading_lang : "unknown");
/*
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
    "    v_texcoord = position; // TEST: Use position (0-1) directly as texcoords\n" 
    "}\n";

    const char *fragment_shader_src = 
        "#version 300 es\n" // Keep this for now, but the key is texture2D
        "precision mediump float;\n"
        "in vec2 v_texcoord;\n"
        "out vec4 frag_color;\n"
        "uniform sampler2D texture;\n" // sampler2D is correct for both
        "void main() {\n"
        "    frag_color = texture2D(texture, v_texcoord).bgra;\n" // CHANGED HERE
        "}\n";
*/
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
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
    if (!vertex_shader || !fragment_shader) {
        if (vertex_shader) glDeleteShader(vertex_shader);
        if (fragment_shader) glDeleteShader(fragment_shader);
        wlr_egl_unset_current(egl);
        return false;
    }

    server->shader_program = glCreateProgram();
    glAttachShader(server->shader_program, vertex_shader);
    glAttachShader(server->shader_program, fragment_shader);
    glLinkProgram(server->shader_program);
    while ((err = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "OpenGL Error after glLinkProgram: 0x%x", err); }

    GLint success;
    glGetProgramiv(server->shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char log_buf[512]; 
        glGetProgramInfoLog(server->shader_program, sizeof(log_buf), NULL, log_buf);
        wlr_log(WLR_ERROR, "Shader program linking failed: %s", log_buf);
        glDeleteProgram(server->shader_program);
        server->shader_program = 0;
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        wlr_egl_unset_current(egl);
        return false;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    wlr_egl_unset_current(egl);
    wlr_log(WLR_INFO, "Shader program created successfully: program=%u",
            server->shader_program);
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

static void reset_cursor_mode(struct tinywl_server *server) {
    server->cursor_mode = TINYWL_CURSOR_PASSTHROUGH;
    server->grabbed_toplevel = NULL;
}

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


/*
static void scene_buffer_iterator(struct wlr_scene_buffer *scene_buffer,
                                  int sx, int sy, void *user_data) {
    struct render_data *rdata = user_data;
    struct wlr_renderer *renderer = rdata->renderer;
    struct wlr_output *output = rdata->output;
    struct tinywl_server *server = rdata->server;
 //   GLenum gl_err_iter; 
    struct wlr_gles2_texture_attribs tex_attribs; 

    enum wl_output_transform initial_buffer_transform_enum = scene_buffer->transform;

    wlr_log(WLR_INFO, "[ITERATOR:%s] VISITED scene_buffer %p (internal wlr_buffer %p) at sx=%d, sy=%d. Node Enabled: %d. Has .buffer: %s. Output WxH: %dx%d OutputTransform: %d RawBufferTransformEnum: %d",
            output->name, (void*)scene_buffer, (void*)scene_buffer->buffer, sx, sy,
            scene_buffer->node.enabled,
            scene_buffer->buffer ? "yes" : "no",
            output->width, output->height, output->transform,
            initial_buffer_transform_enum);

    // Add safety checks for the render_data structure
    if (!rdata || !rdata->server || !rdata->renderer || !rdata->output) {
        wlr_log(WLR_ERROR, "Invalid render_data or null pointers in render_data");
        return;
    }
    
    // Comprehensive validation of server and toplevels list
    if (!server) {
        wlr_log(WLR_ERROR, "Server pointer is null");
        return;
    }
    
    // Check if the toplevels list is properly initialized
    if (server->toplevels.next == NULL || server->toplevels.prev == NULL) {
        wlr_log(WLR_ERROR, "server->toplevels list is not initialized (next=%p, prev=%p)", 
                (void*)server->toplevels.next, (void*)server->toplevels.prev);
        return;
    }
    
    // Additional corruption check - in a valid empty list, next and prev should point to itself
    if (wl_list_empty(&server->toplevels)) {
        if (server->toplevels.next != &server->toplevels || server->toplevels.prev != &server->toplevels) {
            wlr_log(WLR_ERROR, "Empty toplevels list is corrupted: next=%p, prev=%p, expected=%p",
                    (void*)server->toplevels.next, (void*)server->toplevels.prev, (void*)&server->toplevels);
            return;
        }
        wlr_log(WLR_DEBUG, "Server toplevels list is empty but valid");
    }

    if (!scene_buffer->node.enabled || !scene_buffer->buffer) {
        return;
    }
    if (output->width == 0 || output->height == 0) {
        wlr_log(WLR_ERROR, "[ITERATOR:%s] Output width (%d) or height (%d) is ZERO.", output->name, output->width, output->height);
        return;
    }
    
    struct wlr_scene_surface *current_scene_surface = wlr_scene_surface_try_from_buffer(scene_buffer);
    struct wlr_surface *surface_to_render = current_scene_surface ? current_scene_surface->surface : NULL;

    if (!surface_to_render) {
        struct wlr_texture *fallback_texture = wlr_texture_from_buffer(renderer, scene_buffer->buffer);
        if (fallback_texture) {
            struct wlr_render_texture_options tex_opts = {
                .texture = fallback_texture,
                .dst_box = {sx, sy, fallback_texture->width, fallback_texture->height},
                .transform = initial_buffer_transform_enum,
                .blend_mode = WLR_RENDER_BLEND_MODE_PREMULTIPLIED,
            };
            if (rdata->pass) {
                wlr_render_pass_add_texture(rdata->pass, &tex_opts);
            }
            wlr_texture_destroy(fallback_texture);
        }
        return;
    }
    
    struct tinywl_toplevel *toplevel = NULL;
    struct wlr_scene_node *iter_node_find = &scene_buffer->node;
    while(iter_node_find) {
        if(iter_node_find->data) {
            struct tinywl_toplevel *ptl = iter_node_find->data;
            bool is_in_list = false;
        //    struct tinywl_toplevel *check_tl;
            
            // Add comprehensive safety checks before iterating the list
            if (server->toplevels.next != NULL && server->toplevels.prev != NULL && 
                server->toplevels.next != &server->toplevels && 
                !wl_list_empty(&server->toplevels)) {
                
                struct tinywl_toplevel *check_tl, *tmp_tl;
                
                // Use the safe iteration macro to prevent crashes from corrupted lists
                wl_list_for_each_safe(check_tl, tmp_tl, &server->toplevels, link) {
                    if (check_tl == ptl) { 
                        is_in_list = true; 
                        break; 
                    }
                }
            } else {
                wlr_log(WLR_ERROR, "[ITERATOR:%s] server->toplevels list appears corrupted: next=%p, prev=%p", 
                        output->name, (void*)server->toplevels.next, (void*)server->toplevels.prev);
            }
            if (is_in_list) { toplevel = ptl; break; }
        }
        if (!iter_node_find->parent) break;
        iter_node_find = &iter_node_find->parent->node;
    }

    float anim_scale_factor = 1.0f;
    if (toplevel) {
        if (toplevel->is_animating) {
            struct timespec now_anim; clock_gettime(CLOCK_MONOTONIC, &now_anim);
            float current_time_anim = now_anim.tv_sec + now_anim.tv_nsec / 1e9f;
            float elapsed = current_time_anim - toplevel->animation_start;
            float t = elapsed / toplevel->animation_duration;
            if (t >= 1.0f) {
                t = 1.0f; toplevel->is_animating = false;
                anim_scale_factor = toplevel->target_scale;
                toplevel->scale = toplevel->target_scale;
                if (toplevel->target_scale == 0.0f && surface_to_render->mapped && toplevel->scene_tree) {
                    wlr_scene_node_set_enabled(&toplevel->scene_tree->node, false);
                }
            } else {
                anim_scale_factor = toplevel->scale + (toplevel->target_scale - toplevel->scale) * t;
            }
            if (toplevel->is_animating) wlr_output_schedule_frame(output);
        } else {
            anim_scale_factor = toplevel->scale;
        }
    }

    if (anim_scale_factor <= 0.001f && toplevel && toplevel->target_scale == 0.0f) return;
    if (anim_scale_factor <= 0.001f) anim_scale_factor = 0.001f;

    wlr_log(WLR_INFO, "[ITERATOR:%s] Creating texture from scene_buffer->buffer %p",
            output->name, (void*)scene_buffer->buffer);
    struct wlr_texture *texture = wlr_texture_from_buffer(renderer, scene_buffer->buffer);
    if (!texture) {
         wlr_log(WLR_ERROR, "[ITERATOR:%s] FAILED to create texture from buffer %p", output->name, scene_buffer->buffer);
        return;
    }
     wlr_log(WLR_INFO, "[ITERATOR:%s] Created wlr_texture %p from scene_buffer->buffer %p",
            output->name, (void*)texture, (void*)scene_buffer->buffer);
    
    wlr_gles2_texture_get_attribs(texture, &tex_attribs);
    
    struct wlr_box render_box = {
        .x = (int)round(sx + (texture->width * (1.0f - anim_scale_factor) / 2.0f)),
        .y = (int)round(sy + (texture->height * (1.0f - anim_scale_factor) / 2.0f)),
        .width = (int)round(texture->width * anim_scale_factor),
        .height = (int)round(texture->height * anim_scale_factor),
    };
    if (render_box.width <= 0 || render_box.height <= 0) { wlr_texture_destroy(texture); return; }

    float final_shader_mvp[9];
    wlr_matrix_identity(final_shader_mvp);

    float box_scale_x = (float)render_box.width * (2.0f / output->width);
    float box_scale_y = (float)render_box.height * (-2.0f / output->height);
    float box_translate_x = ((float)render_box.x / output->width) * 2.0f - 1.0f;
    float box_translate_y = ((float)render_box.y / output->height) * -2.0f + 1.0f;

    if (output->transform == WL_OUTPUT_TRANSFORM_FLIPPED_180) { 
        final_shader_mvp[0] = -box_scale_x;
        final_shader_mvp[4] = -box_scale_y;
        final_shader_mvp[6] = -box_translate_x;
        final_shader_mvp[7] = -box_translate_y;
    } else if (output->transform == WL_OUTPUT_TRANSFORM_NORMAL) {
        final_shader_mvp[0] = box_scale_x;
        final_shader_mvp[4] = box_scale_y;
        final_shader_mvp[6] = box_translate_x;
        final_shader_mvp[7] = box_translate_y;
    } else {
        float mvp_for_normal_box_temp[9];
        mvp_for_normal_box_temp[0] = box_scale_x; mvp_for_normal_box_temp[1] = 0.0f; mvp_for_normal_box_temp[2] = 0.0f;
        mvp_for_normal_box_temp[3] = 0.0f; mvp_for_normal_box_temp[4] = box_scale_y; mvp_for_normal_box_temp[5] = 0.0f;
        mvp_for_normal_box_temp[6] = box_translate_x; mvp_for_normal_box_temp[7] = box_translate_y; mvp_for_normal_box_temp[8] = 1.0f;
        float output_transform_matrix_ndc_temp[9];
        wlr_matrix_identity(output_transform_matrix_ndc_temp);
        wlr_matrix_transform(output_transform_matrix_ndc_temp, output->transform);
        wlr_matrix_multiply(final_shader_mvp, output_transform_matrix_ndc_temp, mvp_for_normal_box_temp);
    }
    
    if (initial_buffer_transform_enum != WL_OUTPUT_TRANSFORM_NORMAL) {
        float temp_mvp_for_buffer_tf[9];
        memcpy(temp_mvp_for_buffer_tf, final_shader_mvp, sizeof(final_shader_mvp));
        float actual_buffer_tf_matrix[9];
        wlr_matrix_transform(actual_buffer_tf_matrix, initial_buffer_transform_enum);
        wlr_matrix_multiply(final_shader_mvp, temp_mvp_for_buffer_tf, actual_buffer_tf_matrix);
    }

    wlr_log(WLR_ERROR, "[ITERATOR:%s] FinalShaderMVP: [%.6f %.6f %.6f] [%.6f %.6f %.6f] [%.6f %.6f %.6f]",
        output->name,
        final_shader_mvp[0], final_shader_mvp[1], final_shader_mvp[2],
        final_shader_mvp[3], final_shader_mvp[4], final_shader_mvp[5],
        final_shader_mvp[6], final_shader_mvp[7], final_shader_mvp[8]);
    
    glViewport(0, 0, output->width, output->height);
    glScissor(0, 0, output->width, output->height);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST); 
    glDepthMask(GL_TRUE);     
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 
    
    glUseProgram(server->shader_program);
    GLint mvp_loc = glGetUniformLocation(server->shader_program, "mvp");
    if (mvp_loc != -1) {
        glUniformMatrix3fv(mvp_loc, 1, GL_FALSE, final_shader_mvp);
    } else {
        wlr_log(WLR_ERROR, "MVP_Uniform_Not_Found in shader program %u", server->shader_program);
    }

    GLint tex_loc = glGetUniformLocation(server->shader_program, "texture"); 
    if (tex_loc != -1) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(tex_attribs.target, tex_attribs.tex); 
        glTexParameteri(tex_attribs.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(tex_attribs.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glUniform1i(tex_loc, 0);
        wlr_log(WLR_DEBUG, "[ITERATOR] Texture uniform 'texture' FOUND at %d and bound to unit 0. Filters set to GL_LINEAR.", tex_loc);
    } else {
        wlr_log(WLR_ERROR, "[ITERATOR] Texture uniform 'texture' NOT FOUND! This is an error for texture shader.");
    }

    glBindVertexArray(server->quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, server->quad_vbo); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, server->quad_ibo); 

    wlr_log(WLR_DEBUG, "[ITERATOR] Drawing elements (texture shader).");
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    GLenum draw_err = glGetError();
    if (draw_err != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "[ITERATOR:%s] glDrawElements error: 0x%x", output->name, draw_err);
    } else {
        wlr_log(WLR_DEBUG, "[ITERATOR:%s] glDrawElements called successfully.", output->name);
    }
    
    glFinish(); 
    wlr_log(WLR_DEBUG, "[ITERATOR] glFinish completed.");

    if (tex_loc != -1) {
      glBindTexture(tex_attribs.target, 0); 
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
    glUseProgram(0);

    wlr_texture_destroy(texture);
}*/

static void scene_buffer_iterator(struct wlr_scene_buffer *scene_buffer,
                                  int sx, int sy, void *user_data) {
    struct render_data *rdata = user_data;
    struct wlr_renderer *renderer = rdata->renderer;
    struct wlr_output *output = rdata->output;
    struct tinywl_server *server = rdata->server;
    struct wlr_gles2_texture_attribs tex_attribs;

    if (!rdata || !rdata->server || !rdata->renderer || !rdata->output) {
        wlr_log(WLR_ERROR, "Invalid render_data or null pointers");
        return;
    }

    if (!scene_buffer->node.enabled || !scene_buffer->buffer) {
        return;
    }

    if (output->width == 0 || output->height == 0) {
        wlr_log(WLR_ERROR, "[ITERATOR:%s] Output width (%d) or height (%d) is ZERO.", output->name, output->width, output->height);
        return;
    }

    struct wlr_scene_surface *current_scene_surface = wlr_scene_surface_try_from_buffer(scene_buffer);
    struct wlr_surface *surface_to_render = current_scene_surface ? current_scene_surface->surface : NULL;

    if (!surface_to_render) {
        struct wlr_texture *fallback_texture = wlr_texture_from_buffer(renderer, scene_buffer->buffer);
        if (fallback_texture) {
            struct wlr_render_texture_options tex_opts = {
                .texture = fallback_texture,
                .dst_box = {sx, sy, fallback_texture->width, fallback_texture->height},
                .transform = scene_buffer->transform,
                .blend_mode = WLR_RENDER_BLEND_MODE_PREMULTIPLIED,
            };
            if (rdata->pass) {
                wlr_render_pass_add_texture(rdata->pass, &tex_opts);
            }
            wlr_texture_destroy(fallback_texture);
        }
        return;
    }

    struct tinywl_toplevel *toplevel = NULL;
    struct wlr_scene_node *node = &scene_buffer->node;
    while (node) {
        if (node->data) {
            struct tinywl_toplevel *ptl = node->data;
            bool is_in_list = false;
            if (server->toplevels.next && server->toplevels.prev &&
                !wl_list_empty(&server->toplevels)) {
                struct tinywl_toplevel *check_tl, *tmp;
                wl_list_for_each_safe(check_tl, tmp, &server->toplevels, link) {
                    if (check_tl == ptl) {
                        is_in_list = true;
                        break;
                    }
                }
            }
            if (is_in_list) {
                toplevel = ptl;
                break;
            }
        }
        if (!node->parent) break;
        node = &node->parent->node;
    }

    float anim_scale_factor = 1.0f;
    if (toplevel && toplevel->is_animating) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        float current_time = now.tv_sec + now.tv_nsec / 1e9f;
        float elapsed = current_time - toplevel->animation_start;
        float t = elapsed / toplevel->animation_duration;
        if (t >= 1.0f) {
            t = 1.0f;
            toplevel->is_animating = false;
            anim_scale_factor = toplevel->target_scale;
            toplevel->scale = toplevel->target_scale;
            if (toplevel->target_scale == 0.0f && surface_to_render->mapped && toplevel->scene_tree) {
                wlr_scene_node_set_enabled(&toplevel->scene_tree->node, false);
            }
        } else {
            anim_scale_factor = toplevel->scale + (toplevel->target_scale - toplevel->scale) * t;
        }
        if (toplevel->is_animating) {
            wlr_output_schedule_frame(output);
        }
    } else if (toplevel) {
        anim_scale_factor = toplevel->scale;
    }

    if (anim_scale_factor <= 0.001f && toplevel && toplevel->target_scale == 0.0f) {
        return;
    }
    if (anim_scale_factor <= 0.001f) {
        anim_scale_factor = 0.001f;
    }

    struct wlr_texture *texture = wlr_texture_from_buffer(renderer, scene_buffer->buffer);
    if (!texture) {
        wlr_log(WLR_ERROR, "[ITERATOR:%s] Failed to create texture from buffer %p", output->name, scene_buffer->buffer);
        return;
    }

    wlr_gles2_texture_get_attribs(texture, &tex_attribs);

    struct wlr_box render_box = {
        .x = (int)round(sx + (texture->width * (1.0f - anim_scale_factor) / 2.0f)),
        .y = (int)round(sy + (texture->height * (1.0f - anim_scale_factor) / 2.0f)),
        .width = (int)round(texture->width * anim_scale_factor),
        .height = (int)round(texture->height * anim_scale_factor),
    };

    if (render_box.width <= 0 || render_box.height <= 0) {
        wlr_texture_destroy(texture);
        return;
    }

    float mvp[9];
    wlr_matrix_identity(mvp);

    float box_scale_x = (float)render_box.width * (2.0f / output->width);
    float box_scale_y = (float)render_box.height * (-2.0f / output->height);
    float box_translate_x = ((float)render_box.x / output->width) * 2.0f - 1.0f;
    float box_translate_y = ((float)render_box.y / output->height) * -2.0f + 1.0f;

    if (output->transform == WL_OUTPUT_TRANSFORM_FLIPPED_180) {
        mvp[0] = -box_scale_x;
        mvp[4] = -box_scale_y;
        mvp[6] = -box_translate_x;
        mvp[7] = -box_translate_y;
    } else if (output->transform == WL_OUTPUT_TRANSFORM_NORMAL) {
        mvp[0] = box_scale_x;
        mvp[4] = box_scale_y;
        mvp[6] = box_translate_x;
        mvp[7] = box_translate_y;
    } else {
        float temp_mvp[9];
        temp_mvp[0] = box_scale_x; temp_mvp[1] = 0.0f; temp_mvp[2] = 0.0f;
        temp_mvp[3] = 0.0f; temp_mvp[4] = box_scale_y; temp_mvp[5] = 0.0f;
        temp_mvp[6] = box_translate_x; temp_mvp[7] = box_translate_y; temp_mvp[8] = 1.0f;
        float transform_matrix[9];
        wlr_matrix_transform(transform_matrix, output->transform);
        wlr_matrix_multiply(mvp, transform_matrix, temp_mvp);
    }

    if (scene_buffer->transform != WL_OUTPUT_TRANSFORM_NORMAL) {
        float temp_mvp[9];
        memcpy(temp_mvp, mvp, sizeof(mvp));
        float buffer_matrix[9];
        wlr_matrix_transform(buffer_matrix, scene_buffer->transform);
        wlr_matrix_multiply(mvp, temp_mvp, buffer_matrix);
    }

    GLint mvp_loc = glGetUniformLocation(server->shader_program, "mvp");
    if (mvp_loc != -1) {
        glUniformMatrix3fv(mvp_loc, 1, GL_FALSE, mvp);
    }



    GLint tex_loc = glGetUniformLocation(server->shader_program, "texture_sampler_uniform");
    if (tex_loc != -1) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(tex_attribs.target, tex_attribs.tex);
        glTexParameteri(tex_attribs.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(tex_attribs.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glUniform1i(tex_loc, 0);
    }

    mvp_loc = glGetUniformLocation(server->shader_program, "mvp");
if (mvp_loc != -1) {
    glUniformMatrix3fv(mvp_loc, 1, GL_FALSE, mvp);
}

// ADD THE NEW UNIFORMS HERE:
// Time uniform for animation
struct timespec now;
clock_gettime(CLOCK_MONOTONIC, &now);
float time_value = now.tv_sec + now.tv_nsec / 1000000000.0f;
GLint time_loc = glGetUniformLocation(server->shader_program, "time");
if (time_loc != -1) {
    glUniform1f(time_loc, time_value);
}

// Resolution uniform  
float resolution[2] = {(float)output->width, (float)output->height};
GLint resolution_loc = glGetUniformLocation(server->shader_program, "resolution");
if (resolution_loc != -1) {
    glUniform2fv(resolution_loc, 1, resolution);
}

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindTexture(tex_attribs.target, 0);
    wlr_texture_destroy(texture);
}

// --- Revised output_frame ---
// Add a counter to tinywl_output if you don't have one for debugging
/*
struct tinywl_output {
    // ... existing ...
    int frame_count; // Add this
};
*/

#include <pthread.h>
#include <pixman.h>
#include <string.h>

static pthread_mutex_t rdp_transmit_mutex = PTHREAD_MUTEX_INITIALIZER;
static int global_initial_frames_to_force_render = 5;

struct scene_diagnostics_data {
    struct wlr_output *output;
    int buffer_count;
};



#include <pthread.h>
#include <pixman.h>
#include <string.h> // For memcpy if you use it, not directly in this function now


/*
static void output_frame(struct wl_listener *listener, void *data) {
    struct tinywl_output *output_wrapper = wl_container_of(listener, output_wrapper, frame);
    struct wlr_output *wlr_output = output_wrapper->wlr_output;
    struct tinywl_server *server = output_wrapper->server;
    struct wlr_scene *scene = server->scene;

    // Early validation checks
    if (wlr_output->frame_pending) {
        return; // Skip logging for performance
    }

    if (!wlr_output->enabled || !wlr_output->renderer || !wlr_output->allocator) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(scene, wlr_output);
        if (scene_output) {
            wlr_scene_output_send_frame_done(scene_output, &now);
        }
        return;
    }

    struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(scene, wlr_output);
    if (!scene_output) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] No scene_output found.", wlr_output->name);
        if (wlr_output->enabled) {
            wlr_output_schedule_frame(wlr_output);
        }
        return;
    }

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    if (!wlr_renderer_is_gles2(wlr_output->renderer)) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] Renderer is not GLES2.", wlr_output->name);
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

    bool use_custom_shader = (server->shader_program != 0);

    // Initialize damage region and output state
    pixman_region32_t damage;
    pixman_region32_init(&damage);
    struct wlr_output_state state;
    wlr_output_state_init(&state);

    // Build scene output state with damage detection
    struct wlr_scene_output_state_options options = {0};
    // Note: damage tracking may not be available in this wlroots version
    
    bool has_damage = wlr_scene_output_build_state(scene_output, &state, &options);

    if (!use_custom_shader) {
        // Standard rendering path - optimized
        if (!has_damage) {
            // No damage, skip render (this saves significant performance)
            pixman_region32_fini(&damage);
            wlr_output_state_finish(&state);
            wlr_scene_output_send_frame_done(scene_output, &now);
            return;
        }

        // Transmit the built buffer
        if (state.committed & WLR_OUTPUT_STATE_BUFFER && state.buffer &&
            state.buffer->width > 0 && state.buffer->height > 0) {
            
            pthread_mutex_lock(&rdp_transmit_mutex);
            struct wlr_buffer *locked_buffer = wlr_buffer_lock(state.buffer);
            if (locked_buffer) {
                rdp_transmit_surface(locked_buffer);
                wlr_buffer_unlock(locked_buffer);
            }
            pthread_mutex_unlock(&rdp_transmit_mutex);
        }
        
        pixman_region32_fini(&damage);
        wlr_output_state_finish(&state);
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

    // --- Custom Shader Path - Optimized ---
    
    // Optimized animation check - only check when we might have animations
    bool needs_render_this_frame = has_damage || (global_initial_frames_to_force_render > 0);
    
    if (!needs_render_this_frame) {
        // Only check animations if we haven't already determined we need to render
        struct tinywl_toplevel *tl_iter;
        wl_list_for_each(tl_iter, &server->toplevels, link) {
            if (tl_iter->is_animating && tl_iter->xdg_toplevel &&
                tl_iter->xdg_toplevel->base && tl_iter->xdg_toplevel->base->surface &&
                tl_iter->xdg_toplevel->base->surface->mapped) {
                needs_render_this_frame = true;
                break; // Early exit once we find one animation
            }
        }
    }

    if (global_initial_frames_to_force_render > 0) {
        needs_render_this_frame = true;
        global_initial_frames_to_force_render--;
    }
    
    if (!needs_render_this_frame) {
        pixman_region32_fini(&damage);
        wlr_output_state_finish(&state);
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

    // Clean up the build state since we're doing custom rendering
    wlr_output_state_finish(&state);

    // Create new state for custom rendering
    struct wlr_output_state custom_state;
    wlr_output_state_init(&custom_state);
    
    // Use actual damage region instead of full screen when possible
    if (pixman_region32_not_empty(&damage)) {
        wlr_output_state_set_damage(&custom_state, &damage);
    } else {
        pixman_region32_t full_damage;
        pixman_region32_init_rect(&full_damage, 0, 0, wlr_output->width, wlr_output->height);
        wlr_output_state_set_damage(&custom_state, &full_damage);
        pixman_region32_fini(&full_damage);
    }

    struct wlr_render_pass *pass = wlr_output_begin_render_pass(wlr_output, &custom_state, NULL);
    if (!pass) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] wlr_output_begin_render_pass FAILED.", wlr_output->name);
        pixman_region32_fini(&damage);
        wlr_output_state_finish(&custom_state);
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

    // Minimize GL state changes - set clear color once at startup, not every frame
    glClear(GL_COLOR_BUFFER_BIT);

    struct render_data rdata = {
        .renderer = server->renderer,
        .shader_program = server->shader_program,
        .server = server,
        .output = wlr_output,
        .pass = pass 
    };

    wlr_scene_node_for_each_buffer(&scene->tree.node, scene_buffer_iterator, &rdata);
    
    // Check GL errors only in debug builds
    #ifdef DEBUG
    GLenum gl_error = glGetError();
    if (gl_error != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] GL error: 0x%x", wlr_output->name, gl_error);
    }
    #endif

    if (!wlr_render_pass_submit(pass)) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] wlr_render_pass_submit FAILED.", wlr_output->name);
    }

    // Transmit the custom rendered buffer
    if (custom_state.committed & WLR_OUTPUT_STATE_BUFFER && custom_state.buffer &&
        custom_state.buffer->width > 0 && custom_state.buffer->height > 0) {
        
        pthread_mutex_lock(&rdp_transmit_mutex);
        struct wlr_buffer *locked_buffer = wlr_buffer_lock(custom_state.buffer);
        if (locked_buffer) {
            rdp_transmit_surface(locked_buffer);
            wlr_buffer_unlock(locked_buffer);
        }
        pthread_mutex_unlock(&rdp_transmit_mutex);
    }

    // Cleanup
    pixman_region32_fini(&damage);
    wlr_output_state_finish(&custom_state);
    wlr_scene_output_send_frame_done(scene_output, &now);
}*/

static void output_frame(struct wl_listener *listener, void *data) {
    struct tinywl_output *output_wrapper = wl_container_of(listener, output_wrapper, frame);
    struct wlr_output *wlr_output = output_wrapper->wlr_output;
    struct tinywl_server *server = output_wrapper->server;
    struct wlr_scene *scene = server->scene;

    if (wlr_output->frame_pending) {
        return;
    }

    if (!wlr_output->enabled || !wlr_output->renderer || !wlr_output->allocator) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(scene, wlr_output);
        if (scene_output) {
            wlr_scene_output_send_frame_done(scene_output, &now);
        }
        return;
    }

    struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(scene, wlr_output);
    if (!scene_output) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] No scene_output found.", wlr_output->name);
        if (wlr_output->enabled) {
            wlr_output_schedule_frame(wlr_output);
        }
        return;
    }

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    if (!wlr_renderer_is_gles2(wlr_output->renderer)) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] Renderer is not GLES2.", wlr_output->name);
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

    bool use_custom_shader = (server->shader_program != 0);

    pixman_region32_t damage;
    pixman_region32_init(&damage);
    struct wlr_output_state state;
    wlr_output_state_init(&state);

    struct wlr_scene_output_state_options options = {0};
    bool has_damage = wlr_scene_output_build_state(scene_output, &state, &options);

    if (!use_custom_shader) {
        if (!has_damage) {
            pixman_region32_fini(&damage);
            wlr_output_state_finish(&state);
            wlr_scene_output_send_frame_done(scene_output, &now);
            return;
        }

        if (state.committed & WLR_OUTPUT_STATE_BUFFER && state.buffer &&
            state.buffer->width > 0 && state.buffer->height > 0) {
            pthread_mutex_lock(&rdp_transmit_mutex);
            struct wlr_buffer *locked_buffer = wlr_buffer_lock(state.buffer);
            if (locked_buffer) {
                rdp_transmit_surface(locked_buffer);
                wlr_buffer_unlock(locked_buffer);
            }
            pthread_mutex_lock(&rdp_transmit_mutex);
        }

        pixman_region32_fini(&damage);
        wlr_output_state_finish(&state);
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

    // --- Custom Shader Path ---
    bool needs_render_this_frame = has_damage || (global_initial_frames_to_force_render > 0);
    if (!needs_render_this_frame) {
        struct tinywl_toplevel *tl_iter;
        wl_list_for_each(tl_iter, &server->toplevels, link) {
            if (tl_iter->is_animating && tl_iter->xdg_toplevel &&
                tl_iter->xdg_toplevel->base && tl_iter->xdg_toplevel->base->surface &&
                tl_iter->xdg_toplevel->base->surface->mapped) {
                needs_render_this_frame = true;
                break;
            }
        }
    }

    if (global_initial_frames_to_force_render > 0) {
        needs_render_this_frame = true;
        global_initial_frames_to_force_render--;
    }

    if (!needs_render_this_frame) {
        pixman_region32_fini(&damage);
        wlr_output_state_finish(&state);
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

    wlr_output_state_finish(&state);

    struct wlr_output_state custom_state;
    wlr_output_state_init(&custom_state);

    if (pixman_region32_not_empty(&damage)) {
        wlr_output_state_set_damage(&custom_state, &damage);
    } else {
        pixman_region32_t full_damage;
        pixman_region32_init_rect(&full_damage, 0, 0, wlr_output->width, wlr_output->height);
        wlr_output_state_set_damage(&custom_state, &full_damage);
        pixman_region32_fini(&full_damage);
    }

    struct wlr_render_pass *pass = wlr_output_begin_render_pass(wlr_output, &custom_state, NULL);
    if (!pass) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] wlr_output_begin_render_pass FAILED.", wlr_output->name);
        pixman_region32_fini(&damage);
        wlr_output_state_finish(&custom_state);
        wlr_scene_output_send_frame_done(scene_output, &now);
        return;
    }

//glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
    // Set up GL state once per frame
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, wlr_output->width, wlr_output->height);
    glScissor(0, 0, wlr_output->width, wlr_output->height);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glUseProgram(server->shader_program);
    glBindVertexArray(server->quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, server->quad_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, server->quad_ibo);

    struct render_data rdata = {
        .renderer = server->renderer,
        .shader_program = server->shader_program,
        .server = server,
        .output = wlr_output,
        .pass = pass
    };

    wlr_scene_node_for_each_buffer(&scene->tree.node, scene_buffer_iterator, &rdata);

    // Reset GL state
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glDisable(GL_SCISSOR_TEST);

    if (!wlr_render_pass_submit(pass)) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] wlr_render_pass_submit FAILED.", wlr_output->name);
    }

    if (custom_state.committed & WLR_OUTPUT_STATE_BUFFER && custom_state.buffer &&
        custom_state.buffer->width > 0 && custom_state.buffer->height > 0) {
        pthread_mutex_lock(&rdp_transmit_mutex);
        struct wlr_buffer *locked_buffer = wlr_buffer_lock(custom_state.buffer);
        if (locked_buffer) {
            rdp_transmit_surface(locked_buffer);
            wlr_buffer_unlock(locked_buffer);
        }
        pthread_mutex_unlock(&rdp_transmit_mutex);
    }

    pixman_region32_fini(&damage);
    wlr_output_state_finish(&custom_state);
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

static void xdg_toplevel_unmap(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, unmap);
    wlr_log(WLR_INFO, "Unmapping XDG toplevel: %p", toplevel->xdg_toplevel);

    // Start minimize animation (zoom out to 0)
    toplevel->is_animating = true;
    toplevel->scale = 1.0f; // Start at normal size
    toplevel->target_scale = 0.0f; // End at zero size
    toplevel->animation_duration = 0.3f; // 300ms
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    toplevel->animation_start = now.tv_sec + now.tv_nsec / 1e9;

    if (toplevel == toplevel->server->grabbed_toplevel) {
        reset_cursor_mode(toplevel->server);
    }

    // Keep the toplevel in the list until animation completes
    // Defer wl_list_remove(&toplevel->link) to xdg_toplevel_destroy
}

// In tinywl.c

// Ensure you have these includes at the top:
// #include <wlr/types/wlr_scene.h> // For wlr_scene_get_scene_output
static void xdg_toplevel_commit(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, commit);
    struct wlr_surface *surface = toplevel->xdg_toplevel->base->surface;
    struct tinywl_server *server = toplevel->server; 

    if (surface && wlr_surface_has_buffer(surface)) {
        wlr_log(WLR_INFO, "[COMMIT:%s] Surface %p committed with NEW wlr_buffer %p (seq: %u).",
                toplevel->xdg_toplevel->title ? toplevel->xdg_toplevel->title : "N/A",
                (void*)surface, 
                (void*)surface->current.buffer, // Log the wlr_buffer pointer
                surface->current.seq);
    } else if (surface) {
        wlr_log(WLR_DEBUG, "[COMMIT:%s] Surface %p committed WITHOUT a new buffer (config change or ack?). Seq: %u",
                toplevel->xdg_toplevel->title ? toplevel->xdg_toplevel->title : "N/A",
                (void*)surface, surface->current.seq);
    }

    if (toplevel->xdg_toplevel->base->initial_commit) {
        wlr_xdg_toplevel_set_size(toplevel->xdg_toplevel, 0, 0);
        if (!toplevel->xdg_toplevel->base->configured) {
             wlr_xdg_surface_schedule_configure(toplevel->xdg_toplevel->base);
             wlr_log(WLR_DEBUG, "[COMMIT:%s] Scheduled configure for initial commit.",
                     toplevel->xdg_toplevel->title ? toplevel->xdg_toplevel->title : "N/A");
        }
    }

    if (surface && surface->mapped) { 
        struct tinywl_output *output;
        wl_list_for_each(output, &server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_log(WLR_DEBUG, "[COMMIT:%s] Scheduling frame for output %s",
                    toplevel->xdg_toplevel->title ? toplevel->xdg_toplevel->title : "N/A",
                    output->wlr_output->name);
                wlr_output_schedule_frame(output->wlr_output);
            }
        }
    }
}


static void xdg_toplevel_destroy(struct wl_listener *listener, void *data) {
    struct tinywl_toplevel *toplevel = wl_container_of(listener, toplevel, destroy);
    wlr_log(WLR_DEBUG, "Destroying toplevel %p", toplevel);

    // Remove from server toplevels list
    wl_list_remove(&toplevel->link);

    // Remove listeners
    wl_list_remove(&toplevel->map.link);
    wl_list_remove(&toplevel->unmap.link);
    wl_list_remove(&toplevel->commit.link);
    wl_list_remove(&toplevel->destroy.link);
    wl_list_remove(&toplevel->request_move.link);
    wl_list_remove(&toplevel->request_resize.link);
    wl_list_remove(&toplevel->request_maximize.link);
    wl_list_remove(&toplevel->request_fullscreen.link);

    // Destroy scene tree
    if (toplevel->scene_tree) {
        wlr_scene_node_destroy(&toplevel->scene_tree->node);
    }

    // Free toplevel
    free(toplevel);
    wlr_log(WLR_DEBUG, "Toplevel %p freed", toplevel);
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

 if (!create_shader_program(&server)) {
        wlr_log(WLR_ERROR, "Failed to create shader program");
        server_destroy(&server);
        return 1;
    }


   // In main(), after create_shader_program() and before wlr_allocator_autocreate()

 // Inside main()
    // ... (after wlr_renderer_init_wl_display and successful create_shader_program)

    struct wlr_egl *egl_main = wlr_gles2_renderer_get_egl(server.renderer);
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
    // ... rest of main

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