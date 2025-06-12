/*Copyright (c) 2025 Andrew Pliatsikas

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
     int desktop_index; 
};

// First, modify your server struct to support multiple desktops:
struct desktop_fb {
    GLuint fbo;
    GLuint texture;
    GLuint rbo;  // if you end up using renderbuffers
    int width;
    int height;
};


int DestopGridSize ; // 2x2 grid for 4 desktops
float GLOBAL_vertical_offset;

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
    GLint panel_shader_rect_pixel_dimensions_loc;


// NEW: Shader for SSDs
    GLuint ssd_shader_program;
    GLint ssd_shader_mvp_loc;
    GLint ssd_shader_color_loc;
    GLint ssd_shader_time_loc;                  // NEW for SSD shader
    GLint ssd_shader_rect_pixel_dimensions_loc; // Was already there, now used as 
 GLint ssd_shader_resolution_loc;
    struct wlr_scene_node *main_background_node;

// NEW: 2nd Shader for SSDs    
    GLuint ssd2_shader_program;
    GLint ssd2_shader_mvp_loc;
    GLint ssd2_shader_color_loc;
    GLint ssd2_shader_time_loc;
    GLint ssd2_shader_resolution_loc;

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



    // Fullscreen shader effect (as before)
    GLuint fullscreen_shader_program;
    GLint fullscreen_shader_mvp_loc;
     GLint fullscreen_shader_time_loc;
      // Keep if shader uses it for non-rotation anim
    GLint fullscreen_shader_scene_tex0_loc;
   GLint fullscreen_shader_scene_tex1_loc;
    GLint fullscreen_shader_scene_tex2_loc;
    GLint fullscreen_shader_scene_tex3_loc;

     GLint fullscreen_shader_scene_tex4_loc;
   GLint fullscreen_shader_scene_tex5_loc;
    GLint fullscreen_shader_scene_tex6_loc;
    GLint fullscreen_shader_scene_tex7_loc;

     GLint fullscreen_shader_scene_tex8_loc;
   GLint fullscreen_shader_scene_tex9_loc;
    GLint fullscreen_shader_scene_tex10_loc;
    GLint fullscreen_shader_scene_tex11_loc;

     GLint fullscreen_shader_scene_tex12_loc;
   GLint fullscreen_shader_scene_tex13_loc;
    GLint fullscreen_shader_scene_tex14_loc;
    GLint fullscreen_shader_scene_tex15_loc;

    GLint fullscreen_shader_desktop_grid_loc;
  
 GLint fullscreen_shader_resolution_loc;
    GLint fullscreen_switch_mode;
     GLint fullscreen_switchXY; 

   // GLint fullscreen_shader_current_quad_loc;   


// Add these new zoom uniform locations:
    GLint fullscreen_shader_zoom_loc;
    GLint fullscreen_shader_move_loc;
    GLint fullscreen_shader_zoom_center_loc;
    GLint fullscreen_shader_quadrant_loc;


    // cube shader effect (as before)
    GLuint cube_shader_program;
    GLint cube_shader_mvp_loc;
     GLint cube_shader_time_loc; // Keep if shader uses it for non-rotation anim
    GLint cube_shader_scene_tex0_loc;
   GLint cube_shader_scene_tex1_loc;
    GLint cube_shader_scene_tex2_loc;
    GLint cube_shader_scene_tex3_loc;
    GLint cube_shader_tex_matrix_loc;   // Location for texture matrix uniform
 GLint cube_shader_resolution_loc;


    GLint cube_shader_current_quad_loc;   


// Add these new zoom uniform locations:
    GLint cube_shader_zoom_loc;
    GLint cube_shader_zoom_center_loc;
    GLint cube_shader_quadrant_loc;
     GLint cube_shader_vertical_offset_loc;
     GLint cube_shader_global_vertical_offset_loc;







     struct wlr_buffer *scene_capture_buffer;    // Buffer to capture the scene (NEW/REPURPOSED)
    struct wlr_texture *scene_texture;          // Texture from the captured scene buffer (REPURPOSED)
    // GLuint scene_fbo; // REMOVE THIS


 // Variables to control the zoom effect at runtime
   // --- Animation variables for the fullscreen effect's geometric zoom ---
 bool expo_effect_active;
    float effect_zoom_factor_normal;
    float effect_zoom_factor_zoomed;
    
    bool  effect_is_target_zoomed;      // True if the *target* state is zoomed
    bool  effect_is_animating_zoom;
    float effect_anim_current_factor;   // Calculated in output_frame
    float effect_anim_start_factor;
    float effect_anim_target_factor;
    float effect_anim_start_time_sec;
    float effect_anim_duration_sec;
    
    float effect_zoom_center_x;
    float effect_zoom_center_y;

 bool cube_effect_active;
    float cube_zoom_factor_normal;
    float cube_zoom_factor_zoomed;
    
    bool  cube_is_target_zoomed;      // True if the *target* state is zoomed
    bool  cube_is_animating_zoom;
    float cube_anim_current_factor;   // Calculated in output_frame
    float cube_anim_start_factor;
    float cube_anim_target_factor;
    float cube_anim_start_time_sec;
    float cube_anim_duration_sec;
    
    float cube_zoom_center_x;
    float cube_zoom_center_y;

    // Add these to your server struct:
float start_vertical_offset;     
 float current_interpolated_vertical_offset;
 float target_vertical_offset;
 float vertical_offset_animation_start_time;
 bool vertical_offset_animating;


    struct wlr_xdg_decoration_manager_v1 *xdg_decoration_manager;
struct wl_listener xdg_decoration_new;

 // Instead of single desktop_fbos[0], use arrays or separate fields:
    GLuint desktop_fbos[16];        // Support up to 4 virtual desktops
    GLuint desktop_rbos[16];        // Render buffer objects if needed
int intermediate_width[16];
int intermediate_height[16];
GLuint intermediate_texture[16];
GLuint intermediate_rbo[16];

             
    int current_desktop;
     struct desktop_fb *desktops;
    int num_desktops;

     int pending_desktop_switch;

       GLuint desktop_background_shaders[16];
    GLint desktop_bg_shader_mvp_loc[16];
    GLint desktop_bg_shader_time_loc[16];
    GLint desktop_bg_shader_res_loc[16];
    GLint desktop_bg_shader_color_loc[16]; // In case your shaders use a base color

   // New cube geometry (add these)
    GLuint cube_vao, cube_vbo, cube_ibo;

    GLuint cube_background_shader_program;
    GLint cube_background_shader_time_loc;

     // <<< ADD THESE NEW MEMBERS FOR POST-PROCESSING >>>
    GLuint post_process_fbo;
    GLuint post_process_texture;
    int post_process_width;
    int post_process_height;

    GLuint post_process_rbo; 

    // Shader program for the final effect
    GLuint post_process_shader_program;
    GLint post_process_shader_tex_loc;
    GLint post_process_shader_time_loc; // Optional: for animated effects
    GLint post_process_shader_resolution_loc;

    // <<< ADD THESE MEMBERS >>>
     // --- TV EFFECT MEMBERS ---
    bool tv_effect_animating;
    float tv_effect_start_time;
    float tv_effect_duration; 
    bool tv_is_on; 

     GLuint passthrough_shader_program;
    GLint passthrough_shader_mvp_loc;
    GLint passthrough_shader_tex_loc;

     GLint passthrough_shader_res_loc;
    GLint passthrough_shader_cornerRadius_loc;
    GLint passthrough_shader_bevelColor_loc;
    

};

static float current_rotation = 0.0f;
static float target_rotation = 0.0f;
static float animation_start_time = 0.0f;
static float animation_duration = 0.5f; // 500ms animation
static int last_quadrant = -1; // Initialize to -1 to guarantee first animation
static bool animating = false;
static float last_rendered_rotation = 0.0f;

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

#define TITLE_BAR_HEIGHT 50
#define BORDER_WIDTH 10

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

     int desktop;
    
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

   int switch_mode=0;
    int switchXY=0;


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
 static bool setup_desktop_framebuffer(struct tinywl_server *server, int desktop_idx, int width, int height);
 void update_toplevel_visibility(struct tinywl_server *server) ;
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
/*
// Function to start rotation animation
void start_quadrant_animation(int new_quadrant, float current_time) {
    if (new_quadrant != last_quadrant) {
        // Calculate target rotation (-90 degrees per quadrant)
        float new_target = (float)new_quadrant * -M_PI / 2.0f;
        
        // --- THE FIX ---
        // Capture the current visual rotation (including any wobble)
        // as the true starting point for our new animation.
        current_rotation = last_rendered_rotation;
        
        // Handle wrap-around for shortest path (this logic now uses the captured angle)
        float diff = new_target - current_rotation;
        if (diff > M_PI) {
            current_rotation += 2.0f * M_PI;
        } else if (diff < -M_PI) {
            current_rotation -= 2.0f * M_PI;
        }
        
        target_rotation = new_target;
        animation_start_time = current_time;
        last_quadrant = new_quadrant;
        animating = true;
        wlr_log(WLR_INFO, "Starting anim to quad %d. From %.2frad to %.2frad", new_quadrant, current_rotation, target_rotation);
    }
}*/

// Function to start rotation animation
void start_quadrant_animation(int new_quadrant, float current_time) {
    if (new_quadrant != last_quadrant) {
        // Calculate target rotation (-90 degrees per quadrant)
        float new_target = (float)new_quadrant * -M_PI / 2.0f;
        
        // --- THE FIX ---
        // Capture the current visual rotation (including any wobble)
        // as the true starting point for our new animation.
        current_rotation = last_rendered_rotation;
        
        // Handle wrap-around for shortest path (this logic now uses the captured angle)
        float diff = new_target - current_rotation;
        if (diff > M_PI) {
            current_rotation += 2.0f * M_PI;
        } else if (diff < -M_PI) {
            current_rotation -= 2.0f * M_PI;
        }
        
        target_rotation = new_target;
        animation_start_time = current_time;
        last_quadrant = new_quadrant;
        animating = true;
        wlr_log(WLR_INFO, "Starting anim to quad %d. From %.2frad to %.2frad", new_quadrant, current_rotation, target_rotation);
    }
}
/*
// Function to update rotation animation
float update_rotation_animation(float current_time) {
    if (animating) {
        float elapsed = current_time - animation_start_time;
        float progress = elapsed / animation_duration;
        
        if (progress >= 1.0f) {
            animating = false;
            current_rotation = target_rotation;
            return target_rotation;
        }
        
        // Smooth easing function (ease-in-out)
        float eased_progress = progress < 0.5f 
                             ? 4.0f * progress * progress * progress
                             : 1.0f - pow(-2.0f * progress + 2.0f, 3.0f) / 2.0f;
        
        // Interpolate from the rotation at the start of the animation
        return current_rotation + (target_rotation - current_rotation) * eased_progress;
    }
    
    return current_rotation;
}*/

#include <math.h> // Make sure you have this include for powf

// ... (keep the rest of your code the same) ...
// In update_rotation_animation:

float update_rotation_animation(struct tinywl_server *server, float current_time) {
    float base_rotation_this_frame;

    // --- Part 1: Determine the Base Rotation ---
    // This is either the result of the main animation or the stable target angle.
    if (animating) {
        float elapsed = current_time - animation_start_time;
        float progress = elapsed / animation_duration;

        if (progress >= 1.0f) {
            // The main animation has just finished.
            animating = false;
            current_rotation = target_rotation; // Lock in the new base angle for the *next* animation.
            base_rotation_this_frame = target_rotation;
        } else {
            // The animation is in progress. Calculate the eased angle.
            const float c1 = 1.70158f;
            const float c3 = c1 + 1.0f;
            float eased_progress = 1.0f + c3 * powf(progress - 1.0f, 3.0f) + c1 * powf(progress - 1.0f, 2.0f);
            base_rotation_this_frame = current_rotation + (target_rotation - current_rotation) * eased_progress;
        }
    } else {
        // Not animating, so the base rotation is the stable target angle.
        base_rotation_this_frame = target_rotation;
    }

    // --- Part 2: Calculate the Additive Wobble Offset ---
    // This is calculated regardless of the animation state but is only non-zero if zoomed.
    float wobble_offset = 0.0f;
    float zoom_diff = fabs(server->cube_anim_current_factor - server->cube_zoom_factor_normal);

    if (zoom_diff > 0.1f) {
        float wobble_speed = 4.0f;
        float wobble_amplitude = 0.5f; // A subtle amplitude is usually best
        float fade_factor = fminf(1.0f, zoom_diff / 1.0f);
        fade_factor = fade_factor * fade_factor * (3.0f - 2.0f * fade_factor); // Smoothstep
        
        // The wobble is just a small, sinusoidal offset from the base.
        wobble_offset = sin(current_time * wobble_speed) * wobble_amplitude * fade_factor;
    }

    // --- Part 3: Combine and Return the Final Angle ---
    // The final visual angle is the base rotation plus the wobble offset.
    float final_rotation = base_rotation_this_frame + wobble_offset;

    // Always update the last_rendered_rotation for seamless animation starts.
    last_rendered_rotation = final_rotation;
    
    return final_rotation;
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
    if (server->xdg_decoration_manager) { // Check if it was created
        wl_list_remove(&server->xdg_decoration_new.link);
    }


    /* Clean up toplevels */
    struct tinywl_toplevel *toplevel, *toplevel_tmp;
    wl_list_for_each_safe(toplevel, toplevel_tmp, &server->toplevels, link) {
        // xdg_toplevel_destroy will remove listeners and free toplevel
        // For safety, ensure destroy is called if not already
        if (toplevel->type == TINYWL_TOPLEVEL_XDG && toplevel->xdg_toplevel) {
             // xdg_toplevel_destroy is called via signal, no need to call manually here
        } else {
            wl_list_remove(&toplevel->link);
            // remove other specific listeners if it's a custom toplevel type
            free(toplevel);
        }
    }

    /* Clean up outputs */
    struct tinywl_output *output, *output_tmp;
    wl_list_for_each_safe(output, output_tmp, &server->outputs, link) {
        // output_destroy is called via signal
    }

    /* Clean up keyboards */
    struct tinywl_keyboard *kb, *kb_tmp;
    wl_list_for_each_safe(kb, kb_tmp, &server->keyboards, link) {
        // keyboard_handle_destroy is called via signal
    }
    
    /* Clean up popups */
    struct tinywl_popup *popup, *popup_tmp;
    wl_list_for_each_safe(popup, popup_tmp, &server->popups, link) {
        // popup_destroy is called via signal
    }


    /* Destroy resources */
    if (server->scene) { // scene_layout is part of scene
        wlr_scene_node_destroy(&server->scene->tree.node); // This destroys scene_layout too
        server->scene = NULL;
        server->scene_layout = NULL;
    }
     if (server->output_layout) { // Destroy after scene
        wlr_output_layout_destroy(server->output_layout);
        server->output_layout = NULL;
    }
    if (server->xdg_shell) { // Destroy after toplevels/popups
        // wlr_xdg_shell_destroy(server->xdg_shell); // This is usually handled by wl_display_destroy_clients
        server->xdg_shell = NULL;
    }
    if (server->xdg_decoration_manager) {
        // wlr_xdg_decoration_manager_v1_destroy(server->xdg_decoration_manager); // Also usually handled by wl_display_destroy_clients
        server->xdg_decoration_manager = NULL;
    }


    if (server->cursor_mgr) {
        wlr_xcursor_manager_destroy(server->cursor_mgr);
        server->cursor_mgr = NULL;
    }
    if (server->cursor) { // Destroy after output_layout
        wlr_cursor_destroy(server->cursor);
        server->cursor = NULL;
    }

    struct wlr_egl *egl_destroy = NULL;
    if (server->renderer) { 
        egl_destroy = wlr_gles2_renderer_get_egl(server->renderer);
    }

   
        

    if (egl_destroy && wlr_egl_make_current(egl_destroy, NULL)) {
        if (server->shader_program) glDeleteProgram(server->shader_program);
        if (server->passthrough_shader_program) glDeleteProgram(server->passthrough_shader_program);
        if (server->rect_shader_program) glDeleteProgram(server->rect_shader_program);
        if (server->panel_shader_program) glDeleteProgram(server->panel_shader_program);
        if (server->ssd_shader_program) glDeleteProgram(server->ssd_shader_program);
        if (server->back_shader_program) glDeleteProgram(server->back_shader_program);
        if (server->fullscreen_shader_program) glDeleteProgram(server->fullscreen_shader_program); // NEW
    if (server->cube_shader_program) glDeleteProgram(server->cube_shader_program); 
        if (server->quad_vao) glDeleteVertexArrays(1, &server->quad_vao);
        if (server->quad_vbo) glDeleteBuffers(1, &server->quad_vbo);
        if (server->quad_ibo) glDeleteBuffers(1, &server->quad_ibo);
        
        server->shader_program = 0;
        server->rect_shader_program = 0;
        server->panel_shader_program = 0;
        server->ssd_shader_program = 0;
        server->back_shader_program = 0;
 
        server->fullscreen_shader_program = 0; // NEW
        server->cube_shader_program = 0; // NEW
        server->quad_vao = 0;
        server->quad_vbo = 0;
        server->quad_ibo = 0;

        wlr_egl_unset_current(egl_destroy);
    } else if (server->shader_program || server->rect_shader_program || server->panel_shader_program ||
               server->ssd_shader_program || server->back_shader_program ||
               server->fullscreen_shader_program || // NEW
                server->cube_shader_program || // NEW
               server->quad_vao || server->quad_vbo || server->quad_ibo) {
        wlr_log(WLR_ERROR, "Could not make EGL context current to delete GL resources in server_destroy");
    }
    
    if (server->allocator) { // Destroy before renderer
        wlr_allocator_destroy(server->allocator);
        server->allocator = NULL;
    }
    if (server->renderer) { 
         wlr_renderer_destroy(server->renderer);
         server->renderer = NULL;
    }
    if (server->backend) { // Destroy after renderer
        wlr_backend_destroy(server->backend);
        server->backend = NULL;
    }
    if (server->seat) { // Destroy after backend (input devices)
        wlr_seat_destroy(server->seat);
        server->seat = NULL;
    }
    if (server->wl_display) {
        wl_display_destroy_clients(server->wl_display); // Ensure clients are gone before display
        wl_display_destroy(server->wl_display);
        server->wl_display = NULL;
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

/**
 * Handles all keyboard key events.
 *
 * This function processes key presses in the following order of priority:
 * 1. Alt+Shift+[1-4]: Move the focused window to the specified desktop.
 * 2. P (no modifiers): Toggle the "expo" overview effect.
 * 3. O (no modifiers): Cycle to the next virtual desktop.
 * 4. Alt+[Key] (e.g., F1, Escape): Handle compositor-level shortcuts like focus cycling or exit.
 *
 * If a key combination is not handled by the compositor, it is forwarded to the
 * currently focused client application.
 */
static void keyboard_handle_key(struct wl_listener *listener, void *data) {
    struct tinywl_keyboard *keyboard = wl_container_of(listener, keyboard, key);
    struct tinywl_server *server = keyboard->server;
    struct wlr_keyboard_key_event *event = data;
    struct wlr_seat *seat = server->seat;

    // --- Duplicate event detection ---
    static uint32_t last_keycode_event_kbd = 0;
    static uint32_t last_state_event_kbd = 0;
    static uint32_t last_time_event_kbd = 0;
    if (last_keycode_event_kbd == event->keycode &&
        last_state_event_kbd == event->state &&
        event->time_msec > 0 && last_time_event_kbd > 0 &&
        (event->time_msec - last_time_event_kbd < 5 || last_time_event_kbd - event->time_msec < 5)) {
        wlr_log(WLR_DEBUG, "Ignoring duplicate key event: keycode=%u", event->keycode);
        return;
    }
    last_keycode_event_kbd = event->keycode;
    last_state_event_kbd = event->state;
    last_time_event_kbd = event->time_msec;

    // --- Phantom release key detection ---
    if (event->state == WL_KEYBOARD_KEY_STATE_RELEASED && !seat->keyboard_state.focused_surface) {
        xkb_keycode_t xkb_keycode_check = event->keycode + 8;
        struct xkb_state *state_check = keyboard->wlr_keyboard->xkb_state;
        if (xkb_state_key_get_level(state_check, xkb_keycode_check, 0) == 0) {
            wlr_log(WLR_DEBUG, "Ignoring phantom release for keycode=%u", event->keycode);
            return;
        }
    }

    uint32_t keycode_xkb = event->keycode + 8;
    const xkb_keysym_t *syms;
    int nsyms = xkb_state_key_get_syms(keyboard->wlr_keyboard->xkb_state, keycode_xkb, &syms);

    bool handled_by_compositor = false;
    uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard->wlr_keyboard);
    bool needs_redraw = false;

    // --- BINDING: Move window to a desktop (Alt + Shift + [1-4]) ---
    if ((modifiers & (WLR_MODIFIER_ALT | WLR_MODIFIER_SHIFT)) == (WLR_MODIFIER_ALT | WLR_MODIFIER_SHIFT)) {
        if (event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
            struct wlr_surface *focused_surface = server->seat->keyboard_state.focused_surface;
            if (focused_surface) {
                struct wlr_xdg_toplevel *xdg_toplevel = wlr_xdg_toplevel_try_from_wlr_surface(focused_surface);
                // Check that we have our toplevel wrapper struct
                if (xdg_toplevel && xdg_toplevel->base && xdg_toplevel->base->data) {
                    struct tinywl_toplevel *toplevel = xdg_toplevel->base->data;
                    int target_desktop = -1;

                    for (int i = 0; i < nsyms; i++) {
                        if (syms[i] >= XKB_KEY_1 && syms[i] <= XKB_KEY_4) {
                            target_desktop = syms[i] - XKB_KEY_1;
                            break;
                        }
                    }

                    if (target_desktop != -1 && target_desktop < server->num_desktops) {
                        wlr_log(WLR_INFO, "Moving toplevel '%s' to desktop %d",
                                toplevel->xdg_toplevel->title, target_desktop);
                        toplevel->desktop = target_desktop;
                        update_toplevel_visibility(server); // Hide the window if we are not in expo mode
                        handled_by_compositor = true;
                    }
                }
            }
        }
    }

    

   if (!handled_by_compositor && event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
        if (!(modifiers & (WLR_MODIFIER_CTRL | WLR_MODIFIER_ALT | WLR_MODIFIER_SHIFT | WLR_MODIFIER_LOGO))) {
            for (int i = 0; i < nsyms; i++) {

                 // <<< START OF NEW CODE >>>
            // BINDING: Cycle Expo Effect Mode ('A' key)
            if (syms[i] == XKB_KEY_a || syms[i] == XKB_KEY_A) {
                // Cycle between mode 0 (Zoom) and 1 (Slide)
                switch_mode= (switch_mode + 1) % 2;
                wlr_log(WLR_INFO, "Expo effect mode switched to: %s",
                        switch_mode == 0 ? "Zoom" : "Slide");
                handled_by_compositor = true;
                break; // Key handled, exit loop
            }

            // BINDING: Toggle Expo Slide Direction ('Q' key)
            if (syms[i] == XKB_KEY_q || syms[i] == XKB_KEY_Q) {
                // Toggle between 0 (Horizontal) and 1 (Vertical)
                switchXY = 1 - switchXY;
                wlr_log(WLR_INFO, "Expo slide direction switched to: %s",
                        switchXY== 0 ? "Horizontal" : "Vertical");
                handled_by_compositor = true;
                break; // Key handled, exit loop
            }
            // <<< END OF NEW CODE >>>

             if (syms[i] == XKB_KEY_w || syms[i] == XKB_KEY_W) {
                int new_desktop = server->current_desktop - 4;
                
                // Only change if the new desktop would be within range
                if (new_desktop >= 0) {
                    GLOBAL_vertical_offset -= 1.2;
                    server->current_desktop = new_desktop;
                } else {
                    // Clamp to minimum
                    server->current_desktop = 0;
                }
                
                update_toplevel_visibility(server);
                handled_by_compositor = true;
                break;
            }

            // --- BINDING: Toggle Cube View ('S' key) ---
            if (syms[i] == XKB_KEY_S || syms[i] == XKB_KEY_s) {
                int new_desktop = server->current_desktop + 4;
                
                // Only change if the new desktop would be within range
                if (new_desktop <= 15) {
                    GLOBAL_vertical_offset += 1.2;
                    server->current_desktop = new_desktop;
                } else {
                    // Clamp to maximum
                    server->current_desktop = 15;
                }
                
                update_toplevel_visibility(server);
                handled_by_compositor = true;
                break;
            }
            

   

                // --- BINDING: Toggle Expo View ('P' key) ---
                if (syms[i] == XKB_KEY_p || syms[i] == XKB_KEY_P) {
                     if (!server->expo_effect_active && !server->cube_effect_active) {
                        server->expo_effect_active = true;
                        wlr_log(WLR_INFO, "Expo Fullscreen Shader Effect ENABLED via 'P'.");
                        server->effect_is_target_zoomed = true;
                     
                        server->effect_anim_start_factor = server->effect_zoom_factor_normal;
                       
                        server->effect_anim_target_factor = server->effect_zoom_factor_zoomed;
                        if (fabs(server->effect_anim_start_factor - server->effect_anim_target_factor) > 1e-4f) {
                            server->effect_anim_start_time_sec = get_monotonic_time_seconds_as_float();
                            server->effect_is_animating_zoom = true;
                        }
                    } else {
                        
                        server->effect_is_target_zoomed = !server->effect_is_target_zoomed;
                      
                        server->effect_anim_start_factor = server->effect_anim_current_factor;
                       
                        server->effect_anim_target_factor = server->effect_is_target_zoomed ?
                            server->effect_zoom_factor_zoomed : server->effect_zoom_factor_normal;
                        if (fabs(server->effect_anim_start_factor - server->effect_anim_target_factor) > 1e-4f) {
                            server->effect_anim_start_time_sec = get_monotonic_time_seconds_as_float();
                            server->effect_is_animating_zoom = true;
                        }
                    }
                    update_toplevel_visibility(server);
                    handled_by_compositor = true;
                    break;
                }

                // --- BINDING: Toggle Cube View ('L' key) ---
                if (syms[i] == XKB_KEY_l || syms[i] == XKB_KEY_L) {
                     if (!server->cube_effect_active) {
                        server->cube_effect_active = true;
                        wlr_log(WLR_INFO, "Cube Fullscreen Shader Effect ENABLED via 'L'.");
                        // FIX: Ensure we start by animating towards the zoomed-in state
                        server->cube_is_target_zoomed = true; 
                        server->cube_anim_start_factor = server->cube_zoom_factor_normal;
                        server->cube_anim_target_factor = server->cube_zoom_factor_zoomed;
                        if (fabs(server->cube_anim_start_factor - server->cube_anim_target_factor) > 1e-4f) {
                            server->cube_anim_start_time_sec = get_monotonic_time_seconds_as_float();
                            server->cube_is_animating_zoom = true;
                        }
                    } else {
                        server->cube_is_target_zoomed = !server->cube_is_target_zoomed;
                        server->cube_anim_start_factor = server->cube_anim_current_factor;
                        server->cube_anim_target_factor = server->cube_is_target_zoomed ?
                            server->cube_zoom_factor_zoomed : server->cube_zoom_factor_normal;
                        if (fabs(server->cube_anim_start_factor - server->cube_anim_target_factor) > 1e-4f) {
                            server->cube_anim_start_time_sec = get_monotonic_time_seconds_as_float();
                            server->cube_is_animating_zoom = true;
                        }
                    }
                    update_toplevel_visibility(server);
                    handled_by_compositor = true;
                    break;
                }
            }
        }
    

        // --- BINDING: Switch Desktop ('O' key) ---
        if (!handled_by_compositor && !(modifiers & (WLR_MODIFIER_CTRL | WLR_MODIFIER_ALT | WLR_MODIFIER_SHIFT | WLR_MODIFIER_LOGO))) {
    for (int i = 0; i < nsyms; i++) {
        if (syms[i] == XKB_KEY_O || syms[i] == XKB_KEY_o){
            // Calculate which group of 4 the current desktop is in
            int group_start = (server->current_desktop / 4) * 4;
            int group_end = group_start + 3;
            
            // Ensure group_end doesn't exceed available desktops
            if (group_end >= server->num_desktops) {
                group_end = server->num_desktops - 1;
            }
            
            // Calculate next desktop within the group
            int target_desktop;
            if (server->current_desktop >= group_end) {
                target_desktop = group_start; // Wrap to start of group
            } else {
                target_desktop = server->current_desktop + 1; // Next in group
            }
            
            if (server->expo_effect_active) {
               if (switch_mode == 0) {
                server->pending_desktop_switch = target_desktop;
               }
                wlr_log(WLR_INFO, "Desktop switch to %d deferred.", target_desktop);
            } else {
                server->current_desktop = target_desktop;
                server->pending_desktop_switch = -1;
                wlr_log(WLR_INFO, "Switched to desktop %d", server->current_desktop);
                update_toplevel_visibility(server); // Update which windows are visible
            }
            handled_by_compositor = true;
            break;
        }
    }
}
    }

 


    // <<< ADD THIS NEW BINDING FOR 'K' >>>
      if (!handled_by_compositor && !(modifiers & (WLR_MODIFIER_CTRL | WLR_MODIFIER_ALT | WLR_MODIFIER_SHIFT | WLR_MODIFIER_LOGO))) {
        for (int i = 0; i < nsyms; i++) {
            if (syms[i] == XKB_KEY_k || syms[i] == XKB_KEY_K) {
                
                // --- THE FIX ---
                // Only trigger if the key is PRESSED and an animation is NOT already running.
                if (event->state == WL_KEYBOARD_KEY_STATE_PRESSED && !server->tv_effect_animating) {
                    wlr_log(WLR_INFO, "TV Animation Triggered by K press.");
                    
                    // Start the animation
                    server->tv_effect_animating = true;
                    server->tv_effect_start_time = get_monotonic_time_seconds_as_float();
                    
                    // Schedule a frame on all outputs to make the animation start
                    struct tinywl_output *output;
                    wl_list_for_each(output, &server->outputs, link) {
                        if (output->wlr_output && output->wlr_output->enabled) {
                            wlr_output_schedule_frame(output->wlr_output);
                        }
                    }
                }
                // --- END OF FIX ---
                
                // We handle the key regardless of state to prevent it from going to a client.
                handled_by_compositor = true;
                break;
            }
        }
    }
    // --- BINDING: Compositor shortcuts (Alt + [Key]) ---
    // Make sure Shift isn't pressed to avoid conflict with "move window" binding
    if (!handled_by_compositor && (modifiers & WLR_MODIFIER_ALT) && !(modifiers & WLR_MODIFIER_SHIFT)) {
        for (int i = 0; i < nsyms; i++) {
            if (handle_keybinding(server, syms[i])) {
                handled_by_compositor = true;
                needs_redraw = true;
                break;
            }
        }
    }

    if (needs_redraw) {
        struct tinywl_output *output_iter;
        wl_list_for_each(output_iter, &server->outputs, link) {
            if (output_iter->wlr_output && output_iter->wlr_output->enabled) {
                wlr_output_schedule_frame(output_iter->wlr_output);
            }
        }
    }

    // --- Forward key event to the client if not handled by us ---
    if (!handled_by_compositor) {
        static struct wlr_keyboard *last_keyboard_set_on_seat = NULL;
        if (last_keyboard_set_on_seat != keyboard->wlr_keyboard) {
            wlr_seat_set_keyboard(seat, keyboard->wlr_keyboard);
            last_keyboard_set_on_seat = keyboard->wlr_keyboard;
        }
        if (seat->keyboard_state.focused_surface) {
            wlr_seat_keyboard_notify_key(seat, event->time_msec, event->keycode, event->state);
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

 glUseProgram(server->passthrough_shader_program);

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

 // --- SET THE NEW UNIFORMS HERE ---
        // Pass the dimensions of the final rendered box (in pixels) to the shader.
        glUniform2f(server->passthrough_shader_res_loc, (float)main_render_box.width, (float)main_render_box.height);

        // Set the corner radius in pixels. You can make this configurable.
        glUniform1f(server->passthrough_shader_cornerRadius_loc, 12.0f); // e.g., 12 pixel radius

         // ====================================================================
    // --- NEW: LOGIC TO CALCULATE CYCLING BEVEL COLOR ---
    // ====================================================================
    
    // 1. Define our color palette
    static const float color_palette[][4] = {
        {1.0f, 0.2f, 0.2f, 0.7f}, // Red
        {1.0f, 1.0f, 0.2f, 0.7f}, // Yellow
        {0.2f, 1.0f, 0.2f, 0.7f}, // Green
        {0.2f, 1.0f, 1.0f, 0.7f}, // Cyan
        {0.2f, 0.2f, 1.0f, 0.7f}, // Blue
        {1.0f, 0.2f, 1.0f, 0.7f}  // Magenta
    };
    const int num_colors = sizeof(color_palette) / sizeof(color_palette[0]);
    
    // 2. Calculate timing for the cycle
    float time_sec = get_monotonic_time_seconds_as_float();
    float time_per_color = 2.0f; // Each color transition takes 2 seconds
    float total_cycle_duration = (float)num_colors * time_per_color;
    float time_in_cycle = fmod(time_sec, total_cycle_duration);

    // 3. Determine which two colors to blend between
    int current_color_idx = (int)floor(time_in_cycle / time_per_color);
    int next_color_idx = (current_color_idx + 1) % num_colors; // Wrap around to the start

    // 4. Calculate the blend factor for a smooth transition
    float time_in_transition = fmod(time_in_cycle, time_per_color);
    float mix_factor = time_in_transition / time_per_color;
    // Apply a smooth easing function (smoothstep)
    float eased_mix_factor = mix_factor * mix_factor * (3.0f - 2.0f * mix_factor);

    // 5. Interpolate between the two colors
    float final_bevel_color[4];
    for (int i = 0; i < 4; ++i) {
        final_bevel_color[i] = 
            color_palette[current_color_idx][i] * (1.0f - eased_mix_factor) +
            color_palette[next_color_idx][i] * eased_mix_factor;
    }

    // 6. Set the final calculated color as the uniform
    glUniform4fv(server->passthrough_shader_bevelColor_loc, 1, final_bevel_color);
        // --- END OF NEW UNIFORMS ---

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    wlr_log(WLR_DEBUG, "[SCENE_ITERATOR:%s] Drew main window for '%s'", output_name_log, tl_title_for_log);


    // --- Preview Rendering (modified for sequential positioning) ---
    // --- Preview Rendering (Corrected for per-desktop spacing) ---
    if (toplevel && server->top_panel_node && server->top_panel_node->enabled && texture) {
        struct wlr_scene_rect *panel_srect = wlr_scene_rect_from_node(server->top_panel_node);

        if (panel_srect) {
            // This toplevel is being rendered as part of the FBO for 'rdata->desktop_index'.
            // We need to calculate its position in the panel based on its order *among other
            // windows on the same desktop*.
glUseProgram(server->shader_program);
            int per_desktop_index = 0;
            struct tinywl_toplevel *iter_tl;
            wl_list_for_each(iter_tl, &server->toplevels, link) {
                // Only consider windows that belong to the currently rendered desktop.
                if (iter_tl->desktop == rdata->desktop_index) {
                    if (iter_tl == toplevel) {
                        // We've found the current toplevel. The count is its index.
                        break;
                    }
                    // This is a different window on the same desktop that comes earlier.
                    per_desktop_index++;
                }
            }

            wlr_log(WLR_INFO, "[SCENE_ITERATOR_PREVIEW:%s] Rendering preview for '%s' for desktop %d at per-desktop index %d.",
                    output_name_log, tl_title_for_log, rdata->desktop_index, per_desktop_index);

            int panel_screen_x = server->top_panel_node->x;
            int panel_screen_y = server->top_panel_node->y;

            // Position each preview based on its per-desktop index.
            float preview_x_offset_in_panel = 20.0f + (per_desktop_index * 100.0f);
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

                 glUniform1f(server->flame_shader_time_loc, get_monotonic_time_seconds_as_float());

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
        return;
    }

    if (output->width == 0 || output->height == 0) return;

    GLuint program_to_use = 0;
    GLint mvp_loc = -1, color_loc = -1, time_loc = -1, output_res_loc = -1;
    
    const float *color_to_use = scene_rect->color; 

    bool is_main_background = (server->main_background_node == node);

    if (is_main_background) {
        // --- Desktop background logic (unchanged) ---
        int desktop_idx = rdata->desktop_index;
        if (desktop_idx < 0 || desktop_idx >= server->num_desktops) {
            desktop_idx = 0;
        }
        program_to_use = server->desktop_background_shaders[desktop_idx];
        mvp_loc = server->desktop_bg_shader_mvp_loc[desktop_idx];
        color_loc = server->desktop_bg_shader_color_loc[desktop_idx];
        time_loc = server->desktop_bg_shader_time_loc[desktop_idx];
        output_res_loc = server->desktop_bg_shader_res_loc[desktop_idx];
        if (desktop_idx != 0) {
            static const float neutral_color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
            color_to_use = neutral_color;
        }

    } else { 
        // --- SSD (Server-Side Decoration) logic ---
        bool is_alacritty = false;
        struct tinywl_toplevel *toplevel = node->data; // This now works because of Step 1

        if (toplevel && toplevel->xdg_toplevel) {
            // Priority 1: Check app_id (more reliable)
            if (toplevel->xdg_toplevel->app_id && 
                strcasestr(toplevel->xdg_toplevel->app_id, "alacritty") != NULL) {
                is_alacritty = true;
                wlr_log(WLR_DEBUG, "Detected Alacritty via app_id: '%s'", toplevel->xdg_toplevel->app_id);
            } 
            // Priority 2: Fallback to checking the title
            else if (toplevel->xdg_toplevel->title && 
                     strcasestr(toplevel->xdg_toplevel->title, "alacritty") != NULL) {
                is_alacritty = true;
                wlr_log(WLR_DEBUG, "Detected Alacritty via title: '%s'", toplevel->xdg_toplevel->title);
            }
        } else if (node->data) {
             wlr_log(WLR_DEBUG, "SSD rect has node->data, but it's not a valid toplevel or has no xdg_toplevel.");
        } else {
             wlr_log(WLR_DEBUG, "SSD rect has NULL node->data.");
        }


        if (is_alacritty) {
            // Use the special Alacritty shader
            program_to_use = server->ssd2_shader_program;
            mvp_loc = server->ssd2_shader_mvp_loc;
            color_loc = server->ssd2_shader_color_loc;
            time_loc = server->ssd2_shader_time_loc;
            output_res_loc = server->ssd2_shader_resolution_loc;
        } else {
            // Use the default SSD shader
            program_to_use = server->ssd_shader_program;
            mvp_loc = server->ssd_shader_mvp_loc;
            color_loc = server->ssd_shader_color_loc;
            time_loc = server->ssd_shader_time_loc;
            output_res_loc = server->ssd_shader_resolution_loc;
        }
    }

    if (program_to_use == 0) {
        return;
    }
    glUseProgram(program_to_use);

    // --- Uniform setup (unchanged) ---
    float mvp[9];
    float box_scale_x = (float)scene_rect->width * (2.0f / output->width);
    float box_scale_y = (float)scene_rect->height * (-2.0f / output->height);
    float box_translate_x = ((float)render_sx / output->width) * 2.0f - 1.0f;
    float box_translate_y = ((float)render_sy / output->height) * -2.0f + 1.0f;
    float base_mvp[9] = {box_scale_x, 0, 0, 0, box_scale_y, 0, box_translate_x, box_translate_y, 1};
    if (output->transform != WL_OUTPUT_TRANSFORM_NORMAL) {
        float transform_matrix[9];
        wlr_matrix_transform(transform_matrix, output->transform);
        wlr_matrix_multiply(mvp, transform_matrix, base_mvp);
    } else {
        memcpy(mvp, base_mvp, sizeof(base_mvp));
    }

    if (mvp_loc != -1) glUniformMatrix3fv(mvp_loc, 1, GL_FALSE, mvp);
    if (color_loc != -1) glUniform4fv(color_loc, 1, color_to_use);
    if (time_loc != -1) glUniform1f(time_loc, get_monotonic_time_seconds_as_float());
    if (output_res_loc != -1) glUniform2f(output_res_loc, scene_rect->width, scene_rect->height);

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

    if (server->panel_shader_rect_pixel_dimensions_loc != -1) {
        // Get the dimensions directly from the wlr_scene_rect struct.
        float panel_dims[2] = { (float)panel_srect->width, (float)panel_srect->height };
        glUniform2fv(server->panel_shader_rect_pixel_dimensions_loc, 1, panel_dims);
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
/*
static void render_desktop_content(struct tinywl_server *server,
                                   struct wlr_output *wlr_output,
                                   struct render_data *current_rdata,
                                   int desktop_to_render) {
  

    if (server->quad_vao == 0) return;
    glBindVertexArray(server->quad_vao);

    // --- Render Desktop-Specific Chrome (Background, Panel, etc.) ---
    // We'll render these only when drawing desktop 0 to avoid duplication.
    if (desktop_to_render == 0) {
        // Render the main background rectangle
        if (server->main_background_node && server->main_background_node->enabled) {
            render_rect_node(server->main_background_node, current_rdata);
        }
        
    }

if (desktop_to_render == 1) {
        // Render the main background rectangle
        if (server->main_background_node && server->main_background_node->enabled) {
            render_rect_node(server->main_background_node, current_rdata);
        }
        
    }

if (desktop_to_render == 2) {
        // Render the main background rectangle
        if (server->main_background_node && server->main_background_node->enabled) {
            render_rect_node(server->main_background_node, current_rdata);
        }
        
    }
    
if (desktop_to_render == 3) {
        // Render the main background rectangle
        if (server->main_background_node && server->main_background_node->enabled) {
            render_rect_node(server->main_background_node, current_rdata);
        }
        
    }    

    // Render the top panel
        if (server->top_panel_node && server->top_panel_node->enabled) {
            render_panel_node(server->top_panel_node, current_rdata);
        }

    // --- Render Windows (Surface + Decorations) ---
    struct wlr_scene_node *iter_node_lvl1;
    wl_list_for_each(iter_node_lvl1, &server->scene->tree.children, link) {
        // Skip nodes that are disabled or are part of the desktop chrome we handled above
        if (!iter_node_lvl1->enabled || iter_node_lvl1 == server->main_background_node || iter_node_lvl1 == server->top_panel_node) {
            continue;
        }

        if (iter_node_lvl1->type == WLR_SCENE_NODE_TREE) {
            struct tinywl_toplevel *toplevel_ptr = iter_node_lvl1->data;

            // THIS IS THE KEY CHECK: Only render toplevels belonging to the target desktop
            if (toplevel_ptr && toplevel_ptr->type == TINYWL_TOPLEVEL_XDG && toplevel_ptr->desktop == desktop_to_render) {
                struct wlr_scene_tree *toplevel_s_tree = wlr_scene_tree_from_node(iter_node_lvl1);

                // First render SSD decorations if enabled
                if (toplevel_ptr->ssd.enabled) {
                    struct wlr_scene_node *ssd_node_candidate;
                    wl_list_for_each(ssd_node_candidate, &toplevel_s_tree->children, link) {
                        if (ssd_node_candidate->enabled && ssd_node_candidate->type == WLR_SCENE_NODE_RECT) {
                            render_rect_node(ssd_node_candidate, current_rdata);
                        }
                    }
                }

                // Then render the window surface/buffers
                if (server->shader_program != 0) {
                    glUseProgram(server->shader_program);
                    wlr_scene_node_for_each_buffer(&toplevel_s_tree->node, scene_buffer_iterator, current_rdata);
                }
            }
        }
    }

    glBindVertexArray(0);
    glUseProgram(0);
}
*/

static void render_desktop_content(struct tinywl_server *server,
                                   struct wlr_output *wlr_output,
                                   struct render_data *current_rdata,
                                   int desktop_to_render) {
    if (server->quad_vao == 0) return;
    glBindVertexArray(server->quad_vao);

    // --- Render Desktop-Specific Chrome (Background, Panel, etc.) ---

    // *** THE FIX IS HERE: Render the background on EVERY desktop. ***
    // The specific shader for this desktop is chosen inside render_rect_node
    // based on the `desktop_to_render` index passed in `current_rdata`.
    if (server->main_background_node && server->main_background_node->enabled) {
        render_rect_node(server->main_background_node, current_rdata);
    }

    // Render the top panel on every desktop. This part was already correct.
    if (server->top_panel_node && server->top_panel_node->enabled) {
        render_panel_node(server->top_panel_node, current_rdata);
    }

    // --- Render Windows (Surface + Decorations) ---
    struct wlr_scene_node *iter_node_lvl1;
    wl_list_for_each(iter_node_lvl1, &server->scene->tree.children, link) {
        // Skip nodes that are disabled or are part of the desktop chrome we handled above
        if (!iter_node_lvl1->enabled || iter_node_lvl1 == server->main_background_node || iter_node_lvl1 == server->top_panel_node) {
            continue;
        }

        if (iter_node_lvl1->type == WLR_SCENE_NODE_TREE) {
            struct tinywl_toplevel *toplevel_ptr = iter_node_lvl1->data;

            // THIS IS THE KEY CHECK: Only render toplevels belonging to the target desktop
            if (toplevel_ptr && toplevel_ptr->type == TINYWL_TOPLEVEL_XDG && toplevel_ptr->desktop == desktop_to_render) {
                struct wlr_scene_tree *toplevel_s_tree = wlr_scene_tree_from_node(iter_node_lvl1);

                // First render SSD decorations if enabled
                if (toplevel_ptr->ssd.enabled) {
                    struct wlr_scene_node *ssd_node_candidate;
                    wl_list_for_each(ssd_node_candidate, &toplevel_s_tree->children, link) {
                        if (ssd_node_candidate->enabled && ssd_node_candidate->type == WLR_SCENE_NODE_RECT) {
                            render_rect_node(ssd_node_candidate, current_rdata);
                        }
                    }
                }

                // Then render the window surface/buffers
                if (server->shader_program != 0) {
                    glUseProgram(server->shader_program);
                    wlr_scene_node_for_each_buffer(&toplevel_s_tree->node, scene_buffer_iterator, current_rdata);
                }
            }
        }
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

// Helper function to manually create a texture transform matrix (if wlr_matrix_texture_transform is missing)
// This is a simplified version. A full one would handle all 8 transforms.
// tex_coords are typically (0,0) top-left, (1,1) bottom-right.
// Output geometry is (0,0) top-left for NORMAL.
static void manual_texture_transform(float mat[static 9], enum wl_output_transform transform) {
    wlr_matrix_identity(mat);
    switch (transform) {
        case WL_OUTPUT_TRANSFORM_NORMAL:
            // mat is already identity
            break;
        case WL_OUTPUT_TRANSFORM_90:
            // Rotate 90 deg clockwise around (0.5, 0.5)
            // Tex (u,v) -> (v, 1-u)
            // x' = 0*u + 1*v + 0
            // y' = -1*u + 0*v + 1
            // z' = 0*u + 0*v + 1
            mat[0] = 0; mat[1] = -1; mat[2] = 0;
            mat[3] = 1; mat[4] = 0;  mat[5] = 0;
            mat[6] = 0; mat[7] = 1;  mat[8] = 1;
            break;
        case WL_OUTPUT_TRANSFORM_180:
            // Tex (u,v) -> (1-u, 1-v)
            mat[0] = -1; mat[1] = 0; mat[2] = 0;
            mat[3] = 0;  mat[4] = -1;mat[5] = 0;
            mat[6] = 1;  mat[7] = 1; mat[8] = 1;
            break;
        case WL_OUTPUT_TRANSFORM_270:
            // Tex (u,v) -> (1-v, u)
            mat[0] = 0; mat[1] = 1; mat[2] = 0;
            mat[3] = -1;mat[4] = 0; mat[5] = 0;
            mat[6] = 1; mat[7] = 0; mat[8] = 1;
            break;
        case WL_OUTPUT_TRANSFORM_FLIPPED:
            // Tex (u,v) -> (1-u, v)
            mat[0] = -1; mat[1] = 0; mat[2] = 0;
            mat[3] = 0;  mat[4] = 1; mat[5] = 0;
            mat[6] = 1;  mat[7] = 0; mat[8] = 1;
            break;
        case WL_OUTPUT_TRANSFORM_FLIPPED_90:
            // Tex (u,v) -> (v, u) -- Check this one, flipped then 90
            // Flipped: (1-u, v)
            // Rot 90 on that: (v, 1-(1-u)) = (v, u)
            mat[0] = 0; mat[1] = 1; mat[2] = 0;
            mat[3] = 1; mat[4] = 0; mat[5] = 0;
            mat[6] = 0; mat[7] = 0; mat[8] = 1;
            break;
        case WL_OUTPUT_TRANSFORM_FLIPPED_180:
            // Tex (u,v) -> (u, 1-v)
            mat[0] = 1; mat[1] = 0; mat[2] = 0;
            mat[3] = 0; mat[4] = -1;mat[5] = 0;
            mat[6] = 0; mat[7] = 1; mat[8] = 1;
            break;
        case WL_OUTPUT_TRANSFORM_FLIPPED_270:
            // Tex (u,v) -> (1-v, 1-u) -- Check this one
            // Flipped: (1-u, v)
            // Rot 270 on that: (1-v, 1-(1-u)) = (1-v, u) -> NO, (1-v, 1-(1-u)) = (1-v, u)
            // Rot 270 is (y, 1-x). So (v, 1-(1-u)) = (v, u) ... no this is wrong
            // Flipped: (1-u, v)
            // 270: (1-v', u') -> (1-v, 1-u)
            mat[0] = 0; mat[1] = -1; mat[2] = 0;
            mat[3] = -1;mat[4] = 0;  mat[5] = 0;
            mat[6] = 1; mat[7] = 1;  mat[8] = 1;
            break;
    }
}


static bool setup_intermediate_framebuffer(struct tinywl_server *server, int width, int height, int desktop){
    GLenum err;

    struct wlr_egl *egl = wlr_gles2_renderer_get_egl(server->renderer);
    if (!egl) {
        wlr_log(WLR_ERROR, "setup_intermediate_framebuffer%d: Failed to get EGL from renderer.",desktop);
        return false;
    }
    struct wlr_egl_context saved_ctx;
    if (!wlr_egl_make_current(egl, &saved_ctx)) {
        wlr_log(WLR_ERROR, "setup_intermediate_framebuffer%d: Failed to make EGL context current.",desktop);
        return false;
    }

    while ((err = glGetError()) != GL_NO_ERROR) {
        wlr_log(WLR_DEBUG, "setup_intermediate_framebuffer%d: Clearing pre-existing GL error: 0x%x",desktop, err);
    }    


   

    if (server->desktop_fbos[desktop] != 0) {
        if (server->intermediate_width[desktop] != width || server->intermediate_height[desktop] != height) {
            wlr_log(WLR_INFO, "setup_intermediate_framebuffer: Resizing FBO from %dx%d to %dx%d",
                    server->intermediate_width[desktop], server->intermediate_height[desktop], width, height);
            glDeleteFramebuffers(1, &server->desktop_fbos[desktop]); server->desktop_fbos[desktop] = 0;
            glDeleteTextures(1, &server->intermediate_texture[desktop]); server->intermediate_texture[desktop] = 0;
            if (server->intermediate_rbo[desktop] != 0) {
                glDeleteRenderbuffers(1, &server->intermediate_rbo[desktop]); server->intermediate_rbo[desktop] = 0;
            }
        } else {
            // Dimensions are the same, FBO can be reused.
            // Context is already current from above.
            return true; // No need to restore yet if FBO is fine
        }
    }
    
    glGenTextures(1, &server->intermediate_texture[desktop]);
    if ((err = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL error after glGenTextures: 0x%x", err); goto error_cleanup; }
    
    glBindTexture(GL_TEXTURE_2D, server->intermediate_texture[desktop]);
    if ((err = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL error after glBindTexture: 0x%x", err); goto error_cleanup; }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    if ((err = glGetError()) != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "GL error after glTexImage2D(GL_RGBA8, %dx%d): 0x%x. Trying GL_RGBA.", width, height, err);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        if ((err = glGetError()) != GL_NO_ERROR) {
             wlr_log(WLR_ERROR, "GL error after fallback glTexImage2D(GL_RGBA, %dx%d): 0x%x", width, height, err);
             goto error_cleanup;
        } else {
            wlr_log(WLR_INFO, "Used fallback GL_RGBA for intermediate texture.");
        }
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if ((err = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL error after glTexParameteri: 0x%x", err); goto error_cleanup; }

    glGenFramebuffers(1, &server->desktop_fbos[desktop]);
    if ((err = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL error after glGenFramebuffers: 0x%x", err); goto error_cleanup; }
    
    glBindFramebuffer(GL_FRAMEBUFFER, server->desktop_fbos[desktop]);
    if ((err = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL error after glBindFramebuffer: 0x%x", err); goto error_cleanup; }
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, server->intermediate_texture[desktop], 0);
    if ((err = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL error after glFramebufferTexture2D: 0x%x", err); goto error_cleanup; }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ((err = glGetError()) != GL_NO_ERROR) {
        wlr_log(WLR_ERROR, "GL error *during* glCheckFramebufferStatus call: 0x%x. FBO Status was reported as: 0x%x", err, status);
    }

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        wlr_log(WLR_ERROR, "Intermediate framebuffer not complete. Status: 0x%x (width: %d, height: %d)", status, width, height);
        switch (status) {
            // GLES 2.0/3.0 Common Codes (OES suffixes might be needed depending on headers)
            // If your headers define them without _OES, that's fine too.
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: 
                wlr_log(WLR_ERROR, " Reason: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: 
                wlr_log(WLR_ERROR, " Reason: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: // Common in GLES
                wlr_log(WLR_ERROR, " Reason: GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS"); break;
            case GL_FRAMEBUFFER_UNSUPPORTED: 
                wlr_log(WLR_ERROR, " Reason: GL_FRAMEBUFFER_UNSUPPORTED (format combination not supported)"); break;
            
            // GLES 3.0 specific (these might still cause issues if true GLES 3.0 context isn't fully active/supported by Zink here)
            // For wider compatibility, you might comment these out if they still cause compilation errors.
            // For GL_FRAMEBUFFER_UNDEFINED, it means the default framebuffer was queried, which shouldn't happen here.
            // If `status` is 0, then the `default` case will catch it.
#ifdef GL_FRAMEBUFFER_UNDEFINED 
            case GL_FRAMEBUFFER_UNDEFINED: 
                wlr_log(WLR_ERROR, " Reason: GL_FRAMEBUFFER_UNDEFINED (should not happen for non-default FBO)"); break;
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: 
                wlr_log(WLR_ERROR, " Reason: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE (GLES 3.0+)"); break;
#endif
            default: 
                wlr_log(WLR_ERROR, " Reason: Unknown or less common FBO status code (0x%x). Could be 0 if glCheckFramebufferStatus itself failed.", status); break;
        }
        goto error_cleanup;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);    
    
    server->intermediate_width[desktop] = width;
    server->intermediate_height[desktop] = height;
    
    wlr_log(WLR_INFO, "Intermediate framebuffer created/verified: %dx%d, FBO ID: %u, Texture ID: %u",
           width, height, server->desktop_fbos[desktop], server->intermediate_texture[desktop]);
    wlr_egl_restore_context(&saved_ctx);
    return true;


error_cleanup:



    if (server->desktop_fbos[desktop] != 0) {
        glDeleteFramebuffers(1, &server->desktop_fbos[desktop]);
        server->desktop_fbos[desktop] = 0;
    }
    if (server->intermediate_texture[desktop] != 0) {
        glDeleteTextures(1, &server->intermediate_texture[desktop]);
        server->intermediate_texture[desktop] = 0;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    wlr_egl_restore_context(&saved_ctx);
    return false;



}



// Get a specific desktop's framebuffer
GLuint get_desktop_fbo(struct tinywl_server *server, int desktop_idx) {
    if (desktop_idx < 0 || desktop_idx >= server->num_desktops) {
        return 0;
    }
    return server->desktops[desktop_idx].fbo;
}

// =============================================================================
// <<< ADD THIS ENTIRE FUNCTION TO YOUR .c FILE >>>
// =============================================================================
static bool setup_post_process_framebuffer(struct tinywl_server *server, int width, int height) {
    // This function is almost identical to setup_intermediate_framebuffer, but for the
    // single final FBO used for post-processing.
    GLenum err;

    struct wlr_egl *egl = wlr_gles2_renderer_get_egl(server->renderer);
    if (!egl) {
        wlr_log(WLR_ERROR, "setup_post_process_framebuffer: Failed to get EGL from renderer.");
        return false;
    }
    struct wlr_egl_context saved_ctx;
    if (!wlr_egl_make_current(egl, &saved_ctx)) {
        wlr_log(WLR_ERROR, "setup_post_process_framebuffer: Failed to make EGL context current.");
        return false;
    }

    // Check if the FBO already exists and if its size has changed.
    if (server->post_process_fbo != 0) {
        if (server->post_process_width != width || server->post_process_height != height) {
            wlr_log(WLR_INFO, "Post-process FBO: Resizing from %dx%d to %dx%d",
                    server->post_process_width, server->post_process_height, width, height);
            // Delete old resources if size is different
            glDeleteFramebuffers(1, &server->post_process_fbo);
            glDeleteTextures(1, &server->post_process_texture);
            glDeleteRenderbuffers(1, &server->post_process_rbo); // Use post_process_rbo
            server->post_process_fbo = 0;
            server->post_process_texture = 0;
            server->post_process_rbo = 0;
        } else {
            // FBO exists and is the correct size, no need to re-create.
            wlr_egl_restore_context(&saved_ctx);
            return true;
        }
    }

    // --- Create Texture for Color Attachment ---
    glGenTextures(1, &server->post_process_texture);
    glBindTexture(GL_TEXTURE_2D, server->post_process_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // --- Create Renderbuffer for Depth Attachment ---
    glGenRenderbuffers(1, &server->post_process_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, server->post_process_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // --- Create and Configure Framebuffer ---
    glGenFramebuffers(1, &server->post_process_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, server->post_process_fbo);

    // Attach the texture and renderbuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, server->post_process_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, server->post_process_rbo);

    // --- CRITICAL: Check if the framebuffer is complete ---
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        wlr_log(WLR_ERROR, "Post-process framebuffer is not complete! Status: 0x%x", status);
        // Clean up partially created resources
        glDeleteFramebuffers(1, &server->post_process_fbo); server->post_process_fbo = 0;
        glDeleteTextures(1, &server->post_process_texture); server->post_process_texture = 0;
        glDeleteRenderbuffers(1, &server->post_process_rbo); server->post_process_rbo = 0;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        wlr_egl_restore_context(&saved_ctx);
        return false;
    }

    wlr_log(WLR_INFO, "Post-process framebuffer created/verified: %dx%d, FBO ID: %u",
            width, height, server->post_process_fbo);
            
    // Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Store dimensions for next frame check
    server->post_process_width = width;
    server->post_process_height = height;

    wlr_egl_restore_context(&saved_ctx);
    return true;
}

static void output_frame(struct wl_listener *listener, void *data) {
    struct tinywl_output *output_wrapper = wl_container_of(listener, output_wrapper, frame);
    struct wlr_output *wlr_output = output_wrapper->wlr_output;
    struct tinywl_server *server = output_wrapper->server;
    struct wlr_scene *scene = server->scene;
    struct wlr_renderer *renderer = server->renderer;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    struct wlr_output_state output_state_direct;
    struct wlr_output_state output_state_effect;
    struct wlr_render_pass *current_screen_pass = NULL;

     float time_for_shader;

    if (!wlr_output || !wlr_output->enabled || !renderer || !server->allocator) {
        wlr_log(WLR_ERROR, "[OUTPUT_FRAME:%s] Output not ready (output_ptr=%p, enabled=%d, renderer_ptr=%p, allocator=%p).",
            wlr_output ? wlr_output->name : "none",
            wlr_output, wlr_output ? wlr_output->enabled : 0, renderer, server->allocator);
        if (scene && wlr_output) {
            struct wlr_scene_output *output_early_exit = wlr_scene_get_scene_output(scene, wlr_output);
            if (output_early_exit) wlr_scene_output_send_frame_done(output_early_exit, &now);
        }
        return;
    }
    
    if (!scene) {
        wlr_log(WLR_ERROR, "[%s] Output_frame: no scene", wlr_output->name);
        return;
    }
    
    struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(scene, wlr_output);
    if (!scene_output) {
        wlr_log(WLR_ERROR, "[%s] Output_frame: no scene_output", wlr_output->name);
        return;
    }

    // --- Damage Detection and Early Exit (only for non-effect path) ---
    wlr_output_state_init(&output_state_direct);
    struct wlr_scene_output_state_options opts = {0};
    bool has_damage = wlr_scene_output_build_state(scene_output, &output_state_direct, &opts);
    
    // Check if we need effects or if we can use direct path
    bool effects_frame_fbo_path = server->expo_effect_active || server->cube_effect_active;
    
    // Only do early exit if no effects are active
   // if (!effects_frame_fbo_path && !has_damage && !(output_state_direct.committed & WLR_OUTPUT_STATE_BUFFER)) {
   //     wlr_log(WLR_DEBUG, "[%s] Direct: No damage or buffer, skipping.", wlr_output->name);
   //     wlr_output_state_finish(&output_state_direct);
   //     wlr_scene_output_send_frame_done(scene_output, &now);
   //     return;
   // }

if (!server->cube_effect_active) {
  if (last_quadrant != server->current_desktop) {
        // This is the first frame the cube is active after a potential desktop change.
        // Force the animation system to accept the new reality.
        wlr_log(WLR_INFO, "CUBE SYNC: Current desktop is %d, but last cube quadrant was %d. Synchronizing.",
                server->current_desktop, last_quadrant);
        last_quadrant = server->current_desktop;
        
        // Set the cube's rotation directly to the correct angle without animation.
        // This prevents the jarring rotation from the old desktop to the new one.
        target_rotation = (float)server->current_desktop * -M_PI / 2.0f;
        current_rotation = target_rotation;
        last_rendered_rotation = target_rotation;
        animating = false; // Ensure no old rotation animation is running.
    }
}


      // --- Cube Effect Animation State Calculation (Completely Separate) ---
    if (server->cube_effect_active) {

       
        if (server->cube_is_animating_zoom) {
            float now_sec = get_monotonic_time_seconds_as_float();
            float elapsed_sec = now_sec - server->cube_anim_start_time_sec;
            float t = (server->cube_anim_duration_sec > 1e-5f) ? (elapsed_sec / server->cube_anim_duration_sec) : 1.0f;
server->effect_is_animating_zoom = false;
server->expo_effect_active = false;
server->effect_is_target_zoomed=false;
            if (t >= 1.0f) {
                server->cube_anim_current_factor = server->cube_anim_target_factor;
                server->cube_is_animating_zoom = false;
                if (!server->cube_is_target_zoomed) {
                    server->cube_effect_active = false;
                    wlr_log(WLR_INFO, "[%s] Cube Effect OFF.", wlr_output->name);
                    
                    // 1. Apply the deferred desktop switch if there is one.
                    if (server->pending_desktop_switch != -1) {
                        server->current_desktop = server->pending_desktop_switch;
                        server->pending_desktop_switch = -1;
                    }

                    // 2. Update visibility now that the effect is off.
                    update_toplevel_visibility(server);

                    // 3. Find the topmost window on the new desktop to make it active.
                    //    This fixes the "many clicks" problem.
                    struct tinywl_toplevel *toplevel_to_focus = NULL;
                    struct tinywl_toplevel *toplevel_iter;
                    wl_list_for_each(toplevel_iter, &server->toplevels, link) {
                        if (toplevel_iter->desktop == server->current_desktop && toplevel_iter->mapped) {
                            toplevel_to_focus = toplevel_iter;
                            break;
                        }
                    }

                    if (toplevel_to_focus) {
                        focus_toplevel(toplevel_to_focus);
                    } else {
                        if (server->seat->keyboard_state.focused_surface) {
                            wlr_seat_keyboard_clear_focus(server->seat);
                        }
                    }

                    // 4. Update the mouse pointer's focus to restore hover effects.
                    uint32_t time_msec = now.tv_sec * 1000 + now.tv_nsec / 1000000;
                    process_cursor_motion(server, time_msec);
                }
            } else {
                server->cube_anim_current_factor = server->cube_anim_start_factor + (server->cube_anim_target_factor - server->cube_anim_start_factor) * t;
            }
        } else {
            server->cube_anim_current_factor = server->cube_is_target_zoomed ? server->cube_zoom_factor_zoomed : server->cube_zoom_factor_normal;
        }
    } else {
        server->cube_is_animating_zoom = false;
        server->cube_anim_current_factor = server->cube_zoom_factor_normal;
    }
    // --- Expo Effect Animation State Calculation ---
    if (server->expo_effect_active) {
        if (server->effect_is_animating_zoom) {
            float now_sec = get_monotonic_time_seconds_as_float();
            float elapsed_sec = now_sec - server->effect_anim_start_time_sec;
            float t = (server->effect_anim_duration_sec > 1e-5f) ? (elapsed_sec / server->effect_anim_duration_sec) : 1.0f;

              if (t >= 1.0f) {
                 
                        server->effect_anim_start_factor = server->effect_zoom_factor_normal;
                       
                server->effect_is_animating_zoom = false;

                // This code block runs ONLY when the zoom-out animation finishes.
                if (!server->effect_is_target_zoomed) {
                    server->expo_effect_active = false;
                    wlr_log(WLR_INFO, "[%s] Expo Effect OFF.", wlr_output->name);

                    // --- THIS IS THE "REFRESH" LOGIC ---
                    // It does exactly what a direct 'o' press would do.

                    // 1. Apply the deferred desktop switch if there is one.
                    if (server->pending_desktop_switch != -1) {
                        server->current_desktop = server->pending_desktop_switch;
                        server->pending_desktop_switch = -1;
                    }

                    // 2. Update visibility now that the effect is off.
                    update_toplevel_visibility(server);

                    // 3. Find the topmost window on the new desktop to make it active.
                    //    This fixes the "many clicks" problem.
                    struct tinywl_toplevel *toplevel_to_focus = NULL;
                    struct tinywl_toplevel *toplevel_iter;
                    wl_list_for_each(toplevel_iter, &server->toplevels, link) {
                        if (toplevel_iter->desktop == server->current_desktop && toplevel_iter->mapped) {
                            toplevel_to_focus = toplevel_iter;
                            break;
                        }
                    }

                    if (toplevel_to_focus) {
                        focus_toplevel(toplevel_to_focus);
                    } else {
                        if (server->seat->keyboard_state.focused_surface) {
                            wlr_seat_keyboard_clear_focus(server->seat);
                        }
                    }

                    // 4. Update the mouse pointer's focus to restore hover effects.
                    uint32_t time_msec = now.tv_sec * 1000 + now.tv_nsec / 1000000;
                    process_cursor_motion(server, time_msec);
                    
                }
            } else {
                 
                server->effect_anim_current_factor = server->effect_anim_start_factor + (server->effect_anim_target_factor - server->effect_anim_start_factor) * t;
                     
            }
        } else {
            
            server->effect_anim_current_factor = server->effect_is_target_zoomed ? server->effect_zoom_factor_zoomed : server->effect_zoom_factor_normal;
             
        }
    } else {
        server->effect_is_animating_zoom = false;
       
        server->effect_anim_current_factor = server->effect_zoom_factor_normal;
    }

  
if (server->tv_effect_animating) {
    float elapsed = get_monotonic_time_seconds_as_float() - server->tv_effect_start_time;
    float animation_duration = server->tv_effect_duration / 2.0f;
    
    if (elapsed >= animation_duration) {
        // Animation is over, stop animating and toggle state
        server->tv_effect_animating = false;
        
        if (server->tv_effect_duration / 2.0f == 2.5f) {
            // Was at 2.5, now store 5.0
            server->tv_effect_duration = 10.0f; // So duration/2 = 5.0
            time_for_shader = 5.0f;
        } else {
            // Was at 5.0, now store 2.5
            server->tv_effect_duration = 5.0f; // So duration/2 = 2.5
            time_for_shader = 2.5f;
        }
    } else {
        // Animation is in progress - determine start and end values
        float start_value = server->tv_effect_duration / 2.0f;
        float end_value = (start_value == 2.5f) ? 5.0f : 2.5f;
        
        // Interpolate between start and end
        float progress = elapsed / animation_duration;
        time_for_shader = start_value + (end_value - start_value) * progress;
    }
} else {
    // Not animating, so send the stored stable state
    time_for_shader = server->tv_effect_duration / 2.0f;
}

     if (!server->expo_effect_active & !server->cube_effect_active) {
   
            float now_sec = get_monotonic_time_seconds_as_float();
            float elapsed_sec = now_sec - server->effect_anim_start_time_sec;
            float t = (server->effect_anim_duration_sec > 1e-5f) ? (elapsed_sec / server->effect_anim_duration_sec) : 1.0f;

          //    if (t >= 1.0f) {
              //  server->effect_anim_current_factor = server->effect_anim_target_factor;
            //    server->effect_is_animating_zoom = false;

                // This code block runs ONLY when the zoom-out animation finishes.
              //  if (server->effect_is_target_zoomed) {
                   if (!server->cube_effect_active)
                   {
                   server->expo_effect_active = true;
                   }else {
                   server->expo_effect_active = false;
                   }
                   wlr_log(WLR_INFO, "[%s] Expo Effect OFF.", wlr_output->name);

                    // --- THIS IS THE "REFRESH" LOGIC ---
                    // It does exactly what a direct 'o' press would do.

                    

                    // 2. Update visibility now that the effect is off.
                    update_toplevel_visibility(server);

                    // 3. Find the topmost window on the new desktop to make it active.
                    //    This fixes the "many clicks" problem.
                    struct tinywl_toplevel *toplevel_to_focus = NULL;
                    struct tinywl_toplevel *toplevel_iter;
                    wl_list_for_each(toplevel_iter, &server->toplevels, link) {
                        if (toplevel_iter->desktop == server->current_desktop && toplevel_iter->mapped) {
                            toplevel_to_focus = toplevel_iter;
                            break;
                        }
                    }

                    if (toplevel_to_focus) {
                        focus_toplevel(toplevel_to_focus);
                    } else {
                        if (server->seat->keyboard_state.focused_surface) {
                            wlr_seat_keyboard_clear_focus(server->seat);
                        }
                    }

                    // 4. Update the mouse pointer's focus to restore hover effects.
                    uint32_t time_msec = now.tv_sec * 1000 + now.tv_nsec / 1000000;
                    process_cursor_motion(server, time_msec);
                    
             //   }
            } 



    // =========================================================================================
    // --- STAGE 1 (Conditional): Prepare Textures for Multi-Desktop Effects ---
    // =========================================================================================
    if (effects_frame_fbo_path) {
        // When an effect is active, enable all toplevels in the scene so they can be
        // rendered into their respective desktop FBOs.
        struct tinywl_toplevel *toplevel;
        wl_list_for_each(toplevel, &server->toplevels, link) {
            if (toplevel->scene_tree) {
                wlr_scene_node_set_enabled(&toplevel->scene_tree->node, toplevel->scale > 0.01);
            }
        }
      if (server->effect_anim_current_factor<2.0 ||switch_mode==1){
        // Setup and render each desktop into its own intermediate FBO.
        for (int i = 0; i < server->num_desktops; ++i) {
    if (!setup_intermediate_framebuffer(server, wlr_output->width, wlr_output->height, i)) {
         wlr_log(WLR_ERROR, "Failed to setup desktop FBO %d. Aborting frame.", i);
         goto cleanup_frame;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, server->desktop_fbos[i]);
    glViewport(0, 0, server->intermediate_width[i], server->intermediate_height[i]);
    
    // FIX 8: Clear with transparent black for individual desktop FBOs
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Transparent, not opaque
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // FIX 9: Ensure blending is enabled for desktop content rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    struct render_data rdata = { 
        .renderer = renderer, 
        .server = server, 
        .output = wlr_output, 
        .pass = NULL, 
        .desktop_index = i 
    };
    render_desktop_content(server, wlr_output, &rdata, i);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}}else if (server->effect_anim_current_factor==2.0 && switch_mode==0) {
        // Setup and render each desktop into its own intermediate FBO.
     int target_quadrant = server->pending_desktop_switch != -1 ? server->pending_desktop_switch : server->current_desktop;
         
    if (!setup_intermediate_framebuffer(server, wlr_output->width, wlr_output->height, target_quadrant)) {
         wlr_log(WLR_ERROR, "Failed to setup desktop FBO %d. Aborting frame.", target_quadrant);
         goto cleanup_frame;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, server->desktop_fbos[target_quadrant]);
    glViewport(0, 0, server->intermediate_width[target_quadrant], server->intermediate_height[target_quadrant]);
    
    // FIX 8: Clear with transparent black for individual desktop FBOs
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Transparent, not opaque
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // FIX 9: Ensure blending is enabled for desktop content rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    struct render_data rdata = { 
        .renderer = renderer, 
        .server = server, 
        .output = wlr_output, 
        .pass = NULL, 
        .desktop_index = target_quadrant
    };
    render_desktop_content(server, wlr_output, &rdata, target_quadrant);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


}else {
        // If no effects are active, ensure standard visibility rules are applied.
        update_toplevel_visibility(server);
    }

  


// Key fixes for alpha issues in expo and cube effects:

// =========================================================================================
// --- STAGE 2: Render The Composite Scene Into The Single Post-Processing FBO ---
// =========================================================================================

if (!setup_post_process_framebuffer(server, wlr_output->width, wlr_output->height)) {
    wlr_log(WLR_ERROR, "Failed to setup post-processing FBO. Aborting frame.");
    goto cleanup_frame;
}

// Bind our new single FBO as the render target for the entire scene.
glBindFramebuffer(GL_FRAMEBUFFER, server->post_process_fbo);
glViewport(0, 0, wlr_output->width, wlr_output->height);

// FIX 1: Ensure proper alpha blending setup
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glBlendEquation(GL_FUNC_ADD);  // Add this line

// FIX 2: Clear with transparent black instead of opaque black
glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Changed alpha from 1.0f to 0.0f
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// Now, draw either the effect view or the normal desktop view INTO this FBO.
if (effects_frame_fbo_path) {
    // --- Effect Path: Render Expo or Cube effect using textures from Stage 1 ---
    if (server->expo_effect_active) {
        if (server->effect_anim_current_factor<2.0 ||switch_mode==1)
        {
        // FIX 3: Ensure consistent GL state for expo
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);  // Ensure blending is enabled
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glBindVertexArray(server->quad_vao);
        glUseProgram(server->fullscreen_shader_program);
        
        glUniform1f(server->fullscreen_shader_zoom_loc, server->effect_anim_current_factor);
        glUniform1f(server->fullscreen_shader_move_loc, server->effect_anim_current_factor);

        int target_quadrant = server->pending_desktop_switch != -1 ? server->pending_desktop_switch : server->current_desktop;
        
        glUniform1i(server->fullscreen_shader_quadrant_loc, target_quadrant);
        glUniform1i(server->fullscreen_switch_mode, switch_mode);
        glUniform1i(server->fullscreen_switchXY, switchXY);
        // Bind all 4 textures for the fullscreen shader
        
        for(int i = 0; i < server->num_desktops; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, server->intermediate_texture[i]);
            glUniform1i(server->fullscreen_shader_scene_tex0_loc + i, i);
        } 

        glUniform1i(server->fullscreen_shader_desktop_grid_loc,DestopGridSize);
    }else if (server->effect_anim_current_factor==2.0 & switch_mode==0)
        {
        // FIX 3: Ensure consistent GL state for expo
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);  // Ensure blending is enabled
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glBindVertexArray(server->quad_vao);
        glUseProgram(server->fullscreen_shader_program);
        
        glUniform1f(server->fullscreen_shader_zoom_loc, server->effect_anim_current_factor);
        glUniform1f(server->fullscreen_shader_move_loc, server->effect_anim_current_factor);
        int target_quadrant = server->pending_desktop_switch != -1 ? server->pending_desktop_switch : server->current_desktop;
        glUniform1i(server->fullscreen_shader_quadrant_loc, target_quadrant);
        glUniform1i(server->fullscreen_switch_mode, switch_mode);
        glUniform1i(server->fullscreen_switchXY, switchXY);
       
            glActiveTexture(GL_TEXTURE0 + target_quadrant);
            glBindTexture(GL_TEXTURE_2D, server->intermediate_texture[target_quadrant]);
            glUniform1i(server->fullscreen_shader_scene_tex0_loc + target_quadrant, target_quadrant);
        
         glUniform1i(server->fullscreen_shader_desktop_grid_loc,DestopGridSize);
    }
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    } 
else if (server->cube_effect_active) {
    // FIX 4: Proper alpha handling for cube effect
    float now_float_sec = get_monotonic_time_seconds_as_float();
    start_quadrant_animation(server->current_desktop, now_float_sec);
    float animated_rotation = update_rotation_animation(server, now_float_sec);
    last_rendered_rotation = animated_rotation;

    // Interpolate vertical offset
     if (fabs(GLOBAL_vertical_offset - server->target_vertical_offset) > 0.01f) {
        // A new target has been set. Start a new animation.
        // The animation begins from wherever the cube *currently is visually*.
        server->start_vertical_offset = server->current_interpolated_vertical_offset;
        
        // Lock in the new destination.
        server->target_vertical_offset = GLOBAL_vertical_offset;
        
        // Reset the timer and flag that we are animating.
        server->vertical_offset_animation_start_time = now_float_sec;
        server->vertical_offset_animating = true;
    }

    // 2. If an animation is in progress, calculate its current state.
    if (server->vertical_offset_animating) {
        float animation_duration = 0.4f; // A nice, smooth duration
        float elapsed = now_float_sec - server->vertical_offset_animation_start_time;
        float t = elapsed / animation_duration;

        if (t >= 1.0f) {
            // Animation is finished. Snap to the final position.
            server->current_interpolated_vertical_offset = server->target_vertical_offset;
            server->vertical_offset_animating = false;
        } else {
            // Animation is ongoing. Use a smooth easing curve for a professional feel.
            float eased_t = 1.0f - pow(1.0f - t, 3.0f); // Ease-out cubic

            // Interpolate from the fixed start point to the fixed target.
            server->current_interpolated_vertical_offset =
                server->start_vertical_offset + (server->target_vertical_offset - server->start_vertical_offset) * eased_t;
        }
    }

    // Background rendering with proper alpha
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);  // Ensure blending for background
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(server->cube_background_shader_program);
    glUniform1f(server->cube_background_shader_time_loc, now_float_sec);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Cube rendering with depth and alpha
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    // Keep blending enabled for cube faces
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(server->cube_vao);
    glUseProgram(server->cube_shader_program);

    glUniform1f(server->cube_shader_time_loc, animated_rotation);
    glUniform1f(server->cube_shader_zoom_loc, server->cube_anim_current_factor);
    glUniform1i(server->cube_shader_quadrant_loc, server->current_desktop);

    if (server->cube_shader_resolution_loc != -1) {
        glUniform2f(server->cube_shader_resolution_loc, (float)wlr_output->width, (float)wlr_output->width);
    }

    // Loop 4 times with vertical offset
    for(int loop = 0; loop < 4; ++loop) {
        for(int i = 0; i < server->num_desktops/4; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, server->intermediate_texture[i+(loop*4)]);
            glUniform1i(server->cube_shader_scene_tex0_loc + i, i);
        }

        float vertical_offset = (float)loop * -1.2f; // Local offset for each cube
        glUniform1f(server->cube_shader_vertical_offset_loc, vertical_offset);
        
        // Use interpolated global vertical offset for smooth animation
        glUniform1f(server->cube_shader_global_vertical_offset_loc, server->current_interpolated_vertical_offset);
        
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    
    // Schedule next frame if still animating
    if (server->vertical_offset_animating) {
        struct tinywl_output *output;
        wl_list_for_each(output, &server->outputs, link) {
            if (output->wlr_output && output->wlr_output->enabled) {
                wlr_output_schedule_frame(output->wlr_output);
            }
        }
    }
}
} else {
    // --- Normal Path: Render the current desktop directly into post_process_fbo ---
    // FIX 5: Ensure proper blending for normal desktop rendering
   
}

// Unbind our post-process FBO. Its texture is now filled with the complete scene.
glBindFramebuffer(GL_FRAMEBUFFER, 0);

// =========================================================================================
// --- STAGE 3: Render The Post-Process Texture TO THE SCREEN With The Final Shader ---
// =========================================================================================

struct wlr_output_state final_state;
wlr_output_state_init(&final_state);

pixman_region32_t damage;
pixman_region32_init_rect(&damage, 0, 0, wlr_output->width, wlr_output->height);
wlr_output_state_set_damage(&final_state, &damage);
pixman_region32_fini(&damage);

struct wlr_render_pass *screen_pass = wlr_output_begin_render_pass(wlr_output, &final_state, NULL);
if (!screen_pass) {
    wlr_log(WLR_ERROR, "Final Pass: Failed to begin render pass to screen.");
    wlr_output_state_finish(&final_state);
    goto cleanup_frame;
}

// FIX 6: Final screen rendering with proper alpha handling
glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Opaque black for final output
glClear(GL_COLOR_BUFFER_BIT);

// FIX 7: Disable blending for final composite to avoid double-blending
glDisable(GL_BLEND);
glDisable(GL_DEPTH_TEST);
glDisable(GL_CULL_FACE);

glUseProgram(server->post_process_shader_program);
glBindVertexArray(server->quad_vao);

glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, server->post_process_texture);

glUniform1i(server->post_process_shader_tex_loc, 0);
glUniform1f(server->post_process_shader_time_loc, time_for_shader);

 // <<< ADD THIS BLOCK TO SET THE RESOLUTION >>>
    if (server->post_process_shader_resolution_loc != -1) {
        glUniform2f(server->post_process_shader_resolution_loc, 
                    (float)wlr_output->width, 
                    (float)wlr_output->height);
    }

glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

// Cleanup GL state after drawing.
glUseProgram(0);
glBindVertexArray(0);
glBindTexture(GL_TEXTURE_2D, 0);

    if (!wlr_render_pass_submit(screen_pass)) {
        wlr_log(WLR_ERROR, "Failed to submit final render pass.");
    }

    // --- RDP Transmit Logic (now simplified) ---
    if (final_state.committed & WLR_OUTPUT_STATE_BUFFER) {
        struct wlr_buffer *buffer_to_transmit = final_state.buffer;
        if (buffer_to_transmit && buffer_to_transmit->width > 0) {
            pthread_mutex_lock(&rdp_transmit_mutex);
            struct wlr_buffer *locked_buffer = wlr_buffer_lock(buffer_to_transmit);
            if (locked_buffer) {
                rdp_transmit_surface(locked_buffer);
                wlr_buffer_unlock(locked_buffer);
            }
            pthread_mutex_unlock(&rdp_transmit_mutex);
        }
    }

    wlr_output_state_finish(&final_state);
    wlr_output_state_finish(&output_state_direct);

cleanup_frame:
    // Final cleanup and frame signaling.
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);
    wlr_scene_output_send_frame_done(scene_output, &now);

    // --- Frame Scheduling Logic (no changes needed) ---
    
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
    const char *title = (xdg_toplevel->title) ? xdg_toplevel->title : "N/A";

    wlr_log(WLR_INFO, "[MAP:%s] Toplevel mapped. Starting geometry negotiation.", title);
    toplevel->mapped = true;

    // Default content size if client provides no hints.
    int target_content_width = 800;
    int target_content_height = 600;
    bool size_from_client_geom = false;

    // Check if the client provided a preferred geometry.
    struct wlr_box client_geom = xdg_toplevel->base->geometry;
    if (client_geom.width > 0 && client_geom.height > 0) {
        target_content_width = client_geom.width;
        target_content_height = client_geom.height;
        size_from_client_geom = true;
        wlr_log(WLR_DEBUG, "[MAP:%s] Client suggested initial geometry: %dx%d", title, target_content_width, target_content_height);
    }

    // --- CRITICAL FIX: PREDICT IF SSDs WILL BE USED ---
    bool intend_server_decorations = true; // By default, we want to provide SSDs.

    // If a decoration object already exists and the client explicitly requested CSD, then we don't intend to draw SSDs.
    if (toplevel->decoration &&
        (toplevel->decoration->current.mode == WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE ||
         toplevel->decoration->pending.mode == WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE)) {
        intend_server_decorations = false;
    }

    // If we got a size from the client AND we intend to draw decorations,
    // we assume the client might have given us the *total window size*.
    // We derive the *content size* from that.
    if (size_from_client_geom && intend_server_decorations) {
        int derived_content_width = client_geom.width - (2 * BORDER_WIDTH);
        int derived_content_height = client_geom.height - TITLE_BAR_HEIGHT - BORDER_WIDTH;

        // Sanity check: If the derived size is tiny, the client probably
        // meant the content size, not the total size.
        if (derived_content_width >= 50 && derived_content_height >= 50) {
            target_content_width = derived_content_width;
            target_content_height = derived_content_height;
            wlr_log(WLR_INFO, "[MAP:%s] Client suggested total %dx%d. Server intends SSDs. Derived target CONTENT size: %dx%d.",
                    title, client_geom.width, client_geom.height, target_content_width, target_content_height);
        } else {
             wlr_log(WLR_INFO, "[MAP:%s] Derived content size %dx%d was too small. Assuming client's suggestion was for content.",
                    title, derived_content_width, derived_content_height);
        }
    }

    // Now, set the final CONTENT size. The subsequent commit will handle creating
    // the decoration visuals around this correctly-sized content area.
    wlr_log(WLR_INFO, "[MAP:%s] Setting final XDG toplevel CONTENT size to %dx%d.", title, target_content_width, target_content_height);
    wlr_xdg_toplevel_set_size(xdg_toplevel, target_content_width, target_content_height);

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
// Modify the function signature to accept the toplevel
static struct wlr_scene_rect *create_decoration_rect(
    struct wlr_scene_tree *parent,
    struct tinywl_toplevel *toplevel, // <<< ADD THIS ARGUMENT
    int width, int height, int x, int y, float color[4]) {
    
    struct wlr_scene_rect *rect = wlr_scene_rect_create(parent, width, height, color);
    if (rect) {
        wlr_scene_node_set_position(&rect->node, x, y);
        wlr_scene_node_set_enabled(&rect->node, true);
        
        // --- THIS IS THE CRITICAL FIX ---
        // Store a pointer to the parent toplevel in the node's data field.
        rect->node.data = toplevel;
        wlr_log(WLR_DEBUG, "Created decoration rect %p, set its node->data to toplevel %p", (void*)rect, (void*)toplevel);

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

    if (!surface) return;

    wlr_log(WLR_INFO, "[COMMIT:%s] START - Mapped:%d, Init:%d, XDGConf:%d, SSD_EN:%d, SSD_PEND:%d. DecoCurr:%d, DecoPend:%d. Buf:%p(%dx%d), Seq:%u. XDGCurrentSize:%dx%d",
            title, surface->mapped, xdg_surface->initialized, xdg_surface->configured,
            toplevel->ssd.enabled, toplevel->ssd_pending_enable,
            toplevel->decoration ? toplevel->decoration->current.mode : -1,
            toplevel->decoration ? toplevel->decoration->pending.mode : -1,
            (void*)surface->current.buffer, surface->current.buffer ? surface->current.buffer->width : 0, surface->current.buffer ? surface->current.buffer->height : 0,
            surface->current.seq, xdg_toplevel->current.width, xdg_toplevel->current.height);

    bool needs_surface_configure = false;
    enum wlr_xdg_toplevel_decoration_v1_mode server_preferred_mode =
        WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE;

    // If a decoration request is pending, now is the time to handle it.
    if (toplevel->decoration && toplevel->ssd_pending_enable) {
        // We only negotiate when the surface is ready (initialized or on its first commit).
        if (xdg_surface->initialized || xdg_surface->initial_commit) {
            wlr_log(WLR_INFO, "[COMMIT:%s] ssd_pending_enable=true. Setting preferred mode: %d.", title, server_preferred_mode);
            wlr_xdg_toplevel_decoration_v1_set_mode(toplevel->decoration, server_preferred_mode);
            needs_surface_configure = true; // MUST configure after setting mode.
            toplevel->ssd_pending_enable = false;
        }
    }

    // Now, make a decision based on the current, stable decoration mode.
    enum wlr_xdg_toplevel_decoration_v1_mode effective_mode = WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_NONE;
    if (toplevel->decoration) {
        effective_mode = toplevel->decoration->current.mode;
    }

    if (effective_mode == server_preferred_mode) {
        // The protocol state is SERVER_SIDE. Check if our internal state matches.
        if (!toplevel->ssd.enabled) {
            wlr_log(WLR_INFO, "[COMMIT:%s] Effective mode is SERVER_SIDE. Enabling SSDs.", title);
            ensure_ssd_enabled(toplevel);
            // This geometry change requires a configure.
            needs_surface_configure = true;
        }
    } else {
        // The protocol state is CLIENT_SIDE or NONE.
        if (toplevel->ssd.enabled) {
            wlr_log(WLR_INFO, "[COMMIT:%s] Effective mode is %d (NOT SERVER_SIDE). Disabling SSDs.", title, effective_mode);
            ensure_ssd_disabled(toplevel);
            // This geometry change requires a configure.
            needs_surface_configure = true;
        }
    }

    if (toplevel->ssd.enabled) {
        update_decoration_geometry(toplevel);
    }

    // If we determined a configure is needed, or if the surface is still unconfigured, schedule it now.
    if (needs_surface_configure || (xdg_surface->initialized && !xdg_surface->configured)) {
        wlr_log(WLR_INFO, "[COMMIT:%s] Scheduling configure for xdg_surface.", title);
        wlr_xdg_surface_schedule_configure(xdg_surface);
    }

    // Caching and redrawing logic remains the same.
    if (surface->current.buffer) {
        if (!(toplevel->cached_texture && toplevel->last_commit_seq == surface->current.seq)) {
            if (toplevel->cached_texture) wlr_texture_destroy(toplevel->cached_texture);
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

    if (surface->mapped) {
        struct tinywl_output *output_iter;
        wl_list_for_each(output_iter, &server->outputs, link) {
            if (output_iter->wlr_output && output_iter->wlr_output->enabled) {
                wlr_output_schedule_frame(output_iter->wlr_output);
            }
        }
    }
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

    toplevel->desktop = server->current_desktop; // Assign to current desktop
    wlr_log(WLR_INFO, "New toplevel created on desktop %d", toplevel->desktop);

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
/*
void update_toplevel_visibility(struct tinywl_server *server) {
    wlr_log(WLR_DEBUG, "Updating toplevel visibility. Current desktop: %d, Expo active: %d",
            server->current_desktop, server->expo_effect_active);

    struct tinywl_toplevel *toplevel;
    wl_list_for_each(toplevel, &server->toplevels, link) {
        if (toplevel->scene_tree) {
            // A toplevel is visible if expo is on, OR if its desktop matches the current one.
            bool visible = server->expo_effect_active || (toplevel->desktop == server->current_desktop);
            wlr_scene_node_set_enabled(&toplevel->scene_tree->node, visible);
            wlr_log(WLR_DEBUG, "  Toplevel on desktop %d is now %s.",
                    toplevel->desktop, visible ? "ENABLED" : "DISABLED");
        }
    }

    // Schedule a frame on all outputs to reflect the change
    struct tinywl_output *output;
    wl_list_for_each(output, &server->outputs, link) {
        if (output->wlr_output && output->wlr_output->enabled) {
            wlr_output_schedule_frame(output->wlr_output);
        }
    }
}*/
void update_toplevel_visibility(struct tinywl_server *server) {
    wlr_log(WLR_DEBUG, "Updating toplevel visibility. Current desktop: %d, Cube: %d, Expo: %d",
            server->current_desktop, server->cube_effect_active, server->expo_effect_active);

    struct tinywl_toplevel *toplevel;
    wl_list_for_each(toplevel, &server->toplevels, link) {
        if (toplevel->scene_tree) {
            bool should_be_enabled;

            // --- THIS IS THE CRITICAL LOGIC ---
            if (server->expo_effect_active || server->cube_effect_active) {
                // If any multi-desktop effect is active, a window is visible
                // as long as it's not minimized. Its desktop property determines
                // *where* it's rendered, not *if* it's enabled.
                should_be_enabled = (toplevel->scale > 0.01f);
            } else {
                // If no effect is active, only show windows on the current desktop.
                should_be_enabled = (toplevel->desktop == server->current_desktop);
            }

            // Apply the calculated state.
            wlr_scene_node_set_enabled(&toplevel->scene_tree->node, should_be_enabled);
        }
    }

    // Schedule a frame on all outputs to reflect the change.
    struct tinywl_output *output;
    wl_list_for_each(output, &server->outputs, link) {
        if (output->wlr_output && output->wlr_output->enabled) {
            wlr_output_schedule_frame(output->wlr_output);
        }
    }
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

 float title_bar_color[4] = {0.0f, 0.0f, 0.8f, 0.7f};
    float border_color[4]    = {0.1f, 0.1f, 0.1f, 0.7f};

    // Pass the toplevel pointer to the helper function
    toplevel->ssd.title_bar    = create_decoration_rect(toplevel->scene_tree, toplevel, 0, 0, 0, 0, title_bar_color);
    toplevel->ssd.border_left  = create_decoration_rect(toplevel->scene_tree, toplevel, 0, 0, 0, 0, border_color);
    toplevel->ssd.border_right = create_decoration_rect(toplevel->scene_tree, toplevel, 0, 0, 0, 0, border_color);
    toplevel->ssd.border_bottom= create_decoration_rect(toplevel->scene_tree, toplevel, 0, 0, 0, 0, border_color);
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
    struct tinywl_toplevel *toplevel = xdg_toplevel->base->data;

    const char *title_for_log = xdg_toplevel->title ? xdg_toplevel->title : "N/A";

    wlr_log(WLR_INFO, "[DECO_MGR] New decoration object %p for toplevel '%s'.",
            (void*)decoration, title_for_log);

    if (!toplevel) {
        wlr_log(WLR_ERROR, "[DECO_MGR] No tinywl_toplevel for new decoration on '%s'.", title_for_log);
        return;
    }

    // Clean up old listeners if a decoration is being replaced
    if (toplevel->decoration) {
        wl_list_remove(&toplevel->decoration_destroy_listener.link);
        wl_list_remove(&toplevel->decoration_request_mode_listener.link);
    }

    toplevel->decoration = decoration;
    toplevel->decoration_destroy_listener.notify = decoration_handle_destroy;
    wl_signal_add(&decoration->events.destroy, &toplevel->decoration_destroy_listener);
    toplevel->decoration_request_mode_listener.notify = decoration_handle_request_mode;
    wl_signal_add(&decoration->events.request_mode, &toplevel->decoration_request_mode_listener);

    // --- THE FIX ---
    // Instead of setting the mode here, just flag that we need to handle it.
    toplevel->ssd_pending_enable = true;
    wlr_log(WLR_INFO, "[DECO_MGR] Toplevel '%s': Marked ssd_pending_enable=true. Mode set will occur on next commit.", title_for_log);

    // If the surface is already up and running, we need to trigger a commit/configure
    // cycle to apply the new decoration state.
    if (xdg_toplevel->base->initialized) {
        wlr_log(WLR_DEBUG, "[DECO_MGR] Surface for '%s' is already initialized. Scheduling configure to process pending decoration.", title_for_log);
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
    "layout(location = 0) in vec2 position;\n"    // Position at location 0
    "layout(location = 1) in vec2 texcoord;\n"    // Texcoord at location 1
    "out vec2 v_texcoord;\n"
    "uniform mat3 mvp;\n"
    "void main() {\n"
    "    vec3 pos_transformed = mvp * vec3(position, 1.0);\n"
    "    gl_Position = vec4(pos_transformed.xy, 0.0, 1.0);\n"
    "    v_texcoord = texcoord;\n"
    "}\n";

const char *vertex_shader_src =
        "#version 300 es\n"
    "precision mediump float;\n"
    "layout(location = 0) in vec2 position;\n"    // Position at location 0
    "layout(location = 1) in vec2 texcoord;\n"    // Texcoord at location 1
    "out vec2 v_texcoord;\n"
    "uniform mat3 mvp;\n"
    "void main() {\n"
    "    vec3 pos_transformed = mvp * vec3(position, 1.0);\n"
    "    gl_Position = vec4(pos_transformed.xy, 0.0, 1.0);\n"
    "    v_texcoord = texcoord;\n"
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
    "layout(location = 0) in vec2 position;\n"    // Position at location 0
    "layout(location = 1) in vec2 texcoord;\n"    // Texcoord at location 1
    "out vec2 v_texcoord;\n"
    "uniform mat3 mvp;\n"
    "void main() {\n"
    "    vec3 pos_transformed = mvp * vec3(position, 1.0);\n"
    "    gl_Position = vec4(pos_transformed.xy, 0.0, 1.0);\n"
    "    v_texcoord = texcoord;\n"
    "}\n";

/*
static const char *panel_fragment_shader_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "\n"
    "in vec2 v_texcoord; // Normalized UV for the panel quad itself (0,0 to 1,1)\n"
    "out vec4 frag_color;\n"
    "\n"
    "// Time and resolution uniforms for the background animation\n"
    "uniform float time;\n"
    "uniform vec2 iResolution; // Resolution of the viewport/target this shader draws to\n"
    "\n"
    "// Uniforms for the preview functionality\n"
    "uniform sampler2D u_previewTexture;     // The window texture for the preview\n"
    "uniform bool u_isPreviewActive;         // Flag: is there a preview to draw?\n"
    "uniform vec4 u_previewRect;           // Preview position and size on panel: x, y, w, h (normalized 0-1 relative to panel's own UVs)\n"
    "uniform mat3 u_previewTexTransform;   // Transform for sampling the preview texture\n"
    "\n"
    "// --- Start of new \"Electric Grid\" background --- \n"
    "\n"
    "// Simple 2D rotation matrix\n"
    "mat2 rotate2D(float angle) {\n"
    "    float s = sin(angle);\n"
    "    float c = cos(angle);\n"
    "    return mat2(c, -s, s, c);\n"
    "}\n"
    "\n"
    "// Hash function for pseudo-randomness\n"
    "float hash(vec2 p) {\n"
    "    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);\n"
    "}\n"
    "\n"
    "vec4 getElectricGridColor() {\n"
    "    // Use gl_FragCoord.xy for a background that maps to screen pixels.\n"
    "    // This makes the effect appear 'behind' the panel.\n"
    "    vec2 uv = (gl_FragCoord.xy - 0.5 * iResolution.xy) / min(iResolution.x, iResolution.y);\n"
    "    float iTime = time * 0.35; // Control overall animation speed\n"
    "\n"
    "    vec3 col = vec3(0.01, 0.02, 0.05); // Dark blue-ish background\n"
    "\n"
    "    // Apply a slight rotation and zoom to the coordinate system for dynamism\n"
    "    uv *= rotate2D(iTime * 0.05);\n"
    "    uv *= (1.0 + 0.05 * sin(iTime * 0.2));\n"
    "\n"
    "    float grid_scale = 8.0;\n"
    "    vec2 grid_uv = uv * grid_scale;\n"
    "\n"
    "    // Animated scrolling for the grid\n"
    "    grid_uv.x += iTime * 0.3;\n"
    "    grid_uv.y -= iTime * 0.2;\n"
    "\n"
    "    vec2 grid_id = floor(grid_uv);\n"
    "    vec2 grid_fract = fract(grid_uv);\n"
    "\n"
    "    // Grid lines\n"
    "    float line_width = 0.03;\n"
    "    float line_intensity_x = smoothstep(line_width, 0.0, min(grid_fract.x, 1.0 - grid_fract.x));\n"
    "    float line_intensity_y = smoothstep(line_width, 0.0, min(grid_fract.y, 1.0 - grid_fract.y));\n"
    "    float grid_lines = max(line_intensity_x, line_intensity_y);\n"
    "\n"
    "    // Pulsing effect for grid lines\n"
    "    float pulse_wave = 0.6 + 0.4 * sin(grid_id.x * 0.7 - iTime * 2.5 + grid_id.y * 0.5);\n"
    "    grid_lines *= pulse_wave;\n"
    "\n"
    "    vec3 grid_color = vec3(0.1, 0.6, 1.0); // Bright electric blue\n"
    "    col = mix(col, grid_color, grid_lines * 0.7); // Blend grid lines\n"
    "\n"
    "    // Dots at grid intersections\n"
    "    float dot_radius = 0.08;\n"
    "    float dist_to_center = length(grid_fract - 0.5);\n"
    "    float dots = smoothstep(dot_radius, dot_radius - 0.02, dist_to_center); // Smaller, sharper dots\n"
    "    \n"
    "    // Pulsing and slightly randomized dots\n"
    "    float dot_pulse = 0.5 + 0.5 * sin(hash(grid_id) * 6.283 + iTime * 4.0);\n"
    "    dots *= dot_pulse;\n"
    "    vec3 dot_color = vec3(0.8, 1.0, 1.0); // Cyan/white dots\n"
    "    col = mix(col, dot_color, dots * 0.4);\n"
    "\n"
    "    // Add some subtle secondary, thinner, faster moving grid lines\n"
    "    vec2 fine_grid_uv = uv * grid_scale * 2.5;\n"
    "    fine_grid_uv.x -= iTime * 0.7;\n"
    "    fine_grid_uv.y += iTime * 0.5;\n"
    "    vec2 fine_grid_fract = fract(fine_grid_uv);\n"
    "    float fine_line_width = 0.01;\n"
    "    float fine_intensity_x = smoothstep(fine_line_width, 0.0, min(fine_grid_fract.x, 1.0 - fine_grid_fract.x));\n"
    "    float fine_intensity_y = smoothstep(fine_line_width, 0.0, min(fine_grid_fract.y, 1.0 - fine_grid_fract.y));\n"
    "    float fine_grid_lines = max(fine_intensity_x, fine_intensity_y);\n"
    "    col = mix(col, grid_color * 0.5, fine_grid_lines * 0.2); // Dimmer fine lines\n"
    "\n"
    "    // Clamp final color and set alpha for the panel background\n"
    "    return vec4(clamp(col, 0.0, 1.0), 0.8); // MODIFIED ALPHA HERE \n"
    "}\n"
    "// --- End of new \"Electric Grid\" background --- \n"
    "\n"
    "void main() {\n"
    "    // Start with the new Electric Grid animation as the background\n"
    "    vec4 background_color = getElectricGridColor();\n"
    "    vec4 final_pixel_color = background_color;\n"
    "\n"
    "    // Layer the preview on top if active\n"
    "    // THIS PREVIEW LOGIC IS IDENTICAL TO YOUR ORIGINAL OCTAGRAMS SHADER\n"
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
    "                // Blend the preview over the background (using the same mix as your original)\n"
    "                final_pixel_color = mix(background_color, preview_sample, preview_sample.a);\n"
    "            }\n"
    "        }\n"
    "    }\n"
    "\n"
    "    frag_color = final_pixel_color.bgra;\n"
    "}";*/
static const char *panel_fragment_shader_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "\n"
    "in vec2 v_texcoord; // Normalized UV for the panel quad itself (0,0 to 1,1)\n"
    "out vec4 frag_color;\n"
    "\n"
    "// Time and resolution uniforms for the background animation\n"
    "uniform float time;\n"
    "uniform vec2 iResolution; // Resolution of the viewport/target this shader draws to\n"
    "uniform vec2 panelDimensions; // Actual panel dimensions in pixels\n"
    "\n"
    "// Uniforms for the preview functionality\n"
    "uniform sampler2D u_previewTexture;     // The window texture for the preview\n"
    "uniform bool u_isPreviewActive;         // Flag: is there a preview to draw?\n"
    "uniform vec4 u_previewRect;           // Preview position and size on panel: x, y, w, h (normalized 0-1 relative to panel's own UVs)\n"
    "uniform mat3 u_previewTexTransform;   // Transform for sampling the preview texture\n"
    "\n"
    "// Function to calculate rounded top corner mask using pixel-based dimensions\n"
    "float roundedTopCornerMask(vec2 uv, float radiusPixels) {\n"
    "    // Convert UV coordinates to pixel coordinates\n"
    "    vec2 pixelCoord = uv * panelDimensions;\n"
    "    \n"
    "    // --- THE FIX IS HERE --- \n"
    "    // Check the TOP of the panel (where pixelCoord.y is SMALLER than radiusPixels)\n"
    "    if (pixelCoord.y > radiusPixels) {\n"
    "        return 1.0; // The non-rounded part is fully opaque\n"
    "    }\n"
    "    \n"
    "    // We are now in the top-most part of the panel. Check if we're in a corner.\n"
    "    vec2 corner_dist;\n"
    "    \n"
    "    // Top-left corner (pixelCoord.x is small)\n"
    "    if (pixelCoord.x < radiusPixels) {\n"
    "        corner_dist = vec2(radiusPixels - pixelCoord.x, radiusPixels - pixelCoord.y);\n"
    "        float dist_to_center = length(corner_dist);\n"
    "        return smoothstep(radiusPixels + 1.0, radiusPixels - 1.0, dist_to_center);\n"
    "    }\n"
    "    \n"
    "    // Top-right corner (pixelCoord.x is large)\n"
    "    if (pixelCoord.x > (panelDimensions.x - radiusPixels)) {\n"
    "        corner_dist = vec2(pixelCoord.x - (panelDimensions.x - radiusPixels), radiusPixels - pixelCoord.y);\n"
    "        float dist_to_center = length(corner_dist);\n"
    "        return smoothstep(radiusPixels + 1.0, radiusPixels - 1.0, dist_to_center);\n"
    "    }\n"
    "    \n"
    "    // The area between the corners at the top is fully opaque\n"
    "    return 1.0;\n"
    "}\n"
    "\n"
    "// ... (The rest of your shader code remains exactly the same) ...\n"
    "\n"
    "mat2 rotate2D(float angle) {\n"
    "    float s = sin(angle);\n"
    "    float c = cos(angle);\n"
    "    return mat2(c, -s, s, c);\n"
    "}\n"
    "\n"
    "float hash(vec2 p) {\n"
    "    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);\n"
    "}\n"
    "\n"
    "vec4 getElectricGridColor() {\n"
    "    vec2 uv = (gl_FragCoord.xy - 0.5 * iResolution.xy) / min(iResolution.x, iResolution.y);\n"
    "    float iTime = time * 0.35;\n"
    "    vec3 col = vec3(0.01, 0.02, 0.05);\n"
    "    uv *= rotate2D(iTime * 0.05);\n"
    "    uv *= (1.0 + 0.05 * sin(iTime * 0.2));\n"
    "    float grid_scale = 8.0;\n"
    "    vec2 grid_uv = uv * grid_scale;\n"
    "    grid_uv.x += iTime * 0.3;\n"
    "    grid_uv.y -= iTime * 0.2;\n"
    "    vec2 grid_id = floor(grid_uv);\n"
    "    vec2 grid_fract = fract(grid_uv);\n"
    "    float line_width = 0.03;\n"
    "    float line_intensity_x = smoothstep(line_width, 0.0, min(grid_fract.x, 1.0 - grid_fract.x));\n"
    "    float line_intensity_y = smoothstep(line_width, 0.0, min(grid_fract.y, 1.0 - grid_fract.y));\n"
    "    float grid_lines = max(line_intensity_x, line_intensity_y);\n"
    "    float pulse_wave = 0.6 + 0.4 * sin(grid_id.x * 0.7 - iTime * 2.5 + grid_id.y * 0.5);\n"
    "    grid_lines *= pulse_wave;\n"
    "    vec3 grid_color = vec3(0.1, 0.6, 1.0);\n"
    "    col = mix(col, grid_color, grid_lines * 0.7);\n"
    "    float dot_radius = 0.08;\n"
    "    float dist_to_center = length(grid_fract - 0.5);\n"
    "    float dots = smoothstep(dot_radius, dot_radius - 0.02, dist_to_center);\n"
    "    float dot_pulse = 0.5 + 0.5 * sin(hash(grid_id) * 6.283 + iTime * 4.0);\n"
    "    dots *= dot_pulse;\n"
    "    vec3 dot_color = vec3(0.8, 1.0, 1.0);\n"
    "    col = mix(col, dot_color, dots * 0.4);\n"
    "    vec2 fine_grid_uv = uv * grid_scale * 2.5;\n"
    "    fine_grid_uv.x -= iTime * 0.7;\n"
    "    fine_grid_uv.y += iTime * 0.5;\n"
    "    vec2 fine_grid_fract = fract(fine_grid_uv);\n"
    "    float fine_line_width = 0.01;\n"
    "    float fine_intensity_x = smoothstep(fine_line_width, 0.0, min(fine_grid_fract.x, 1.0 - fine_grid_fract.x));\n"
    "    float fine_intensity_y = smoothstep(fine_line_width, 0.0, min(fine_grid_fract.y, 1.0 - fine_grid_fract.y));\n"
    "    float fine_grid_lines = max(fine_intensity_x, fine_intensity_y);\n"
    "    col = mix(col, grid_color * 0.5, fine_grid_lines * 0.2);\n"
    "    return vec4(clamp(col, 0.0, 1.0), 0.8);\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    float corner_radius_pixels = 40.0;\n"
    "    float corner_mask = roundedTopCornerMask(v_texcoord, corner_radius_pixels);\n"
    "    vec4 background_color = getElectricGridColor();\n"
    "    background_color.a *= corner_mask;\n"
    "    vec4 final_pixel_color = background_color;\n"
    "\n"
    "    if (u_isPreviewActive) {\n"
    "        bool is_inside_preview_x = (v_texcoord.x >= u_previewRect.x && v_texcoord.x < (u_previewRect.x + u_previewRect.z));\n"
    "        bool is_inside_preview_y = (v_texcoord.y >= u_previewRect.y && v_texcoord.y < (u_previewRect.y + u_previewRect.w));\n"
    "        \n"
    "        if (is_inside_preview_x && is_inside_preview_y) {\n"
    "            vec3 transformed_uv_homogeneous = u_previewTexTransform * vec3(v_texcoord, 1.0);\n"
    "            vec2 final_sample_uv = transformed_uv_homogeneous.xy;\n"
    "            vec4 preview_sample = texture(u_previewTexture, final_sample_uv);\n"
    "            \n"
    "            if (preview_sample.a > 0.05) {\n"
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
    "layout(location = 0) in vec2 position;\n"    // Position at location 0
    "layout(location = 1) in vec2 texcoord;\n"    // Texcoord at location 1
    "out vec2 v_texcoord;\n"
    "uniform mat3 mvp;\n"
    "void main() {\n"
    "    vec3 pos_transformed = mvp * vec3(position, 1.0);\n"
    "    gl_Position = vec4(pos_transformed.xy, 0.0, 1.0);\n"
    "    v_texcoord = texcoord;\n"
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
    "layout(location = 0) in vec2 position;\n"    // Position at location 0
    "layout(location = 1) in vec2 texcoord;\n"    // Texcoord at location 1
    "out vec2 v_texcoord;\n"
    "uniform mat3 mvp;\n"
    "void main() {\n"
    "    vec3 pos_transformed = mvp * vec3(position, 1.0);\n"
    "    gl_Position = vec4(pos_transformed.xy, 0.0, 1.0);\n"
    "    v_texcoord = texcoord;\n"
    "}\n";

//army

static const char *ssd2_fragment_shader_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "\n"
    "in vec2 v_texcoord; // IMPORTANT: This MUST be correctly supplied for the panel's quad\n"
    "out vec4 frag_color;\n"
    "\n"
    "uniform float time;\n"
    "uniform vec2 iResolution; // Resolution of the viewport/target this shader draws to\n"
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
    "// Helper function for Signed Distance to a 2D Box with individual corner control\n"
    "// p: point to sample in pixel coordinates\n"
    "// b: half-extents of the box in pixel coordinates\n"
    "// r: corner radius in pixels\n"
    "float sdfTopRoundBox2D(vec2 p, vec2 b, float r) {\n"
    "    r = min(r, min(b.x, b.y)); // Clamp radius to prevent issues with small rectangles\n"
    "    \n"
    "    // **Top Left**: roundTopLeft = false (SHARP)\n"
    "    // **Top Right**: roundTopRight = false (SHARP)\n"
    "    // **Bottom Left**: roundBottomLeft = true (ROUNDED)\n"
    "    // **Bottom Right**: roundBottomRight = true (ROUNDED)\n"
    "    bool roundTopLeft = false;\n"
    "    bool roundTopRight = false;\n"
    "    bool roundBottomLeft = true;\n"
    "    bool roundBottomRight = true;\n"
    "    \n"
    "    // Determine which corner we're in\n"
    "    bool inTopLeft = (p.x < -b.x + r && p.y > b.y - r);\n"
    "    bool inTopRight = (p.x > b.x - r && p.y > b.y - r);\n"
    "    bool inBottomLeft = (p.x < -b.x + r && p.y < -b.y + r);\n"
    "    bool inBottomRight = (p.x > b.x - r && p.y < -b.y + r);\n"
    "    \n"
    "    // Apply rounding to specific corners\n"
    "    if (inTopLeft && roundTopLeft) {\n"
    "        vec2 corner_center = vec2(-b.x + r, b.y - r);\n"
    "        return length(p - corner_center) - r;\n"
    "    }\n"
    "    else if (inTopRight && roundTopRight) {\n"
    "        vec2 corner_center = vec2(b.x - r, b.y - r);\n"
    "        return length(p - corner_center) - r;\n"
    "    }\n"
    "    else if (inBottomLeft && roundBottomLeft) {\n"
    "        vec2 corner_center = vec2(-b.x + r, -b.y + r);\n"
    "        return length(p - corner_center) - r;\n"
    "    }\n"
    "    else if (inBottomRight && roundBottomRight) {\n"
    "        vec2 corner_center = vec2(b.x - r, -b.y + r);\n"
    "        return length(p - corner_center) - r;\n"
    "    }\n"
    "    \n"
    "    // Default to regular box SDF for all other areas\n"
    "    vec2 q = abs(p) - b;\n"
    "    return max(q.x, q.y);\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    // Scene rendering part (uses v_texcoord to keep effect fixed to panel)\n"
    "    vec2 fragCoord_for_scene = v_texcoord * iResolution;\n"
    "    float iTime = time*200.0;\n"
    "    \n"
    "    // Calculate normalized coordinates 'p' using v_texcoord instead of gl_FragCoord\n"
    "    // This keeps the effect fixed to the panel regardless of window position\n"
    "    vec2 p = (2.0*fragCoord_for_scene - iResolution.xy) / iResolution.y;\n"
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
    "    // --- Individual Corner Control Rounded Masking with FIXED 40px corner radius ---\n"
    "    // Convert v_texcoord [0,1] to actual pixel coordinates from center\n"
    "    // This ensures the coordinate system matches the actual pixel dimensions\n"
    "    vec2 pixel_pos = (v_texcoord - 0.5) * iResolution;\n"
    "\n"
    "    // Half-extents of the rectangle in pixels\n"
    "    vec2 box_half_extents_pixels = iResolution * 0.5;\n"
    "    \n"
    "    // FIXED corner radius in pixels - this will always be 40 pixels regardless of shape size\n"
    "    float corner_radius_pixels = 40.0;\n"
    "\n"
    "    // Calculate signed distance to the selectively-rounded rectangle edge in pixels\n"
    "    float dist_to_edge = sdfTopRoundBox2D(pixel_pos, box_half_extents_pixels, corner_radius_pixels);\n"
    "\n"
    "    // Antialiasing width in pixels (using 2.0 for smoother edges with larger radius)\n"
    "    float aa_pixels = 2.0;\n"
    "    \n"
    "    // Create alpha mask: 1.0 inside, 0.0 outside, smooth transition at the edge\n"
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
    "    float original_alpha = 0.9; // Original alpha of the content\n"
    "    float final_alpha = original_alpha * shape_alpha_mask;\n"
    "    // --- End Individual Corner Control Rounded Masking ---\n"
    "\n"
    "    frag_color = vec4(col, final_alpha*0.8).bgra;\n"
    "}\n"
    "// --- End of Inigo Quilez's shader code (adapted) ---\n";
/*
static const char *ssd_fragment_shader_src =
  "// Seigaiha Mandala shader converted to panel format, with curved corners using v_texcoord \n"
    "#version 300 es\n"
    "precision mediump float;\n"
    "\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "\n"
    "// --- Start of Seigaiha Mandala shader code (adapted) ---\n"
    "// Seigaiha Mandala by Philippe Desgranges\n"
    "// Email: Philippe.desgranges@gmail.com\n"
    "// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.\n"
    "\n"
    "#define S(a,b,c) smoothstep(a,b,c)\n"
    "\n"
    "vec3 premulMix(vec4 src, vec3 dst) {\n"
    "    return dst.rgb * (1.0 - src.a) + src.rgb;\n"
    "}\n"
    "\n"
    "vec4 premulMix(vec4 src, vec4 dst) {\n"
    "    vec4 res;\n"
    "    res.rgb = premulMix(src, dst.rgb);\n"
    "    res.a = 1.0 - (1.0 - src.a) * (1.0 - dst.a);\n"
    "    return res;\n"
    "}\n"
    "\n"
    "vec4 roundPattern(vec2 uv, vec2 resolution) {\n"
    "    float dist = length(uv);\n"
    "    float aa = 0.02;\n"
    "    float triangle = abs(fract(dist * 11.0 + 0.3) - 0.5);\n"
    "    float circles = S(0.25 - aa, 0.25 + aa, triangle);\n"
    "    float grad = dist * 2.0;\n"
    "    vec3 col = mix(vec3(0.0, 0.5, 0.6),  vec3(0.0, 0.2, 0.5), grad * grad);\n"
    "    col = mix(col, vec3(1.0), circles);\n"
    "    vec3 borderColor = vec3(0.7, 0.2, 0.2);\n"
    "    col = mix(col, borderColor, S(0.44 - aa, 0.44 + aa, dist));\n"
    "    col = mix(col, borderColor, S(0.05 + aa, 0.05 - aa, dist));\n"
    "    float mask = S(0.5, 0.49, dist);\n"
    "    return vec4(col * mask, mask);\n"
    "}\n"
    "\n"
    "vec4 ring(vec2 uv, float angle, float angleOffet, float centerDist, float numcircles, float circlesRad, vec2 resolution) {\n"
    "    float quadId = floor(angle * numcircles + angleOffet);\n"
    "    float quadAngle = (quadId + 0.5 - angleOffet) * (6.283 / numcircles);\n"
    "    vec2 quadCenter = vec2(cos(quadAngle), sin(quadAngle)) * centerDist;\n"
    "    vec2 circleUv = (uv + quadCenter) / circlesRad;\n"
    "    return roundPattern(circleUv, resolution);\n"
    "}\n"
    "\n"
    "vec4 dblRing(vec2 uv, float angle, float centerDist, float numcircles, float circlesRad, float t, vec2 resolution) {\n"
    "    float s = sin(t * 3.0 + centerDist * 10.0) * 0.05;\n"
    "    float d1 = 1.05 + s;\n"
    "    float d2 = 1.05 - s;\n"
    "    float rot = t * centerDist * 0.4 + sin(t + centerDist * 5.0) * 0.2;\n"
    "    vec4 ring1 = ring(uv, angle, 0.0 + rot, centerDist * d1, numcircles, circlesRad, resolution);\n"
    "    vec4 ring2 = ring(uv, angle, 0.5 + rot, centerDist * d2, numcircles, circlesRad, resolution);\n"
    "    vec4 col = premulMix(ring1, ring2);\n"
    "    return col;\n"
    "}\n"
    "\n"
    "vec4 autoRing(vec2 uv, float angle, float centerDist, float t, vec2 resolution) {\n"
    "    float nbCircles = 1.0 + floor(centerDist * 23.0);\n"
    "    return dblRing(uv, angle, centerDist, nbCircles, 0.23, t, resolution);\n"
    "}\n"
    "\n"
    "vec3 fullImage(vec2 uv, float angle, float centerDist, float t, vec2 resolution) {\n"
    "    vec3 col;\n"
    "    float space = 0.1;\n"
    "    float ringRad = floor(centerDist / space) * space;\n"
    "    vec4 ringCol1 = autoRing(uv, angle, ringRad - space, t, resolution);\n"
    "    vec4 ringCol2 = autoRing(uv, angle, ringRad, t, resolution);\n"
    "    vec4 ringCol3 = autoRing(uv, angle, ringRad + space, t, resolution);\n"
    "    if (ringRad > 0.0) {\n"
    "        col.rgb = ringCol3.rgb;\n"
    "        col.rgb = premulMix(ringCol2, col.rgb);\n"
    "        col.rgb = premulMix(ringCol1, col.rgb);\n"
    "    } else {\n"
    "        col.rgb = ringCol2.rgb;\n"
    "    }\n"
    "    return col;\n"
    "}\n"
    "\n"
    "float sdfTopRoundBox2D(vec2 p, vec2 b, float r) {\n"
    "    r = min(r, min(b.x, b.y));\n"
    "    bool roundTopLeft = false;\n"
    "    bool roundTopRight = false;\n"
    "    bool roundBottomLeft = true;\n"
    "    bool roundBottomRight = true;\n"
    "    bool inTopLeft = (p.x < -b.x + r && p.y > b.y - r);\n"
    "    bool inTopRight = (p.x > b.x - r && p.y > b.y - r);\n"
    "    bool inBottomLeft = (p.x < -b.x + r && p.y < -b.y + r);\n"
    "    bool inBottomRight = (p.x > b.x - r && p.y < -b.y + r);\n"
    "    if (inTopLeft && roundTopLeft) {\n"
    "        vec2 corner_center = vec2(-b.x + r, b.y - r);\n"
    "        return length(p - corner_center) - r;\n"
    "    } else if (inTopRight && roundTopRight) {\n"
    "        vec2 corner_center = vec2(b.x - r, b.y - r);\n"
    "        return length(p - corner_center) - r;\n"
    "    } else if (inBottomLeft && roundBottomLeft) {\n"
    "        vec2 corner_center = vec2(-b.x + r, -b.y + r);\n"
    "        return length(p - corner_center) - r;\n"
    "    } else if (inBottomRight && roundBottomRight) {\n"
    "        vec2 corner_center = vec2(b.x - r, -b.y + r);\n"
    "        return length(p - corner_center) - r;\n"
    "    }\n"
    "    vec2 q = abs(p) - b;\n"
    "    return max(q.x, q.y);\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    vec2 fragCoord = v_texcoord * iResolution;\n"
    "    float iTime = time;\n"
    "\n"
    "    // --- DEFINE PATTERN SCALE HERE ---\n"
    "    // This value now controls the size. It is independent of the panel's resolution.\n"
    "    // SMALLER number = LARGER pattern (zoomed in).\n"
    "    // LARGER number = SMALLER pattern (zoomed out).\n"
    "    float patternScale = 800.0; // <-- EDIT THIS VALUE TO CHANGE SIZE\n"
    "\n"
    "    // Calculate pattern coordinates using the fixed scale\n"
    "    vec2 uv = (fragCoord - 0.5 * iResolution.xy) / patternScale;\n"
    "uv *= 4.0; "
    "\n"
    "    // Computes polar coordinates for the pattern\n"
    "    float angle = atan(uv.y, uv.x) / 6.283 + 0.5;\n"
    "    float centerDist = length(uv);\n"
    "\n"
    "    // Generate the pattern color\n"
    "    vec3 col = fullImage(uv, angle, centerDist, iTime, iResolution);\n"
    "\n"
    "    // --- MASKING logic (remains the same) ---\n"
    "    vec2 pixel_pos = (v_texcoord - 0.5) * iResolution;\n"
    "    vec2 box_half_extents_pixels = iResolution * 0.5;\n"
    "    float corner_radius_pixels = 40.0;\n"
    "    float dist_to_edge = sdfTopRoundBox2D(pixel_pos, box_half_extents_pixels, corner_radius_pixels);\n"
    "    float aa_pixels = 2.0;\n"
    "    \n"
    "    float shape_alpha_mask;\n"
    "    if (dist_to_edge > aa_pixels * 0.5) {\n"
    "        shape_alpha_mask = 0.0;\n"
    "    } else if (dist_to_edge < -aa_pixels * 0.5) {\n"
    "        shape_alpha_mask = 1.0;\n"
    "    } else {\n"
    "        shape_alpha_mask = 1.0 - smoothstep(-aa_pixels * 0.5, aa_pixels * 0.5, dist_to_edge);\n"
    "    }\n"
    "    shape_alpha_mask = clamp(shape_alpha_mask, 0.0, 1.0);\n"
    "    float final_alpha = 1.0 * shape_alpha_mask;\n"
    "\n"
    "    frag_color = vec4(col, final_alpha).bgra;\n"
    "}\n"
    "// --- End of Seigaiha Mandala shader ---";*/

//https://www.shadertoy.com/view/wlscWX
static const char *ssd_fragment_shader_src =
  "/*\n"
  "   Shader Combination: Pyramid Pattern by Shane (https://www.shadertoy.com/view/tdVBRh)\n"
  "   integrated into a resolution-independent, rounded-corner panel framework.\n"
  "   The core pattern logic is from the Pyramid shader, while the scaling and\n"
  "   masking are from the pre-existing panel setup.\n"
  "*/\n"
  "\n"
    "#version 300 es\n"
    "precision mediump float;\n"
    "\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "\n"
    "#define OFFSET_ROW\n"
    "\n"
    "// --- Start of Pyramid Pattern Helper Functions ---\n"
    "mat2 rot2(in float a){ float c = cos(a), s = sin(a); return mat2(c, -s, s, c); }\n"
    "\n"
    "float hash21(vec2 p){  return fract(sin(dot(p, vec2(27.619, 57.583)))*43758.5453); }\n"
    "\n"
    "vec2 hash22B(vec2 p) {\n"
    "    float n = sin(dot(p, vec2(41, 289)));\n"
    "    return fract(vec2(262144, 32768)*n)*2. - 1.;\n"
    "}\n"
    "\n"
    "float n2D3G( in vec2 p ){\n"
    "    vec2 i = floor(p); p -= i;\n"
    "    vec4 v;\n"
    "    v.x = dot(hash22B(i), p);\n"
    "    v.y = dot(hash22B(i + vec2(1, 0)), p - vec2(1, 0));\n"
    "    v.z = dot(hash22B(i + vec2(0, 1)), p - vec2(0, 1));\n"
    "    v.w = dot(hash22B(i + 1.), p - 1.);\n"
    "    p = p*p*(3. - 2.*p);\n"
    "    return mix(mix(v.x, v.y, p.x), mix(v.z, v.w, p.x), p.y);\n"
    "}\n"
    "\n"
    "float fBm(vec2 p){ return n2D3G(p)*.66 + n2D3G(p*2.)*.34; }\n"
    "\n"
    "// Refactored to pass cellID out instead of using a global\n"
    "float bMap(vec2 p, out vec2 cellID) {\n"
    "    p *= rot2(-3.14159/5.);\n"
    "    #ifdef OFFSET_ROW\n"
    "    if(mod(floor(p.y), 2.)<.5) p.x += .5;\n"
    "    #endif\n"
    "    vec2 ip = floor(p);\n"
    "    p -= ip + .5;\n"
    "    cellID = ip;\n"
    "    float ang = -3.14159*3./5. + (fBm(ip/8. + time/3.))*6.2831*2.;\n"
    "    vec2 offs = vec2(cos(ang), sin(ang))*.35;\n"
    "    if(p.x<offs.x)  p.x = 1. - (p.x + .5)/abs(offs.x  + .5);\n"
    "    else p.x = (p.x - offs.x)/(.5 - offs.x);\n"
    "    if(p.y<offs.y) p.y = 1. - (p.y + .5)/abs(offs.y + .5);\n"
    "    else p.y = (p.y - offs.y)/(.5 - offs.y);\n"
    "    return 1. - max(p.x, p.y);\n"
    "}\n"
    "\n"
    "vec3 doBumpMap(in vec2 p, in vec3 n, float bumpfactor, inout float edge){\n"
    "    vec2 e = vec2(.025, 0);\n"
    "    vec2 cell_id; // Dummy variable, not used here but needed by bMap\n"
    "    float f = bMap(p, cell_id);\n"
    "    float fx = bMap(p - e.xy, cell_id);\n"
    "    float fy = bMap(p - e.yx, cell_id);\n"
    "    float fx2 = bMap(p + e.xy, cell_id);\n"
    "    float fy2 = bMap(p + e.yx, cell_id);\n"
    "    vec3 grad = (vec3(fx - fx2, fy - fx2, 0))/e.x/2.;\n"
    "    edge = length(vec2(fx, fy) + vec2(fx2, fy2) - f*2.);\n"
    "    edge = smoothstep(0., 1., edge/e.x);\n"
    "    grad -= n*dot(n, grad);\n"
    "    return normalize( n + grad*bumpfactor );\n"
    "}\n"
    "\n"
    "float doHatch(vec2 p, float res){\n"
    "    p *= res/16.;\n"
    "    float hatch = clamp(sin((p.x - p.y)*3.14159*200.)*2. + .5, 0., 1.);\n"
    "    float hRnd = hash21(floor(p*6.) + .73);\n"
    "    if(hRnd>.66) hatch = hRnd;\n"
    "    return hatch;\n"
    "}\n"
    "// --- End of Pyramid Pattern Helper Functions ---\n"
    "\n"
    "// --- Start of Panel Masking Function ---\n"
    "float sdfTopRoundBox2D(vec2 p, vec2 b, float r) {\n"
    "    r = min(r, min(b.x, b.y));\n"
    "    bool roundTopLeft = false, roundTopRight = false, roundBottomLeft = true, roundBottomRight = true;\n"
    "    if (p.x < -b.x + r && p.y > b.y - r && roundTopLeft) { return length(p - vec2(-b.x + r, b.y - r)) - r; }\n"
    "    if (p.x > b.x - r && p.y > b.y - r && roundTopRight) { return length(p - vec2(b.x - r, b.y - r)) - r; }\n"
    "    if (p.x < -b.x + r && p.y < -b.y + r && roundBottomLeft) { return length(p - vec2(-b.x + r, -b.y + r)) - r; }\n"
    "    if (p.x > b.x - r && p.y < -b.y + r && roundBottomRight) { return length(p - vec2(b.x - r, -b.y + r)) - r; }\n"
    "    vec2 q = abs(p) - b;\n"
    "    return max(q.x, q.y);\n"
    "}\n"
    "// --- End of Panel Masking Function ---\n"
    "\n"
    "void main() {\n"
    "    vec2 fragCoord = v_texcoord * iResolution;\n"
    "    float iTime = time;\n"
    "\n"
    "    // --- DEFINE PATTERN SCALE HERE ---\n"
    "    float patternScale = 800.0; // LARGER number = SMALLER pattern.\n"
    "\n"
    "    // --- Core Pattern Logic ---\n"
    "    vec2 uv = (fragCoord - 0.5 * iResolution.xy) / patternScale;\n"
    "   uv *=4.0 ; \n"
    "    vec3 col;\n"
    "    {\n"
    "        // This block is the adapted main logic from the Pyramid shader.\n"
    "        vec3 rd = normalize(vec3(uv, .5));\n"
    "        const float gSc = 10.;\n"
    "        vec2 p = uv*gSc + vec2(0, iTime/2.);\n"
    "        vec2 oP = p;\n"
    "        vec2 svID; // Local variable for cell ID\n"
    "        float m = bMap(p, svID);\n"
    "        vec3 n = vec3(0, 0, -1);\n"
    "        float edge = 0., bumpFactor = .25;\n"
    "        n = doBumpMap(p, n, bumpFactor, edge);\n"
    "        vec3 lp = vec3(-0. + sin(iTime)*.3, .0 + cos(iTime*1.3)*.3, -1) - vec3(uv, 0);\n"
    "        float lDist = max(length(lp), .001);\n"
    "        vec3 ld = lp/lDist;\n"
    "        float diff = max(dot(n, ld), 0.);\n"
    "        diff = pow(diff, 4.);\n"
    "        float spec = pow(max(dot(reflect(-ld, n), -rd), 0.), 16.);\n"
    "        float fre = min(pow(max(1. + dot(n, rd), 0.), 4.), 3.);\n"
    "        col = vec3(.15)*(diff + .251 + spec*vec3(1, .7, .3)*9. + fre*vec3(.1, .3, 1)*12.);\n"
    "        vec2 dummy_id;\n"
    "        float rf = smoothstep(0., .35, bMap(reflect(rd, n).xy*2., dummy_id)*fBm(reflect(rd, n).xy*3.) + .1);\n"
    "        col += col*col*rf*rf*vec3(1, .1, .1)*15.;\n"
    "        float shade = m*.83 + .17;\n"
    "        col *= shade;\n"
    "        col *= 1. - edge*.8;\n"
    "        // Use patternScale to keep hatch size consistent with the main pattern\n"
    "        float hatch = doHatch(oP/gSc, patternScale);\n"
    "        col *= hatch*.5 + .7;\n"
    "    }\n"
    "    // --- End of Core Pattern Logic ---\n"
    "\n"
    "    // Apply gamma correction from original Pyramid shader\n"
    "    vec3 final_pattern_color = sqrt(max(col, 0.));\n"
    "\n"
    "    // --- MASKING logic (remains the same) ---\n"
    "    vec2 pixel_pos = (v_texcoord - 0.5) * iResolution;\n"
    "    vec2 box_half_extents_pixels = iResolution * 0.5;\n"
    "    float corner_radius_pixels = 40.0;\n"
    "    float dist_to_edge = sdfTopRoundBox2D(pixel_pos, box_half_extents_pixels, corner_radius_pixels);\n"
    "    float aa_pixels = 2.0;\n"
    "\n"
    "    float shape_alpha_mask = 1.0 - smoothstep(-aa_pixels * 0.5, aa_pixels * 0.5, dist_to_edge);\n"
    "    shape_alpha_mask = clamp(shape_alpha_mask, 0.0, 1.0);\n"
    "\n"
    "    // Combine the pattern color with the shape mask\n"
    "    frag_color = vec4(final_pattern_color, shape_alpha_mask);\n"
    "}\n"
    "// --- End of shader ---";   
/*
// Add these near your other shader source strings
static const char *fullscreen_vertex_shader_src =
       "#version 300 es\n"
       "precision mediump float;\n"
       "layout(location = 0) in vec2 a_position_01;\n" // Vertices of the quad (0,0 to 1,1)
       "layout(location = 1) in vec2 a_texcoord;\n"    // Texture coords for the quad (0,0 to 1,1)
       "uniform mat3 mvp;\n"
       "out vec2 v_texcoord_to_fs;\n"
       "void main() {\n"
       "    gl_Position = vec4((mvp * vec3(a_position_01, 1.0)).xy, 0.0, 1.0);\n"
       // Pass texture coordinates directly. The FBO texture (scene content)
       // will be mapped entirely onto the scaled quad.
       "    v_texcoord_to_fs = a_texcoord;\n"
       "}\n";

static const char *expo_fragment_shader_src =
       "#version 300 es\n"
       "precision mediump float;\n"
       "in vec2 v_texcoord_to_fs;\n"
       "out vec4 frag_color;\n"
       "uniform sampler2D u_scene_texture;\n"
       "void main() {\n"
       "    vec4 original_color = texture(u_scene_texture, v_texcoord_to_fs);\n"
       "    vec3 inverted_rgb = vec3(1.0) - original_color.rgb;\n" // Invert R, G, B
       "    frag_color = vec4(inverted_rgb, original_color.a);\n"  // Keep original alpha
       "}\n";
*//*
static const char *fullscreen_vertex_shader_src =
       "#version 300 es\n"
       "precision mediump float;\n"
       "layout(location = 0) in vec2 a_position_01;\n" // Vertices of the quad (0,0 to 1,1)
       "layout(location = 1) in vec2 a_texcoord;\n"    // Texture coords for the quad (0,0 to 1,1)
       "uniform mat3 mvp;\n"
       "out vec2 v_texcoord_to_fs;\n"
       "void main() {\n"
       "    float zoom = 0.5; // 2x smaller\n"
       "    \n"
       "    // Scale the quad geometry and position it based on which instance/quadrant\n"
       "    vec2 scaled_pos = a_position_01 * zoom;\n"
       "    \n"
       "    // Offset to different quadrants (you'd need to pass quadrant info)\n"
       "    // For now, this will scale everything to center\n"
       "    vec2 center_offset = vec2(0.5 - zoom * 0.5);\n"
       "    vec2 final_pos = scaled_pos + center_offset;\n"
       "    \n"
       "    gl_Position = vec4((mvp * vec3(final_pos, 1.0)).xy, 0.0, 1.0);\n"
       // Pass texture coordinates directly. The FBO texture (scene content)\n"
       // will be mapped entirely onto the scaled quad.\n"
       "    v_texcoord_to_fs = a_texcoord;\n"
       "}\n";

static const char *expo_fragment_shader_src =
       "#version 300 es\n"
       "precision mediump float;\n"
       "in vec2 v_texcoord_to_fs;\n"
       "out vec4 frag_color;\n"
       "uniform sampler2D u_scene_texture;\n"
       "void main() {\n"
       "    vec4 original_color = texture(u_scene_texture, v_texcoord_to_fs);\n"
       "    vec3 inverted_rgb = vec3(1.0) - original_color.rgb;\n" // Invert R, G, B
       "    frag_color = vec4(inverted_rgb, original_color.a);\n"  // Keep original alpha
       "}\n";*/




/*interesting effcet
static const char *fullscreen_vertex_shader_src =
       "#version 300 es\n"
       "precision mediump float;\n"
       "layout(location = 0) in vec2 a_position_01;\n"
       "layout(location = 1) in vec2 a_texcoord;\n"
       "\n"
       // MVP is not directly used by this shader logic for positioning quadrants
       // uniform mat3 mvp; \n"
       "uniform float u_zoom;        // Main Q0 zoom: 1.0 (normal) to 2.0 (Q0 fullscreen)\n"
       "uniform int u_quadrant;      // <<< ORIGINAL AND CONSISTENT NAME NOW\n"
       "\n"
       "out vec2 v_texcoord_to_fs;\n"
       "\n"
       "const float ANIM_PARAM_MAX = 1.0; \n"
       "\n"
       "void main() {\n"
       "    vec2 final_ndc_pos;\n"
       "    float t_anim = (u_zoom - 1.0) / ANIM_PARAM_MAX; \n"
       "    t_anim = clamp(t_anim, 0.0, 1.0);\n"
       "\n"
       "    if (u_quadrant == 0) { // MAIN ZOOMING QUADRANT (Top-Left)\n"
       "        float main_quad_ndc_span = 1.0 + t_anim;\n"
       "        final_ndc_pos.x = -1.0 + (a_position_01.x * main_quad_ndc_span);\n"
       "        final_ndc_pos.y =  1.0 - (a_position_01.y * main_quad_ndc_span);\n"
       "\n"
       "    } else { // --- Other (non-main) quadrants --- \n"
       "        float other_quad_scale = 1.0 - t_anim;\n"
       "        vec2 quad_min_ndc, quad_max_ndc, anchor_ndc;\n"
       "\n"
       "        if (u_quadrant == 1) { // Top-Right Quadrant\n"
       "            quad_min_ndc = vec2(0.0, 0.0);\n"
       "            quad_max_ndc = vec2(1.0, 1.0);\n"
       "            anchor_ndc   = vec2(1.0, 1.0);\n"
       "        } else if (u_quadrant == 2) { // Bottom-Left Quadrant\n"
       "            quad_min_ndc = vec2(-1.0, -1.0);\n"
       "            quad_max_ndc = vec2(0.0, 0.0);\n"
       "            anchor_ndc   = vec2(-1.0, -1.0);\n"
       "        } else { // u_quadrant == 3, Bottom-Right Quadrant\n"
       "            quad_min_ndc = vec2(0.0, -1.0);\n"
       "            quad_max_ndc = vec2(1.0, 0.0);\n"
       "            anchor_ndc   = vec2(1.0, -1.0);\n"
       "        }\n"
       "\n"
       "        vec2 vertex_in_unscaled_quad_ndc;\n"
       "        vertex_in_unscaled_quad_ndc.x = mix(quad_min_ndc.x, quad_max_ndc.x, a_position_01.x);\n"
       "        vertex_in_unscaled_quad_ndc.y = mix(quad_min_ndc.y, quad_max_ndc.y, 1.0 - a_position_01.y);\n"
       "\n"
       "        final_ndc_pos = (vertex_in_unscaled_quad_ndc - anchor_ndc) * other_quad_scale + anchor_ndc;\n"
       "    }\n"
       "\n"
       "    gl_Position = vec4(final_ndc_pos, 0.0, 1.0);\n"
       "    v_texcoord_to_fs = vec2(a_texcoord.x, 1.0 - a_texcoord.y);\n"
       "}\n";*/

// Vertex Shader
static const char *fullscreen_vertex_shader_src =
       "#version 300 es\n"
       "precision mediump float;\n"
       "layout(location = 0) in vec2 a_position_01;\n"
       "layout(location = 1) in vec2 a_texcoord;\n"
       "\n"
       "uniform float u_zoom;       // 1.0 to 2.0\n"
       "uniform highp int u_quadrant;    // Which quadrant to expand (0-3)\n"
       "\n"
       "out vec2 v_texcoord_to_fs;\n"
       "\n"
       "void main() {\n"
       "    gl_Position = vec4(a_position_01 * 2.0 - 1.0, 0.0, 1.0);\n"
       "    v_texcoord_to_fs = a_texcoord;\n"
       "}\n";

// Fragment Shader
/*
static const char *expo_fragment_shader_src =
"#version 300 es\n"
"precision mediump float;\n"
"precision mediump int;\n"
"\n"
"uniform sampler2D u_scene_texture0;  // Texture for top-left quadrant\n"
"uniform sampler2D u_scene_texture1;  // Texture for top-right quadrant\n"
"uniform sampler2D u_scene_texture2;  // Texture for bottom-left quadrant\n"
"uniform sampler2D u_scene_texture3;  // Texture for bottom-right quadrant\n"
"uniform float u_zoom;               // 1.0 to 2.0\n"
"uniform highp int u_quadrant;       // Which quadrant to expand (0-3)\n"
"\n"
"in vec2 v_texcoord_to_fs;\n"
"out vec4 FragColor;\n"
"\n"
"void main() {\n"
"    float t = clamp(u_zoom - 1.0, 0.0, 1.0);\n"
"    vec2 uv = v_texcoord_to_fs;\n"
"    \n"
"    // Define the center point of each quadrant in the 2x2 grid\n"
"    vec2 quad_centers[4];\n"
"    quad_centers[0] = vec2(0.0, 1.0);  // Top-left\n"
"    quad_centers[1] = vec2(1.0, 1.0);  // Top-right\n"
"    quad_centers[2] = vec2(0.00, 0.00);  // Bottom-left\n"
"    quad_centers[3] = vec2(1.00, 0.00);  // Bottom-right\n"
"    \n"
"    // Get the center of the selected quadrant\n"
"    vec2 zoom_center = quad_centers[u_quadrant];\n"
"    \n"
"    // Transform UV: zoom in from the selected quadrant's center\n"
"    vec2 offset_from_center = uv - zoom_center;\n"
"    vec2 scaled_offset = offset_from_center / (1.0 + t);\n"
"    vec2 transformed_uv = zoom_center + scaled_offset;\n"
"    \n"
"    // Map the transformed UV to the original texture coordinates\n"
"    vec2 texture_uv;\n"
"    int target_quad;\n"
"    if (transformed_uv.x < 0.5 && transformed_uv.y > 0.5) target_quad = 0;\n"
"    else if (transformed_uv.x > 0.5 && transformed_uv.y > 0.5) target_quad = 1;\n"
"    else if (transformed_uv.x < 0.5 && transformed_uv.y < 0.5) target_quad = 2;\n"
"    else target_quad = 3;\n"
"    \n"
"    // Map to texture coordinates based on which quadrant\n"
"    if (target_quad == 0) {\n"
"        // Top-left quadrant\n"
"        texture_uv = vec2(transformed_uv.x * 2.0, (transformed_uv.y - 0.5) * 2.0);\n"
"        FragColor = texture(u_scene_texture0, texture_uv);\n"
"    } else if (target_quad == 1) {\n"
"        // Top-right quadrant\n"
"        texture_uv = vec2((transformed_uv.x - 0.5) * 2.0, (transformed_uv.y - 0.5) * 2.0);\n"
"        FragColor = texture(u_scene_texture1, texture_uv);\n"
"    } else if (target_quad == 2) {\n"
"        // Bottom-left quadrant\n"
"        texture_uv = vec2(transformed_uv.x * 2.0, transformed_uv.y * 2.0);\n"
"        FragColor = texture(u_scene_texture2, texture_uv);\n"
"    } else {\n"
"        // Bottom-right quadrant\n"
"        texture_uv = vec2((transformed_uv.x - 0.5) * 2.0, transformed_uv.y * 2.0);\n"

"        FragColor = texture(u_scene_texture3, texture_uv);\n"
"    }\n"
"}\n";
*/

static const char *expo_fragment_shader_src =
"#version 300 es\n"
"precision mediump float;\n"
"precision mediump int;\n"
"\n"
"uniform sampler2D u_scene_texture0;   // Desktop 0 (row 0, col 0)\n"
"uniform sampler2D u_scene_texture1;   // Desktop 1 (row 0, col 1)\n"
"uniform sampler2D u_scene_texture2;   // Desktop 2 (row 0, col 2)\n"
"uniform sampler2D u_scene_texture3;   // Desktop 3 (row 0, col 3)\n"
"uniform sampler2D u_scene_texture4;   // Desktop 4 (row 1, col 0)\n"
"uniform sampler2D u_scene_texture5;   // Desktop 5 (row 1, col 1)\n"
"uniform sampler2D u_scene_texture6;   // Desktop 6 (row 1, col 2)\n"
"uniform sampler2D u_scene_texture7;   // Desktop 7 (row 1, col 3)\n"
"uniform sampler2D u_scene_texture8;   // Desktop 8 (row 2, col 0)\n"
"uniform sampler2D u_scene_texture9;   // Desktop 9 (row 2, col 1)\n"
"uniform sampler2D u_scene_texture10;  // Desktop 10 (row 2, col 2)\n"
"uniform sampler2D u_scene_texture11;  // Desktop 11 (row 2, col 3)\n"
"uniform sampler2D u_scene_texture12;  // Desktop 12 (row 3, col 0)\n"
"uniform sampler2D u_scene_texture13;  // Desktop 13 (row 3, col 1)\n"
"uniform sampler2D u_scene_texture14;  // Desktop 14 (row 3, col 2)\n"
"uniform sampler2D u_scene_texture15;  // Desktop 15 (row 3, col 3)\n"
"uniform float u_zoom;                 // 1.0 to 2.0\n"
"uniform float u_move;\n"
"uniform highp int u_quadrant;         // Which desktop to expand (0-15)\n"
"uniform int DesktopGrid;\n"
"uniform int switch_mode;\n"
"uniform int u_switchXY;\n"
"\n"
"in vec2 v_texcoord_to_fs;\n"
"out vec4 FragColor;\n"
"\n"
"vec4 sampleDesktop(int desktop_id, vec2 uv) {\n"
"    if (desktop_id == 0) return texture(u_scene_texture0, uv);\n"
"    else if (desktop_id == 1) return texture(u_scene_texture1, uv);\n"
"    else if (desktop_id == 2) return texture(u_scene_texture2, uv);\n"
"    else if (desktop_id == 3) return texture(u_scene_texture3, uv);\n"
"    else if (desktop_id == 4) return texture(u_scene_texture4, uv);\n"
"    else if (desktop_id == 5) return texture(u_scene_texture5, uv);\n"
"    else if (desktop_id == 6) return texture(u_scene_texture6, uv);\n"
"    else if (desktop_id == 7) return texture(u_scene_texture7, uv);\n"
"    else if (desktop_id == 8) return texture(u_scene_texture8, uv);\n"
"    else if (desktop_id == 9) return texture(u_scene_texture9, uv);\n"
"    else if (desktop_id == 10) return texture(u_scene_texture10, uv);\n"
"    else if (desktop_id == 11) return texture(u_scene_texture11, uv);\n"
"    else if (desktop_id == 12) return texture(u_scene_texture12, uv);\n"
"    else if (desktop_id == 13) return texture(u_scene_texture13, uv);\n"
"    else if (desktop_id == 14) return texture(u_scene_texture14, uv);\n"
"    else return texture(u_scene_texture15, uv);\n"
"}\n"
"\n"
"void main() {\n"
"    if (switch_mode == 0) {\n"
"        // --- ZOOM MODE (4x4 grid) ---\n"
"        float t = clamp(u_zoom - 1.0, 0.0, 1.0) * (float(DesktopGrid) - 1.0);\n"
"        vec2 uv = v_texcoord_to_fs;\n"
"        \n"
"        // Calculate logical row and column of the selected desktop (e.g., top-left is row 0, col 0)\n"
"        int selected_row = u_quadrant / DesktopGrid;\n"
"        int selected_col = u_quadrant % DesktopGrid;\n"
"        \n"
"        // Calculate the zoom center in screen coordinates\n"
"        // We invert the selected_row because screen Y-coordinates are 0 at the bottom.\n"
"        vec2 zoom_center = vec2(\n"
"            float(selected_col) / (float(DesktopGrid) - 1.0), \n"
"            float((DesktopGrid - 1) - selected_row) / float(DesktopGrid - 1)\n"
"        );\n"
"        \n"
"        // Transform UV: zoom in from the selected desktop's center\n"
"        vec2 offset_from_center = uv - zoom_center;\n"
"        vec2 scaled_offset = offset_from_center / (1.0 + t);\n"
"        vec2 transformed_uv = zoom_center + scaled_offset;\n"
"        \n"
"        // Determine which desktop we're sampling from in the 4x4 grid\n"
"        // Invert the row lookup to match the visual layout\n"
"        int target_row = (DesktopGrid - 1) - int(floor(transformed_uv.y * float(DesktopGrid)));\n"
"        int target_col = int(floor(transformed_uv.x * float(DesktopGrid)));\n"
"        \n"
"        // Clamp to valid range\n"
"        target_row = clamp(target_row, 0, 3);\n"
"        target_col = clamp(target_col, 0, 3);\n"
"        \n"
"        int target_desktop = target_row * DesktopGrid + target_col;\n"
"        \n"
"        // Calculate texture coordinates within the desktop\n"
"        vec2 texture_uv = vec2(\n"
"            fract(transformed_uv.x * float(DesktopGrid)),\n"
"            fract(transformed_uv.y * float(DesktopGrid))\n"
"        );\n"
"        \n"
"        vec4 color = sampleDesktop(target_desktop, texture_uv);\n"
"        FragColor = color;\n"
"        \n"
"    } else if (switch_mode == 1) {\n"
"        // --- SLIDE MODE ---\n"
"        float slide_progress = clamp(u_move - 1.0, 0.0, 1.0);\n"
"        vec2 uv = v_texcoord_to_fs;\n"
"        float slide_offset = slide_progress;\n"
"        \n"
"        int current_desktop = u_quadrant;\n"
"        int next_desktop;\n"
"        \n"
"        vec2 current_uv, next_uv;\n"
"\n"
"        if (u_switchXY == 0) {\n"
"            // === HORIZONTAL SLIDE LOGIC (4x4 grid) ===\n"
"            int current_row = current_desktop / 4;\n"
"            int current_col = current_desktop % 4;\n"
"            \n"
"            // Move to next column in same row (wrap around)\n"
"            int next_col = (current_col + 1) % 4;\n"
"            next_desktop = current_row * 4 + next_col;\n"
"\n"
"            // Current desktop slides LEFT, next desktop is revealed from RIGHT\n"
"            current_uv = vec2(uv.x + slide_offset, uv.y);\n"
"            next_uv    = vec2(uv.x + slide_offset - 1.0, uv.y);\n"
"\n"
"        } else {\n"
"            // === VERTICAL SLIDE LOGIC (4x4 grid) ===\n"
"            int current_row = current_desktop / 4;\n"
"            int current_col = current_desktop % 4;\n"
"            \n"
"            // Move to next row in same column (wrap around)\n"
"            int next_row = (current_row + 1) % 4;\n"
"            next_desktop = next_row * 4 + current_col;\n"
"\n"
"            // Current desktop slides UP, next desktop is revealed from BOTTOM\n"
"            current_uv = vec2(uv.x, uv.y + slide_offset);\n"
"            next_uv    = vec2(uv.x, uv.y + slide_offset - 1.0);\n"
"        }\n"
"        \n"
"        vec4 current_color = sampleDesktop(current_desktop, current_uv);\n"
"        vec4 next_color = sampleDesktop(next_desktop, next_uv);\n"
"        \n"
"        // Choose which color to use based on whether the current UV is within bounds\n"
"        vec4 color;\n"
"        if (current_uv.x >= 0.0 && current_uv.x <= 1.0 && current_uv.y >= 0.0 && current_uv.y <= 1.0) {\n"
"             color = current_color;\n"
"        } else {\n"
"             color = next_color;\n"
"        }\n"
"        \n"
"        FragColor = color;\n"
"    }\n"
"}\n";
static const char *cube_vertex_shader_src =
       "#version 300 es\n"
       "precision mediump float;\n"
       "layout(location = 0) in vec3 a_position;    // 3D cube vertices\n"
       "layout(location = 1) in vec2 a_texcoord;\n"
       "layout(location = 2) in float a_face_id;    // Face ID (0-5)\n"
       "\n"
       "uniform float u_zoom;           // Zoom factor (higher is further away)\n"
       "uniform float u_rotation_y;     // The final Y rotation angle from C code\n"
       "uniform float u_time;           // Time uniform for animation (add this to your C code)\n"
       "uniform float u_vertical_offset; // Vertical movement offset\n"
       "uniform float GLOBAL_u_vertical_offset; // Vertical movement offset\n"
       "\n"
       "out vec2 v_texcoord_to_fs;\n"
       "out float v_face_id;\n"
       "\n"
       "mat4 rotationX(float angle) {\n"
       "    float c = cos(angle); float s = sin(angle);\n"
       "    return mat4(1.0, 0.0, 0.0, 0.0, 0.0, c, -s, 0.0, 0.0, s, c, 0.0, 0.0, 0.0, 0.0, 1.0);\n"
       "}\n"
       "\n"
       "mat4 rotationY(float angle) {\n"
       "    float c = cos(angle); float s = sin(angle);\n"
       "    return mat4(c, 0.0, s, 0.0, 0.0, 1.0, 0.0, 0.0, -s, 0.0, c, 0.0, 0.0, 0.0, 0.0, 1.0);\n"
       "}\n"
       "\n"
       "mat4 perspective(float fov, float aspect, float near, float far) {\n"
       "    float f = 1.0 / tan(fov * 0.5);\n"
       "    return mat4(f/aspect, 0.0, 0.0, 0.0, 0.0, f, 0.0, 0.0, 0.0, 0.0, (far+near)/(near-far), -1.0, 0.0, 0.0, (2.0*far*near)/(near-far), 0.0);\n"
       "}\n"
       "\n"
       "void main() {\n"
       "    float zoom_distance = 1.8 - (u_zoom - 1.85) * 1.0;\n"
       "    float slight_x_rotation = 0.0; // ~11 degrees for a better view\n"
       "\n"
       "    mat4 rotX = rotationX(slight_x_rotation);\n"
       "    mat4 rotY = rotationY(u_rotation_y); // Use the angle passed directly from C\n"
       "\n"
       "    vec4 pos = vec4(a_position, 1.0);\n"
       "    \n"
       "    // Scale the geometry to make side faces rectangular while keeping top/bottom square\n"
       "    // Assuming your textures have a 16:9 or similar aspect ratio\n"
       "    float aspect_ratio = 1.0; // Adjust this to match your actual texture aspect ratio\n"
       "    \n"
       "    // Only scale the Y dimension for side faces (faces 0-3)\n"
       "    // Top and bottom faces (4-5) remain square but will be distorted\n"
       "    int face = int(round(a_face_id));\n"
       "    if (face >= 0 && face <= 3) {\n"
       "        // Side faces: scale Y to match texture aspect ratio\n"
       "        pos.y *= aspect_ratio;\n"
       "    }\n"
       "    // Top and bottom faces (4-5) keep original scaling\n"
       "    \n"
       "    pos = rotY * rotX * pos;\n"
       "    pos.z -= zoom_distance;\n"
       "   pos.y += u_vertical_offset;\n"
       "   pos.y += GLOBAL_u_vertical_offset;\n"
       "\n"
       "    mat4 proj = perspective(45.0 * 3.14159 / 180.0, 1.0, 0.1, 10.0);\n"
       "    gl_Position = proj * pos;\n"
       "\n"
       "    v_texcoord_to_fs = a_texcoord;\n"
       "    v_face_id = a_face_id;\n"
       "}\n";

/*
// Your cube fragment shader with multicolored background
static const char *cube_fragment_shader_src =
"#version 300 es\n"
"precision mediump float;\n"
"precision highp int;\n" // Use highp for the int uniform
"\n"
"uniform sampler2D u_scene_texture0;\n"
"uniform sampler2D u_scene_texture1;\n"
"uniform sampler2D u_scene_texture2;\n"
"uniform sampler2D u_scene_texture3;\n"
"uniform float u_zoom;               // For brightness/intensity\n"
"uniform highp int u_quadrant;       // Which quadrant to expand (0-3)\n"
"\n"
"in vec2 v_texcoord_to_fs;\n"
"in float v_face_id; // The ID of the cube face being rendered (0-5)\n"
"out vec4 FragColor;\n"
"\n"
"// Function to calculate rounded corner mask\n"
"float roundedCornerMask(vec2 uv, float radius) {\n"
"    // Convert UV to range [-0.5, 0.5] to center the coordinates\n"
"    vec2 centered_uv = uv - 0.5;\n"
"    \n"
"    // Calculate distance from each corner\n"
"    vec2 corner_dist = abs(centered_uv) - (0.5 - radius);\n"
"    \n"
"    // If we're inside the rounded area, return 1.0 (fully opaque)\n"
"    if (corner_dist.x <= 0.0 && corner_dist.y <= 0.0) {\n"
"        return 1.0;\n"
"    }\n"
"    \n"
"    // If we're in a corner region, calculate circular distance\n"
"    if (corner_dist.x > 0.0 && corner_dist.y > 0.0) {\n"
"        float dist_to_corner = length(corner_dist);\n"
"        return smoothstep(radius + 0.01, radius - 0.01, dist_to_corner);\n"
"    }\n"
"    \n"
"    // If we're in an edge region (not corner), return 1.0\n"
"    return 1.0;\n"
"}\n"
"\n"
"// Function to calculate edge anti-aliasing mask\n"
"float edgeAntiAliasMask(vec2 uv) {\n"
"    // Distance to each edge (0.0 at edge, 1.0 at center)\n"
"    float left_dist = uv.x;\n"
"    float right_dist = 1.0 - uv.x;\n"
"    float top_dist = uv.y;\n"
"    float bottom_dist = 1.0 - uv.y;\n"
"    \n"
"    // Find the minimum distance to any edge\n"
"    float min_edge_dist = min(min(left_dist, right_dist), min(top_dist, bottom_dist));\n"
"    \n"
"    // Apply anti-aliasing at the edges\n"
"    float aa_width = 0.01;\n"
"    return smoothstep(0.0, aa_width, min_edge_dist);\n"
"}\n"
"\n"
"void main() {\n"
"    // The texture coordinates for this fragment on the current face (always 0.0-1.0).\n"
"    vec2 uv = v_texcoord_to_fs;\n"
"    \n"
"    // Convert the incoming float face ID to an integer.\n"
"    int face = int(round(v_face_id)); // Use round() for safety\n"
"    int desktop_id = face % 4; // Which desktop this face represents (0-3)\n"
"    \n"
"    // Apply zoom effect when this face matches the selected quadrant\n"
"    vec2 sampling_uv = uv;\n"
"    if (desktop_id == u_quadrant) {\n"
"        // Show full texture but scaled up - no cropping\n"
"        // This makes the texture appear larger on the face\n"
"        sampling_uv = uv; // Use original UV coordinates to show full texture\n"
"    }\n"
"    \n"
"    vec4 color;\n"
"    \n"
"    // *** THE CORRECT LOGIC ***\n"
"    // Use the face ID to select which single texture to sample from.\n"
"    // We use the modulo operator (%) so that the 4 textures repeat\n"
"    // across the 6 faces of the cube.\n"
"    if (face % 4 == 0) {\n"
"        // Faces 0 and 4 will get Desktop 0\n"
"        color = texture(u_scene_texture0, sampling_uv);\n"
"    } else if (face % 4 == 1) {\n"
"        // Faces 1 and 5 will get Desktop 1\n"
"        color = texture(u_scene_texture2, sampling_uv);\n"
"    } else if (face % 4 == 2) {\n"
"        // Face 2 will get Desktop 2\n"
"        color = texture(u_scene_texture1, sampling_uv);\n"
"    } else { // face % 4 == 3\n"
"        // Face 3 will get Desktop 3\n"
"        color = texture(u_scene_texture3, sampling_uv);\n"
"    }\n"
"    \n"
"    // Step 1: Calculate rounded corner mask\n"
"    float corner_radius = 0.08; // Adjust this value to change corner roundness (0.0 = sharp, 0.5 = circle)\n"
"    float corner_mask = roundedCornerMask(uv, corner_radius);\n"
"    \n"
"    // Step 2: Calculate edge anti-aliasing mask\n"
"    float edge_mask = edgeAntiAliasMask(uv);\n"
"    \n"
"    // Combine both masks: corners handle the rounded parts, edges handle the straight parts\n"
"    float final_mask = min(corner_mask, edge_mask);\n"
"    \n"
"    // Apply combined mask by modifying alpha\n"
"    color.a *= final_mask;\n"
"    \n"
"    // Intensity based on zoom (closer zoom = brighter)\n"
"    float intensity = 1.0; //0.7 + (2.0 - u_zoom) * 0.3;\n"
"    color.rgb *= intensity;\n"
"    \n"
"    FragColor = color;\n"
"}\n";*/
// Your cube fragment shader with multicolored background

// Your cube fragment shader with multicolored background// Your cube fragment shader with multicolored background
static const char *cube_fragment_shader_src =
"#version 300 es\n"
"precision mediump float;\n"
"precision highp int;\n" // Use highp for the int uniform
"\n"
"uniform sampler2D u_scene_texture0;\n"
"uniform sampler2D u_scene_texture1;\n"
"uniform sampler2D u_scene_texture2;\n"
"uniform sampler2D u_scene_texture3;\n"
"uniform float u_zoom;               // For brightness/intensity\n"
"uniform highp int u_quadrant;       // Which quadrant to expand (0-3)\n"
"uniform float u_rotation_y;         // Renamed from u_time for clarity in this context\n"
"uniform vec2 iResolution;           // For the cap shader effect\n"
"\n"
"in vec2 v_texcoord_to_fs;\n"
"in float v_face_id; // The ID of the cube face being rendered (0-5)\n"
"out vec4 FragColor;\n"
"\n"
"// Function to calculate rounded corner mask\n"
"float roundedCornerMask(vec2 uv, float radius) {\n"
"    vec2 centered_uv = uv - 0.5;\n"
"    vec2 corner_dist = abs(centered_uv) - (0.5 - radius);\n"
"    if (corner_dist.x <= 0.0 && corner_dist.y <= 0.0) {\n"
"        return 1.0;\n"
"    }\n"
"    if (corner_dist.x > 0.0 && corner_dist.y > 0.0) {\n"
"        float dist_to_corner = length(corner_dist);\n"
"        return smoothstep(radius + 0.01, radius - 0.01, dist_to_corner);\n"
"    }\n"
"    return 1.0;\n"
"}\n"
"\n"
"// Function to calculate edge anti-aliasing mask\n"
"float edgeAntiAliasMask(vec2 uv) {\n"
"    float min_edge_dist = min(min(uv.x, 1.0 - uv.x), min(uv.y, 1.0 - uv.y));\n"
"    float aa_width = 0.01;\n"
"    return smoothstep(0.0, aa_width, min_edge_dist);\n"
"}\n"
"\n"
"// Function to create a fake bump mapping / bevel effect on the edges.\n"
"float edgeBumpEffect(vec2 uv) {\n"
"    float bevel_width = 0.01;\n"
"    float highlight = 1.3;\n"
"    float shadow = 0.7;\n"
"    float min_dist = min(min(uv.x, 1.0 - uv.x), min(uv.y, 1.0 - uv.y));\n"
"    if (min_dist >= bevel_width) {\n"
"        return 1.0;\n"
"    }\n"
"    float gradient = smoothstep(bevel_width, 0.0, min_dist);\n"
"    float x_effect = mix(highlight, shadow, uv.x);\n"
"    float y_effect = mix(highlight, shadow, uv.y);\n"
"    float light_effect = (x_effect + y_effect) * 0.5;\n"
"    return mix(1.0, light_effect, gradient);\n"
"}\n"
"\n"
"void main() {\n"
"    int face = int(round(v_face_id));\n"
"\n"
"    // --- CUBE CAP LOGIC (Black with Centered Grey Ripple) ---\n"
"    if (face >= 4) { // Faces 4 (bottom) and 5 (top) are the caps\n"
"        vec2 uv = v_texcoord_to_fs - 0.5;\n"
"        float frequency = 80.0;\n"
"        float speed = 1.8;\n"
"        float amplitude = 0.1;\n"
"        float dist = length(uv);\n"
"        \n"
"        // Ripple normal calculation\n"
"        float height = sin(dist * frequency - u_rotation_y * speed) * amplitude;\n"
"        float delta = 0.001;\n"
"        float hX1 = sin(length(uv + vec2(delta, 0.0)) * frequency - u_rotation_y * speed) * amplitude;\n"
"        float hX2 = sin(length(uv - vec2(delta, 0.0)) * frequency - u_rotation_y * speed) * amplitude;\n"
"        float dx = (hX1 - hX2) / (2.0 * delta);\n"
"        float hY1 = sin(length(uv + vec2(0.0, delta)) * frequency - u_rotation_y * speed) * amplitude;\n"
"        float hY2 = sin(length(uv - vec2(0.0, delta)) * frequency - u_rotation_y * speed) * amplitude;\n"
"        float dy = (hY1 - hY2) / (2.0 * delta);\n"
"        vec3 normal = normalize(vec3(-dx, -dy, 1.0));\n"
"        vec3 lightDir = normalize(vec3(0.3, sin(u_rotation_y * 0.2), 0.5));\n"
"        \n"
"        // Ripple color calculation\n"
"        float ripple_brightness = clamp(dot(normal, lightDir) + 0.5, 0.0, 1.0);\n"
"        vec3 final_color = vec3(0.0);\n"
"        float ripple_intensity = 0.35;\n"
"        vec3 grey_ripple_color = vec3(ripple_brightness * ripple_intensity);\n"
"        final_color += grey_ripple_color;\n"
"        \n"
"        // --- FIX: Apply rounded corner mask to the cap color ---\n"
"        float corner_radius = 0.08; // Use the same radius as the sides\n"
"        float corner_mask = roundedCornerMask(v_texcoord_to_fs, corner_radius);\n"
"        float edge_mask = edgeAntiAliasMask(v_texcoord_to_fs);\n"
"        float final_mask = min(corner_mask, edge_mask);\n"
"        \n"
"        FragColor = vec4(final_color, final_mask);\n"
"        return; // Exit early\n"
"    }\n"
"\n"
"    // --- SIDE FACE LOGIC (EXISTING) ---\n"
"    vec2 uv = v_texcoord_to_fs;\n"
"    int desktop_id = face % 4;\n"
"    vec2 sampling_uv = uv;\n"
"    vec4 color;\n"
"\n"
"    if (face % 4 == 0) {\n"
"        color = texture(u_scene_texture0, sampling_uv);\n"
"    } else if (face % 4 == 1) {\n"
"        color = texture(u_scene_texture2, sampling_uv);\n"
"    } else if (face % 4 == 2) {\n"
"        color = texture(u_scene_texture1, sampling_uv);\n"
"    } else { // face % 4 == 3\n"
"        color = texture(u_scene_texture3, sampling_uv);\n"
"    }\n"
"\n"
"    float bump_factor = edgeBumpEffect(uv);\n"
"    float corner_radius = 0.08;\n"
"    float corner_mask = roundedCornerMask(uv, corner_radius);\n"
"    float edge_mask = edgeAntiAliasMask(uv);\n"
"    color.rgb *= bump_factor;\n"
"    float final_mask = min(corner_mask, edge_mask);\n"
"    color.a *= final_mask;\n"
"    float intensity = 1.0;\n"
"    color.rgb *= intensity;\n"
"    FragColor = color;\n"
"}\n";
//https://www.shadertoy.com/view/WcKGWd
static const char *desktop_0_fs_src =
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


// Shader for Desktop 1 (Starfield)
static const char *desktop_1_fs_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "#define A 9.\n"
    "#define T (time/3e2)\n"
    "#define H(a) (cos(radians(vec3(180, 90, 0))+(a)*6.2832)*.5+.5)\n"
    "float hash(vec2 p) { return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453); }\n"
    "mat2 rot(float a) { float s=sin(a), c=cos(a); return mat2(c,-s,s,c); }\n"
    "float map(vec3 u, float v) {\n"
    "    float t = T,\n"
    "          l = 5.,\n"
    "          f = 1e10, i = 0., y, z;\n"
    "    u.xy = vec2(atan(u.x, u.y), length(u.xy));\n"
    "    u.x += t*v*3.1416*.7;\n"
    "    for (; i++<l;) {\n"
    "        vec3 p = u;\n"
    "        y = round((p.y-i)/l)*l+i;\n"
    "        p.x *= y;\n"
    "        p.x -= y*y*t*3.1416;\n"
    "        p.x -= round(p.x/6.2832)*6.2832;\n"
    "        p.y -= y;\n"
    "        z = cos(y*t*6.2832)*.5+.5;\n"
    "        f = min(f, max(length(p.xy), -p.z-z*A) -.1 -z*.2 -p.z/1e2);\n"
    "    }\n"
    "    return f;\n"
    "}\n"
    "void main() {\n"
    "    vec2 R = iResolution.xy, j,\n"
    "         U = gl_FragCoord.xy;\n"
    "    vec2 m = vec2(0, .5);\n"
    "    vec3 o = vec3(0, 0, -130.),\n"
    "         u = normalize(vec3(U - R/2., R.y)),\n"
    "         c = vec3(0),\n"
    "         p, k;\n"
    "    float t = T,\n"
    "          v = -o.z/3.,\n"
    "          i = 0., d = i,\n"
    "          s, f, z, r;\n"
    "    bool b;\n"
    "    for (; i++<70.;) {\n"
    "        p = u*d + o;\n"
    "        p.xy /= v;\n"
    "        r = length(p.xy);\n"
    "        z = abs(1.-r*r);\n"
    "        b = r < 1.;\n"
    "        if (b) z = sqrt(z);\n"
    "        p.xy /= z+1.;\n"
    "        p.xy -= m;\n"
    "        p.xy *= v;\n"
    "        p.xy -= cos(p.z/8. +t*3e2 +vec2(0, 1.5708) +z/2.)*.2;\n"
    "        s = map(p, v);\n"
    "        r = length(p.xy);\n"
    "        f = cos(round(r)*t*6.2832)*.5+.5;\n"
    "        k = H(.2 -f/3. +t +p.z/2e2);\n"
    "        if (b) k = 1.-k;\n"
    "        c += min(exp(s/-.05), s)\n"
    "           * (f+.01)\n"
    "           * min(z, 1.)\n"
    "           * sqrt(cos(r*6.2832)*.5+.5)\n"
    "           * k*k;\n"
    "        if (s < 1e-3 || d > 1e3) break;\n"
    "        d += s*clamp(z, .2, .9);\n"
    "    }\n"
    "    c += min(exp(-p.z-f*A)*z*k*.01/s, 1.);\n"
    "    j = p.xy/v+m;\n"
    "    c /= clamp(dot(j, j)*4., .04, 4.);\n"
    "    frag_color = vec4(exp(log(c * u_color.rgb)/2.2), 1.0).bgra;\n"
    "}\n";

// Shader for Desktop 2 (Tunnel)
static const char *desktop_2_fs_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "const float PI = 3.14159265;\n"
    "vec3 cmap(float x) { return pow(.5+.5*cos(PI * x + vec3(1,2,3)), vec3(2.5)); }\n"
    "void main() {\n"
    "    vec2 fragCoord = gl_FragCoord.xy;\n"
    "    vec2 uv = (2. * fragCoord-iResolution.xy)/iResolution.y;\n"
    "    float focal = 2.25;\n"
    "    vec3 ro = vec3(0,0,time);\n"
    "    vec3 rd = normalize(vec3(uv, -focal));\n"
    "    vec3 color = vec3(0);\n"
    "    float t = 0.;\n"
    "    for(int i = 0; i < 99; i++) {\n"
    "        vec3 p = t * rd + ro;\n"
    "        float T = (t+time)/5.;\n"
    "        float c = cos(T), s = sin(T);\n"
    "        p.xy = mat2(c,-s,s,c) * p.xy;\n"
    "        for(float f = 0.; f < 9.; f++) {\n"
    "            float a = exp(f)/exp2(f);\n"
    "            p += cos(p.yzx * a + time)/a;\n"
    "        }\n"
    "        float d = 1./50. + abs((ro -p-vec3(0,1,0)).y-1.)/10.;\n"
    "        color += cmap(t) * 2e-3 / d;\n"
    "        t += d;\n"
    "    }\n"
    "    color *= color*color;\n"
    "    color = 1.-exp(-color);\n"
    "    color = pow(color, vec3(1./2.2));\n"
    "    frag_color = vec4(color * u_color.rgb, 1.0).bgra;\n"
    "}\n";

// Shader for Desktop 3 (Simple Noise)
static const char *desktop_3_fs_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "void main() {\n"
    "    vec2 I = gl_FragCoord.xy;\n"
    "    vec4 O = vec4(0.0);\n"
    "    float t = time,\n"
    "          i = 0.0,\n"
    "          z = 0.0,\n"
    "          d,\n"
    "          s;\n"
    "    for(O*=i; i++<1e2; ) {\n"
    "        vec3 p = z * normalize( vec3(I+I,0) - iResolution.xyy );\n"
    "        for(d=5.; d<2e2; d+=d)\n"
    "            p += .6*sin(p.yzx*d - .2*t) / d;\n"
    "        z += d = .005 + max(s=.3-abs(p.y), -s*.2)/4.;\n"
    "        O += (cos(s/.07+p.x+.5*t-vec4(3,4,5,0)) + 1.5) * exp(s/.1) / d;\n"
    "    }\n"
    "    O = tanh(O*O / 4e8);\n"
    "    frag_color = vec4(O.rgb * u_color.rgb, 1.0).bgra;\n"
    "}\n";
//https://www.shadertoy.com/view/3f3SWf
static const char *desktop_4_fs_src = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "void main() {\n"
    "    vec2 C = gl_FragCoord.xy;\n"
    "    vec4 O = vec4(0.0);\n"
    "    float i = 0.0,\n"
    "          d,\n"
    "          t = 0.2 * time,\n"
    "          z = 0.1 * fract(dot(C, sin(C)));\n"
    "    vec4 p, P;\n"
    "    \n"
    "    for (vec2 r = iResolution.xy; ++i < 77.0; z += 0.2 * d + 1e-3) {\n"
    "        p = vec4(z * normalize(vec3(C - 0.5 * r, r.y)), 0.0);\n"
    "        p.xy += 6.0;\n"
    "        p.z += t;\n"
    "        \n"
    "        d = 4.0;\n"
    "        \n"
    "        for (P = p; d < 7.0; d /= 0.8)\n"
    "            p += cos(p.zxyw * d + 0.6 * t) / d;\n"
    "        \n"
    "        P = 1.0 + sin(vec4(0, 1, 2, 0) + 9.0 * length(P - p));\n"
    "        \n"
    "        p -= round(p);\n"
    "        d = abs(min(length(p.yz), min(length(p.xy), length(p.xz))) - 0.1 * tanh(z) + 2e-2);\n"
    "        \n"
    "        O += P.w / max(d, 1e-3) * P;\n"
    "    }\n"
    "    \n"
    "    O = tanh(O / 2e4);\n"
    "    frag_color = vec4(O.rgb * u_color.rgb, 1.0).bgra;\n"
    "}\n";  
    
//https://www.shadertoy.com/view/tf3SWB    
static const char *desktop_5_fs_src = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "void main() {\n"
    "    vec2 C = gl_FragCoord.xy;\n"
    "    vec4 O = vec4(0.0);\n"
    "    float d, s, j, i = 0.0, z = 0.0, N, D, k, t = time;\n"
    "    vec4 o = vec4(0.0), p, U = vec4(3, 1, 2, 0);\n"
    "    \n"
    "    for (vec2 q, r = iResolution.xy; ++i < 70.0; z += 0.5 * d + 1E-3) {\n"
    "        p = vec4(z * normalize(vec3(C - 0.5 * r, r.y)), 0.0);\n"
    "        p.z -= 3.0;\n"
    "        \n"
    "        // Rotation matrix multiplication: p.xz *= mat2(.8,.6,-.8,.6)\n"
    "        vec2 temp = p.xz;\n"
    "        p.x = temp.x * 0.8 + temp.y * 0.6;\n"
    "        p.z = temp.x * (-0.8) + temp.y * 0.6;\n"
    "        \n"
    "        p *= k = 8.0 / dot(p, p);\n"
    "        q = p.xy;\n"
    "        q -= round(q / 5.0) * 5.0;\n"
    "        \n"
    "        // Create rotation matrix R\n"
    "        float angle = 0.5 * t + log2(k) + 11.0 * U.w;\n"
    "        float cos_a = cos(angle);\n"
    "        float sin_a = sin(angle);\n"
    "        mat2 R = mat2(cos_a, sin_a, -sin_a, cos_a);\n"
    "        \n"
    "        d = 1.0;\n"
    "        s = 1.0;\n"
    "        j = 1.0;\n"
    "        \n"
    "        for (; ++j < 6.0; s *= 0.5) {\n"
    "            q = abs(q * R) - 2.0 * s / j;\n"
    "            D = length(q) - s / 8.0;\n"
    "            if (D < d) {\n"
    "                N = j;\n"
    "                d = D;\n"
    "            }\n"
    "        }\n"
    "        \n"
    "        d = abs(d) / k;\n"
    "        p = 1.0 + sin(p.z + U.zywz - t + N);\n"
    "        \n"
    "        o += p.w / max(d, 1E-3) * p + exp(0.3 * k) * 6.0 * U;\n"
    "    }\n"
    "    \n"
    "    O = tanh(o / 3e4);\n"
    "    frag_color = vec4(O.rgb * u_color.rgb, 1.0).bgra;\n"
    "}\n"; 

// https://www.shadertoy.com/view/w32XDc
static const char *desktop_6_fs_src = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "\n"
    "/*\n"
    "    See here for how to do a tiny raymarch loop and translucency:\n"
    "        https://www.shadertoy.com/view/wXjSRt\n"
    "        https://www.shadertoy.com/view/wXSXzV\n"
    "        \n"
    "    See here for how to do a basic tunnel:\n"
    "        https://www.shadertoy.com/view/Wcf3D7\n"
    "        \n"
    "    Converted to OpenGL ES 3.0\n"
    "*/\n"
    "\n"
    "#define iTime time\n"
    "#define P(z) vec3(cos(vec2(.15,.2)*(z))*5.,z)\n"
    "\n"
    "void main() {\n"
    "    vec2 u = gl_FragCoord.xy;\n"
    "    vec4 o = vec4(0.0);\n"
    "    float i = 0.0, d = 0.0, s = 0.0, n = 0.0, t = iTime * 3.0;\n"
    "    vec3 q = vec3(iResolution, 0.0);\n"
    "    vec3 p = P(t);\n"
    "    vec3 Z = normalize(P(t + 1.0) - p);\n"
    "    vec3 X = normalize(vec3(Z.z, 0.0, -Z.x));\n"
    "    vec3 D = vec3((u - q.xy / 2.0) / q.y, 1.0) * mat3(-X, cross(X, Z), Z);\n"
    "    \n"
    "    for(o *= i; i < 100.0; i++) {\n"
    "        p += D * s;\n"
    "        q = P(p.z) + cos(t + p.yzx) * 0.3;\n"
    "        s = 2.0 - min(length((p - q).xy),\n"
    "                 min(length(p.xy - vec2(q.y)),\n"
    "                     length(p.xy - vec2(q.x))));\n"
    "        \n"
    "        for (n = 0.1; n < 1.0; n += n) {\n"
    "            s -= abs(dot(sin(p * n * 16.0), vec3(0.03))) / n;\n"
    "        }\n"
    "        \n"
    "        d += s = 0.04 + abs(s) * 0.2;\n"
    "        o += (1.0 + cos(d + vec4(4.0, 2.0, 1.0, 0.0))) / s / d;\n"
    "    }\n"
    "    \n"
    "    o = tanh(o / 200.0);\n"
    "    frag_color = vec4(o.rgb * u_color.rgb, 1.0);\n"
    "}\n";
    
    
// https://www.shadertoy.com/view/XcS3zK
static const char *desktop_7_fs_src = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "\n"
    "/* Shading constants */\n"
    "const vec3 LP = vec3(-0.6, 0.7, -0.3);\n"
    "const vec3 LC = vec3(0.85, 0.80, 0.70);\n"
    "const vec3 HC1 = vec3(0.5, 0.4, 0.3);\n"
    "const vec3 HC2 = vec3(0.1, 0.1, 0.6) * 0.5;\n"
    "const vec3 HLD = vec3(0.0, 1.0, 0.0);\n"
    "const vec3 BC = vec3(0.25, 0.25, 0.25);\n"
    "const vec3 FC = vec3(1.30, 1.20, 1.00);\n"
    "const float AS = 0.5;\n"
    "const float DS = 1.0;\n"
    "const float BS = 0.3;\n"
    "const float FS = 0.3;\n"
    "\n"
    "/* Raymarching constants */\n"
    "const float MAX_TRACE_DISTANCE = 10.0;\n"
    "const float INTERSECTION_PRECISION = 0.0001;\n"
    "const int NUM_OF_TRACE_STEPS = 64;\n"
    "const float STEP_MULTIPLIER = 1.0;\n"
    "\n"
    "/* Structures */\n"
    "struct Camera {\n"
    "    vec3 ro;\n"
    "    vec3 rd;\n"
    "    vec3 forward;\n"
    "    vec3 right;\n"
    "    vec3 up;\n"
    "    float FOV;\n"
    "};\n"
    "\n"
    "struct Surface {\n"
    "    float len;\n"
    "    vec3 position;\n"
    "    vec3 colour;\n"
    "    float id;\n"
    "    float steps;\n"
    "    float AO;\n"
    "};\n"
    "\n"
    "struct Model {\n"
    "    float dist;\n"
    "    vec3 colour;\n"
    "    float id;\n"
    "};\n"
    "\n"
    "/* Utilities */\n"
    "vec2 toScreenspace(in vec2 p) {\n"
    "    vec2 uv = (p - 0.5 * iResolution.xy) / min(iResolution.y, iResolution.x);\n"
    "    return uv;\n"
    "}\n"
    "\n"
    "mat2 R(float a) {\n"
    "    float c = cos(a);\n"
    "    float s = sin(a);\n"
    "    return mat2(c, -s, s, c);\n"
    "}\n"
    "\n"
    "Camera getCamera(in vec2 uv, in vec3 pos, in vec3 target) {\n"
    "    vec3 f = normalize(target - pos);\n"
    "    vec3 r = normalize(vec3(f.z, 0.0, -f.x));\n"
    "    vec3 u = normalize(cross(f, r));\n"
    "    \n"
    "    float FOV = 1.0 + cos(time * 0.1) * 0.8;\n"
    "    \n"
    "    return Camera(pos, normalize(f + FOV * uv.x * r + FOV * uv.y * u), f, r, u, FOV);\n"
    "}\n"
    "\n"
    "/* Modelling */\n"
    "float G(vec3 p) {\n"
    "    return dot(sin(p.yzx), cos(p.zxy));\n"
    "}\n"
    "\n"
    "Model model(vec3 p) {\n"
    "    float t = time * 0.1;\n"
    "    p.xz *= R(t);\n"
    "    p.xy *= R(0.3);\n"
    "    p.xy -= 0.5;\n"
    "    float d = abs(-(length(vec2(p.y, length(p.xz) - 2.0)) - 1.8 + cos(t) * 0.3));\n"
    "    \n"
    "    float g = G(p.yxz * 4.0) / 4.0;\n"
    "    \n"
    "    d = length(vec2(d, g)) - 0.3;\n"
    "    vec3 colour = vec3(g);\n"
    "    \n"
    "    return Model(d, colour, 1.0);\n"
    "}\n"
    "\n"
    "Model map(vec3 p) {\n"
    "    return model(p);\n"
    "}\n"
    "\n"
    "vec3 calcNormal(in vec3 pos) {\n"
    "    vec3 eps = vec3(0.001, 0.0, 0.0);\n"
    "    vec3 nor = vec3(\n"
    "        map(pos + eps.xyy).dist - map(pos - eps.xyy).dist,\n"
    "        map(pos + eps.yxy).dist - map(pos - eps.yxy).dist,\n"
    "        map(pos + eps.yyx).dist - map(pos - eps.yyx).dist\n"
    "    );\n"
    "    return normalize(nor);\n"
    "}\n"
    "\n"
    "/* Raymarcher */\n"
    "Surface march(in Camera cam) {\n"
    "    float h = 1e4;\n"
    "    float d = 0.0;\n"
    "    float id = -1.0;\n"
    "    float s = 0.0;\n"
    "    float ao = 0.0;\n"
    "    vec3 p;\n"
    "    vec3 c;\n"
    "\n"
    "    for (int i = 0; i < NUM_OF_TRACE_STEPS; i++) {\n"
    "        if (abs(h) < INTERSECTION_PRECISION || d > MAX_TRACE_DISTANCE) break;\n"
    "        p = cam.ro + cam.rd * d;\n"
    "        Model m = map(p);\n"
    "        h = m.dist;\n"
    "        d += h * STEP_MULTIPLIER;\n"
    "        id = m.id;\n"
    "        s += 1.0;\n"
    "        ao += max(h, 0.0);\n"
    "        c = m.colour;\n"
    "    }\n"
    "\n"
    "    if (d >= MAX_TRACE_DISTANCE) id = -1.0;\n"
    "\n"
    "    return Surface(d, p, c, id, s, ao);\n"
    "}\n"
    "\n"
    "/* Shading */\n"
    "float softshadow(in vec3 ro, in vec3 rd, in float mint, in float tmax) {\n"
    "    float res = 1.0;\n"
    "    float t = mint;\n"
    "    for (int i = 0; i < 16; i++) {\n"
    "        float h = map(ro + rd * t).dist;\n"
    "        res = min(res, 8.0 * h / t);\n"
    "        t += clamp(h, 0.02, 0.10);\n"
    "        if (h < 0.001 || t > tmax) break;\n"
    "    }\n"
    "    return clamp(res, 0.0, 1.0);\n"
    "}\n"
    "\n"
    "float AO(in vec3 pos, in vec3 nor) {\n"
    "    float occ = 0.0;\n"
    "    float sca = 1.0;\n"
    "    for (int i = 0; i < 5; i++) {\n"
    "        float hr = 0.01 + 0.12 * float(i) / 4.0;\n"
    "        vec3 aopos = nor * hr + pos;\n"
    "        float dd = map(aopos).dist;\n"
    "        occ += -(dd - hr) * sca;\n"
    "        sca *= 0.95;\n"
    "    }\n"
    "    return clamp(1.0 - 3.0 * occ, 0.0, 1.0);\n"
    "}\n"
    "\n"
    "vec3 shade(vec3 col, vec3 pos, vec3 nor, vec3 ref, Camera cam) {\n"
    "    vec3 plp = LP - pos;\n"
    "    \n"
    "    float o = AO(pos, nor);\n"
    "    vec3 l = normalize(plp);\n"
    "    \n"
    "    float d = clamp(dot(nor, l), 0.0, 1.0) * DS;\n"
    "    float b = clamp(dot(nor, normalize(vec3(-l.x, 0.0, -l.z))), 0.0, 1.0) * clamp(1.0 - pos.y, 0.0, 1.0) * BS;\n"
    "    float f = pow(clamp(1.0 + dot(nor, cam.rd), 0.0, 1.0), 2.0) * FS;\n"
    "\n"
    "    vec3 c = vec3(0.0);\n"
    "    c += d * LC;\n"
    "    c += mix(HC1, HC2, dot(nor, HLD)) * AS;\n"
    "    c += b * BC * o;\n"
    "    c += f * FC * o;\n"
    "    \n"
    "    return col * c;\n"
    "}\n"
    "\n"
    "vec3 render(Surface surface, Camera cam, vec2 uv) {\n"
    "    vec3 colour = vec3(0.04, 0.045, 0.05);\n"
    "    colour = vec3(0.35, 0.5, 0.75);\n"
    "    vec3 colourB = vec3(0.9, 0.85, 0.8);\n"
    "    \n"
    "    colour = mix(colourB, colour, pow(length(uv), 2.0) / 1.5);\n"
    "\n"
    "    if (surface.id > -1.0) {\n"
    "        vec3 surfaceNormal = calcNormal(surface.position);\n"
    "        vec3 ref = reflect(cam.rd, surfaceNormal);\n"
    "        colour = surfaceNormal;\n"
    "        vec3 pos = surface.position;\n"
    "        \n"
    "        float t = time;\n"
    "        vec3 col = mix(\n"
    "            mix(\n"
    "                vec3(0.8, 0.3, 0.6),\n" 
    "                vec3(0.6, 0.3, 0.8),\n"
    "                dot(surfaceNormal, surfaceNormal.zxy)\n"
    "            ),\n"
    "            vec3(1.0),\n"
    "            smoothstep(0.0, 0.1, cos(surface.colour.r * 40.0))\n"
    "        );\n"
    "        \n"
    "        colour = shade(col, pos, surfaceNormal, ref, cam);\n"
    "    }\n"
    "\n"
    "    return colour;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    vec3 c = vec3(0.0);\n"
    "    for (int x = 0; x < 2; x++) {\n"
    "        for (int y = 0; y < 2; y++) {\n"
    "            vec2 uv = toScreenspace(gl_FragCoord.xy + vec2(float(x), float(y)) * 0.5);\n"
    "\n"
    "            Camera cam = getCamera(uv, vec3(1.5, 0.0, 1.5), vec3(0.0));\n"
    "            Surface surface = march(cam);\n"
    "\n"
    "            c += render(surface, cam, uv);\n"
    "        }\n"
    "    }\n"
    "    \n"
    "    vec4 final_color = vec4(c * 0.25, 1.0);\n"
    "    frag_color = vec4(final_color.rgb * u_color.rgb, 1.0).bgra;\n"
    "}\n"; 
    
    
// https://www.shadertoy.com/view/Wc3SDX
static const char *desktop_8_fs_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "void main() {\n"
    "    vec2 C = gl_FragCoord.xy;\n"
    "    vec4 O = vec4(0.0);\n"
    "    float i = 0.0, d, z = 0.0, k, c, t = time;\n"
    "    vec4 o = vec4(0.0), p;\n"
    "    \n"
    "    // Create rotation matrix\n"
    "    mat2 R = mat2(cos(0.2*t), -sin(0.2*t), sin(0.2*t), cos(0.2*t));\n"
    "    \n"
    "    for(; i < 80.0; i += 1.0) {\n"
    "        z += 0.7*d + 1e-3;\n"
    "        \n"
    "        // Ray setup\n"
    "        p = vec4(z*normalize(vec3(C+C,0.0)-iResolution.xyy), 0.0);\n"
    "        \n"
    "        // Transform the 3D point\n"
    "        p.z += 3.5;\n"
    "        \n"
    "        // Apply 4D rotation using 2D rotation matrices\n"
    "        vec2 temp_xw = vec2(p.x, p.w);\n"
    "        temp_xw = R * temp_xw;\n"
    "        p.x = temp_xw.x; p.w = temp_xw.y;\n"
    "        \n"
    "        vec2 temp_wy = vec2(p.w, p.y);\n"
    "        temp_wy = R * temp_wy;\n"
    "        p.w = temp_wy.x; p.y = temp_wy.y;\n"
    "        \n"
    "        vec2 temp_zw = vec2(p.z, p.w);\n"
    "        temp_zw = R * temp_zw;\n"
    "        p.z = temp_zw.x; p.w = temp_zw.y;\n"
    "        \n"
    "        // Distance to bounding sphere\n"
    "        c = length(p) - 3.0;\n"
    "        \n"
    "        // Inversion\n"
    "        k = 9.0 / dot(p,p);\n"
    "        p *= k;\n"
    "        \n"
    "        // Create repeating pattern\n"
    "        p += 0.5*t;\n"
    "        p -= floor(p + 0.5);\n"
    "        \n"
    "        // Create rounded hypercube shapes\n"
    "        p *= p;  // p²\n"
    "        p *= p;  // p⁴\n"
    "        \n"
    "        // Distance estimation\n"
    "        d = max(abs(pow(dot(p,p), 0.125) - 0.4) / k, c);\n"
    "        \n"
    "        // Lighting/coloring\n"
    "        vec4 color_mult = 1.0 + sin(vec4(5,6,7,5) + 2.0*log2(k));\n"
    "        o += color_mult.w / max(d, 1e-3) * color_mult;\n"
    "    }\n"
    "    \n"
    "    // Tone mapping\n"
    "    O = tanh(o / 2e4);\n"
    "    frag_color = vec4(O.rgb * u_color.rgb, 1.0).bgra;\n"
    "}\n";
    
// https://www.shadertoy.com/view/tcdXzf
static const char *desktop_9_fs_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "void main() {\n"
    "    vec2 C = gl_FragCoord.xy;\n"
    "    vec4 O = vec4(0.0);\n"
    "    float i = 0.0, d, z = 0.0;\n"
    "    vec4 o = vec4(0.0), p, P, D;\n"
    "    vec2 r = iResolution.xy;\n"
    "    \n"
    "    for(; i < 77.0; i += 1.0) {\n"
    "        z += 0.7*d + 1e-3;\n"
    "        \n"
    "        // Create ray from camera through current pixel\n"
    "        p = vec4(z * normalize(vec3(C - 0.5*r, r.y)), 0.0);\n"
    "        \n"
    "        // Animate scene by moving along Z axis over time\n"
    "        p.z += 0.5*time;\n"
    "        \n"
    "        // 2D rotation matrix to twist the space\n"
    "        float angle = 0.3*p.z;\n"
    "        mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));\n"
    "        p.xy = rot * p.xy;\n"
    "        \n"
    "        // Center the coordinate system\n"
    "        p.xy -= 0.5;\n"
    "        \n"
    "        // Save original position for lighting calculations\n"
    "        P = p;\n"
    "        \n"
    "        // Create infinite repetition\n"
    "        p = abs(p - floor(p + 0.5));\n"
    "        \n"
    "        // Scale position for surface detail distortion\n"
    "        D = 25.0 * p;\n"
    "        \n"
    "        // Calculate distance to nearest surface using two shapes:\n"
    "        float shape1 = min(p.x, p.y) + 4e-3 + 0.05*sin(D.x)*sin(D.y)*sin(D.z);\n"
    "        float shape2 = min(length(p.zy), min(length(p.xy), length(p.xz))) - 0.1;\n"
    "        d = abs(min(shape1, shape2));\n"
    "        \n"
    "        // Calculate color based on original position\n"
    "        vec4 color_base = 1.0 + sin(0.5*P.z + 5.0*P.y + vec4(0,1,2,0));\n"
    "        \n"
    "        // Accumulate color\n"
    "        o += color_base.w * color_base / max(d, 1e-3);\n"
    "    }\n"
    "    \n"
    "    // Tone mapping\n"
    "    O = tanh(o / 2e4);\n"
    "    frag_color = vec4(O.rgb * u_color.rgb, 1.0).bgra;\n"
    "}\n";
    
    
// https://www.shadertoy.com/view/MfjyWK
static const char *desktop_10_fs_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "\n"
    "const float pi = 3.14159265359;\n"
    "const float tau = 6.28318530718;\n"
    "const float planeDist = 0.5;\n"
    "const float furthest = 16.0;\n"
    "const float fadeFrom = 8.0;\n"
    "const vec2 pathA = vec2(0.31, 0.41);\n"
    "const vec2 pathB = vec2(1.0, 0.7071067812);\n"
    "const vec4 U = vec4(0, 1, 2, 3);\n"
    "\n"
    "mat2 ROT(float a) {\n"
    "    return mat2(cos(a), sin(a), -sin(a), cos(a));\n"
    "}\n"
    "\n"
    "vec3 aces_approx(vec3 v) {\n"
    "    v = max(v, 0.0);\n"
    "    v *= 0.6;\n"
    "    float a = 2.51;\n"
    "    float b = 0.03;\n"
    "    float c = 2.43;\n"
    "    float d = 0.59;\n"
    "    float e = 0.14;\n"
    "    return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0, 1.0);\n"
    "}\n"
    "\n"
    "vec3 offset(float z) {\n"
    "    return vec3(pathB*sin(pathA*z), z);\n"
    "}\n"
    "\n"
    "vec3 doffset(float z) {\n"
    "    return vec3(pathA*pathB*cos(pathA*z), 1.0);\n"
    "}\n"
    "\n"
    "vec3 ddoffset(float z) {\n"
    "    return vec3(-pathA*pathA*pathB*sin(pathA*z), 0.0);\n"
    "}\n"
    "\n"
    "vec4 alphaBlend(vec4 back, vec4 front) {\n"
    "    float w = front.w + back.w*(1.0-front.w);\n"
    "    vec3 xyz = (front.xyz*front.w + back.xyz*back.w*(1.0-front.w))/w;\n"
    "    return w > 0.0 ? vec4(xyz, w) : vec4(0.0);\n"
    "}\n"
    "\n"
    "float pmin(float a, float b, float k) {\n"
    "    float h = clamp(0.5+0.5*(b-a)/k, 0.0, 1.0);\n"
    "    return mix(b, a, h) - k*h*(1.0-h);\n"
    "}\n"
    "\n"
    "float pmax(float a, float b, float k) {\n"
    "    return -pmin(-a, -b, k);\n"
    "}\n"
    "\n"
    "float pabs(float a, float k) {\n"
    "    return -pmin(a, -a, k);\n"
    "}\n"
    "\n"
    "float star5(vec2 p, float r, float rf, float sm) {\n"
    "    p = -p;\n"
    "    const vec2 k1 = vec2(0.809016994375, -0.587785252292);\n"
    "    const vec2 k2 = vec2(-k1.x, k1.y);\n"
    "    p.x = abs(p.x);\n"
    "    p -= 2.0*max(dot(k1,p),0.0)*k1;\n"
    "    p -= 2.0*max(dot(k2,p),0.0)*k2;\n"
    "    p.x = pabs(p.x, sm);\n"
    "    p.y -= r;\n"
    "    vec2 ba = rf*vec2(-k1.y,k1.x) - vec2(0,1);\n"
    "    float h = clamp( dot(p,ba)/dot(ba,ba), 0.0, r );\n"
    "    return length(p-ba*h) * sign(p.y*ba.x-p.x*ba.y);\n"
    "}\n"
    "\n"
    "vec3 palette(float n) {\n"
    "    return 0.5+0.5*sin(vec3(0.,1.,2.)+n);\n"
    "}\n"
    "\n"
    "vec4 plane(vec3 ro, vec3 rd, vec3 pp, vec3 npp, float pd, vec3 cp, vec3 off, float n) {\n"
    "    float aa = 3.0*pd*distance(pp.xy, npp.xy);\n"
    "    vec4 col = vec4(0.0);\n"
    "    vec2 p2 = pp.xy;\n"
    "    p2 -= offset(pp.z).xy;\n"
    "    vec2 doff = ddoffset(pp.z).xz;\n"
    "    vec2 ddoff = doffset(pp.z).xz;\n"
    "    float dd = dot(doff, ddoff);\n"
    "    p2 = ROT(dd*pi*5.0) * p2;\n"
    "\n"
    "    float d0 = star5(p2, 0.45, 1.6, 0.2) - 0.02;\n"
    "    float d1 = d0 - 0.01;\n"
    "    float d2 = length(p2);\n"
    "    const float colp = pi*100.0;\n"
    "    float colaa = aa*200.0;\n"
    "    \n"
    "    col.xyz = palette(0.5*n+2.0*d2)*mix(0.5/(d2*d2), 1.0, smoothstep(-0.5+colaa, 0.5+colaa, sin(d2*colp)))/max(3.0*d2*d2, 1e-1);\n"
    "    col.xyz = mix(col.xyz, vec3(2.0), smoothstep(aa, -aa, d1));\n"
    "    col.w = smoothstep(aa, -aa, -d0);\n"
    "    return col;\n"
    "}\n"
    "\n"
    "vec3 color(vec3 ww, vec3 uu, vec3 vv, vec3 ro, vec2 p) {\n"
    "    float lp = length(p);\n"
    "    vec2 np = p + 1.0/iResolution.xy;\n"
    "    float rdd = 2.0-0.25;\n"
    "    \n"
    "    vec3 rd = normalize(p.x*uu + p.y*vv + rdd*ww);\n"
    "    vec3 nrd = normalize(np.x*uu + np.y*vv + rdd*ww);\n"
    "\n"
    "    float nz = floor(ro.z / planeDist);\n"
    "\n"
    "    vec4 acol = vec4(0.0);\n"
    "    vec3 aro = ro;\n"
    "    float apd = 0.0;\n"
    "\n"
    "    for (float i = 1.0; i <= furthest; i += 1.0) {\n"
    "        if (acol.w > 0.95) {\n"
    "            break;\n"
    "        }\n"
    "        float pz = planeDist*nz + planeDist*i;\n"
    "\n"
    "        float lpd = (pz - aro.z)/rd.z;\n"
    "        float npd = (pz - aro.z)/nrd.z;\n"
    "        float cpd = (pz - aro.z)/ww.z;\n"
    "\n"
    "        vec3 pp = aro + rd*lpd;\n"
    "        vec3 npp = aro + nrd*npd;\n"
    "        vec3 cp = aro+ww*cpd;\n"
    "\n"
    "        apd += lpd;\n"
    "\n"
    "        vec3 off = offset(pp.z);\n"
    "\n"
    "        float dz = pp.z-ro.z;\n"
    "        float fadeIn = smoothstep(planeDist*furthest, planeDist*fadeFrom, dz);\n"
    "        float fadeOut = smoothstep(0.0, planeDist*0.1, dz);\n"
    "        float fadeOutRI = smoothstep(0.0, planeDist*1.0, dz);\n"
    "\n"
    "        float ri = mix(1.0, 0.9, fadeOutRI*fadeIn);\n"
    "\n"
    "        vec4 pcol = plane(ro, rd, pp, npp, apd, cp, off, nz+i);\n"
    "\n"
    "        pcol.w *= fadeOut*fadeIn;\n"
    "        acol = alphaBlend(pcol, acol);\n"
    "        aro = pp;\n"
    "    }\n"
    "\n"
    "    return acol.xyz*acol.w;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    vec2 r = iResolution.xy;\n"
    "    vec2 fragCoord = gl_FragCoord.xy;\n"
    "    vec2 q = fragCoord/r;\n"
    "    vec2 pp = -1.0+2.0*q;\n"
    "    vec2 p = pp;\n"
    "    p.x *= r.x/r.y;\n"
    "\n"
    "    float tm = planeDist*time;\n"
    "\n"
    "    vec3 ro = offset(tm);\n"
    "    vec3 dro = doffset(tm);\n"
    "    vec3 ddro = ddoffset(tm);\n"
    "\n"
    "    vec3 ww = normalize(dro);\n"
    "    vec3 uu = normalize(cross(U.xyx+ddro, ww));\n"
    "    vec3 vv = cross(ww, uu);\n"
    "    \n"
    "    vec3 col = color(ww, uu, vv, ro, p);\n"
    "    col = aces_approx(col);\n"
    "    col = sqrt(col);\n"
    "    frag_color = vec4(col * u_color.rgb, 1.0).bgra;\n"
    "}\n";
    
    
// https://www.shadertoy.com/view/wXSXzV
static const char *desktop_11_fs_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "void main() {\n"
    "    vec2 I = gl_FragCoord.xy;\n"
    "    vec4 O = vec4(0.0);\n"
    "    float t = time, i = 0.0, z = 0.0, d, s;\n"
    "    \n"
    "    for(; i < 1e2; i += 1.0) {\n"
    "        vec3 p = z * normalize(vec3(I+I, 0.0) - iResolution.xyy);\n"
    "        p.z -= t;\n"
    "        \n"
    "        for(d = 1.0; d < 64.0; d += d) {\n"
    "            p += 0.7 * cos(p.yzx * d) / d;\n"
    "        }\n"
    "        \n"
    "        float angle = z * 0.2;\n"
    "        mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));\n"
    "        p.xy = rot * p.xy;\n"
    "        \n"
    "        s = 3.0 - abs(p.x);\n"
    "        d = 0.03 + 0.1 * max(s, -s * 0.2);\n"
    "        z += d;\n"
    "        \n"
    "        vec4 color_contrib = (cos(s + s - vec4(5,0,1,3)) + 1.4) / d / z;\n"
    "        O += color_contrib;\n"
    "    }\n"
    "    \n"
    "    O = tanh(O * O / 4e5);\n"
    "    frag_color = vec4(O.rgb * u_color.rgb, 1.0).bgra;\n"
    "}\n"; 

// https://www.shadertoy.com/view/3fK3D1
static const char *desktop_12_fs_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "void main() {\n"
    "    vec2 u = gl_FragCoord.xy;\n"
    "    vec4 o = vec4(0.0);\n"
    "    vec3 p = vec3(iResolution, 0.0);\n"
    "    u = (u - p.xy/2.0) / p.y * 0.6;\n"
    "    \n"
    "    float i = 0.0, d = 0.0, s, n;\n"
    "    float t = time * 0.05 + 2.0;\n"
    "    float a = atan(u.x, u.y);\n"
    "    \n"
    "    for(; i < 1e2; i += 1.0) {\n"
    "        p = vec3(u * d, d + t * 4.0);\n"
    "        p += cos(p.z + t + p.yzx * 0.5) * 0.5;\n"
    "        s = 5.0 - length(p.xy);\n"
    "        \n"
    "        for(n = 0.06; n < 2.0; n += n) {\n"
    "            float angle = t * 0.1;\n"
    "            mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));\n"
    "            p.xy = rot * p.xy;\n"
    "            s -= abs(dot(sin(p.z + t + p * n * 20.0), vec3(0.05))) / n;\n"
    "        }\n"
    "        \n"
    "        d += s = 0.02 + abs(s) * 0.1;\n"
    "        \n"
    "        // Colorful version with base reddish color and angle-based coloring\n"
    "        vec4 c = cos(vec4(a - t + 1.0, a + a - t * 0.8 + 5.0, d * 1.5 + t * 1.4 + 2.0, d + t * 3.0 + 4.0)) + 1.0;\n"
    "        o += d * 0.05 * vec4(4.0, 1.0, 1.0, 0.0) / length(u) / s;\n"
    "        o.yz += c.xy * vec2(c.z, c.w + 0.5) * 1.5 / s;\n"
    "    }\n"
    "    \n"
    "    o = tanh(o / d / 9e2);\n"
    "    frag_color = vec4(o.rgb * u_color.rgb, 1.0).bgra;\n"
    "}\n";


// https://www.shadertoy.com/view/43VcD3
static const char *desktop_13_fs_src = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "\n"
    "#define PI 3.141592\n"
    "#define TAU (2.*PI)\n"
    "#define SIN(x) (sin(x)*.5+.5)\n"
    "#define BUMP_EPS 0.004\n"
    "#define sabsk(x, k) sqrt(x * x + k * k)\n"
    "#define sabs(x) (sabsk(x, .1))\n"
    "#define S(a, b, x) smoothstep(a, b, x)\n"
    "\n"
    "float tt, g_mat;\n"
    "vec3 ro;\n"
    "\n"
    "mat2 rot(float a) { return mat2(cos(a), -sin(a), sin(a), cos(a)); }\n"
    "\n"
    "float smin(float a, float b, float k) {\n"
    "  float h = clamp((a-b)/k * .5 + .5, 0.0, 1.0);\n"
    "  return mix(a, b, h) - h*(1.-h)*k;\n"
    "}\n"
    "\n"
    "float pMod(float p, float size) {\n"
    "	float halfsize = size*0.5;\n"
    "	float c = floor((p + halfsize)/size);\n"
    "	p = mod(p + halfsize, size) - halfsize;\n"
    "	return p;\n"
    "}\n"
    "\n"
    "float n21(vec2 p) {\n"
    "      return fract(sin(dot(p, vec2(524.423, 123.34)))*3228324.345);\n"
    "}\n"
    "\n"
    "float noise(vec2 n) {\n"
    "    const vec2 d = vec2(0., 1.0);\n"
    "    vec2 b = floor(n);\n"
    "    vec2 f = mix(vec2(0.0), vec2(1.0), fract(n));\n"
    "    return mix(mix(n21(b), n21(b + d.yx), f.x), mix(n21(b + d.xy), n21(b + d.yy), f.x), f.y);\n"
    "}\n"
    "\n"
    "vec3 g_p;\n"
    "float smax( float a, float b, float k )\n"
    "{\n"
    "    float h = max(k-abs(a-b),0.0);\n"
    "    return max(a, b) + h*h*0.25/k;\n"
    "}\n"
    "\n"
    "vec3 fold(vec3 p) {\n"
    "    float c = cos(PI/5.), s = sqrt(.75 - c*c);\n"
    "    vec3 n = vec3(-.5, -c, s);\n"
    "    p = abs(p);;\n"
    "    p -= 2.*min(0., dot(p, n))*n;\n"
    "    p.xy = abs(p.xy);\n"
    "    p -= 2.*min(0., dot(p, n))*n;\n"
    "    p.xy = abs(p.xy);\n"
    "    p -= 2.*min(0., dot(p, n))*n;\n"
    "    return p;\n"
    "}\n"
    "\n"
    "vec4 inverseStereographic(vec3 p, out float k) {\n"
    "    k = 2.0/(1.0+dot(p,p));\n"
    "    return vec4(k*p,k-1.0);\n"
    "}\n"
    "\n"
    "float fTorus(vec4 p4) {\n"
    "    float d1 = length(p4.xy) / length(p4.zw) - 1.;\n"
    "    float d2 = length(p4.zw) / length(p4.xy) - 1.;\n"
    "    float d = d1 < 0. ? -d1 : d2;\n"
    "    d /= PI;\n"
    "    return d;\n"
    "}\n"
    "\n"
    "float fixDistance(float d, float k) {\n"
    "    float sn = sign(d);\n"
    "    d = abs(d);\n"
    "    d = d / k * 1.82;\n"
    "    d += 1.;\n"
    "    d = pow(d, .5);\n"
    "    d -= 1.;\n"
    "    d *= 5./3.;\n"
    "    d *= sn;\n"
    "    return d;\n"
    "}\n"
    "\n"
    "float map(vec3 p) {\n"
    "    p.xy *= rot(.1*tt);\n"
    "    p.xz *= rot(.3*tt);\n"
    "    vec3 bp0 = p;\n"
    "    p = fold(p);\n"
    "    vec3 bp = p;\n"
    "    float k;\n"
    "    p = bp0;\n"
    "    p.x = sabsk(p.x, 1.2) - 2.;\n"
    "    p.xy *= rot(p.z*.3+tt);\n"
    "    vec4 p4 = inverseStereographic(p,k);\n"
    "    p4.y -= SIN(.3*tt);\n"
    "    p4.zy *= rot(tt*.5);\n"
    "    p4.xw *= rot(tt*.5);\n"
    "    float d = fTorus(p4);\n"
    "    d = abs(d);\n"
    "    d -= .2;\n"
    "    d = fixDistance(d, k);\n"
    "    p = bp;\n"
    "    p.xy *= rot(1.*p.z*.5+tt);\n"
    "    p.xz *= rot(.5*p.y+.5*tt);\n"
    "    p.xy -= sin(p.z*mix(2., 15., SIN(.7*tt))+tt)*.2;\n"
    "    bp = p;\n"
    "    p.x -= .3;\n"
    "    p.xz = sabsk(p.xz, .4) - .5*SIN(tt);\n"
    "    p.xz*= rot(tt);\n"
    "    p.xz -= vec2(sin(.4*tt)*.3, cos(.3*tt)*.4);\n"
    "    float d1 = length(p) - 1.4;\n"
    "    p = bp;\n"
    "    p.x += .8;\n"
    "    p.xy = sabsk(p.xy, .4) + 1.*SIN(.5*tt);\n"
    "    p.xz += vec2(sin(tt)*.4, cos(tt)*1.5);\n"
    "    float d2 = smax(d1, -(length(p) - 1.4), 1.);\n"
    "    d = smax(d, (d2)-.5, .5);\n"
    "    return d*.8;\n"
    "}\n"
    "\n"
    "vec3 pal(float x) {\n"
    "    return .5 + .5*cos( 6.28318*(vec3(1.0,1.0,1.0)*x+vec3(0.0,0.3,0.5)));\n"
    "}\n"
    "\n"
    "vec3 getRayDir(vec2 uv, vec3 p, vec3 l, float z) {\n"
    "    vec3 f = normalize(l - p),\n"
    "         r = normalize(cross(vec3(0, 1, 0), f)),\n"
    "         u = cross(f, r),\n"
    "         c = p + f * z,\n"
    "         i = c + uv.x * r + uv.y * u,\n"
    "         rd = normalize(i - p);\n"
    "    return rd;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    vec2 uv = (gl_FragCoord.xy - .5*iResolution.xy)/iResolution.y;\n"
    "    tt = time + 30.;\n"
    "    vec3 col;\n"
    "    ro = vec3(.0, 0, -4);\n"
    "    vec3 lookat = vec3(0, 0, 0), p;\n"
    "    vec3 rd = getRayDir(uv, ro, lookat, 1.);\n"
    "    float mat = 0.,\n"
    "          t   = 0.,\n"
    "          d   = 0.;\n"
    "    float alpha = 0.;\n"
    "    p = ro;\n"
    "    float MAX_DIST = 15.;\n"
    "    vec3 c;\n"
    "    for (float i = 0.; i < 100.; i++) {\n"
    "        t += max(0.008, abs(d) * .8);\n"
    "        p = ro + rd * t;\n"
    "        d = map(p);\n"
    "        c = vec3(max(0., .01 - abs(d)) * .7);\n"
    "        c += vec3(0.271,0.094,0.306) * 0.0008;\n"
    "        c *= smoothstep(20., .4, length(t));\n"
    "        float l = smoothstep(MAX_DIST, .1, t);\n"
    "        c *= l;\n"
    "        c *= pal(l * 8. - .9);\n"
    "        col += c;\n"
    "        if (t > MAX_DIST) {\n"
    "            break;\n"
    "        }\n"
    "    }\n"
    "    col = pow(col, vec3(1. / 1.8)) * 2.;\n"
    "    col = pow(col, vec3(2.)) * 5.;\n"
    "    col *= tanh(col);\n"
    "    col = pow(col, vec3(1. / 2.2));\n"
    "    frag_color = vec4(col * u_color.rgb, 1.0).bgra;\n"
    "}\n";
    
    
// https://www.shadertoy.com/view/tlGfzd
static const char *desktop_14_fs_src = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "\n"
    "vec3 objcol;\n"
    "\n"
    "float hash12(vec2 p)\n"
    "{\n"
    "	vec3 p3  = fract(vec3(p.xyx) * .1031);\n"
    "    p3 += dot(p3, p3.yzx + 33.33);\n"
    "    return fract((p3.x + p3.y) * p3.z);\n"
    "}\n"
    "\n"
    "mat2 rot(float a){\n"
    "    float s = sin(a), c = cos(a);\n"
    "    return mat2(c, s, -s, c);\n"
    "}\n"
    "\n"
    "float de(vec3 pos)\n"
    "{\n"
    "    float t = mod(time,17.);\n"
    "    float a=smoothstep(13.,15.,t)*8.-smoothstep(4.,0.,t)*4.;\n"
    "    float f=sin(time*5.+sin(time*20.)*.2);\n"
    "    pos.xz *= rot(time+.5);\n"
    "    pos.yz *= rot(time);\n"
    "    vec3 p = pos;\n"
    "    float s=1.;\n"
    "    for (int i=0; i<4; i++){\n"
    "        p=abs(p)*1.3-.5-f*.1-a;\n"
    "        p.xy*=rot(radians(45.));\n"
    "        p.xz*=rot(radians(45.));\n"
    "        s*=1.3;\n"
    "    }\n"
    "    float fra = length(p)/s-.5;\n"
    "    pos.xy *= rot(time);\n"
    "    p = abs(pos) - 2. - a;\n"
    "    float d = length(p) - .7;\n"
    "    d = min(d, max(length(p.xz)-.1,p.y));\n"
    "    d = min(d, max(length(p.yz)-.1,p.x));\n"
    "    d = min(d, max(length(p.xy)-.1,p.z));\n"
    "    p = abs(pos);\n"
    "    p.x -= 4.+a+f*.5;\n"
    "    d = min(d, length(p) - .7);\n"
    "    d = min(d, length(p.yz-abs(sin(p.x*.5-time*10.)*.3)));\n"
    "    p = abs(pos);\n"
    "    p.y -= 4.+a+f*.5;\n"
    "    d = min(d, length(p) - .7);\n"
    "    d = min(d, max(length(p.xz)-.1,p.y));\n"
    "    d = min(d, fra);\n"
    "    objcol = abs(p);\n"
    "    if (d==fra) objcol=vec3(2.,0.,0.);\n"
    "    return d;\n"
    "}\n"
    "\n"
    "vec3 normal(vec3 p) {\n"
    "    vec2 d = vec2(0., .01);\n"
    "    return normalize(vec3(de(p+d.yxx), de(p+d.xyx), de(p+d.xxy))-de(p));\n"
    "}\n"
    "\n"
    "vec3 march(vec3 from, vec3 dir)\n"
    "{\n"
    "    float d = 0., td = 0., maxdist = 30.;\n"
    "    vec3 p = from, col = vec3(0.);\n"
    "    for (int i = 0; i<100; i++)\n"
    "    {\n"
    "        float d2 = de(p) * (1.-hash12(gl_FragCoord.xy+time)*.2);\n"
    "        if (d2<0.)\n"
    "        {\n"
    "            vec3 n = normal(p);\n"
    "            dir = reflect(dir, n);\n"
    "            d2 = .1;\n"
    "        }\n"
    "        d = max(.01, abs(d2));\n"
    "        p += d * dir;\n"
    "        td += d;\n"
    "        if (td>maxdist) break;\n"
    "        col += .01 * objcol;\n"
    "    }\n"
    "    return pow(col, vec3(2.));\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec2 uv = gl_FragCoord.xy / iResolution.xy - .5;\n"
    "    uv.x *= iResolution.x / iResolution.y;\n"
    "    vec3 from = vec3(0.,0.,-10.);\n"
    "    vec3 dir = normalize(vec3(uv, 1.));\n"
    "    vec3 col = march(from, dir);\n"
    "    frag_color = vec4(col * u_color.rgb, 1.0).bgra;\n"
    "}\n";
    
// https://www.shadertoy.com/view/mts3RB
static const char *desktop_15_fs_src = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "\n"
    "#define Rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))\n"
    "#define antialiasing(n) n/min(iResolution.y,iResolution.x)\n"
    "#define S(d,b) smoothstep(antialiasing(1.0),b,d)\n"
    "#define B(p,s) max(abs(p).x-s.x,abs(p).y-s.y)\n"
    "#define Tri(p,s,a) max(-dot(p,vec2(cos(-a),sin(-a))),max(dot(p,vec2(cos(a),sin(a))),max(abs(p).x-s.x,abs(p).y-s.y)))\n"
    "#define DF(a,b) length(a) * cos( mod( atan(a.y,a.x)+6.28/(b*8.0), 6.28/((b*8.0)*0.5))+(b-1.)*6.28/(b*8.0) + vec2(0,11) )\n"
    "#define SkewX(a) mat2(1.0,tan(a),0.0,1.0)\n"
    "#define SkewY(a) mat2(1.0,0.0,tan(a),1.0)\n"
    "\n"
    "float SimpleVesicaDistanceY(vec2 p, float r, float d) {\n"
    "    p.x = abs(p.x);\n"
    "    p.x+=d;\n"
    "    return length(p)-r;\n"
    "}\n"
    "\n"
    "float SimpleVesicaDistanceX(vec2 p, float r, float d) {\n"
    "    p.y = abs(p.y);\n"
    "    p.y+=d;\n"
    "    return length(p)-r;\n"
    "}\n"
    "\n"
    "float eyeBall(vec2 p){\n"
    "    vec2 prevP = p;\n"
    "    p.x += sin(time)*0.05;\n"
    "    float thickness = 0.002;\n"
    "    float d = abs(length(p)-0.1)-thickness;\n"
    "    float d2 = abs(length(p)-0.04)-thickness;\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    p = DF(p,6.0);\n"
    "    p -= vec2(0.05);\n"
    "    p*=Rot(radians(45.));\n"
    "    d2 = B(p,vec2(0.001,0.015));\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    p = prevP;\n"
    "    p.x += sin(time)*0.05;\n"
    "    d2 = length(p-vec2(-0.03,0.03))-0.015;\n"
    "    d = min(d,d2);\n"
    "    return d;\n"
    "}\n"
    "\n"
    "float eye(vec2 p){\n"
    "    p*=1.2;\n"
    "    vec2 prevP = p;\n"
    "    float thickness = 0.002;\n"
    "    float d = eyeBall(p);\n"
    "    float s = mod(time*0.5,2.3);\n"
    "    if(s<1.){\n"
    "        p.y*=1.+s;\n"
    "    } else if(s>=1. && s<2.){\n"
    "        p.y*=1.+2.-s;\n"
    "    }\n"
    "    float d2 = abs(SimpleVesicaDistanceX(p,0.21,0.1))-thickness;\n"
    "    float d3 = SimpleVesicaDistanceX(p,0.21,0.1);\n"
    "    d = max(d3,d);\n"
    "    \n"
    "    d = min(d,d2);\n"
    "    return d;\n"
    "}\n"
    "\n"
    "float arrow(vec2 p){\n"
    "    float d = Tri(p,vec2(0.22),radians(45.));\n"
    "    float d2 =Tri(p-vec2(0.0,-0.11),vec2(0.22),radians(45.));\n"
    "    d = max(-d2,d);\n"
    "    return d;\n"
    "}\n"
    "\n"
    "float featherBG(vec2 p){\n"
    "    p*=2.3;\n"
    "    vec2 prevP = p;\n"
    "    p.x*=mix(0.3,2.,smoothstep(-1.2,0.9,p.y));\n"
    "    float d = SimpleVesicaDistanceY(p,0.41,0.2);\n"
    "    return d;\n"
    "}\n"
    "\n"
    "float feather(vec2 p){\n"
    "    p*=2.3;\n"
    "    vec2 prevP = p;\n"
    "    p.x*=mix(0.3,2.,smoothstep(-1.2,0.9,p.y));\n"
    "    float d = abs(SimpleVesicaDistanceY(p,0.41,0.2))-0.003;\n"
    "    \n"
    "    p = prevP;\n"
    "    float d2 = B(p-vec2(0.,-0.035),vec2(0.003,0.38));\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    p-=vec2(0.062,0.1);\n"
    "    p*=Rot(radians(-30.));\n"
    "    d2 = B(p,vec2(0.072,0.003));\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    p = prevP;\n"
    "    p-=vec2(-0.048,0.18);\n"
    "    p*=Rot(radians(30.));\n"
    "    d2 = B(p,vec2(0.055,0.003));\n"
    "    d = min(d,d2);\n"
    "        \n"
    "    p = prevP;\n"
    "    p-=vec2(0.079,-0.03);\n"
    "    p*=Rot(radians(-30.));\n"
    "    d2 = B(p,vec2(0.089,0.003));\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    p = prevP;\n"
    "    p-=vec2(-0.07,0.03);\n"
    "    p*=Rot(radians(30.));\n"
    "    d2 = B(p,vec2(0.083,0.003));\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    p = prevP;\n"
    "    d2 = abs(length(p-vec2(-0.08,-0.06))-0.06)-0.003;\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    p = prevP;\n"
    "    d2 = length(p-vec2(0.05,-0.11))-0.02;\n"
    "    d = min(d,d2);\n"
    "    d2 = length(p-vec2(0.11,-0.075))-0.02;\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    d2 = B(p-vec2(0.07,0.032),vec2(0.003,0.068));\n"
    "    d = min(d,d2);\n"
    "    d2 = B(p-vec2(-0.06,0.105),vec2(0.003,0.081));\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    d2 = abs(length(p-vec2(-0.035,0.25))-0.02)-0.003;\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    d2 = abs(length(p-vec2(0.052,0.17))-0.03)-0.003;\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    d2 = abs(length(p-vec2(0.035,0.24))-0.015)-0.003;\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    p = prevP;\n"
    "    \n"
    "    p.x = abs(p.x);\n"
    "    \n"
    "    p-=vec2(0.08,-0.16);\n"
    "    p*=Rot(radians(-30.));\n"
    "    d2 = B(p,vec2(0.095,0.003));\n"
    "    d = min(d,d2);\n"
    "    p*=Rot(radians(30.));\n"
    "    p-=vec2(-0.03,-0.09);\n"
    "    d2 = B(p,vec2(0.003,0.075));\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    p-=vec2(0.05,0.035);\n"
    "    d2 = B(p,vec2(0.003,0.066));\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    return d;\n"
    "}\n"
    "\n"
    "float drawMainGraphic(vec2 p){\n"
    "    vec2 prevP = p;\n"
    "\n"
    "    float d = eye(p);\n"
    "\n"
    "    p*=Rot(radians(10.*time));\n"
    "    p = DF(p,3.0);\n"
    "    p -= vec2(0.235);\n"
    "    p*=Rot(radians(45.));\n"
    "    float d2 = feather(p);\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    p = prevP;\n"
    "    p*=Rot(radians(10.*time));\n"
    "    p = DF(p,3.0);\n"
    "    p -= vec2(0.108);\n"
    "    d2 = abs(length(p)-0.02)-0.001;\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    p = prevP;\n"
    "    d2 = abs(length(p)-0.155)-0.001;\n"
    "    d = min(d,d2);\n"
    "    \n"
    "    return d;\n"
    "}\n"
    "\n"
    "float isoCube(vec2 p){\n"
    "    vec2 prevP = p;\n"
    "    p.y*=1.5;\n"
    "    p*=Rot(radians(45.));\n"
    "    \n"
    "    float d = abs(B(p,vec2(0.1)))-0.002;\n"
    "    p = prevP;\n"
    "    p.x=abs(p.x);\n"
    "    p-=vec2(0.072,-0.12);\n"
    "    p.x*=1.41;\n"
    "    p.y*=1.41;\n"
    "    p*=SkewY(radians(-34.));\n"
    "    float d2 = abs(B(p,vec2(0.1)))-0.002;\n"
    "    d = min(d,d2);\n"
    "    return d;\n"
    "}\n"
    "\n"
    "float background(vec2 p){\n"
    "    p.y-=time*0.1;\n"
    "    p*=2.;\n"
    "    vec2 prevP = p;\n"
    "    p.x = mod(p.x,0.288)-0.144;\n"
    "    p.y = mod(p.y,0.48)-0.24;\n"
    "    float d = isoCube(p);\n"
    "    p = prevP;\n"
    "    p.x+=0.144;\n"
    "    p.x = mod(p.x,0.288)-0.144;\n"
    "    p.y+=0.24;\n"
    "    p.y = mod(p.y,0.48)-0.24;\n"
    "    float d2 = isoCube(p);\n"
    "    \n"
    "    return min(d,d2);\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec2 p = (gl_FragCoord.xy-0.5*iResolution.xy)/iResolution.y;\n"
    "    vec2 prevP = p;\n"
    "    \n"
    "    vec3 col = vec3(0.);\n"
    "    float d = drawMainGraphic(p);\n"
    "    \n"
    "    float d6 = background(p);\n"
    "    d6 = max(-(length(p)-0.25),d6);\n"
    "    col = mix(col,vec3(0.5),S(d6,0.0));\n"
    "    \n"
    "    p = prevP;\n"
    "    p*=Rot(radians(7.*time));\n"
    "    p = DF(p,3.0);\n"
    "    p -= vec2(0.45);\n"
    "    p*=Rot(radians(45.));\n"
    "    float d5 =arrow(p);\n"
    "    col = mix(col,vec3(0.),S(d5,0.0));\n"
    "    col = mix(col,vec3(0.7),S(abs(d5)-0.001,0.0));\n"
    "    \n"
    "    // feather bg\n"
    "    p = prevP;\n"
    "    p*=Rot(radians(-5.*time));\n"
    "    p = DF(p,4.0);\n"
    "    p -= vec2(0.3);\n"
    "    p*=Rot(radians(45.));\n"
    "    float d4 = featherBG(p);\n"
    "    col = mix(col,vec3(0.),S(d4,0.0));\n"
    "    \n"
    "    p = prevP;\n"
    "    p*=Rot(radians(-5.*time));\n"
    "    p = DF(p,4.0);\n"
    "    p -= vec2(0.3);\n"
    "    p*=Rot(radians(45.));\n"
    "    float d3 = feather(p);\n"
    "    col = mix(col,vec3(0.8),S(d3,0.0));\n"
    "     \n"
    "    p = prevP;\n"
    "    \n"
    "    // feather bg\n"
    "    p*=Rot(radians(10.*time));\n"
    "    p = DF(p,3.0);\n"
    "    p -= vec2(0.235);\n"
    "    p*=Rot(radians(45.));\n"
    "    float d2 = featherBG(p);\n"
    "    col = mix(col,vec3(0.),S(d2,0.0));\n"
    "    col = mix(col,vec3(1.),S(d,0.0));\n"
    "    \n"
    "    frag_color = vec4(col * u_color.rgb, 1.0).bgra;\n"
    "}\n";

// Background Vertex Shader - Simple fullscreen quad

// Background Vertex Shader - Simple fullscreen quad
static const char *background_vertex_shader_src =
"#version 300 es\n"
"precision mediump float;\n"
// We define a simple quad in C, so we don't need complex inputs here
"const vec2 verts[4] = vec2[](vec2(-1,-1), vec2(1,-1), vec2(-1,1), vec2(1,1));\n"
"out vec2 v_uv;\n"
"void main() {\n"
"    vec2 pos = verts[gl_VertexID];\n"
"    gl_Position = vec4(pos, 0.0, 1.0);\n"
"    v_uv = pos * 0.5 + 0.5; // Convert from [-1,1] to [0,1] for UV coords\n"
"}\n";

// Background Fragment Shader - Multicolored flowing gradient

// Background Fragment Shader - Volumetric space/nebula shader
static const char *background_fragment_shader_src =
"#version 300 es\n"
"precision mediump float;\n"
"uniform float u_time; // Time for animation\n"
"in vec2 v_uv;\n"
"out vec4 FragColor;\n"
"\n"
"#define iterations 4\n"
"#define formuparam2 0.89\n"
"\n"
"#define volsteps 10\n"
"#define stepsize 0.190\n"
"\n"
"#define zoom 3.900\n"
"#define tile   0.450\n"
"#define speed2  0.010\n"
"\n"
"#define brightness 0.2\n"
"#define darkmatter 0.400\n"
"#define distfading 0.560\n"
"#define saturation 0.400\n"
"\n"
"#define transverseSpeed 1.1\n"
"#define cloud 0.2\n"
"\n"
"// Note: Using u_time instead of iTime, and v_uv for coordinates\n"
"// iResolution will be approximated with vec2(800.0, 600.0) or similar\n"
"\n"
"float triangle(float x, float a) {\n"
"    float output2 = 2.0 * abs(3.0 * ((x / a) - floor((x / a) + 0.5))) - 1.0;\n"
"    return output2;\n"
"}\n"
"\n"
"float field(in vec3 p) {\n"
"    float strength = 7.0 + 0.03 * log(1.e-6 + fract(sin(u_time) * 4373.11));\n"
"    float accum = 0.;\n"
"    float prev = 0.;\n"
"    float tw = 0.;\n"
"\n"
"    for (int i = 0; i < 6; ++i) {\n"
"        float mag = dot(p, p);\n"
"        p = abs(p) / mag + vec3(-.5, -.8 + 0.1 * sin(u_time * 0.2 + 2.0), -1.1 + 0.3 * cos(u_time * 0.15));\n"
"        float w = exp(-float(i) / 7.);\n"
"        accum += w * exp(-strength * pow(abs(mag - prev), 2.3));\n"
"        tw += w;\n"
"        prev = mag;\n"
"    }\n"
"    return max(0., 5. * accum / tw - .7);\n"
"}\n"
"\n"
"void main() {\n"
"    // Approximate iResolution - adjust these values based on your actual resolution\n"
"    vec2 iResolution = vec2(800.0, 600.0);\n"
"    float iTime = u_time/3.0;\n"
"    \n"
"    // Convert v_uv back to fragCoord style coordinates\n"
"    vec2 fragCoord = v_uv * iResolution;\n"
"    \n"
"    vec2 uv2 = 2. * fragCoord.xy / iResolution.xy - 1.;\n"
"    vec2 uvs = uv2 * iResolution.xy / max(iResolution.x, iResolution.y);\n"
"\n"
"    float time2 = iTime;\n"
"    \n"
"    float speed = speed2;\n"
"    speed = 0.005 * cos(time2*0.02 + 3.1415926/4.0);\n"
"    float formuparam = formuparam2;\n"
"    \n"
"    //get coords and direction\n"
"    vec2 uv = uvs;\n"
"    //mouse rotation\n"
"    float a_xz = 0.9;\n"
"    float a_yz = -.6;\n"
"    float a_xy = 0.9 + iTime*0.04;\n"
"\n"
"    mat2 rot_xz = mat2(cos(a_xz),sin(a_xz),-sin(a_xz),cos(a_xz));\n"
"    mat2 rot_yz = mat2(cos(a_yz),sin(a_yz),-sin(a_yz),cos(a_yz));\n"
"    mat2 rot_xy = mat2(cos(a_xy),sin(a_xy),-sin(a_xy),cos(a_xy));\n"
"\n"
"    float v2 =1.0;\n"
"\n"
"    vec3 dir=vec3(uv*zoom,1.);\n"
"    vec3 from=vec3(0.0, 0.0,0.0);\n"
"\n"
"    from.x -= 5.0* (0.5);\n"
"    from.y -= 5.0* (0.5);\n"
"\n"
"    vec3 forward = vec3(0.,0.,1.);\n"
"\n"
"    from.x += transverseSpeed*(1.0)*cos(0.01*iTime) + 0.001*iTime;\n"
"    from.y += transverseSpeed*(1.0)*sin(0.01*iTime) +0.001*iTime;\n"
"    from.z += 0.003*iTime;\n"
"\n"
"    dir.xy*=rot_xy;\n"
"    forward.xy *= rot_xy;\n"
"    dir.xz*=rot_xz;\n"
"    forward.xz *= rot_xz;\n"
"    dir.yz*= rot_yz;\n"
"    forward.yz *= rot_yz;\n"
"\n"
"    from.xy*=-rot_xy;\n"
"    from.xz*=rot_xz;\n"
"    from.yz*= rot_yz;\n"
"\n"
"    //zoom\n"
"    float zooom = (time2-3311.)*speed;\n"
"    from += forward* zooom;\n"
"    float sampleShift = mod( zooom, stepsize );\n"
"\n"
"    float zoffset = -sampleShift;\n"
"    sampleShift /= stepsize; // make from 0 to 1\n"
"\n"
"    //volumetric rendering\n"
"    float s=0.24;\n"
"    float s3 = s + stepsize/2.0;\n"
"    vec3 v=vec3(0.);\n"
"    float t3 = 0.0;\n"
"\n"
"    vec3 backCol2 = vec3(0.);\n"
"    for (int r=0; r<volsteps; r++) {\n"
"        vec3 p2=from+(s+zoffset)*dir;// + vec3(0.,0.,zoffset);\n"
"        vec3 p3=(from+(s3+zoffset)*dir )* (1.9/zoom);// + vec3(0.,0.,zoffset);\n"
"\n"
"        p2 = abs(vec3(tile)-mod(p2,vec3(tile*2.))); // tiling fold\n"
"        p3 = abs(vec3(tile)-mod(p3,vec3(tile*2.))); // tiling fold\n"
"\n"
"        #ifdef cloud\n"
"        t3 = field(p3);\n"
"        #endif\n"
"\n"
"        float pa,a=pa=0.;\n"
"        for (int i=0; i<iterations; i++) {\n"
"            p2=abs(p2)/dot(p2,p2)-formuparam; // the magic formula\n"
"            //p=abs(p)/max(dot(p,p),0.005)-formuparam; // another interesting way to reduce noise\n"
"            float D = abs(length(p2)-pa); // absolute sum of average change\n"
"\n"
"            if (i > 2)\n"
"            {\n"
"            a += i > 7 ? min( 12., D) : D;\n"
"            }\n"
"                pa=length(p2);\n"
"        }\n"
"\n"
"        //float dm=max(0.,darkmatter-a*a*.001); //dark matter\n"
"        a*=a*a; // add contrast\n"
"        //if (r>3) fade*=1.-dm; // dark matter, don't render near\n"
"        // brightens stuff up a bit\n"
"        float s1 = s+zoffset;\n"
"        // need closed form expression for this, now that we shift samples\n"
"        float fade = pow(distfading,max(0.,float(r)-sampleShift));\n"
"\n"
"        //t3 += fade;\n"
"        v+=fade;\n"
"        //backCol2 -= fade;\n"
"\n"
"        // fade out samples as they approach the camera\n"
"        if( r == 0 )\n"
"            fade *= (1. - (sampleShift));\n"
"        // fade in samples as they approach from the distance\n"
"        if( r == volsteps-1 )\n"
"            fade *= sampleShift;\n"
"        v+=vec3(s1,s1*s1,s1*s1*s1*s1)*a*brightness*fade; // coloring based on distance\n"
"\n"
"        backCol2 += mix(.4, 1., v2) * vec3(0.20 * t3 * t3 * t3, 0.4 * t3 * t3, t3 * 0.7) * fade;\n"
"\n"
"        s+=stepsize;\n"
"        s3 += stepsize;\n"
"    }\n"
"\n"
"    v=mix(vec3(length(v)),v,saturation);\n"
"    vec4 forCol2 = vec4(v*.01,1.);\n"
"\n"
"    #ifdef cloud\n"
"    backCol2 *= cloud;\n"
"    #endif\n"
"\n"
"    FragColor = forCol2 + vec4(backCol2*0.6, 1.0).bgra;\n"
"}\n";

//https://www.shadertoy.com/view/3ccSzN
//cubecap shader
static const char *desktop_cubecap_fs_src = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform float time;\n"
    "uniform vec2 iResolution;\n"
    "uniform vec4 u_color;\n"
    "\n"
    "#define iTime time\n"
    "\n"
    "void main() {\n"
    "    vec2 uv = (gl_FragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;\n"
    "    \n"
    "    float frequency = 40.0;\n"
    "    float speed = 1.8;\n"
    "    float amplitude = 0.1;\n"
    "    \n"
    "    float dist = length(uv);\n"
    "    float height = sin(dist * frequency - iTime * speed) * amplitude;\n"
    "    \n"
    "    float delta = 0.001;\n"
    "    \n"
    "    // X gradient\n"
    "    float distX1 = length(uv + vec2(delta, 0.0));\n"
    "    float distX2 = length(uv - vec2(delta, 0.0));\n"
    "    float hX1 = sin(distX1 * frequency - iTime * speed) * amplitude;\n"
    "    float hX2 = sin(distX2 * frequency - iTime * speed) * amplitude;\n"
    "    float dx = (hX1 - hX2) / (2.0 * delta);\n"
    "    \n"
    "    // Y gradient\n"
    "    float distY1 = length(uv + vec2(0.0, delta));\n"
    "    float distY2 = length(uv - vec2(0.0, delta));\n"
    "    float hY1 = sin(distY1 * frequency - iTime * speed) * amplitude;\n"
    "    float hY2 = sin(distY2 * frequency - iTime * speed) * amplitude;\n"
    "    float dy = (hY1 - hY2) / (2.0 * delta);\n"
    "    \n"
    "    // Normal from gradient\n"
    "    vec3 normal = normalize(vec3(-dx, -dy, 1.0));\n"
    "    vec3 lightDir = normalize(vec3(0.3, sin(iTime * 0.2), 0.5));\n"
    "    \n"
    "    float brightness = clamp(exp(dot(normal, lightDir)) * 0.5, 0.0, 1.0);\n"
    "    \n"
    "    vec3 baseColor = vec3(1.0);\n"
    "    vec3 color = baseColor * brightness;\n"
    "    \n"
    "    frag_color = vec4(color * u_color.rgb, 1.0);\n"
    "}";

//tv effect
static const char *post_process_vert =
    "#version 300 es\n"
    "precision mediump float;\n"
    "\n"
    "// Input vertex position in [0, 1] range\n"
    "layout(location = 0) in vec2 a_position_01;\n"
    "// Input texture coordinate in [0, 1] range\n"
    "layout(location = 1) in vec2 a_texcoord;\n"
    "\n"
    "// Pass texture coordinate to the fragment shader\n"
    "out vec2 v_texcoord;\n"
    "\n"
    "void main() {\n"
    "    // Convert [0, 1] position to [-1, 1] Normalized Device Coordinates\n"
    "    gl_Position = vec4(a_position_01 * 2.0 - 1.0, 0.0, 1.0);\n"
    "\n"
    "    // Pass the texture coordinate through unchanged\n"
    "    v_texcoord = a_texcoord;\n"
    "}\n";


//tv effect
/*    
static const char *post_process_frag =
    "#version 300 es\n"
    "precision highp float;\n"
    "\n"
    "uniform sampler2D u_scene_texture;\n"
    "uniform float u_time;\n"
    "uniform vec2 iResolution;\n"
    "\n"
    "in vec2 v_texcoord;\n"
    "out vec4 FragColor;\n"
    "\n"
    "// --- HELPER FUNCTIONS (No change) ---\n"
    "float rand(vec2 co){ return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453); }\n"
    "float noise(vec2 pos) { vec2 i=floor(pos),f=fract(pos),u=f*f*(3.0-2.0*f); return mix(mix(rand(i),rand(i+vec2(1,0)),u.x),mix(rand(i+vec2(0,1)),rand(i+vec2(1,1)),u.x),u.y); }\n"
    "vec3 chromaticAberration(sampler2D tex,vec2 uv,float s){ vec2 o=(uv-0.5)*s; return vec3(texture(tex,uv+o).r,texture(tex,uv).g,texture(tex,uv-o).b); }\n"
    "float onOff(float a,float b,float c,float t){ return step(c,sin(t+a*cos(t*b))); }\n"
    "float ramp(float y,float s,float e){ float i=step(s,y)-step(e,y); return(1.-(y-s)/(e-s))*i; }\n"
    "float stripes(vec2 uv,float t){ float n=noise(uv*vec2(0.5,1)+vec2(1,3)); return ramp(mod(uv.y*4.+t/2.+sin(t+sin(t*0.63)),1.),0.5,0.6)*n; }\n"
    "vec2 screenDistort(vec2 uv){ uv-=0.5; uv=uv*1.2*(1./1.2+2.*uv.x*uv.x*uv.y*uv.y); uv+=0.5; return uv; }\n"
    "\n"
    "void main() {\n"
    "    // --- TIMING & PROGRESS (No change) ---\n"
    "    float cycle_duration = 5.0;\n"
    "    float half_cycle = cycle_duration / 2.0;\n"
    "    float time_in_cycle = mod(u_time, cycle_duration);\n"
    "    float progress;\n"
    "    bool turning_on = (time_in_cycle < half_cycle);\n"
    "    if (turning_on) { progress = time_in_cycle / half_cycle; } \n"
    "    else { progress = 1.0 - (time_in_cycle - half_cycle) / half_cycle; }\n"
    "    float eased_progress = pow(progress, 3.0);\n"
    "\n"
    "    float uniform_scale = eased_progress;\n"
    "    vec2 center = vec2(0.5, 0.5);\n"
    "    vec2 pos = v_texcoord - center;\n"
    "\n"
    "    // --- TV OFF STATE (No change) ---\n"
    "    if (abs(pos.x) > uniform_scale / 2.0 || abs(pos.y) > uniform_scale / 2.0) {\n"
    "        // ... (your existing off-state logic) ...\n"
    "        FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "        return;\n"
    "    }\n"
    "\n"
    "    // --- TV ON STATE --- \n"
    "    vec2 sample_coords = pos / uniform_scale + center;\n"
    "\n"
    "    // <<< START OF THE FIX: Calculate a single 'glitch_amount' >>>\n"
    "    // This variable will control ALL effects. It peaks in the middle of the animation\n"
    "    // and is 0.0 when the animation starts and ends.\n"
    "    float glitch_amount = progress * (1.0 - progress) * 7.0; // Parabolic curve, peaks at 1.0\n"
    "    glitch_amount = pow(glitch_amount, 2.0); // Sharpen the peak\n"
    "\n"
    "    // 1. Calculate the final, clean texture coordinate\n"
    "    vec2 clean_coords = sample_coords;\n"
    "    vec3 clean_color = texture(u_scene_texture, clean_coords).rgb;\n"
    "\n"
    "    // 2. Calculate the fully distorted texture coordinate\n"
    "    vec2 distorted_coords = screenDistort(sample_coords);\n"
    "    float h_dist = sin(distorted_coords.y * 800.0 + u_time * 8.0) * 0.01;\n"
    "    float v_dist = sin(distorted_coords.x * 600.0 + u_time * 6.0) * 0.005;\n"
    "    distorted_coords += vec2(h_dist, v_dist);\n"
    "    float vShift = 0.4 * onOff(2.0,3.0,0.9,u_time)*(sin(u_time)*sin(u_time*20.0) + (0.5 + 0.1*sin(u_time*200.0)*cos(u_time)));\n"
    "    distorted_coords.y = mod(distorted_coords.y + vShift, 1.0);\n"
    "    \n"
    "    // 3. Blend between clean and distorted coordinates based on glitch_amount\n"
    "    vec2 final_coords = mix(clean_coords, distorted_coords, glitch_amount);\n"
    "\n"
    "    // 4. Sample the texture and apply chromatic aberration\n"
    "    vec3 color = chromaticAberration(u_scene_texture, final_coords, glitch_amount * 0.015);\n"
    "\n"
    "    // 5. Calculate all additive glitch effects (noise, stripes)\n"
    "    float vigAmt = 3.0 + 0.3*sin(u_time + 5.0*cos(u_time*5.0));\n"
    "    float vignette = (1.0 - vigAmt*(v_texcoord.y-0.5)*(v_texcoord.y-0.5)) * (1.0-vigAmt*(v_texcoord.x-0.5)*(v_texcoord.x-0.5));\n"
    "    vec3 additive_glitches = vec3(0.0);\n"
    "    additive_glitches += stripes(v_texcoord, u_time);\n"
    "    additive_glitches += noise(v_texcoord * 2.0) * 0.5;\n"
    "    additive_glitches *= vignette;\n"
    "    additive_glitches *= (12.0 + mod(v_texcoord.y*30.0 + u_time, 1.0)) / 13.0;\n"
    "\n"
    "    // 6. Apply the glitches based on glitch_amount\n"
    "    color += additive_glitches * glitch_amount;\n"
    "\n"
    "    // 7. Apply the flash, which is independent of the main glitch amount\n"
    "    if (turning_on) {\n"
    "        float flash_intensity = smoothstep(0.35, 0.0, progress) * 0.6;\n"
    "        vec3 flash_color = vec3(1.8, 1.7, 1.6);\n"
    "        color = mix(color, flash_color, flash_intensity);\n"
    "    }\n"
    "\n"
    "    // 8. FINAL MIX: Blend the fully glitched color back to the clean color as the animation finishes.\n"
    "    // When eased_progress is 1.0, glitch_amount is 0.0, so this becomes 100% clean_color.\n"
    "    vec3 final_color = mix(color, clean_color, 1.0 - glitch_amount);\n"
    "\n"
    "    FragColor = vec4(clamp(final_color, 0.0, 1.0), 1.0);\n"
    "}\n";
*/

//swirl
/*
static const char *post_process_frag =
    "#version 300 es\n"
    "precision highp float;\n"
    "\n"
    "uniform sampler2D u_scene_texture;\n"
    "uniform float u_time;\n"
    "uniform vec2 iResolution;\n"
    "\n"
    "in vec2 v_texcoord;\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main() {\n"
    "    // --- TIMING & PROGRESS ---\n"
    "    float cycle_duration = 5.0;\n"
    "    float half_cycle = cycle_duration / 2.0;\n"
    "    float time_in_cycle = mod(u_time, cycle_duration);\n"
    "    float progress;\n"
    "    bool flowing_in = (time_in_cycle < half_cycle);\n"
    "    if (flowing_in) { \n"
    "        progress = time_in_cycle / half_cycle; \n"
    "    } else { \n"
    "        progress = 1.0 - (time_in_cycle - half_cycle) / half_cycle; \n"
    "    }\n"
    "    \n"
    "    float smooth_progress = smoothstep(0.0, 1.0, progress);\n"
    "    \n"
    "    // --- LIQUID FLOW DISTORTION ---\n"
    "    vec2 uv = v_texcoord;\n"
    "    \n"
    "    // Create flowing liquid motion\n"
    "    float wave1 = sin(uv.x * 6.0 + u_time * 2.0) * 0.03;\n"
    "    float wave2 = sin(uv.y * 6.0 - u_time * 2.0) * 0.03;\n"
    "    float wave3 = sin((uv.x + uv.y) * 10.0 + u_time * 3.0) * 0.02;\n"
    "    \n"
    "    // Flowing displacement based on progress\n"
    "    vec2 flow_offset = vec2(wave1, wave2 ) * smooth_progress;\n"
    "    \n"
    "    // Radial distortion from center\n"
    "    vec2 center = vec2(0.5);\n"
    "    vec2 to_center = center - uv;\n"
    "    float dist_to_center = length(to_center);\n"
    "    \n"
    "    // Smooth radial flow\n"
    "    float radial_strength = smooth_progress * 0.01;\n"
    "    vec2 radial_flow = normalize(to_center) * sin(dist_to_center * 175.0 - u_time * 4.0) * radial_strength;\n"
    "    \n"
    "    // Combine distortions\n"
    "    vec2 distorted_uv = uv + flow_offset + radial_flow;\n"
    "    \n"
    "    // --- SAMPLING ---\n"
    "    vec3 original_color = texture(u_scene_texture, uv).rgb;\n"
    "    vec3 distorted_color = texture(u_scene_texture, distorted_uv).rgb;\n"
    "    \n"
    "    // --- FINAL BLEND ---\n"
    "    vec3 final_color = mix(distorted_color, distorted_color, smooth_progress);\n"
    "    \n"
    "    FragColor = vec4(final_color, 1.0);\n"
    "}\n";*/
//crt
/**/
 static const char *post_process_frag =
    "#version 300 es\n"
    "precision mediump float;\n"
    "\n"
    "// The texture containing our fully rendered scene\n"
    "uniform sampler2D u_scene_texture;\n"
    "// A time uniform, useful for animated effects\n"
    "uniform float u_time;\n"
    "uniform vec2 iResolution;\n"
    "\n"
    "// The texture coordinate received from the vertex shader\n"
    "in vec2 v_texcoord;\n"
    "\n"
    "// The final output color for the pixel\n"
    "out vec4 FragColor;\n"
    "\n"
    "// Enhanced CRT effect parameters\n"
    "const float SCANLINE_INTENSITY = 0.6;\n"
    "const float CHROMATIC_ABERRATION = 0.018;\n"
    "const float BRIGHTNESS = 2.0;\n"
    "const float CONTRAST = 1.3;\n"
    "const float VIGNETTE_STRENGTH = 0.5;\n"
    "const float FLICKER_INTENSITY = 0.18;\n"
    "const float PHOSPHOR_GLOW = 0.3;\n"
    "const float SCREEN_CURVE = 0.0;\n"
    "\n"
    "// Apply screen curvature like old CRT monitors\n"
    "vec2 screenCurve(vec2 uv) {\n"
    "    uv = uv * 2.0 - 1.0;\n"
    "    vec2 offset = abs(uv.yx) * SCREEN_CURVE;\n"
    "    uv = uv + uv * offset * offset;\n"
    "    uv = uv * 0.5 + 0.5;\n"
    "    return uv;\n"
    "}\n"
    "\n"
    "// Generate enhanced scanlines with phosphor effect\n"
    "float scanline(vec2 uv) {\n"
    "    float line = sin(uv.y * 1000.0) * 0.5 + 0.5;\n"
    "    float scanlineEffect = pow(line, 2.0);\n"
    "    return 1.0 - SCANLINE_INTENSITY * (1.0 - scanlineEffect);\n"
    "}\n"
    "\n"
    "// RGB phosphor pattern simulation\n"
    "vec3 phosphorPattern(vec2 uv, vec3 color) {\n"
    "    vec2 pixel = uv * iResolution.xy;\n"
    "    \n"
    "    // Create RGB sub-pixel pattern\n"
    "    float subPixel = mod(pixel.x, 3.0);\n"
    "    vec3 phosphor = vec3(1.0);\n"
    "    \n"
    "    if (subPixel < 1.0) {\n"
    "        phosphor = vec3(1.2, 0.8, 0.8); // Red phosphor\n"
    "    } else if (subPixel < 2.0) {\n"
    "        phosphor = vec3(0.8, 1.2, 0.8); // Green phosphor\n"
    "    } else {\n"
    "        phosphor = vec3(0.8, 0.8, 1.2); // Blue phosphor\n"
    "    }\n"
    "    \n"
    "    return color * mix(vec3(1.0), phosphor, PHOSPHOR_GLOW);\n"
    "}\n"
    "\n"
    "// Apply vignette effect\n"
    "float vignette(vec2 uv) {\n"
    "    float dist = distance(uv, vec2(0.5));\n"
    "    return 1.0 - smoothstep(0.3, 0.8, dist) * VIGNETTE_STRENGTH;\n"
    "}\n"
    "\n"
    "// Enhanced flicker with multiple frequencies\n"
    "float flicker(float time) {\n"
    "    float flick1 = sin(time * 15.0) * 0.5 + 0.5;\n"
    "    float flick2 = sin(time * 30.0 + 3.14159) * 0.3 + 0.7;\n"
    "    float flick3 = sin(time * 60.0) * 0.1 + 0.9;\n"
    "    return 1.0 + FLICKER_INTENSITY * (flick1 * flick2 * flick3 - 0.5);\n"
    "}\n"
    "\n"
    "// Rolling interference lines\n"
    "float interference(vec2 uv, float time) {\n"
    "    float roll = sin((uv.y + time * 0.1) * 20.0) * 0.02;\n"
    "    float noise = sin((uv.y + time * 0.05) * 200.0) * 0.01;\n"
    "    return 1.0 + roll + noise;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    // --- TIMING SYSTEM ---\n"
    "    float cycle_duration = 5.0;\n"
    "    float time_in_cycle = mod(u_time, cycle_duration);\n"
    "    float effect_intensity = smoothstep(0.0, 1.0, time_in_cycle / cycle_duration);\n"
    "    effect_intensity = sin(effect_intensity * 3.14159); // Smooth rise and fall\n"
    "    \n"
    "    // Apply screen curvature\n"
    "    vec2 curved_uv = mix(v_texcoord, screenCurve(v_texcoord), effect_intensity);\n"
    "    \n"
    "    // Check if we're outside the curved screen bounds\n"
    "    if (curved_uv.x < 0.0 || curved_uv.x > 1.0 || curved_uv.y < 0.0 || curved_uv.y > 1.0) {\n"
    "        FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "        return;\n"
    "    }\n"
    "    \n"
    "    // Sample with enhanced chromatic aberration - now with wavy horizontal shift\n"
    "    float wave = sin(curved_uv.y * 10.0 + u_time * 10.0) * 0.5;\n"
    "    float aberration = CHROMATIC_ABERRATION * effect_intensity * (1.0 + wave);\n"
    "    vec2 offset = vec2(aberration, 0.0);\n"
    "    float r = texture(u_scene_texture, curved_uv + offset).r;\n"
    "    float g = texture(u_scene_texture, curved_uv).g;\n"
    "    float b = texture(u_scene_texture, curved_uv - offset).b;\n"
    "    \n"
    "    vec3 color = vec3(r, g, b);\n"
    "    \n"
    "    // Apply brightness and contrast\n"
    "    float brightness = mix(1.0, BRIGHTNESS, effect_intensity);\n"
    "    float contrast = mix(1.0, CONTRAST, effect_intensity);\n"
    "    color = color * brightness;\n"
    "    color = (color - 0.5) * contrast + 0.5;\n"
    "    \n"
    "    // Apply scanlines\n"
    "    float scanline_effect = mix(1.0, scanline(curved_uv), effect_intensity);\n"
    "    color *= scanline_effect;\n"
    "    \n"
    "    // Apply phosphor pattern\n"
    "    color = mix(color, phosphorPattern(curved_uv, color), effect_intensity);\n"
    "    \n"
    "    // Apply vignette\n"
    "    float vignette_effect = mix(1.0, vignette(curved_uv), effect_intensity);\n"
    "    color *= vignette_effect;\n"
    "    \n"
    "    // Apply flicker\n"
    "    float flicker_effect = mix(1.0, flicker(u_time), effect_intensity);\n"
    "    color *= flicker_effect;\n"
    "    \n"
    "    // Apply interference\n"
    "    float interference_effect = mix(1.0, interference(curved_uv, u_time), effect_intensity);\n"
    "    color *= interference_effect;\n"
    "    \n"
    "    // Add authentic CRT green tint\n"
    "    color.g *= mix(1.0, 1.08, effect_intensity);\n"
    "    \n"
    "    // Add slight warm glow\n"
    "    vec3 glow = vec3(0.02, 0.01, 0.0) * effect_intensity;\n"
    "    color += glow;\n"
    "    \n"
    "    // Clamp to prevent over-brightness\n"
    "    color = clamp(color, 0.0, 1.0);\n"
    "    \n"
    "    FragColor = vec4(color, 1.0);\n"
    "}\n";

/*
// NEW: A simple vertex shader, can be the same as your others.
static const char *passthrough_vertex_shader_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "layout(location = 0) in vec2 position;\n"
    "layout(location = 1) in vec2 texcoord;\n"
    "out vec2 v_texcoord;\n"
    "uniform mat3 mvp;\n"
    "void main() {\n"
    "    vec3 pos_transformed = mvp * vec3(position, 1.0);\n"
    "    gl_Position = vec4(pos_transformed.xy, 0.0, 1.0);\n"
    "    v_texcoord = texcoord;\n"
    "}\n";

// NEW: A simple fragment shader that just renders a texture as-is.
static const char *passthrough_fragment_shader_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "uniform sampler2D u_texture;\n"
    "void main() {\n"
    "    // Just sample the texture and output its color, swizzling to BGRA.\n"
    "    frag_color = texture(u_texture, v_texcoord).bgra;\n"
    "}\n";
*/

// The vertex shader can remain the same.
static const char *passthrough_vertex_shader_src =
    "#version 300 es\n"
    "precision mediump float;\n"
    "layout(location = 0) in vec2 position;\n"
    "layout(location = 1) in vec2 texcoord;\n"
    "out vec2 v_texcoord;\n"
    "uniform mat3 mvp;\n"
    "void main() {\n"
    "    vec3 pos_transformed = mvp * vec3(position, 1.0);\n"
    "    gl_Position = vec4(pos_transformed.xy, 0.0, 1.0);\n"
    "    v_texcoord = texcoord;\n"
    "}\n";

// NEW: Enhanced fragment shader with rounded corners and a bevel.
// NEW: Enhanced fragment shader with a cycling gradient bevel.
// NEW: Enhanced fragment shader with a cycling gradient bevel.
static const char *passthrough_fragment_shader_src =
    "#version 300 es\n"
    "precision highp float;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 frag_color;\n"
    "\n"
    "uniform sampler2D u_texture;\n"
    "uniform vec2 iResolution;\n"
    "uniform float cornerRadius;\n"
    "uniform vec4 bevelColor; // This is now IGNORED, but kept for compatibility.\n"
    "uniform float time; // We now use the time uniform.\n"
    "\n"
    "const float bevelWidth = 4.0;\n"
    "const float aa = 1.5;\n"
    "\n"
    "// SDF for a 2D rounded box (unchanged).\n"
    "float sdRoundedBox(vec2 p, vec2 b, float r) {\n"
    "    vec2 q = abs(p) - b + r;\n"
    "    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;\n"
    "}\n"
    "\n"
    "// Helper function to create a rainbow color from a single value (hue).\n"
    "vec3 hsv2rgb(vec3 c) {\n"
    "    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"
    "    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"
    "    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    // --- 1. Shape Calculation (unchanged) ---\n"
    "    vec2 halfRes = iResolution * 0.5;\n"
    "    vec2 p = (v_texcoord - 0.5) * iResolution;\n"
    "    float d = sdRoundedBox(p, halfRes, cornerRadius);\n"
    "\n"
    "    // --- 2. Alpha Mask (unchanged) ---\n"
    "    float shape_alpha = 1.0 - smoothstep(-aa, aa, d);\n"
    "\n"
    "    // --- 3. Bevel Calculation (unchanged) ---\n"
    "    float bevel_intensity = smoothstep(-bevelWidth, 0.0, d);\n"
    "    bevel_intensity -= smoothstep(0.0, aa, d);\n"
    "\n"
    "    // --- 4. NEW: Cycling Gradient Color Calculation ---\n"
    "    // Calculate the angle of the current pixel relative to the center.\n"
    "    // atan(y, x) gives an angle from -PI to PI.\n"
    "    float angle = atan(p.y, p.x);\n"
    "    // Normalize angle to a 0.0 - 1.0 range.\n"
    "    float hue = angle / (2.0 * 3.14159) + 0.5;\n"
    "\n"
    "    // Make the hue cycle over time.\n"
    "    hue = fract(hue + time * 0.1);\n"
    "\n"
    "    // Convert the cycling hue into an RGB color.\n"
    "    // We use full saturation (1.0) and brightness (1.0) for a vibrant rainbow.\n"
    "    vec3 gradient_bevel_color = hsv2rgb(vec3(hue, 1.0, 1.0));\n"
    "    float bevel_alpha = 0.8; // Set a fixed alpha for the bevel.\n"
    "\n"
    "    // --- 5. Color Sampling & Composition ---\n"
    "    vec4 tex_color = texture(u_texture, v_texcoord);\n"
    "\n"
    "    // Mix the texture color with our NEW dynamic gradient color.\n"
    "    vec3 final_rgb = mix(tex_color.rgb, gradient_bevel_color, bevel_intensity * bevel_alpha);\n"
    "\n"
    "    // --- 6. Final Output ---\n"
    "    float final_alpha = tex_color.a * shape_alpha;\n"
    "    frag_color = vec4(final_rgb, final_alpha).bgra;\n"
    "}\n";

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
    server.tv_is_on = true; // Start with the TV on
       server.tv_effect_animating = false; // The effect is off by default
    server.tv_effect_start_time = 0.0f;
    server.tv_effect_duration = 5.0f; // This is our 0 -> 5 second timeline
    server.expo_effect_active = true;
server.effect_is_target_zoomed=true;
server.effect_anim_current_factor=1.0;
 // <<< ADD THIS INITIALIZATION >>>
       server.tv_effect_animating = false; // Start with the animation off
    server.tv_effect_start_time = 0.0f;
    server.tv_effect_duration = 5.0f; // The animation lasts 5 seconds
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
                                     desktop_0_fs_src, // Use the modified one from above
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
         {"panelDimensions", &server.panel_shader_rect_pixel_dimensions_loc},
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
         {"u_rectPixelDimensions", &server.ssd_shader_rect_pixel_dimensions_loc},
         {"iResolution", &server.ssd_shader_resolution_loc},
    };

    // Use the correctly named global shader sources
    if (!create_generic_shader_program(server.renderer, "SSDShader", // Updated log name
                                     ssd_vertex_shader_src,  // Use the renamed global VS
                                     ssd_fragment_shader_src, // Use the renamed global FS
                                     &server.ssd_shader_program,
                                     ssd_shader_uniforms,
                                     sizeof(ssd_shader_uniforms) / sizeof(ssd_shader_uniforms[0]))) {
        wlr_log(WLR_ERROR, "Failed to create SSD shader program.");
        server_destroy(&server); return 1;
    }

    
   struct shader_uniform_spec ssd2_shader_uniforms[] = {
        {"mvp", &server.ssd2_shader_mvp_loc},
        {"u_color", &server.ssd2_shader_color_loc},
        // These are not used by the checkerboard shader, so their locations will be -1.
        // This is fine as render_rect_node doesn't try to set them for SSDs.
        {"time", &server.ssd2_shader_time_loc},
         {"u_rectPixelDimensions", &server.ssd_shader_rect_pixel_dimensions_loc},
         {"iResolution", &server.ssd2_shader_resolution_loc},
    };

    // Use the correctly named global shader sources
    if (!create_generic_shader_program(server.renderer, "SSDShader2", // Updated log name
                                     ssd_vertex_shader_src,  // Use the renamed global VS
                                     ssd2_fragment_shader_src, // Use the renamed global FS
                                     &server.ssd2_shader_program,
                                     ssd2_shader_uniforms,
                                     sizeof(ssd2_shader_uniforms) / sizeof(ssd2_shader_uniforms[0]))) {
        wlr_log(WLR_ERROR, "Failed to create SSD2 shader program.");
        server_destroy(&server); return 1;
    }



// /server.scale_down_effect_active = false; // Or true if you want it on by default

    // ... (creation of your other shaders: flame, rect, panel, ssd, back) ...

    // --- Create Fullscreen Shader (for Scaled Down Scene View) ---
    struct shader_uniform_spec scaled_scene_uniforms[] = {
    {"mvp", &server.fullscreen_shader_mvp_loc},
    {"u_scene_texture0", &server.fullscreen_shader_scene_tex0_loc},
    {"u_scene_texture1", &server.fullscreen_shader_scene_tex1_loc},
    {"u_scene_texture2", &server.fullscreen_shader_scene_tex2_loc},
    {"u_scene_texture3", &server.fullscreen_shader_scene_tex3_loc},
    {"u_scene_texture4", &server.fullscreen_shader_scene_tex4_loc},
    {"u_scene_texture5", &server.fullscreen_shader_scene_tex5_loc},
    {"u_scene_texture6", &server.fullscreen_shader_scene_tex6_loc},
    {"u_scene_texture7", &server.fullscreen_shader_scene_tex7_loc},
    {"u_scene_texture8", &server.fullscreen_shader_scene_tex8_loc},
    {"u_scene_texture9", &server.fullscreen_shader_scene_tex9_loc},
    {"u_scene_texture10", &server.fullscreen_shader_scene_tex10_loc},
    {"u_scene_texture11", &server.fullscreen_shader_scene_tex11_loc},
    {"u_scene_texture12", &server.fullscreen_shader_scene_tex12_loc},
    {"u_scene_texture13", &server.fullscreen_shader_scene_tex13_loc},
    {"u_scene_texture14", &server.fullscreen_shader_scene_tex14_loc},
    {"u_scene_texture15", &server.fullscreen_shader_scene_tex15_loc},
    {"DesktopGrid", &server.fullscreen_shader_desktop_grid_loc},
    {"u_zoom", &server.fullscreen_shader_zoom_loc},
    {"u_move", &server.fullscreen_shader_move_loc},
    {"u_zoom_center", &server.fullscreen_shader_zoom_center_loc},
    {"u_quadrant", &server.fullscreen_shader_quadrant_loc},
    {"switch_mode",&server.fullscreen_switch_mode},
    {"u_switchXY",&server.fullscreen_switchXY} // Add this if using the uniform version
// {"u_current_quad", &server.fullscreen_shader_current_quad_loc} 
};

if (!create_generic_shader_program(server.renderer, "ScaledSceneViewShader",
                                 fullscreen_vertex_shader_src,
                                 expo_fragment_shader_src,
                                 &server.fullscreen_shader_program,
                                 scaled_scene_uniforms,
                                 sizeof(scaled_scene_uniforms) / sizeof(scaled_scene_uniforms[0]))) {
    wlr_log(WLR_ERROR, "Failed to create scaled scene view shader program.");
    server_destroy(&server); 
    return 1;
}


    struct shader_uniform_spec cube_scene_uniforms[] = {
        {"mvp", &server.cube_shader_mvp_loc},
        {"u_scene_texture0", &server.cube_shader_scene_tex0_loc},
        {"u_scene_texture1", &server.cube_shader_scene_tex1_loc},
        {"u_scene_texture2", &server.cube_shader_scene_tex2_loc},
        {"u_scene_texture3", &server.cube_shader_scene_tex3_loc},
        {"u_zoom", &server.cube_shader_zoom_loc},
        {"u_zoom_center", &server.cube_shader_zoom_center_loc},
         {"u_zoom", &server.cube_shader_zoom_loc},
        {"u_rotation_y", &server.cube_shader_time_loc}, // RENAMED in shader, but C variable is the same
        {"u_quadrant", &server.cube_shader_quadrant_loc},
        {"u_vertical_offset", &server.cube_shader_vertical_offset_loc}, // Not used, but defined for consistency
         {"GLOBAL_u_vertical_offset", &server.cube_shader_global_vertical_offset_loc} ,
          {"iResolution", &server.cube_shader_resolution_loc}
    };

if (!create_generic_shader_program(server.renderer, "CubeShader",
                                 cube_vertex_shader_src,
                                 cube_fragment_shader_src,
                                 &server.cube_shader_program,
                                 cube_scene_uniforms, // <-- USE THE CORRECT STRUCT HERE
                                 sizeof(cube_scene_uniforms) / sizeof(cube_scene_uniforms[0]))) {
    wlr_log(WLR_ERROR, "Failed to create cube effect shader program.");
    server_destroy(&server); 
    return 1;
}



// --- Create Cube Background Shader ---
struct shader_uniform_spec cube_bg_uniforms[] = {
    {"u_time", &server.cube_background_shader_time_loc},
};
if (!create_generic_shader_program(server.renderer, "CubeBackgroundShader",
                                 background_vertex_shader_src,
                                 background_fragment_shader_src,
                                 &server.cube_background_shader_program,
                                 cube_bg_uniforms,
                                 sizeof(cube_bg_uniforms) / sizeof(cube_bg_uniforms[0]))) {
    wlr_log(WLR_ERROR, "Failed to create cube background shader program.");
    server_destroy(&server);
    return 1;
}

const char *desktop_fs_sources[] = {
    desktop_0_fs_src, // Desktop 0 uses the "Melt" shader
    desktop_1_fs_src,         // Desktop 1 uses "Starfield"
    desktop_2_fs_src,         // Desktop 2 uses "Tunnel"
    desktop_3_fs_src,          // Desktop 3 uses "Noise"
    desktop_4_fs_src, // Desktop 0 uses the "Melt" shader
    desktop_5_fs_src,         // Desktop 1 uses "Starfield"
    desktop_6_fs_src,         // Desktop 2 uses "Tunnel"
    desktop_7_fs_src,          // Desktop 3 uses "Noise"
    desktop_8_fs_src, // Desktop 0 uses the "Melt" shader
    desktop_9_fs_src,         // Desktop 1 uses "Starfield"
    desktop_10_fs_src,         // Desktop 2 uses "Tunnel"
    desktop_11_fs_src,          // Desktop 3 uses "Noise"
    desktop_12_fs_src, // Desktop 0 uses the "Melt" shader
    desktop_13_fs_src,         // Desktop 1 uses "Starfield"
    desktop_14_fs_src,         // Desktop 2 uses "Tunnel"
    desktop_15_fs_src          // Desktop 3 uses "Noise"
    
};

for (int i = 0; i < 16; ++i) {
    char log_name[64];
    snprintf(log_name, sizeof(log_name), "DesktopBGShader%d", i);

    struct shader_uniform_spec bg_uniforms[] = {
        {"mvp", &server.desktop_bg_shader_mvp_loc[i]},
        {"time", &server.desktop_bg_shader_time_loc[i]},
        {"iResolution", &server.desktop_bg_shader_res_loc[i]},
        {"u_color", &server.desktop_bg_shader_color_loc[i]}
    };

    if (!create_generic_shader_program(server.renderer, log_name,
                                     rect_vertex_shader_src, // All use the same vertex shader
                                     desktop_fs_sources[i],
                                     &server.desktop_background_shaders[i],
                                     bg_uniforms, sizeof(bg_uniforms) / sizeof(bg_uniforms[0]))) {
        wlr_log(WLR_ERROR, "Failed to create desktop background shader %d.", i);
        server_destroy(&server);
        return 1;
    }
}

  struct shader_uniform_spec post_process_uniforms[] = {
        // The texture containing the entire pre-rendered scene.
        // The shader expects this on texture unit 0, which we set with glUniform1i.
        {"u_scene_texture", &server.post_process_shader_tex_loc},
        // A time value (in seconds) for creating animated effects.
        {"u_time", &server.post_process_shader_time_loc},
        {"iResolution",     &server.post_process_shader_resolution_loc}
    };

    // 2. Call your generic function to create the shader program.
    //    It will populate server.post_process_shader_program and the uniform locations.
    if (!create_generic_shader_program(server.renderer, "PostProcessShader",
                                     post_process_vert, post_process_frag,
                                     &server.post_process_shader_program,
                                     post_process_uniforms, 
                                     sizeof(post_process_uniforms) / sizeof(post_process_uniforms[0]))) {
        wlr_log(WLR_ERROR, "Failed to create the final post-processing shader program.");
        // Your error handling here, e.g.:
        server_destroy(&server); 
        return 1;
    }


       struct shader_uniform_spec passthrough_uniforms[] = {
        {"mvp", &server.passthrough_shader_mvp_loc},
        {"u_texture", &server.passthrough_shader_tex_loc},
        // --- ADD THESE ---
        {"iResolution", &server.passthrough_shader_res_loc},
        {"cornerRadius", &server.passthrough_shader_cornerRadius_loc},
        {"bevelColor", &server.passthrough_shader_bevelColor_loc}
    };
    if (!create_generic_shader_program(server.renderer, "PassThroughShader",
                                     passthrough_vertex_shader_src, 
                                     passthrough_fragment_shader_src,
                                     &server.passthrough_shader_program,
                                     passthrough_uniforms, 
                                     sizeof(passthrough_uniforms) / sizeof(passthrough_uniforms[0]))) {
        wlr_log(WLR_ERROR, "Failed to create pass-through shader program.");
        server_destroy(&server); 
        return 1;
    }
    // Initialize zoom effect variables
    // Initialize zoom effect variables

    server.current_desktop = 0;
    DestopGridSize = 4; // Assuming a 4x4 grid for 16 desktops
    server.num_desktops = DestopGridSize* DestopGridSize; // Assuming 4 desktops for the example
   
    server.effect_zoom_factor_normal = 2.0f;
    server.effect_zoom_factor_zoomed = 1.0f;
    
    server.effect_is_target_zoomed = true;
    server.effect_is_animating_zoom = false;
    server.effect_anim_current_factor = server.effect_zoom_factor_normal;
    server.effect_anim_start_factor = server.effect_zoom_factor_normal;
    server.effect_anim_target_factor = server.effect_zoom_factor_normal;
    server.effect_anim_duration_sec = 0.3f; 

    // For the current shader, u_zoom_center should be (0,0) to scale around the quadrant's local center
    server.effect_zoom_center_x = 0.0f;
    server.effect_zoom_center_y = 1.0f;

    // --- Initialize Cube effect variables ---
    server.cube_effect_active = false;
    server.cube_zoom_factor_normal = 2.0f; // Start zoomed out
    server.cube_zoom_factor_zoomed = 1.0f; // Zoomed in
    
    server.cube_is_target_zoomed = true; // The desired state is not zoomed
    server.cube_is_animating_zoom = false;
    server.cube_anim_current_factor = server.cube_zoom_factor_normal;
    server.cube_anim_start_factor = server.cube_zoom_factor_normal;
    server.cube_anim_target_factor = server.cube_zoom_factor_normal;
    server.cube_anim_duration_sec = 0.3f; // A slightly different duration
    
    // These centers likely won't be used if your shader doesn't have a u_zoom_center, but good to init
    server.cube_zoom_center_x = 0.0f; 
    server.cube_zoom_center_y = 1.0f;
 /* wlr_log(WLR_INFO, "Fullscreen Shader Locations: mvp=%d, scene_tex=%d, zoom=%d, zoom_center=%d, quadrant=%d",
            server.fullscreen_shader_mvp_loc,
            server.fullscreen_shader_scene_tex0_loc,
            server.fullscreen_shader_zoom_loc,
            server.fullscreen_shader_zoom_center_loc,
            server.fullscreen_shader_quadrant_loc); // Log the new location too

    wlr_log(WLR_INFO, "ScaledSceneViewShader (fullscreen_shader_program) created. MVP@%d, SceneTex@%d",
            server.fullscreen_shader_mvp_loc, server.fullscreen_shader_scene_tex_loc);
*/
    // Updated log message to be more accurate for the checkerboard shader
    wlr_log(WLR_INFO, "SSDShader created (ID: %u). MVP@%d, Color@%d. (Unused: Time@%d, iResolution@%d)",
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
    GLenum gl_error;
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main A (after make_current): 0x%x", gl_error); }

    // EXISTING 2D QUAD DATA (keep this for your other effects)
    const GLfloat verts[] = {
        // positions // texcoords (Y flipped from your original)
        0.0f, 0.0f,  0.0f, 0.0f, // Top-left quad -> Tex (0,0)
        1.0f, 0.0f,  1.0f, 0.0f, // Top-right quad -> Tex (1,0)
        0.0f, 1.0f,  0.0f, 1.0f, // Bottom-left quad -> Tex (0,1)
        1.0f, 1.0f,  1.0f, 1.0f, // Bottom-right quad -> Tex (1,1)
    };
    const GLuint indices[] = {0, 1, 2, 1, 3, 2}; 

    // NEW CUBE DATA (for the rotating cube effect)
 const GLfloat cube_verts[] = {
    // Front face (z = 0.5) - Counter-clockwise from outside
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  // 0: Bottom-left
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  // 1: Bottom-right
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   0.0f,  // 2: Top-right
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,   0.0f,  // 3: Top-left
    
    // Back face (z = -0.5) - Counter-clockwise from outside  
    -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,   1.0f,  // 4: Bottom-left
    -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   1.0f,  // 5: Top-left
     0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   1.0f,  // 6: Top-right
     0.5f, -0.5f, -0.5f,   0.0f, 0.0f,   1.0f,  // 7: Bottom-right
    
    // Left face (x = -0.5)
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,   2.0f,  // 8: Bottom-back
    -0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   2.0f,  // 9: Bottom-front
    -0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   2.0f,  // 10: Top-front
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   2.0f,  // 11: Top-back
    
    // Right face (x = 0.5)
     0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   3.0f,  // 12: Bottom-front
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f,   3.0f,  // 13: Bottom-back
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   3.0f,  // 14: Top-back
     0.5f,  0.5f,  0.5f,   0.0f, 1.0f,   3.0f,  // 15: Top-front
    
    // Bottom face (y = -0.5)
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   4.0f,  // 16
     0.5f, -0.5f, -0.5f,   1.0f, 1.0f,   4.0f,  // 17
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   4.0f,  // 18
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   4.0f,  // 19
    
    // Top face (y = 0.5)
    -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,   5.0f,  // 20
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   5.0f,  // 21
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   5.0f,  // 22
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   5.0f   // 23
};

const GLuint cube_indices[] = {
    // Front face
    0, 1, 2,   0, 2, 3,
    // Back face  
    4, 5, 6,   4, 6, 7,
    // Left face
    8, 9, 10,  8, 10, 11,
    // Right face
    12, 13, 14, 12, 14, 15,
    // Bottom face
    16, 17, 18, 16, 18, 19,
    // Top face
    20, 21, 22, 20, 22, 23
};
    // ===========================================
    // SETUP EXISTING 2D QUAD (for other effects)
    // ===========================================
    glGenVertexArrays(1, &server.quad_vao);
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main B (after GenVertexArrays): 0x%x", gl_error); }
    glBindVertexArray(server.quad_vao);
    wlr_log(WLR_DEBUG, "MAIN_VAO_SETUP: VAO ID: %d bound", server.quad_vao);
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main C (after BindVertexArray): 0x%x", gl_error); }

    glGenBuffers(1, &server.quad_vbo);
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main D (after GenBuffers VBO): 0x%x", gl_error); }
    glBindBuffer(GL_ARRAY_BUFFER, server.quad_vbo); 
    wlr_log(WLR_DEBUG, "MAIN_VAO_SETUP: VBO ID: %d bound to GL_ARRAY_BUFFER", server.quad_vbo);
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main E (after BindBuffer VBO): 0x%x", gl_error); }
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main F (after BufferData VBO): 0x%x", gl_error); }

    glGenBuffers(1, &server.quad_ibo);
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main G (after GenBuffers IBO): 0x%x", gl_error); }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, server.quad_ibo); 
    wlr_log(WLR_DEBUG, "MAIN_VAO_SETUP: IBO ID: %d bound to GL_ELEMENT_ARRAY_BUFFER (associated with VAO %d)", server.quad_ibo, server.quad_vao);
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main H (after BindBuffer IBO): 0x%x", gl_error); }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error main I (after BufferData IBO): 0x%x", gl_error); }

    // Set up 2D quad vertex attributes (existing code)
    glBindVertexArray(server.quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, server.quad_vbo);
    
    // Position attribute at fixed location 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    wlr_log(WLR_DEBUG, "VAO Setup: Enabled attrib 0 for position");

    // Texcoord attribute at fixed location 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    wlr_log(WLR_DEBUG, "VAO Setup: Enabled attrib 1 for texcoord");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, server.quad_ibo);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ===========================================
    // SETUP NEW CUBE GEOMETRY
    // ===========================================
    glGenVertexArrays(1, &server.cube_vao);
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error cube A (after GenVertexArrays): 0x%x", gl_error); }
    glBindVertexArray(server.cube_vao);
    wlr_log(WLR_DEBUG, "CUBE_VAO_SETUP: VAO ID: %d bound", server.cube_vao);

    glGenBuffers(1, &server.cube_vbo);
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error cube B (after GenBuffers VBO): 0x%x", gl_error); }
    glBindBuffer(GL_ARRAY_BUFFER, server.cube_vbo); 
    wlr_log(WLR_DEBUG, "CUBE_VAO_SETUP: VBO ID: %d bound", server.cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW);
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error cube C (after BufferData VBO): 0x%x", gl_error); }

    glGenBuffers(1, &server.cube_ibo);
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error cube D (after GenBuffers IBO): 0x%x", gl_error); }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, server.cube_ibo); 
    wlr_log(WLR_DEBUG, "CUBE_VAO_SETUP: IBO ID: %d bound", server.cube_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    while ((gl_error = glGetError()) != GL_NO_ERROR) { wlr_log(WLR_ERROR, "GL Error cube E (after BufferData IBO): 0x%x", gl_error); }

    // Set up cube vertex attributes
    // Position attribute at location 0 (vec3)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    wlr_log(WLR_DEBUG, "CUBE Setup: Enabled attrib 0 for position (vec3)");

    // Texcoord attribute at location 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    wlr_log(WLR_DEBUG, "CUBE Setup: Enabled attrib 1 for texcoord");

    // Face ID attribute at location 2
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
    wlr_log(WLR_DEBUG, "CUBE Setup: Enabled attrib 2 for face_id");

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    wlr_egl_unset_current(egl_main);
}else {
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