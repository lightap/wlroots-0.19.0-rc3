#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L
#include <wlr/types/wlr_input_device.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <wayland-server-core.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h> 
#include <wlr/types/wlr_pointer.h> 
#include <wlr/backend/interface.h>      // wlr_backend_impl
#include <wlr/interfaces/wlr_output.h>  // wlr_output_impl, wlr_output_init
#include <wlr/render/wlr_renderer.h>    // wlr_renderer_autocreate, wlr_renderer_destroy
#include <wlr/util/log.h>
#include <wlr/types/wlr_output.h>
#include <EGL/egl.h>
#include <EGL/eglext.h> 
#include <GLES2/gl2.h>
#include <wlr/backend/interface.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/interfaces/wlr_output.h>
#include <wlr/util/log.h>
// Add these near the top of backend.c, with other includes
#include <wlr/render/gles2.h>
#include <wlr/render/egl.h>
#include <drm_fourcc.h>      // Add this line near other includes
#include <wayland-util.h>    // Ensure this is included for WL_SHM_FORMAT_* constants

#include <freerdp/freerdp.h>
#include <freerdp/listener.h>
#include <freerdp/version.h>
#include <winpr/crt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>  // Add this for pthread functions
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_subcompositor.h>  // For struct wlr_surface definition
#include <wlr/types/wlr_buffer.h>  
#include <pthread.h>
#include <freerdp/codec/nsc.h>
#include <freerdp/codec/color.h>
#include <winpr/stream.h>
#include <linux/input-event-codes.h> // For BTN_LEFT, BTN_RIGHT, BTN_MIDDLE
#include <wlr/types/wlr_pointer.h>   // For WLR_AXIS_ORIENTATION_VERTICAL, WLR_AXIS_SOURCE_WHEEL

#if WLR_HAS_RDP_BACKEND
#include <wlr/backend/RDP.h>
#endif


#include <wlr/types/wlr_output.h>
#include <wlr/util/log.h>

#include <wlr/types/wlr_seat.h> // For struct wlr_seat
#include <wayland-server-protocol.h> // For WL_POINTER_BUTTON_STATE_* etc.
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_pointer.h> 

static int setter_call_count = 0; 


//#define MAX_FREERDP_FDS 32  // Add this definition near the top of the file

#define DEFAULT_PIXEL_FORMAT PIXEL_FORMAT_BGRX32

// Add this near the top of backend.c with other global variables
//freerdp_peer *global_rdp_peer = NULL;

static pthread_mutex_t rdp_peer_mutex = PTHREAD_MUTEX_INITIALIZER;
//static freerdp_peer *global_rdp_peer = NULL;
static bool rdp_connection_established = false;

#define RDP_PEER_ACTIVATED 0x0001
#define RDP_PEER_OUTPUT_ENABLED 0x0002

#ifndef DRM_FORMAT_R8G8B8A8_UNORM
#define DRM_FORMAT_R8G8B8A8_UNORM 0x34325258 /* 'XR24' */
#endif

#ifndef DRM_FORMAT_B8G8R8A8_UNORM
#define DRM_FORMAT_B8G8R8A8_UNORM 0x34324258 /* 'XB24' */
#endif

#ifndef DRM_FORMAT_A8B8G8R8_UNORM
#define DRM_FORMAT_A8B8G8R8_UNORM 0x34324142 /* 'AB24' */
#endif

/* ------------------------------------------------------------------------
 * FreedRDP placeholders (fake)
 * ------------------------------------------------------------------------ */
typedef struct fake_peer {
    /* FreedRDP peer object placeholders */
} fake_peer;

typedef struct fake_listener {
    /* FreedRDP listener placeholders */
} fake_listener;

 struct wlr_seat *g_rdp_backend_compositor_seat = NULL;

typedef struct {
    freerdp_peer *peer;
    pthread_mutex_t lock;
    bool initialized;
} RDPPeerManager;


void *rdp_transmit_worker(void *arg) ;
// Function to set the global seat (called by the compositor)

void wlr_RDP_backend_set_compositor_seat(struct wlr_seat *seat) {
    setter_call_count++;
    wlr_log(WLR_ERROR, "!!!!!!!!!! RDP_BACKEND.C: wlr_RDP_backend_set_compositor_seat (call #%d). Received seat: %p (%s). Current g_seat before set: %p. Addr of g_seat: %p",
            setter_call_count,
            (void*)seat, seat ? seat->name : "NULL_ARG_SEAT", 
            (void*)g_rdp_backend_compositor_seat, 
            (void*)&g_rdp_backend_compositor_seat); // Log address of the global

    g_rdp_backend_compositor_seat = seat; 

    wlr_log(WLR_ERROR, "!!!!!!!!!! RDP_BACKEND.C: wlr_RDP_backend_set_compositor_seat EXIT. g_rdp_backend_compositor_seat is NOW: %p (%s)",
            (void*)g_rdp_backend_compositor_seat, 
            g_rdp_backend_compositor_seat ? g_rdp_backend_compositor_seat->name : "NULL_G_SEAT_AFTER");
}

/* ------------------------------------------------------------------------
 * wlroots RDP backend definitions
 * ------------------------------------------------------------------------ */

/* Forward declarations 
struct wlr_RDP_output;
struct wlr_RDP_backend;

// At the top of the file with other includes and defines
struct rdp_peers_item {
    freerdp_peer *peer;
    uint32_t flags;
    struct wl_list link;
    struct wlr_seat *seat;
};

struct rdp_peer_context {
    rdpContext context;
    struct wlr_RDP_backend *backend;
    freerdp_peer *peer;
    rdpContext *ctx;
    rdpUpdate *update;
    rdpSettings *settings;
    NSC_CONTEXT* nsc_context;
    wStream* encode_stream;
    
    // Add these missing members
    void *vcm;  // Virtual channel manager
    struct wl_event_source *events[MAX_FREERDP_FDS];
    int loop_task_event_source_fd;
    struct wl_event_source *loop_task_event_source;
    struct wl_list loop_task_list;
    
    struct rdp_peers_item item;

    bool frame_ack_pending;
    struct wl_event_source *frame_timer;
    struct wlr_output *current_output;
};*/





static BOOL xf_mouseEvent(rdpInput *input, UINT16 flags, UINT16 x, UINT16 y);
static BOOL xf_extendedMouseEvent(rdpInput *input, UINT16 flags, UINT16 x, UINT16 y);
BOOL xf_input_keyboard_event(rdpInput *input, UINT16 flags, UINT16 code);
BOOL xf_input_unicode_keyboard_event(rdpInput *input, UINT16 flags, UINT16 code);

// Add these near the top of the file, before any function definitions
BOOL rdp_peer_context_new(freerdp_peer *client, rdpContext *context);
void rdp_peer_context_free(freerdp_peer *client, rdpContext *ctx);
static BOOL rdp_peer_activate(freerdp_peer *client);
static BOOL xf_peer_adjust_monitor_layout(freerdp_peer *client);
static BOOL xf_input_synchronize_event(rdpInput *input, UINT32 flags);
static BOOL rdp_suppress_output(rdpContext *context, BYTE allow, const RECTANGLE_16 *area);


// Placeholder function prototypes
//static int rdp_initialize_dispatch_task_event_source(struct rdp_peer_context *peerCtx);
//static void rdp_destroy_dispatch_task_event_source(struct rdp_peer_context *peerCtx);
static int rdp_rail_peer_init(freerdp_peer *client, struct rdp_peer_context *peerCtx);

static BOOL xf_input_synchronize_event(rdpInput *input, UINT32 flags);
static BOOL xf_peer_adjust_monitor_layout(freerdp_peer *client);

static BOOL xf_peer_post_connect(freerdp_peer *client);

/* Function prototype for wlr_backend_is_RDP */
bool wlr_backend_is_RDP(struct wlr_backend *backend);
// First, declare the function at the top to prevent unused warnings
void rdp_transmit_surface(struct wlr_buffer *buffer);

/* This is our backend’s public constructor */
//struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display);
// /struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display); 
struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display);
struct wlr_allocator *wlr_rdp_allocator_create(struct wlr_renderer *renderer);

// Function declarations
struct wlr_renderer *wlr_gles2_renderer_create_surfaceless(void);
struct wlr_egl *wlr_gles2_renderer_get_egl(struct wlr_renderer *renderer);
bool wlr_egl_make_current(struct wlr_egl *egl);


// Add these with other include statements or function declarations
struct wlr_allocator *wlr_allocator_autocreate(struct wlr_backend *backend, 
                                               struct wlr_renderer *renderer);

// Add these before the implementations
void init_rdp_peer_manager(void);
//static void set_global_rdp_peer(freerdp_peer *peer);
freerdp_peer *global_rdp_peer = NULL;   


// If this function is used across multiple files
void rdp_transmit_surface(struct wlr_buffer *buffer);

// Add these near the top of the file, with other function declarations
static BOOL rdp_peer_activate(freerdp_peer *client);

//static BOOL rdp_peer_initialize(freerdp_peer *client);

// Declare buffer_is_opaque function or include the appropriate header
bool buffer_is_opaque(struct wlr_buffer *buffer);
 

/* Backend and Output Implementations */
static const struct wlr_backend_impl rdp_backend_impl;
static const struct wlr_output_impl rdp_output_impl;


int rdp_peer_init(freerdp_peer *client, struct wlr_RDP_backend *b);



static bool rdp_backend_start(struct wlr_backend *wlr_backend);
static void rdp_backend_destroy(struct wlr_backend *wlr_backend);
//static uint32_t backend_get_buffer_caps(struct wlr_backend *wlr_backend);
static BOOL rdp_incoming_peer(freerdp_listener *instance, freerdp_peer *client);

// First declare the function prototype at the top with other declarations
static int rdp_listener_activity(int fd, uint32_t mask, void *data);

static struct rdp_peer_context *rdp_peer_context_from_input(rdpInput *input);


/*
static uint32_t rdp_backend_get_buffer_caps(struct wlr_backend *wlr_backend) {
    return WLR_BUFFER_CAP_DATA_PTR | WLR_BUFFER_CAP_SHM;  // Only support data pointer and shared memory
}*/


static RDPPeerManager rdp_peer_manager = {
    .peer = NULL,
    .initialized = false
};
/*
static int rdp_initialize_dispatch_task_event_source(struct rdp_peer_context *peerCtx) {
    // Placeholder implementation
    return 1;
}

static void rdp_destroy_dispatch_task_event_source(struct rdp_peer_context *peerCtx) {
    // Placeholder implementation
}*/

// Initialize the mutex in your backend creation or initialization
void init_rdp_peer_manager(void) {
    if (!rdp_peer_manager.initialized) {
        int result = pthread_mutex_init(&rdp_peer_manager.lock, NULL);
        if (result != 0) {
            wlr_log(WLR_ERROR, "Failed to initialize RDP peer mutex: %s", 
                    strerror(result));
        }
        rdp_peer_manager.initialized = true;
    }
}





/* Replace your set_global_rdp_peer function with this version */
freerdp_peer* get_global_rdp_peer(void) {
    freerdp_peer* peer;
    pthread_mutex_lock(&rdp_peer_mutex);
    peer = global_rdp_peer;
    pthread_mutex_unlock(&rdp_peer_mutex);
    printf("[%s:%d] get_global_rdp_peer called, returning: %p\n", 
           __FILE__, __LINE__, peer);
    return peer;
}

void set_global_rdp_peer(freerdp_peer* peer) {
    pthread_mutex_lock(&rdp_peer_mutex);
    global_rdp_peer = peer;
    pthread_mutex_unlock(&rdp_peer_mutex);
    wlr_log(WLR_DEBUG, "set_global_rdp_peer: peer=%p", peer);
}


#include <pthread.h>
#include <time.h>
#include <wlr/types/wlr_buffer.h>
#include <wlr/types/wlr_output.h>
#include <freerdp/freerdp.h>


// Check if the RDP peer is still connected
static bool is_rdp_peer_connected(freerdp_peer *peer) {
    if (!peer || !peer->context) {
        wlr_log(WLR_DEBUG, "RDP peer or context is NULL");
        return false;
    }
    // Check if the peer's socket is still active
    if (!peer->CheckFileDescriptor(peer)) {
        wlr_log(WLR_DEBUG, "RDP peer socket is disconnected");
        return false;
    }
    return true;
}



struct rdp_transmit_job {
    struct wlr_buffer *buffer;
    freerdp_peer *peer;
    void *data;
    uint32_t format;
    size_t stride;
    int width;
    int height;
};

// Global mutex and flag to track transmission state
static pthread_mutex_t transmit_mutex = PTHREAD_MUTEX_INITIALIZER;
static bool transmission_in_progress = false;



void *rdp_transmit_worker(void *arg) {
    struct rdp_transmit_job *job = arg;
    struct wlr_buffer *buffer = job->buffer;
    freerdp_peer *peer = job->peer;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (!is_rdp_peer_connected(peer)) {
        wlr_log(WLR_ERROR, "RDP peer disconnected, skipping transmission");
        wlr_buffer_unlock(buffer);
        free(job);
        
        // Mark transmission as complete
        pthread_mutex_lock(&transmit_mutex);
        transmission_in_progress = false;
        pthread_mutex_unlock(&transmit_mutex);
        
        return NULL;
    }

if (job->width <= 0 || job->height <= 0 || job->stride <= 0) {
    wlr_log(WLR_ERROR, "Invalid job dimensions or stride");
    wlr_buffer_unlock(buffer);
    free(job);
    pthread_mutex_lock(&transmit_mutex);
    transmission_in_progress = false;
    pthread_mutex_unlock(&transmit_mutex);
    return NULL;
}

    struct rdp_peer_context *peerContext = (struct rdp_peer_context *)peer->context;
    rdpSettings *settings = peer->context->settings;
    rdpUpdate *update = peer->context->update;

    wlr_log(WLR_DEBUG, "Transmitting surface: width=%d, height=%d, stride=%zu, format=0x%x",
            job->width, job->height, job->stride, job->format);

    SURFACE_BITS_COMMAND cmd = { 0 };
    cmd.cmdType = CMDTYPE_SET_SURFACE_BITS;
    cmd.bmp.bpp = 32;
    cmd.bmp.width = job->width;
    cmd.bmp.height = job->height;
    cmd.destLeft = 0;
    cmd.destTop = 0;
    cmd.destRight = job->width;
    cmd.destBottom = job->height;

    if (settings->NSCodec && peerContext->nsc_context && peerContext->encode_stream &&
        job->width * job->height > 1920 * 1080) {
        wlr_log(WLR_DEBUG, "Using NSCodec compression");
        cmd.bmp.codecID = settings->NSCodecId;

        if (!Stream_Buffer(peerContext->encode_stream)) {
            wlr_log(WLR_ERROR, "Invalid encode stream");
            wlr_buffer_unlock(buffer);
            free(job);
            
            // Mark transmission as complete
            pthread_mutex_lock(&transmit_mutex);
            transmission_in_progress = false;
            pthread_mutex_unlock(&transmit_mutex);
            
            return NULL;
        }

        Stream_Clear(peerContext->encode_stream);
        Stream_SetPosition(peerContext->encode_stream, 0);

        BOOL nsc_result = nsc_compose_message(peerContext->nsc_context,
                                            peerContext->encode_stream,
                                            (BYTE *)job->data,
                                            job->width,
                                            job->height,
                                            job->stride);
        if (!nsc_result) {
            wlr_log(WLR_ERROR, "NSCodec compression failed");
            wlr_buffer_unlock(buffer);
            free(job);
            
            // Mark transmission as complete
            pthread_mutex_lock(&transmit_mutex);
            transmission_in_progress = false;
            pthread_mutex_unlock(&transmit_mutex);
            
            return NULL;
        }

        cmd.bmp.bitmapDataLength = Stream_GetPosition(peerContext->encode_stream);
        cmd.bmp.bitmapData = Stream_Buffer(peerContext->encode_stream);
    } else {
        wlr_log(WLR_DEBUG, "Using raw bitmap transmission");
        cmd.bmp.codecID = 0;
        cmd.bmp.bitmapDataLength = job->stride * job->height;
        cmd.bmp.bitmapData = (BYTE *)job->data;
    }

    if (!update->SurfaceBits(update->context, &cmd)) {
        wlr_log(WLR_ERROR, "Failed to send surface bits - codec: %d, length: %u",
                cmd.bmp.codecID, cmd.bmp.bitmapDataLength);
    } else {
        wlr_log(WLR_DEBUG, "Surface bits sent successfully");
    }

    wlr_buffer_unlock(buffer);
    free(job);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 +
                        (end.tv_nsec - start.tv_nsec) / 1000000.0;
    wlr_log(WLR_INFO, "rdp_transmit_worker took %.2f ms", elapsed_ms);

    // Mark transmission as complete
    pthread_mutex_lock(&transmit_mutex);
    transmission_in_progress = false;
    pthread_mutex_unlock(&transmit_mutex);

    return NULL;
}

void rdp_transmit_surface(struct wlr_buffer *buffer) {
    static int transmission_attempts = 0;

    // Check if a transmission is already in progress
    pthread_mutex_lock(&transmit_mutex);
    if (transmission_in_progress) {
        pthread_mutex_unlock(&transmit_mutex);
        wlr_log(WLR_DEBUG, "Transmission already in progress, skipping frame");
        return;
    }
    // Mark transmission as starting
    transmission_in_progress = true;
    pthread_mutex_unlock(&transmit_mutex);

    transmission_attempts++;
    wlr_log(WLR_INFO, "RDP Transmission Attempt #%d", transmission_attempts);

    freerdp_peer *peer = get_global_rdp_peer();
    if (!is_rdp_peer_connected(peer)) {
        wlr_log(WLR_ERROR, "Invalid or disconnected RDP peer, skipping transmission");
        
        // Mark transmission as complete since we're not starting one
        pthread_mutex_lock(&transmit_mutex);
        transmission_in_progress = false;
        pthread_mutex_unlock(&transmit_mutex);
        
        return;
    }

    void *data = NULL;
    uint32_t format = 0;
    size_t stride = 0;
    if (!wlr_buffer_begin_data_ptr_access(buffer,
                                         WLR_BUFFER_DATA_PTR_ACCESS_READ,
                                         &data,
                                         &format,
                                         &stride)) {
        wlr_log(WLR_ERROR, "Failed to access buffer data");
        
        // Mark transmission as complete since we're not starting one
        pthread_mutex_lock(&transmit_mutex);
        transmission_in_progress = false;
        pthread_mutex_unlock(&transmit_mutex);
        
        return;
    }

    struct wlr_buffer *locked_buffer = wlr_buffer_lock(buffer);
    if (!locked_buffer) {
        wlr_log(WLR_ERROR, "Failed to lock buffer");
        wlr_buffer_end_data_ptr_access(buffer);
        
        // Mark transmission as complete since we're not starting one
        pthread_mutex_lock(&transmit_mutex);
        transmission_in_progress = false;
        pthread_mutex_unlock(&transmit_mutex);
        
        return;
    }
    wlr_buffer_end_data_ptr_access(buffer);

    struct rdp_transmit_job *job = calloc(1, sizeof(*job));
    if (!job) {
        wlr_log(WLR_ERROR, "Failed to allocate rdp_transmit_job");
        wlr_buffer_unlock(locked_buffer);
        
        // Mark transmission as complete since we're not starting one
        pthread_mutex_lock(&transmit_mutex);
        transmission_in_progress = false;
        pthread_mutex_unlock(&transmit_mutex);
        
        return;
    }
    job->buffer = locked_buffer;
    job->peer = peer;
    job->data = data;
    job->format = format;
    job->stride = stride;
    job->width = buffer->width;
    job->height = buffer->height;

    pthread_t thread;
    if (pthread_create(&thread, NULL, rdp_transmit_worker, job) != 0) {
        wlr_log(WLR_ERROR, "Failed to create transmission thread");
        wlr_buffer_unlock(locked_buffer);
        free(job);
        
        // Mark transmission as complete since we failed to start one
        pthread_mutex_lock(&transmit_mutex);
        transmission_in_progress = false;
        pthread_mutex_unlock(&transmit_mutex);
        
        return;
    }
    pthread_detach(thread);
}


/*void rdp_transmit_surface(struct wlr_buffer *buffer) {
    static int transmission_attempts = 0;

    // Check if a transmission is already in progress
    pthread_mutex_lock(&transmit_mutex);
    if (transmission_in_progress) {
        pthread_mutex_unlock(&transmit_mutex);
        wlr_log(WLR_DEBUG, "Transmission already in progress, skipping frame");
        return;
    }
    // Mark transmission as starting
    transmission_in_progress = true;
    pthread_mutex_unlock(&transmit_mutex);

    transmission_attempts++;
    wlr_log(WLR_INFO, "RDP Transmission Attempt #%d", transmission_attempts);

    freerdp_peer *peer = get_global_rdp_peer();
    if (!is_rdp_peer_connected(peer)) {
        wlr_log(WLR_ERROR, "Invalid or disconnected RDP peer, skipping transmission");
        
        // Mark transmission as complete since we're not starting one
        pthread_mutex_lock(&transmit_mutex);
        transmission_in_progress = false;
        pthread_mutex_unlock(&transmit_mutex);
        
        return;
    }

    void *data = NULL;
    uint32_t format = 0;
    size_t stride = 0;
    if (!wlr_buffer_begin_data_ptr_access(buffer,
                                         WLR_BUFFER_DATA_PTR_ACCESS_READ,
                                         &data,
                                         &format,
                                         &stride)) {
        wlr_log(WLR_ERROR, "Failed to access buffer data");
        
        // Mark transmission as complete since we're not starting one
        pthread_mutex_lock(&transmit_mutex);
        transmission_in_progress = false;
        pthread_mutex_unlock(&transmit_mutex);
        
        return;
    }

    struct wlr_buffer *locked_buffer = wlr_buffer_lock(buffer);
    if (!locked_buffer) {
        wlr_log(WLR_ERROR, "Failed to lock buffer");
        wlr_buffer_end_data_ptr_access(buffer);
        
        // Mark transmission as complete since we're not starting one
        pthread_mutex_lock(&transmit_mutex);
        transmission_in_progress = false;
        pthread_mutex_unlock(&transmit_mutex);
        
        return;
    }

    // Allocate a new buffer for flipped data
    uint8_t *flipped_data = malloc(stride * buffer->height);
    if (!flipped_data) {
        wlr_log(WLR_ERROR, "Failed to allocate flipped buffer");
        wlr_buffer_end_data_ptr_access(buffer);
        wlr_buffer_unlock(locked_buffer);
        
        // Mark transmission as complete since we're not starting one
        pthread_mutex_lock(&transmit_mutex);
        transmission_in_progress = false;
        pthread_mutex_unlock(&transmit_mutex);
        
        return;
    }

    // Flip the buffer vertically
    for (uint32_t y = 0; y < (uint32_t)buffer->height; y++) {
        uint32_t src_y = buffer->height - 1 - y; // Read from bottom to top
        memcpy(flipped_data + y * stride, (uint8_t *)data + src_y * stride, stride);
    }

    wlr_buffer_end_data_ptr_access(buffer);

    struct rdp_transmit_job *job = calloc(1, sizeof(*job));
    if (!job) {
        wlr_log(WLR_ERROR, "Failed to allocate rdp_transmit_job");
        wlr_buffer_unlock(locked_buffer);
        free(flipped_data);
        
        // Mark transmission as complete since we're not starting one
        pthread_mutex_lock(&transmit_mutex);
        transmission_in_progress = false;
        pthread_mutex_unlock(&transmit_mutex);
        
        return;
    }

    // Set job fields with flipped data
    job->buffer = locked_buffer;
    job->peer = peer;
    job->data = flipped_data; // Use flipped data
    job->format = format;
    job->stride = stride;
    job->width = buffer->width;
    job->height = buffer->height;

    pthread_t thread;
    if (pthread_create(&thread, NULL, rdp_transmit_worker, job) != 0) {
        wlr_log(WLR_ERROR, "Failed to create transmission thread");
        wlr_buffer_unlock(locked_buffer);
        free(flipped_data);
        free(job);
        
        // Mark transmission as complete since we failed to start one
        pthread_mutex_lock(&transmit_mutex);
        transmission_in_progress = false;
        pthread_mutex_unlock(&transmit_mutex);
        
        return;
    }
    pthread_detach(thread);
}*/
/*
void rdp_transmit_surface(struct wlr_buffer *buffer) {
    static int transmission_attempts = 0;
    transmission_attempts++;

    freerdp_peer *peer = get_global_rdp_peer();

    wlr_log(WLR_ERROR, "RDP Transmission Attempt #%d", transmission_attempts);
    wlr_log(WLR_ERROR, "Transmitting surface: peer=%p", (void*)peer);

    if (!peer || !peer->context || !peer->context->update) {
        wlr_log(WLR_ERROR, "Invalid peer state during surface transmission");
        return;
    }

    struct rdp_peer_context *peerContext = (struct rdp_peer_context *)peer->context;
    rdpSettings *settings = peer->context->settings;
    rdpUpdate *update = peer->context->update;

    void *data = NULL;
    uint32_t format = 0;
    size_t stride = 0;

    if (!wlr_buffer_begin_data_ptr_access(buffer, 
                                         WLR_BUFFER_DATA_PTR_ACCESS_READ, 
                                         &data, 
                                         &format, 
                                         &stride)) {
        wlr_log(WLR_ERROR, "Failed to access buffer data");
        return;
    }

    wlr_log(WLR_ERROR, "Transmitting surface details:"
            " width=%d, height=%d, stride=%zu, format=0x%x", 
            buffer->width, buffer->height, stride, format);

    // Allocate a temporary buffer for flipped image
    size_t buffer_size = stride * buffer->height;
    BYTE *flipped_data = malloc(buffer_size);
    if (!flipped_data) {
        wlr_log(WLR_ERROR, "Failed to allocate memory for flipped image");
        wlr_buffer_end_data_ptr_access(buffer);
        return;
    }

    // Flip the image vertically by copying rows in reverse order
    for (int y = 0; y < buffer->height; y++) {
        memcpy(flipped_data + (y * stride),
               (BYTE *)data + ((buffer->height - 1 - y) * stride),
               stride);
    }

    // Configure surface bits command
    SURFACE_BITS_COMMAND cmd = { 0 };
    cmd.cmdType = CMDTYPE_SET_SURFACE_BITS;
    cmd.bmp.bpp = 32;
    cmd.bmp.width = buffer->width;
    cmd.bmp.height = buffer->height;
    cmd.destLeft = 0;
    cmd.destTop = 0;
    cmd.destRight = buffer->width;
    cmd.destBottom = buffer->height;

    // Try NSCodec if available
    if (settings->NSCodec && peerContext->nsc_context && peerContext->encode_stream) {
        wlr_log(WLR_DEBUG, "Using NSCodec compression");
        cmd.bmp.codecID = settings->NSCodecId;

        Stream_Clear(peerContext->encode_stream);
        Stream_SetPosition(peerContext->encode_stream, 0);

        wlr_log(WLR_DEBUG, "NSCodec state - context: %p, stream: %p", 
                (void*)peerContext->nsc_context, 
                (void*)peerContext->encode_stream);

        BOOL nsc_result = nsc_compose_message(peerContext->nsc_context,
                                            peerContext->encode_stream,
                                            flipped_data,
                                            buffer->width,
                                            buffer->height,
                                            stride);

        if (!nsc_result) {
            wlr_log(WLR_ERROR, "NSCodec compression failed");
            free(flipped_data);
            wlr_buffer_end_data_ptr_access(buffer);
            return;
        }

        size_t stream_pos = Stream_GetPosition(peerContext->encode_stream);
        wlr_log(WLR_DEBUG, "NSCodec compressed size: %zu bytes", stream_pos);

        cmd.bmp.bitmapDataLength = stream_pos;
        cmd.bmp.bitmapData = Stream_Buffer(peerContext->encode_stream);

    } else {
        wlr_log(WLR_DEBUG, "Using raw bitmap transmission (NSCodec not available - settings: %d, context: %p)", 
                settings->NSCodec,
                peerContext->nsc_context);
        // Fall back to raw bitmap if NSCodec not available
        cmd.bmp.codecID = 0;  // Raw bitmap
        cmd.bmp.bitmapDataLength = stride * buffer->height;
        cmd.bmp.bitmapData = flipped_data;
    }

    if (!update->SurfaceBits(update->context, &cmd)) {
        wlr_log(WLR_ERROR, "Failed to send surface bits - codec: %d, length: %u", 
                cmd.bmp.codecID, cmd.bmp.bitmapDataLength);
        free(flipped_data);
        wlr_buffer_end_data_ptr_access(buffer);
        return;
    }

    wlr_log(WLR_DEBUG, "Surface bits sent successfully");
    free(flipped_data);
    wlr_buffer_end_data_ptr_access(buffer);
}*/
static int rdp_listener_activity(int fd, uint32_t mask, void *data) {
    freerdp_listener* instance = (freerdp_listener*)data;
    
    wlr_log(WLR_ERROR, "RDP LISTENER ACTIVITY");
    wlr_log(WLR_ERROR, "  Current global_rdp_peer: %p", (void*)global_rdp_peer);
    wlr_log(WLR_ERROR, "  Listener instance: %p", (void*)instance);
    wlr_log(WLR_ERROR, "  File descriptor: %d", fd);
    wlr_log(WLR_ERROR, "  Event mask: 0x%x", mask);
    
    if (!instance) {
        wlr_log(WLR_ERROR, "RDP: null listener instance");
        return 0;
    }

    if (mask & WL_EVENT_READABLE) {
        wlr_log(WLR_ERROR, "RDP: Connection attempt detected on fd %d", fd);
        
        if (instance->CheckFileDescriptor(instance) != TRUE) {
            wlr_log(WLR_ERROR, "RDP: Failed to check file descriptor: %s", strerror(errno));
            return 1;
        }
        
        wlr_log(WLR_ERROR, "RDP: Connection accepted");
        wlr_log(WLR_ERROR, "  Global RDP Peer after connection: %p", (void*)global_rdp_peer);
    }
    
    return 1;
}






// Create RDP output function adjusted for your specific wlroots version
// You'll need to customize this based on your actual struct definitions

// Add this prototype near the top of your file
static bool create_rdp_output_for_client(struct wlr_RDP_backend *backend, freerdp_peer *client);

static bool setup_rdp_output_buffers(struct wlr_RDP_output *output, int width, int height) {
    wlr_log(WLR_DEBUG, "RDP: Skipping buffer setup - using existing mechanism");
    
    // Set dimensions in output structure if needed
    output->wlr_output.width = width;
    output->wlr_output.height = height;
    
    return true;
}

static bool create_rdp_output_for_client(struct wlr_RDP_backend *backend, freerdp_peer *client) {
    if (!backend || !client) {
        wlr_log(WLR_ERROR, "RDP: Invalid parameters for output creation");
        return false;
    }

    // Validate backend's event loop
    if (!backend->event_loop) {
        wlr_log(WLR_ERROR, "RDP: Backend has no event loop");
        return false;
    }

    // Get client resolution from settings
    int width = 1920;  // Default width
    int height = 1080; // Default height
    
    if (client->settings) {
        if (client->settings->DesktopWidth > 0) {
            width = client->settings->DesktopWidth;
        }
        if (client->settings->DesktopHeight > 0) {
            height = client->settings->DesktopHeight;
        }
    }
    
    wlr_log(WLR_INFO, "RDP: Creating output with dimensions %dx%d", width, height);
    
    // Allocate and initialize the RDP output
    struct wlr_RDP_output *output = calloc(1, sizeof(struct wlr_RDP_output));
    if (!output) {
        wlr_log(WLR_ERROR, "RDP: Failed to allocate memory for output");
        return false;
    }
    
    // Set output parameters
    output->backend = backend;
    
    // Get the event loop from the display
    struct wl_event_loop *event_loop = backend->event_loop;
    
    // Initialize the output state
    struct wlr_output_state initial_state;
    wlr_output_state_init(&initial_state);
    wlr_output_state_set_enabled(&initial_state, true);
    
    // Create and set up the output mode
    struct wlr_output_mode *mode = calloc(1, sizeof(struct wlr_output_mode));
    if (!mode) {
        wlr_log(WLR_ERROR, "RDP: Failed to allocate memory for output mode");
        wlr_output_state_finish(&initial_state);
        free(output);
        return false;
    }
    
    mode->width = width;
    mode->height = height;
    mode->refresh = 60000; // 60 Hz refresh rate in mHz
    wlr_output_state_set_mode(&initial_state, mode);
    
    // Initialize wlr_output with all required arguments
    wlr_output_init(&output->wlr_output, &backend->backend, &rdp_output_impl, event_loop, &initial_state);
    
    // Clean up the initial state
    wlr_output_state_finish(&initial_state);
    
    // Set output name
    static int output_counter = 0;
    char name[64];
    snprintf(name, sizeof(name), "RDP-%d", output_counter++);
    wlr_output_set_name(&output->wlr_output, name);
    
    // Add mode to output's mode list
    wl_list_insert(&output->wlr_output.modes, &mode->link);
    
    // Set current mode
    output->wlr_output.current_mode = mode;
    output->wlr_output.width = width;
    output->wlr_output.height = height;
    
    // Set enabled state
    output->wlr_output.enabled = true;
    
    // Set up buffers and rendering context for this output
    if (!setup_rdp_output_buffers(output, width, height)) {
        wlr_log(WLR_ERROR, "RDP: Failed to set up output buffers");
        wlr_output_destroy(&output->wlr_output); // This will free the mode if linked
        free(output);
        return false;
    }
    
    // Add to the list of outputs
    wl_list_insert(&backend->outputs, &output->link);
    
    wlr_log(WLR_INFO, "RDP: Output '%s' created successfully", output->wlr_output.name);
    
    // Notify the compositor of the new output
    wl_signal_emit(&backend->backend.events.new_output, &output->wlr_output);
    
    return true;
}

/*
static BOOL rdp_peer_activate(freerdp_peer *client) {
    wlr_log(WLR_INFO, "RDP: Peer Activation for client %p", client);
    if (!client || !client->settings) {
        wlr_log(WLR_ERROR, "RDP: Invalid client during activation");
        set_global_rdp_peer(NULL);
        return FALSE;
    }
    set_global_rdp_peer(client);
    client->settings->SurfaceCommandsEnabled = TRUE;
    wlr_log(WLR_INFO, "RDP: Peer Activated Successfully, global_rdp_peer=%p", global_rdp_peer);

    if (!client->context) {
        wlr_log(WLR_ERROR, "RDP: Invalid client context during activation");
        return TRUE; // Continue with activation but skip frame rendering
    }

    // Get the backend from the peer context
    struct rdp_peer_context *peerCtx = (struct rdp_peer_context *)client->context;
    struct wlr_RDP_backend *rdp_be = peerCtx->backend;
    if (!rdp_be) {
        wlr_log(WLR_ERROR, "RDP: No backend associated with peer context");
        return TRUE; // Continue with activation but skip frame rendering
    }

    // Check if outputs list is initialized and not empty
    if (!rdp_be->outputs.next || rdp_be->outputs.next == &rdp_be->outputs) {
        wlr_log(WLR_DEBUG, "RDP: No outputs available in backend, creating one for this client");

        // Create an output for this session
        if (!create_rdp_output_for_client(rdp_be, client)) {
            wlr_log(WLR_ERROR, "RDP: Failed to create output for client");
            return TRUE; // Continue with activation but skip frame rendering
        }

        // Check again if output was created successfully
        if (!rdp_be->outputs.next || rdp_be->outputs.next == &rdp_be->outputs) {
            wlr_log(WLR_ERROR, "RDP: Output creation succeeded but output list is still empty");
            return TRUE; // Continue with activation but skip frame rendering
        }
    }

    // Extract output from the list
    struct wlr_RDP_output *rdp_output_priv = NULL;
    rdp_output_priv = wl_container_of(rdp_be->outputs.next, rdp_output_priv, link);

    if (!rdp_output_priv) {
        wlr_log(WLR_ERROR, "RDP: Could not get RDP output from list");
        return TRUE; // Continue with activation but skip frame rendering
    }

    // Access wlr_output safely
    struct wlr_output *wlr_output = &rdp_output_priv->wlr_output;
    if (wlr_output) {
        if (wlr_output->enabled) {
            wlr_log(WLR_INFO, "RDP: Peer %p activated, sending initial frame signal for output '%s'",
                   (void*)client, wlr_output->name ? wlr_output->name : "unnamed");
            wlr_output_send_frame(wlr_output); // Kick off rendering for the new peer
        } else {
            wlr_log(WLR_INFO, "RDP: Output found but not enabled, skipping initial frame");
        }
    } else {
        wlr_log(WLR_ERROR, "RDP: Invalid wlr_output structure");
    }

    return TRUE;
}*/
/*
static BOOL rdp_peer_activate(freerdp_peer *client) {
    wlr_log(WLR_INFO, "RDP: Peer Activation for client %p", client);
    if (!client || !client->settings) {
        wlr_log(WLR_ERROR, "RDP: Invalid client during activation");
        set_global_rdp_peer(NULL);
        return FALSE;
    }
    set_global_rdp_peer(client);
    client->settings->SurfaceCommandsEnabled = TRUE;
    wlr_log(WLR_INFO, "RDP: Peer Activated Successfully, global_rdp_peer=%p", global_rdp_peer);

    if (!client->context) {
        wlr_log(WLR_ERROR, "RDP: Invalid client context during activation");
        return TRUE;
    }

    struct rdp_peer_context *peerCtx = (struct rdp_peer_context *)client->context;
    struct wlr_RDP_backend *rdp_be = peerCtx->backend;
    if (!rdp_be) {
        wlr_log(WLR_ERROR, "RDP: No backend associated with peer context");
        return TRUE;
    }

    // Check if outputs list is initialized and not empty
    if (!rdp_be->outputs.next || rdp_be->outputs.next == &rdp_be->outputs) {
        wlr_log(WLR_DEBUG, "RDP: No outputs available in backend, creating one for this client");
        if (!create_rdp_output_for_client(rdp_be, client)) {
            wlr_log(WLR_ERROR, "RDP: Failed to create output for client");
            return TRUE;
        }
    }

    // Extract output from the list
    struct wlr_RDP_output *rdp_output_priv = NULL;
    rdp_output_priv = wl_container_of(rdp_be->outputs.next, rdp_output_priv, link);

    if (!rdp_output_priv) {
        wlr_log(WLR_ERROR, "RDP: Could not get RDP output from list");
        return TRUE;
    }

    struct wlr_output *wlr_output = &rdp_output_priv->wlr_output;
    if (wlr_output) {
        // Set the output in the peer context
        peerCtx->current_output = wlr_output;
        wlr_log(WLR_INFO, "RDP: Associated output '%s' with peer %p",
                wlr_output->name ? wlr_output->name : "unnamed", (void*)client);
        if (wlr_output->enabled) {
            wlr_log(WLR_INFO, "RDP: Peer %p activated, sending initial frame signal for output '%s'",
                   (void*)client, wlr_output->name ? wlr_output->name : "unnamed");
            wlr_output_send_frame(wlr_output);
        } else {
            wlr_log(WLR_INFO, "RDP: Output found but not enabled, skipping initial frame");
        }
    } else {
        wlr_log(WLR_ERROR, "RDP: Invalid wlr_output structure");
    }

    return TRUE;
}*/

static BOOL rdp_peer_activate(freerdp_peer *client) {
     wlr_log(WLR_INFO, "RDP: Peer Activation for client %p", client);
    if (!client || !client->settings) {
        wlr_log(WLR_ERROR, "RDP: Invalid client during activation");
        set_global_rdp_peer(NULL);
        return FALSE;
    }
    set_global_rdp_peer(client);
    client->settings->SurfaceCommandsEnabled = TRUE;
    wlr_log(WLR_INFO, "RDP: Peer Activated Successfully, global_rdp_peer=%p", global_rdp_peer);

    if (!client->context) {
        wlr_log(WLR_ERROR, "RDP: Invalid client context during activation");
        return TRUE; // Continue with activation but skip frame rendering
    }

    // Get the backend from the peer context
    struct rdp_peer_context *peerCtx = (struct rdp_peer_context *)client->context;
    struct wlr_RDP_backend *rdp_be = peerCtx->backend;
    if (!rdp_be) {
        wlr_log(WLR_ERROR, "RDP: No backend associated with peer context");
        return TRUE; // Continue with activation but skip frame rendering
    }

    // Check if outputs list is initialized and not empty
    if (!rdp_be->outputs.next || rdp_be->outputs.next == &rdp_be->outputs) {
        wlr_log(WLR_DEBUG, "RDP: No outputs available in backend, creating one for this client");

        // Create an output for this session
        if (!create_rdp_output_for_client(rdp_be, client)) {
            wlr_log(WLR_ERROR, "RDP: Failed to create output for client");
            return TRUE; // Continue with activation but skip frame rendering
        }

        // Check again if output was created successfully
        if (!rdp_be->outputs.next || rdp_be->outputs.next == &rdp_be->outputs) {
            wlr_log(WLR_ERROR, "RDP: Output creation succeeded but output list is still empty");
            return TRUE; // Continue with activation but skip frame rendering
        }
    }

    // Extract output from the list
    struct wlr_RDP_output *rdp_output_priv = NULL;
    rdp_output_priv = wl_container_of(rdp_be->outputs.next, rdp_output_priv, link);

    if (!rdp_output_priv) {
        wlr_log(WLR_ERROR, "RDP: Could not get RDP output from list");
        return TRUE; // Continue with activation but skip frame rendering
    }

    struct wlr_output *wlr_output = &rdp_output_priv->wlr_output;
    
    // Only send frame if output already exists and is ready
    if (wlr_output && wlr_output->enabled) {
        // Consider making this async or rate-limited
        wlr_output_send_frame(wlr_output);
    }
    
    return TRUE;
}


/*
struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display) {
    wlr_log(WLR_INFO, "Creating RDP backend (surfaceless approach)");

    // Initialize peer manager and clear global peer
    set_global_rdp_peer(NULL);
    rdp_connection_established = false;
    init_rdp_peer_manager();

    // Allocate backend structure
    struct wlr_RDP_backend *backend = calloc(1, sizeof(*backend));
    if (!backend) {
        wlr_log(WLR_ERROR, "Failed to allocate RDP backend");
        return NULL;
    }

    // Initialize backend
    wlr_backend_init(&backend->backend, &rdp_backend_impl);
    backend->backend.buffer_caps = WLR_BUFFER_CAP_DATA_PTR | WLR_BUFFER_CAP_SHM;
    backend->display = display;
    wl_list_init(&backend->outputs);
    wl_list_init(&backend->peers);

    // Get event loop
    backend->event_loop = wl_display_get_event_loop(display);
    if (!backend->event_loop) {
        wlr_log(WLR_ERROR, "Failed to get event loop");
        free(backend);
        return NULL;
    }

    // Create FreeRDP listener
    backend->listener = freerdp_listener_new();
    if (!backend->listener) {
        wlr_log(WLR_ERROR, "RDP Backend: Failed to create listener");
        free(backend);
        return NULL;
    }

    backend->listener->info = backend;
    backend->listener->PeerAccepted = rdp_incoming_peer;

    if (!backend->listener->Open(backend->listener, NULL, 3389)) {
        wlr_log(WLR_ERROR, "RDP Backend: Failed to open listener");
        freerdp_listener_free(backend->listener);
        free(backend);
        return NULL;
    }

    // Get file descriptors for the listener
    void *rfds[32] = {0};
    int rcount = 0;
    if (!backend->listener->GetFileDescriptor(backend->listener, rfds, &rcount)) {
        wlr_log(WLR_ERROR, "Failed to get RDP file descriptors");
        freerdp_listener_free(backend->listener);
        free(backend);
        return NULL;
    }

    // Add file descriptors to event loop
    backend->fd_count = 0;
    for (int i = 0; i < rcount; i++) {
        int fd = (int)(intptr_t)rfds[i];
        struct wl_event_source *source = wl_event_loop_add_fd(
            backend->event_loop, fd, WL_EVENT_READABLE,
            rdp_listener_activity, backend->listener);
        if (!source) {
            wlr_log(WLR_ERROR, "Failed to add fd %d to event loop", fd);
            for (int j = 0; j < backend->fd_count; j++) {
                wl_event_source_remove(backend->fd_event_sources[j]);
            }
            freerdp_listener_free(backend->listener);
            free(backend);
            return NULL;
        }
        backend->fd_event_sources[backend->fd_count++] = source;
        wlr_log(WLR_DEBUG, "Added RDP listener fd %d to event loop", fd);
    }

    // Set environment variables for surfaceless rendering
    setenv("MESA_LOADER_DRIVER_OVERRIDE", "zink", 1);
    setenv("EGL_PLATFORM", "surfaceless", 1);
    setenv("WLR_RENDERER", "gles2", 1);

    // Create renderer
    backend->renderer = wlr_renderer_autocreate(&backend->backend);
    if (!backend->renderer) {
        wlr_log(WLR_ERROR, "Failed to create RDP renderer");
        for (int i = 0; i < backend->fd_count; i++) {
            wl_event_source_remove(backend->fd_event_sources[i]);
        }
        freerdp_listener_free(backend->listener);
        free(backend);
        return NULL;
    }

    // Initialize renderer for Wayland display
    wlr_renderer_init_wl_display(backend->renderer, display);

    // Create allocator
    backend->allocator = wlr_allocator_autocreate(&backend->backend, backend->renderer);
    if (!backend->allocator) {
        wlr_log(WLR_ERROR, "Failed to create RDP allocator");
        wlr_renderer_destroy(backend->renderer);
        for (int i = 0; i < backend->fd_count; i++) {
            wl_event_source_remove(backend->fd_event_sources[i]);
        }
        freerdp_listener_free(backend->listener);
        free(backend);
        return NULL;
    }

    wlr_log(WLR_INFO, "RDP backend created successfully with surfaceless renderer");
    return &backend->backend;
}*/

struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display) {
    wlr_log(WLR_INFO, "Creating RDP backend (surfaceless approach)");

    // Initialize peer manager and clear global peer
    set_global_rdp_peer(NULL);
    rdp_connection_established = false;
    init_rdp_peer_manager();

    // Allocate backend structure
    struct wlr_RDP_backend *backend = calloc(1, sizeof(*backend));
    if (!backend) {
        wlr_log(WLR_ERROR, "Failed to allocate RDP backend");
        return NULL;
    }

    // Initialize backend
    wlr_backend_init(&backend->backend, &rdp_backend_impl);
    backend->backend.buffer_caps = WLR_BUFFER_CAP_DATA_PTR | WLR_BUFFER_CAP_SHM;
    backend->display = display;
    wl_list_init(&backend->outputs);
    wl_list_init(&backend->peers);

    // Get event loop
    backend->event_loop = wl_display_get_event_loop(display);
    if (!backend->event_loop) {
        wlr_log(WLR_ERROR, "Failed to get event loop");
        free(backend);
        return NULL;
    }

    // Create FreeRDP listener
    backend->listener = freerdp_listener_new();
    if (!backend->listener) {
        wlr_log(WLR_ERROR, "RDP Backend: Failed to create listener");
        free(backend);
        return NULL;
    }

    backend->listener->info = backend;
    backend->listener->PeerAccepted = rdp_incoming_peer;

    if (!backend->listener->Open(backend->listener, NULL, 3389)) {
        wlr_log(WLR_ERROR, "RDP Backend: Failed to open listener");
        freerdp_listener_free(backend->listener);
        free(backend);
        return NULL;
    }

    // Get file descriptors for the listener
    void *rfds[32] = {0};
    int rcount = 0;
    if (!backend->listener->GetFileDescriptor(backend->listener, rfds, &rcount)) {
        wlr_log(WLR_ERROR, "Failed to get RDP file descriptors");
        freerdp_listener_free(backend->listener);
        free(backend);
        return NULL;
    }

    // Add file descriptors to event loop
    backend->fd_count = 0;
    for (int i = 0; i < rcount; i++) {
        int fd = (int)(intptr_t)rfds[i];
        struct wl_event_source *source = wl_event_loop_add_fd(
            backend->event_loop, fd, WL_EVENT_READABLE,
            rdp_listener_activity, backend->listener);
        if (!source) {
            wlr_log(WLR_ERROR, "Failed to add fd %d to event loop", fd);
            for (int j = 0; j < backend->fd_count; j++) {
                wl_event_source_remove(backend->fd_event_sources[j]);
            }
            freerdp_listener_free(backend->listener);
            free(backend);
            return NULL;
        }
        backend->fd_event_sources[backend->fd_count++] = source;
        wlr_log(WLR_DEBUG, "Added RDP listener fd %d to event loop", fd);
    }

    // Set environment variables for surfaceless rendering
     setenv("MESA_VK_VERSION_OVERRIDE", "1.2", 1);
    setenv("MESA_LOADER_DRIVER_OVERRIDE", "zink", 1);
   // setenv("GALLIUM_DRIVER", "zink", 1);
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

    // Try creating GLES2 renderer
    wlr_log(WLR_INFO, "Attempting to create GLES2 renderer with surfaceless EGL");


    // 1. Check for client extensions (can use EGL_NO_DISPLAY)
    const char *client_extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    if (!client_extensions) {
        wlr_log(WLR_ERROR, "Could not query EGL client extensions");
        return NULL;
    }
    wlr_log(WLR_INFO, "EGL client extensions: %s", client_extensions);

    bool has_surfaceless = strstr(client_extensions, "EGL_MESA_platform_surfaceless") != NULL;
    bool has_platform_base = strstr(client_extensions, "EGL_EXT_platform_base") != NULL;
    wlr_log(WLR_INFO, "Surfaceless platform support: %s", has_surfaceless ? "YES" : "NO");
    wlr_log(WLR_INFO, "Platform base extension: %s", has_platform_base ? "YES" : "NO");

    if (!has_surfaceless || !has_platform_base) {
        wlr_log(WLR_ERROR, "Required EGL client extensions not available");
        return NULL;
    }

    // 2. Get platform display function
    PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform_display =
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    if (!get_platform_display) {
        wlr_log(WLR_ERROR, "Platform display function not available");
        return NULL;
    }
    wlr_log(WLR_INFO, "Retrieved eglGetPlatformDisplayEXT");

    // 3. Create surfaceless display
    // FIX: Use a different variable name for EGL display
    EGLDisplay egl_display = get_platform_display(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, NULL);
    if (egl_display == EGL_NO_DISPLAY) {
        wlr_log(WLR_ERROR, "Failed to create surfaceless display. Error: 0x%x", eglGetError());
        return NULL;
    }
    wlr_log(WLR_INFO, "Created EGL display: %p", (void*)egl_display);

    // 4. Initialize EGL
    EGLint major, minor;
    if (!eglInitialize(egl_display, &major, &minor)) {
        wlr_log(WLR_ERROR, "EGL initialization failed. Error: 0x%x", eglGetError());
        eglTerminate(egl_display);
        return NULL;
    }
    wlr_log(WLR_INFO, "EGL initialized, version: %d.%d", major, minor);

    // 5. Query display extensions (must use initialized display)
    const char *display_extensions = eglQueryString(egl_display, EGL_EXTENSIONS);
    if (!display_extensions) {
        wlr_log(WLR_ERROR, "Failed to query EGL display extensions. Error: 0x%x", eglGetError());
        eglTerminate(egl_display);
        return NULL;
    }
    wlr_log(WLR_INFO, "EGL display extensions: %s", display_extensions);

    // 6. Set bind API first (before choosing config)
    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        wlr_log(WLR_ERROR, "Failed to bind OpenGL ES API. Error: 0x%x", eglGetError());
        eglTerminate(egl_display);
        return NULL;
    }
    wlr_log(WLR_INFO, "Successfully bound OpenGL ES API");

    // 7. Choose a config with explicit GLES2
    const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    EGLConfig config;
    EGLint num_config;
    if (!eglChooseConfig(egl_display, config_attribs, &config, 1, &num_config) || num_config < 1) {
        wlr_log(WLR_ERROR, "Failed to choose EGL config. Error: 0x%x", eglGetError());
        eglTerminate(egl_display);
        return NULL;
    }
    wlr_log(WLR_INFO, "Found suitable EGL configuration");

    // 8. Create context with explicit version
    const EGLint ctx_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE // Remove profile mask to avoid compatibility issues
    };

    EGLContext context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, ctx_attribs);
    if (context == EGL_NO_CONTEXT) {
        wlr_log(WLR_ERROR, "Failed to create EGL context. Error: 0x%x", eglGetError());
        eglTerminate(egl_display);
        return NULL;
    }
    wlr_log(WLR_INFO, "Created EGL context");

    // 9. Query EGL_CONTEXT_CLIENT_TYPE
    EGLint client_type;
    if (!eglQueryContext(egl_display, context, EGL_CONTEXT_CLIENT_TYPE, &client_type)) {
        wlr_log(WLR_ERROR, "Failed to query EGL_CONTEXT_CLIENT_TYPE. Error: 0x%x", eglGetError());
    } else {
        wlr_log(WLR_INFO, "EGL_CONTEXT_CLIENT_TYPE: 0x%x", client_type);
        if (client_type == EGL_OPENGL_ES_API) {
            wlr_log(WLR_INFO, "Context client type is EGL_OPENGL_ES_API");
        } else {
            wlr_log(WLR_ERROR, "Unexpected context client type: 0x%x", client_type);
            eglDestroyContext(egl_display, context);
            eglTerminate(egl_display);
            return NULL;
        }
    }

    // 10. Create pbuffer surface
    const EGLint pbuffer_attribs[] = {
        EGL_WIDTH, 16,
        EGL_HEIGHT, 16,
        EGL_NONE
    };

    EGLSurface surface = eglCreatePbufferSurface(egl_display, config, pbuffer_attribs);
    if (surface == EGL_NO_SURFACE) {
        wlr_log(WLR_ERROR, "Failed to create pbuffer surface. Error: 0x%x", eglGetError());
        eglDestroyContext(egl_display, context);
        eglTerminate(egl_display);
        return NULL;
    }
    wlr_log(WLR_INFO, "Created EGL pbuffer surface");

    // 11. Make context current to verify it works
    if (!eglMakeCurrent(egl_display, surface, surface, context)) {
        wlr_log(WLR_ERROR, "Failed to make context current. Error: 0x%x", eglGetError());
        eglDestroySurface(egl_display, surface);
        eglDestroyContext(egl_display, context);
        eglTerminate(egl_display);
        return NULL;
    }
    wlr_log(WLR_INFO, "Made EGL context current");

    // 12. Verify the context is GLES2 as expected
    const char *gl_vendor = (const char *)glGetString(GL_VENDOR);
    const char *gl_renderer = (const char *)glGetString(GL_RENDERER);
    const char *gl_version = (const char *)glGetString(GL_VERSION);
    wlr_log(WLR_INFO, "OpenGL ES Vendor: %s", gl_vendor ? gl_vendor : "Unknown");
    wlr_log(WLR_INFO, "OpenGL ES Renderer: %s", gl_renderer ? gl_renderer : "Unknown");
    wlr_log(WLR_INFO, "OpenGL ES Version: %s", gl_version ? gl_version : "Unknown");

   

    // 14. Create wlr_egl with the valid context
    wlr_log(WLR_INFO, "Creating wlr_egl with context");
    struct wlr_egl *wlr_egl = wlr_egl_create_with_context(egl_display, context);
    if (!wlr_egl) {
        wlr_log(WLR_ERROR, "Failed to create wlr_egl with context");
        eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(egl_display, surface);
        eglDestroyContext(egl_display, context);
        eglTerminate(egl_display);
        return NULL;
    }

    backend->renderer = wlr_renderer_autocreate(&backend->backend);
 
    // Query OpenGL state
    const char *vendor = (const char *)glGetString(GL_VENDOR);
    const char *renderer = (const char *)glGetString(GL_RENDERER);
    const char *version = (const char *)glGetString(GL_VERSION);
    const char *shading_lang = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);

    if (!vendor || !renderer || !version || !shading_lang) {
        wlr_log(WLR_ERROR, "Failed to query OpenGL state: context may not be active");
    } else {
        wlr_log(WLR_INFO, "wlr_renderer GL_VENDOR: %s", vendor);
        wlr_log(WLR_INFO, "wlr_renderer GL_RENDERER: %s", renderer);
        wlr_log(WLR_INFO, "wlr_renderer GL_VERSION: %s", version);
        wlr_log(WLR_INFO, "wlr_renderer GL_SHADING_LANGUAGE_VERSION: %s", shading_lang);
    }

/*
    if (!backend->renderer) {
        wlr_log(WLR_ERROR, "Failed to create GLES2 renderer, falling back to Pixman");
        // Fallback to Pixman renderer
        setenv("WLR_RENDERER", "pixman", 1);
        backend->renderer = wlr_renderer_autocreate(&backend->backend);
        if (!backend->renderer) {
            wlr_log(WLR_ERROR, "Failed to create Pixman renderer");
            for (int i = 0; i < backend->fd_count; i++) {
                wl_event_source_remove(backend->fd_event_sources[i]);
            }
            freerdp_listener_free(backend->listener);
            free(backend);
            return NULL;
        }
        wlr_log(WLR_INFO, "Successfully created Pixman renderer");
    } else {
        wlr_log(WLR_INFO, "Successfully created GLES2 renderer");
    }
*/
    // Initialize renderer for Wayland display
    // FIX: Use backend->display which is the original Wayland display
    wlr_renderer_init_wl_display(backend->renderer, backend->display);


    // Query OpenGL state
    vendor = (const char *)glGetString(GL_VENDOR);
    renderer = (const char *)glGetString(GL_RENDERER);
    version = (const char *)glGetString(GL_VERSION);
    shading_lang = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);

    if (!vendor || !renderer || !version || !shading_lang) {
        wlr_log(WLR_ERROR, "Failed to query OpenGL state: context may not be active");
    } else {
        wlr_log(WLR_INFO, "2wlr_renderer GL_VENDOR: %s", vendor);
        wlr_log(WLR_INFO, "2wlr_renderer GL_RENDERER: %s", renderer);
        wlr_log(WLR_INFO, "2wlr_renderer GL_VERSION: %s", version);
        wlr_log(WLR_INFO, "2wlr_renderer GL_SHADING_LANGUAGE_VERSION: %s", shading_lang);
    }

    wlr_log(WLR_INFO, "Successfully created GLES2 renderer");

    // Create allocator
    backend->allocator = wlr_allocator_autocreate(&backend->backend, backend->renderer);
    if (!backend->allocator) {
        wlr_log(WLR_ERROR, "Failed to create RDP allocator");
        wlr_renderer_destroy(backend->renderer);
        for (int i = 0; i < backend->fd_count; i++) {
            wl_event_source_remove(backend->fd_event_sources[i]);
        }
        freerdp_listener_free(backend->listener);
        free(backend);
        return NULL;
    }

     if (!g_rdp_backend_compositor_seat) {
        wlr_log(WLR_ERROR, "RDP Backend: Created, but compositor seat has not been set yet. Will be set before listener starts.");
    }

    wlr_log(WLR_INFO, "RDP: Backend structure created. Listener will open on backend_start.");
    return &backend->backend;
}






// Add this function to handle incoming RDP peers
/*
static void rdp_peer_disconnect(freerdp_peer* client) {
    wlr_log(WLR_ERROR, "RDP: Peer Disconnection");
    
    // Thread-safe peer cleanup
    pthread_mutex_lock(&rdp_peer_mutex);
    if (global_rdp_peer == client) {
        wlr_log(WLR_ERROR, "RDP: Clearing disconnected peer");
        global_rdp_peer = NULL;
        rdp_connection_established = false;
    }
    pthread_mutex_unlock(&rdp_peer_mutex);

    wlr_log(WLR_ERROR, "RDP: Peer Disconnected");
}*/

/*
// Add callback handlers
static BOOL rdp_begin_paint(rdpContext* context) {
    wlr_log(WLR_DEBUG, "RDP: Begin paint");
    return TRUE;
}

static BOOL rdp_end_paint(rdpContext* context) {
    wlr_log(WLR_DEBUG, "RDP: End paint");
    return TRUE;
}*/
/*
static BOOL rdp_peer_post_connect(freerdp_peer* client) {
    wlr_log(WLR_DEBUG, "RDP: Post connect");
    client->settings->CompressionLevel = PACKET_COMPR_TYPE_RDP61;
    return TRUE;
}*/

/*
// Modify the rdp_peer_initialize function to use the existing xf_peer_post_connect
static BOOL rdp_peer_initialize(freerdp_peer* client) {
    wlr_log(WLR_ERROR, "RDP: Peer Initialization START");

    if (!client || !client->context || !client->settings) {
        wlr_log(WLR_ERROR, "Invalid client state");
        return FALSE;
    }

    // Enable basic events
    client->update->BeginPaint = rdp_begin_paint;
    client->update->EndPaint = rdp_end_paint;
    
    // Use the existing xf_peer_post_connect function
    client->PostConnect = xf_peer_post_connect;
    client->Activate = rdp_peer_activate;
    client->Disconnect = rdp_peer_disconnect;

    // Set synchronized drawing
    client->settings->SupportGraphicsPipeline = FALSE;
    client->settings->DrawAllowSkipAlpha = TRUE;
    client->settings->DrawAllowColorSubsampling = TRUE;
    client->settings->DrawAllowDynamicColorFidelity = TRUE;
    
    return TRUE;
}*/

static BOOL xf_peer_capabilities(freerdp_peer* client)
{
    // In the original code, this simply returns TRUE
    return TRUE;
}

static BOOL xf_peer_post_connect(freerdp_peer *client)
{
    // In the original code, this simply returns TRUE
    return TRUE;
}

static BOOL xf_suppress_output(rdpContext *context, BYTE allow, const RECTANGLE_16 *area)
{
    struct rdp_peer_context *peerContext = (struct rdp_peer_context *)context;

    if (allow)
        peerContext->item.flags |= RDP_PEER_OUTPUT_ENABLED;
    else
        peerContext->item.flags &= (~RDP_PEER_OUTPUT_ENABLED);

    return TRUE;
}
#include <wlr/types/wlr_keyboard.h> // Ensure this is included for WLR_MODIFIER_* constants
static BOOL xf_input_synchronize_event(rdpInput *input, UINT32 flags) {
    struct rdp_peer_context *peerCtx = rdp_peer_context_from_input(input);
    if (!peerCtx || !peerCtx->item.seat || !peerCtx->item.keyboard) {
        wlr_log(WLR_ERROR, "RDP: Invalid peer context, seat, or keyboard for sync event");
        return FALSE;
    }
    wlr_log(WLR_DEBUG, "RDP backend: Synchronize Event - Scroll Lock: %d, Num Lock: %d, Caps Lock: %d, Kana Lock: %d",
            flags & KBD_SYNC_SCROLL_LOCK ? 1 : 0,
            flags & KBD_SYNC_NUM_LOCK ? 1 : 0,
            flags & KBD_SYNC_CAPS_LOCK ? 1 : 0,
            flags & KBD_SYNC_KANA_LOCK ? 1 : 0);
    // Update modifier state without emitting key events
    struct wlr_keyboard *keyboard = peerCtx->item.keyboard;
    uint32_t modifiers = 0;
    if (flags & KBD_SYNC_CAPS_LOCK) modifiers |= WLR_MODIFIER_CAPS;
    if (flags & KBD_SYNC_NUM_LOCK) modifiers |= WLR_MODIFIER_MOD5; // Fixed: Use WLR_MODIFIER_MOD5 for Num Lock
    // Note: Kana Lock and Scroll Lock may need additional handling if supported by wlr_keyboard
    // Notify modifiers (use seat-based notification for compatibility)
    struct wlr_seat *seat = peerCtx->item.seat;
    wlr_seat_keyboard_notify_modifiers(seat, &keyboard->modifiers); // Fixed: Use seat-based API
    // Ensure XKB state is clean
    struct xkb_state *state = keyboard->xkb_state;
    if (state) {
        xkb_state_unref(state);
        keyboard->xkb_state = xkb_state_new(keyboard->keymap);
        wlr_log(WLR_DEBUG, "RDP: Reset XKB state during sync event");
    }
    return TRUE;
}
#include <unistd.h> // For usleep
#include <wlr/util/log.h>

#define MAX_RETRIES 30
#define RETRY_DELAY_US 10000 // 10ms delay between retries

static int rdp_client_activity(int fd, uint32_t mask, void *data)
{
    freerdp_peer *client = (freerdp_peer *)data;
    // struct rdp_peer_context *peerContext = (struct rdp_peer_context *)client->context;
    
    // Retry checking the file descriptor
    int retries = 0;
    bool check_success = false;
    
    while (retries < MAX_RETRIES) {
        if (client->CheckFileDescriptor(client)) {
            check_success = true;
            break;
        }
        
        retries++;
        wlr_log(WLR_ERROR, "CheckFileDescriptor failed for client %p (attempt %d/%d)", 
                client, retries, MAX_RETRIES);
        
        if (retries < MAX_RETRIES) {
            // Wait before retrying
            USleep(RETRY_DELAY_US);
        }
    }

    if (!check_success) {
        wlr_log(WLR_ERROR, "Unable to check descriptor for client %p after %d retries", 
                client, MAX_RETRIES);
        goto out_clean;
    }

    // Comment out or remove WTS-specific code for now
    // if (peerContext->vcm) {
    //     if (!WTSVirtualChannelManagerCheckFileDescriptor(peerContext->vcm)) {
    //         wlr_log(WLR_ERROR, "Failed to check FreeRDP WTS virtual channel file descriptor for %p", client);
    //         goto out_clean;
    //     }
    // }

    return 0;

out_clean:
    freerdp_peer_context_free(client);
    freerdp_peer_free(client);
    return 0;
}

static BOOL xf_peer_adjust_monitor_layout(freerdp_peer *client) {
    __attribute__((unused)) struct rdp_peer_context *peerContext = 
        (struct rdp_peer_context *)client->context;
    rdpSettings *settings = client->context->settings;

    wlr_log(WLR_ERROR, "Monitor Layout Adjustment:");
    wlr_log(WLR_ERROR, "  DesktopWidth: %d", settings->DesktopWidth);
    wlr_log(WLR_ERROR, "  DesktopHeight: %d", settings->DesktopHeight);
    wlr_log(WLR_ERROR, "  MonitorCount: %d", settings->MonitorCount);

    return TRUE;
}
/*
BOOL rdp_peer_context_new(freerdp_peer *client, rdpContext *context) {
    struct rdp_peer_context *peer_context = (struct rdp_peer_context *)context;
    
    if (!peer_context) {
        wlr_log(WLR_ERROR, "RDP: Failed to allocate peer context");
        return FALSE;
    }

    // Initialize context fields
    peer_context->peer = client;
    peer_context->backend = NULL;  // Optionally set backend if available
    peer_context->update = client->update;
    peer_context->settings = client->settings;

    peer_context->nsc_context = nsc_context_new();
    if (!peer_context->nsc_context) {
        wlr_log(WLR_ERROR, "Failed to create NSCodec context");
        return FALSE;
    }

    nsc_context_set_parameters(peer_context->nsc_context, 
                             NSC_COLOR_FORMAT, 
                             DEFAULT_PIXEL_FORMAT);

    peer_context->encode_stream = Stream_New(NULL, 65536);
    if (!peer_context->encode_stream) {
        wlr_log(WLR_ERROR, "Failed to create encode stream");
        nsc_context_free(peer_context->nsc_context);
        return FALSE;
    }

    wlr_log(WLR_INFO, "RDP: New peer context created successfully");
    return TRUE;
}*/

BOOL rdp_peer_context_new(freerdp_peer *client, rdpContext *context) {
    struct rdp_peer_context *peer_context = (struct rdp_peer_context *)context;
    
    if (!peer_context) {
        wlr_log(WLR_ERROR, "RDP: Failed to allocate peer context");
        return FALSE;
    }

    // Initialize context fields
    peer_context->peer = client;
    peer_context->backend = NULL;  // Optionally set backend if available
    peer_context->update = client->update;
    peer_context->settings = client->settings;

    // Initialize frame sync fields
    peer_context->frame_ack_pending = false;
    peer_context->current_output = NULL;

    // Create NSCodec context
    peer_context->nsc_context = nsc_context_new();
    if (!peer_context->nsc_context) {
        wlr_log(WLR_ERROR, "Failed to create NSCodec context");
        return FALSE;
    }

    nsc_context_set_parameters(peer_context->nsc_context, 
                             NSC_COLOR_FORMAT, 
                             DEFAULT_PIXEL_FORMAT);

    peer_context->encode_stream = Stream_New(NULL, 65536);
    if (!peer_context->encode_stream) {
        wlr_log(WLR_ERROR, "Failed to create encode stream");
        nsc_context_free(peer_context->nsc_context);
        return FALSE;
    }

    // Set up suppress output callback
    client->update->SuppressOutput = rdp_suppress_output;

    wlr_log(WLR_INFO, "RDP: New peer context created successfully");
    return TRUE;
}

void rdp_peer_context_free(freerdp_peer *client, rdpContext *ctx) {
    struct rdp_peer_context *context = (struct rdp_peer_context *)ctx;
    wlr_log(WLR_INFO, "RDP: free peer context");
    if (global_rdp_peer == client) {
        global_rdp_peer = NULL;
    }
    for (int i = 0; i < MAX_FREERDP_FDS; i++) {
        if (context->events[i]) {
            wl_event_source_remove(context->events[i]);
            context->events[i] = NULL;
        }
    }
    if (context->nsc_context) {
        nsc_context_free(context->nsc_context);
        context->nsc_context = NULL;
    }
    if (context->encode_stream) {
        Stream_Free(context->encode_stream, TRUE);
        context->encode_stream = NULL;
    }
    if (context->item.pointer) {
        free(context->item.pointer->base.name);
        free(context->item.pointer);
        context->item.pointer = NULL;
    }
    if (context->item.keyboard) {
        free(context->item.keyboard->base.name);
        free(context->item.keyboard);
        context->item.keyboard = NULL;
    }
}

/*
// Placeholder for these functions - you'll need to implement them based on your specific requirements
static int rdp_initialize_dispatch_task_event_source(struct rdp_peer_context *peerCtx)
{
    // Placeholder implementation
    return 1;
}

static void rdp_destroy_dispatch_task_event_source(struct rdp_peer_context *peerCtx)
{
    // Placeholder implementation
}
*/
static int rdp_rail_peer_init(freerdp_peer *client, struct rdp_peer_context *peerCtx)
{
    // Placeholder implementation
    return 1;
}


static BOOL rdp_incoming_peer(freerdp_listener *instance, freerdp_peer *client)
{
    struct wlr_RDP_backend *b = (struct wlr_RDP_backend *)instance->info;

    // Directly call rdp_peer_init and handle the initialization
    if (rdp_peer_init(client, b) < 0) {
        wlr_log(WLR_ERROR, "error when treating incoming peer");
        return FALSE;
    }

    return TRUE;
}

#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_pointer.h>

#ifndef WLR_AXIS_ORIENTATION_VERTICAL
enum wlr_axis_orientation {
    WLR_AXIS_ORIENTATION_VERTICAL,
    WLR_AXIS_ORIENTATION_HORIZONTAL,
};
#endif

#ifndef WLR_AXIS_SOURCE_WHEEL
enum wlr_axis_source {
    WLR_AXIS_SOURCE_WHEEL,
    WLR_AXIS_SOURCE_FINGER,
    WLR_AXIS_SOURCE_CONTINUOUS,
    WLR_AXIS_SOURCE_WHEEL_TILT,
};
#endif

// FreeRDP mouse button flags


#include <time.h>


#include <wlr/types/wlr_pointer.h>     // For struct wlr_pointer and its event structs

// ... (get_time_msec definition) ...
// Helper function for time
static uint32_t get_time_msec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)ts.tv_sec * 1000 + (uint32_t)ts.tv_nsec / 1000000;
}


static struct rdp_peer_context *rdp_peer_context_from_input(rdpInput *input) {
    if (!input || !input->context) {
        wlr_log(WLR_ERROR, "Invalid rdpInput or context");
        return NULL;
    }
    return (struct rdp_peer_context *)input->context;
}   
/*
static BOOL xf_mouseEvent(rdpInput *input, UINT16 flags, UINT16 x, UINT16 y) {
    struct rdp_peer_context *peerCtx = rdp_peer_context_from_input(input);
    if (!peerCtx) {
        wlr_log(WLR_ERROR, "No peer context for RDP input");
        return FALSE;
    }

    struct wlr_pointer *pointer = peerCtx->item.pointer;
    if (!pointer) {
        wlr_log(WLR_ERROR, "No pointer associated with RDP peer");
        return FALSE;
    }

    static UINT16 last_x = 0, last_y = 0;
    double delta_x = (double)x - last_x;
    double delta_y = (double)y - last_y;
    last_x = x;
    last_y = y;

    // Motion event
    if (delta_x != 0.0 || delta_y != 0.0) {
        struct wlr_pointer_motion_event motion_event = {
            .pointer = pointer,
            .time_msec = get_time_msec(), // Use wlroots timing
            .delta_x = delta_x,
            .delta_y = delta_y,
            .unaccel_dx = delta_x,
            .unaccel_dy = delta_y,
        };
        wl_signal_emit_mutable(&pointer->events.motion, &motion_event);
        wlr_log(WLR_DEBUG, "RDP mouse motion: delta=(%f,%f)", delta_x, delta_y);
    }

    // Button events
    if (flags & (PTR_FLAGS_BUTTON1 | PTR_FLAGS_BUTTON2 | PTR_FLAGS_BUTTON3)) {
        uint32_t button;
        enum wl_pointer_button_state state;

        if (flags & PTR_FLAGS_BUTTON1) {
            button = BTN_LEFT;
        } else if (flags & PTR_FLAGS_BUTTON2) {
            button = BTN_RIGHT;
        } else if (flags & PTR_FLAGS_BUTTON3) {
            button = BTN_MIDDLE;
        } else {
            return TRUE;
        }

        state = (flags & PTR_FLAGS_DOWN) ? WL_POINTER_BUTTON_STATE_PRESSED : WL_POINTER_BUTTON_STATE_RELEASED;

        struct wlr_pointer_button_event button_event = {
            .pointer = pointer,
            .time_msec = get_time_msec(), // Use wlroots timing
            .button = button,
            .state = state,
        };
        wl_signal_emit_mutable(&pointer->events.button, &button_event);
        wlr_log(WLR_DEBUG, "RDP mouse button: %u, state=%d", button, state);
    }

    return TRUE;
}*/
/*
static BOOL xf_mouseEvent(rdpInput *input, UINT16 flags, UINT16 x, UINT16 y) {
    struct rdp_peer_context *peerCtx = rdp_peer_context_from_input(input);
    if (!peerCtx) {
        wlr_log(WLR_ERROR, "No peer context for RDP input");
        return FALSE;
    }

    struct wlr_pointer *pointer = peerCtx->item.pointer;
    if (!pointer) {
        wlr_log(WLR_ERROR, "No pointer associated with RDP peer");
        return FALSE;
    }

    // Scale coordinates to match compositor's 1024x768 output
    const float compositor_width = 1024.0f;
    const float compositor_height = 768.0f;
    // Assume client resolution (adjust based on your RDP client)
    const float client_width = 1024.0f;  // Example, replace with actual client resolution
    const float client_height = 768.0f; // Example, replace with actual client resolution
    float scale_x = compositor_width / client_width;
    float scale_y = compositor_height / client_height;
    float scaled_x = (float)x * scale_x;
    float scaled_y = (float)y * scale_y;

    wlr_log(WLR_DEBUG, "RDP input: raw=(%u,%u), scaled=(%f,%f), scale=(%f,%f)",
            x, y, scaled_x, scaled_y, scale_x, scale_y);

    static UINT16 last_x = 0, last_y = 0;
    double delta_x = (double)scaled_x - last_x;
    double delta_y = (double)scaled_y - last_y;
    last_x = scaled_x;
    last_y = scaled_y;

    // Motion event
    if (delta_x != 0.0 || delta_y != 0.0) {
        struct wlr_pointer_motion_event motion_event = {
            .pointer = pointer,
            .time_msec = get_time_msec(),
            .delta_x = delta_x,
            .delta_y = delta_y,
            .unaccel_dx = delta_x,
            .unaccel_dy = delta_y,
        };
        wl_signal_emit_mutable(&pointer->events.motion, &motion_event);
        wlr_log(WLR_DEBUG, "RDP mouse motion: delta=(%f,%f), scaled_pos=(%f,%f)",
                delta_x, delta_y, scaled_x, scaled_y);
    }

    // Button events
    if (flags & (PTR_FLAGS_BUTTON1 | PTR_FLAGS_BUTTON2 | PTR_FLAGS_BUTTON3)) {
        uint32_t button;
        enum wl_pointer_button_state state;

        if (flags & PTR_FLAGS_BUTTON1) {
            button = BTN_LEFT;
        } else if (flags & PTR_FLAGS_BUTTON2) {
            button = BTN_RIGHT;
        } else if (flags & PTR_FLAGS_BUTTON3) {
            button = BTN_MIDDLE;
        } else {
            return TRUE;
        }

        state = (flags & PTR_FLAGS_DOWN) ? WL_POINTER_BUTTON_STATE_PRESSED : WL_POINTER_BUTTON_STATE_RELEASED;

        struct wlr_pointer_button_event button_event = {
            .pointer = pointer,
            .time_msec = get_time_msec(),
            .button = button,
            .state = state,
        };
        wl_signal_emit_mutable(&pointer->events.button, &button_event);
        wlr_log(WLR_DEBUG, "RDP mouse button: %u, state=%d, scaled_pos=(%f,%f)",
                button, state, scaled_x, scaled_y);
    }

    return TRUE;
}*/
/*
static BOOL xf_mouseEvent(rdpInput *input, UINT16 flags, UINT16 x, UINT16 y) {
    struct rdp_peer_context *peerCtx = rdp_peer_context_from_input(input);
    if (!peerCtx) {
        wlr_log(WLR_ERROR, "No peer context for RDP input");
        return FALSE;
    }
    struct wlr_pointer *pointer = peerCtx->item.pointer;
    if (!pointer) {
        wlr_log(WLR_ERROR, "No pointer associated with RDP peer");
        return FALSE;
    }
    // Scale coordinates to match compositor's 1024x768 output
    const float compositor_width = 1024.0f;
    const float compositor_height = 768.0f;
    // Assume client resolution (adjust based on your RDP client)
    const float client_width = 1024.0f;  // Example, replace with actual client resolution
    const float client_height = 768.0f; // Example, replace with actual client resolution
    float scale_x = compositor_width / client_width;
    float scale_y = compositor_height / client_height;
    float scaled_x = (float)x * scale_x;
    float scaled_y = (float)y * scale_y;
    wlr_log(WLR_DEBUG, "RDP input: raw=(%u,%u), scaled=(%f,%f), scale=(%f,%f)",
            x, y, scaled_x, scaled_y, scale_x, scale_y);
    
    // Send absolute motion position to the compositor
    struct wlr_pointer_motion_absolute_event abs_event = {
        .pointer = pointer,
        .time_msec = get_time_msec(),
        .x = scaled_x / compositor_width,
        .y = scaled_y / compositor_height,
    };
    wl_signal_emit_mutable(&pointer->events.motion_absolute, &abs_event);
    
    static UINT16 last_x = 0, last_y = 0;
    double delta_x = (double)scaled_x - last_x;
    double delta_y = (double)scaled_y - last_y;
    last_x = scaled_x;
    last_y = scaled_y;
    
    // Motion event
    if (delta_x != 0.0 || delta_y != 0.0) {
        struct wlr_pointer_motion_event motion_event = {
            .pointer = pointer,
            .time_msec = get_time_msec(),
            .delta_x = delta_x,
            .delta_y = delta_y,
            .unaccel_dx = delta_x,
            .unaccel_dy = delta_y,
        };
        wl_signal_emit_mutable(&pointer->events.motion, &motion_event);
        wlr_log(WLR_DEBUG, "RDP mouse motion: delta=(%f,%f), scaled_pos=(%f,%f)",
                delta_x, delta_y, scaled_x, scaled_y);
    }
    
    // Button events - fixed button handling for GTK apps
    // RDP protocol uses specific bit patterns for buttons
    // Left button: flags & PTR_FLAGS_BUTTON1 to check if bit is set, flags & PTR_FLAGS_DOWN to check if pressed
    if (flags & PTR_FLAGS_BUTTON1) {
        enum wl_pointer_button_state state = (flags & PTR_FLAGS_DOWN) ? 
                                            WL_POINTER_BUTTON_STATE_PRESSED : 
                                            WL_POINTER_BUTTON_STATE_RELEASED;
        struct wlr_pointer_button_event button_event = {
            .pointer = pointer,
            .time_msec = get_time_msec(),
            .button = BTN_LEFT,
            .state = state,
        };
        wl_signal_emit_mutable(&pointer->events.button, &button_event);
        wlr_log(WLR_DEBUG, "RDP mouse button: BTN_LEFT, state=%d, pos=(%f,%f)",
                state, scaled_x, scaled_y);
    }
    
    if (flags & PTR_FLAGS_BUTTON2) {
        enum wl_pointer_button_state state = (flags & PTR_FLAGS_DOWN) ? 
                                            WL_POINTER_BUTTON_STATE_PRESSED : 
                                            WL_POINTER_BUTTON_STATE_RELEASED;
        struct wlr_pointer_button_event button_event = {
            .pointer = pointer,
            .time_msec = get_time_msec(),
            .button = BTN_RIGHT,
            .state = state,
        };
        wl_signal_emit_mutable(&pointer->events.button, &button_event);
        wlr_log(WLR_DEBUG, "RDP mouse button: BTN_RIGHT, state=%d, pos=(%f,%f)",
                state, scaled_x, scaled_y);
    }
    
    if (flags & PTR_FLAGS_BUTTON3) {
        enum wl_pointer_button_state state = (flags & PTR_FLAGS_DOWN) ? 
                                            WL_POINTER_BUTTON_STATE_PRESSED : 
                                            WL_POINTER_BUTTON_STATE_RELEASED;
        struct wlr_pointer_button_event button_event = {
            .pointer = pointer,
            .time_msec = get_time_msec(),
            .button = BTN_MIDDLE,
            .state = state,
        };
        wl_signal_emit_mutable(&pointer->events.button, &button_event);
        wlr_log(WLR_DEBUG, "RDP mouse button: BTN_MIDDLE, state=%d, pos=(%f,%f)",
                state, scaled_x, scaled_y);
    }
    
    return TRUE;
}*/

static BOOL xf_mouseEvent(rdpInput *input, UINT16 flags, UINT16 x, UINT16 y) {
    struct rdp_peer_context *peerCtx = rdp_peer_context_from_input(input);
    if (!peerCtx) {
        wlr_log(WLR_ERROR, "No peer context for RDP input");
        return FALSE;
    }
    struct wlr_pointer *pointer = peerCtx->item.pointer;
    if (!pointer) {
        wlr_log(WLR_ERROR, "No pointer associated with RDP peer");
        return FALSE;
    }
 
    // Scale coordinates to match compositor's 1024x768 output
    const float compositor_width = 1024.0f;
    const float compositor_height = 768.0f;
    const float client_width = 1024.0f;  // Replace with actual client resolution
    const float client_height = 768.0f;  // Replace with actual client resolution
    float scale_x = compositor_width / client_width;
    float scale_y = compositor_height / client_height;
    float scaled_x = (float)x * scale_x;
    float scaled_y = (float)y * scale_y;
    wlr_log(WLR_DEBUG, "RDP input: raw=(%u,%u), scaled=(%f,%f), scale=(%f,%f)",
            x, y, scaled_x, scaled_y, scale_x, scale_y);
    
    // Send absolute motion position to the compositor (flipped Y for 180-degree transform)
    struct wlr_pointer_motion_absolute_event abs_event = {
        .pointer = pointer,
        .time_msec = get_time_msec(),
        .x = scaled_x / compositor_width,
        .y = (scaled_y / compositor_height), // Flip Y to match WL_OUTPUT_TRANSFORM_FLIPPED_180
    };
    wl_signal_emit_mutable(&pointer->events.motion_absolute, &abs_event);
    
    static UINT16 last_x = 0, last_y = 0;
    double delta_x = (double)scaled_x - last_x;
    double delta_y = (double)scaled_y - last_y;
    last_x = scaled_x;
    last_y = scaled_y;
    
    // Motion event
    if (delta_x != 0.0 || delta_y != 0.0) {
        struct wlr_pointer_motion_event motion_event = {
            .pointer = pointer,
            .time_msec = get_time_msec(),
            .delta_x = delta_x,
            .delta_y = -delta_y, // Flip delta_y for consistency with transform
            .unaccel_dx = delta_x,
            .unaccel_dy = -delta_y,
        };
        wl_signal_emit_mutable(&pointer->events.motion, &motion_event);
        wlr_log(WLR_DEBUG, "RDP mouse motion: delta=(%f,%f), scaled_pos=(%f,%f)",
                delta_x, -delta_y, scaled_x, scaled_y);
    }
    
    // Button events
    if (flags & PTR_FLAGS_BUTTON1) {
        enum wl_pointer_button_state state = (flags & PTR_FLAGS_DOWN) ? 
                                            WL_POINTER_BUTTON_STATE_PRESSED : 
                                            WL_POINTER_BUTTON_STATE_RELEASED;
        struct wlr_pointer_button_event button_event = {
            .pointer = pointer,
            .time_msec = get_time_msec(),
            .button = BTN_LEFT,
            .state = state,
        };
        wl_signal_emit_mutable(&pointer->events.button, &button_event);
        wlr_log(WLR_DEBUG, "RDP mouse button: BTN_LEFT, state=%d, pos=(%f,%f)",
                state, scaled_x, scaled_y);
    }
    
    if (flags & PTR_FLAGS_BUTTON2) {
        enum wl_pointer_button_state state = (flags & PTR_FLAGS_DOWN) ? 
                                            WL_POINTER_BUTTON_STATE_PRESSED : 
                                            WL_POINTER_BUTTON_STATE_RELEASED;
        struct wlr_pointer_button_event button_event = {
            .pointer = pointer,
            .time_msec = get_time_msec(),
            .button = BTN_RIGHT,
            .state = state,
        };
        wl_signal_emit_mutable(&pointer->events.button, &button_event);
        wlr_log(WLR_DEBUG, "RDP mouse button: BTN_RIGHT, state=%d, pos=(%f,%f)",
                state, scaled_x, scaled_y);
    }
    
    if (flags & PTR_FLAGS_BUTTON3) {
        enum wl_pointer_button_state state = (flags & PTR_FLAGS_DOWN) ? 
                                            WL_POINTER_BUTTON_STATE_PRESSED : 
                                            WL_POINTER_BUTTON_STATE_RELEASED;
        struct wlr_pointer_button_event button_event = {
            .pointer = pointer,
            .time_msec = get_time_msec(),
            .button = BTN_MIDDLE,
            .state = state,
        };
        wl_signal_emit_mutable(&pointer->events.button, &button_event);
        wlr_log(WLR_DEBUG, "RDP mouse button: BTN_MIDDLE, state=%d, pos=(%f,%f)",
                state, scaled_x, scaled_y);
    }
    
    return TRUE;
}



static BOOL xf_extendedMouseEvent(rdpInput *input, UINT16 flags, UINT16 x, UINT16 y)
{
    struct rdp_peer_context *peerContext = (struct rdp_peer_context *)input->context;
    struct wlr_seat *seat = peerContext->item.seat;
    if (!seat) {
        wlr_log(WLR_ERROR, "No seat available for extended mouse event");
        return TRUE;
    }

   
    

    // Notify frame completion
    wlr_seat_pointer_notify_frame(seat);

    return TRUE;
}


#include <wlr/types/wlr_keyboard.h>

BOOL xf_input_keyboard_event(rdpInput *input, UINT16 flags, UINT16 code) {
    struct rdp_peer_context *peerCtx = rdp_peer_context_from_input(input);
    if (!peerCtx || !peerCtx->item.seat || !peerCtx->item.keyboard) {
        wlr_log(WLR_ERROR, "RDP: Invalid peer context, seat, or keyboard for keyboard event");
        return FALSE;
    }
    
    struct wlr_keyboard *keyboard = peerCtx->item.keyboard;
    uint32_t keycode = code; // Adjust FreeRDP scancode
    bool is_pressed = !(flags & KBD_FLAGS_RELEASE);
    wlr_log(WLR_DEBUG, "RDP keyboard event: code=%u, keycode=%u, pressed=%d", code, keycode, is_pressed);
    
    // Create the keyboard event
    struct wlr_keyboard_key_event key_event = {
        .time_msec = get_time_msec(),
        .keycode = keycode,
        .update_state = true,
        .state = is_pressed ? WL_KEYBOARD_KEY_STATE_PRESSED : WL_KEYBOARD_KEY_STATE_RELEASED,
    };
    
    // ONLY emit the key event signal - this will be handled by wlroots and routed
    // through your keyboard_handle_key callback which already handles seat notification
    wl_signal_emit_mutable(&keyboard->events.key, &key_event);
    
    // DO NOT call wlr_seat_keyboard_notify_key here - it causes duplicate key presses!
    // The keyboard_handle_key callback will handle notifying the seat
    
    return TRUE;
}

BOOL xf_input_unicode_keyboard_event(rdpInput *input, UINT16 flags, UINT16 code) {
    struct rdp_peer_context *peerCtx = rdp_peer_context_from_input(input);
    if (!peerCtx || !peerCtx->item.seat || !peerCtx->item.keyboard) {  // Also check for keyboard
        wlr_log(WLR_ERROR, "RDP: Invalid peer context, seat, or keyboard for Unicode keyboard event");
        return FALSE;
    }
    
    wlr_log(WLR_DEBUG, "RDP Unicode keyboard event: code=%u", code);
    
    // Unicode handling is complex - we should use xkb to map from Unicode to keysyms
    // For now, log that we received a Unicode event but don't process it directly
    wlr_log(WLR_INFO, "Unicode keyboard events not fully implemented (received: U+%04X)", code);
    
    // Return success but don't process the event as a standard key press
    // In the future, proper Unicode handling would be implemented here
    return TRUE;
}



/*
// backend.c: rdp_peer_init 
int rdp_peer_init(freerdp_peer *client, struct wlr_RDP_backend *b)
{
    wlr_log(WLR_ERROR, "RDP: Peer Initialization START");

    // Validate inputs
    if (!client || !b) {
        wlr_log(WLR_ERROR, "RDP: Invalid client or backend");
        return -1;
    }

    // Ensure peers list is initialized
    if (wl_list_empty(&b->peers)) {
        wlr_log(WLR_DEBUG, "Initializing RDP peers list");
        wl_list_init(&b->peers);
    }

    // Set context size and callbacks
    client->ContextSize = sizeof(struct rdp_peer_context);
    client->ContextNew = (psPeerContextNew)rdp_peer_context_new;
    client->ContextFree = (psPeerContextFree)rdp_peer_context_free;
    
    // Create peer context
    if (!freerdp_peer_context_new(client)) {
        wlr_log(WLR_ERROR, "Failed to create peer context");
        return -1;
    }

    struct rdp_peer_context *peerCtx = (struct rdp_peer_context *)client->context;
    
    // Validate created context
    if (!peerCtx) {
        wlr_log(WLR_ERROR, "Peer context creation failed");
        return -1;
    }

    // Initialize context fields
    peerCtx->backend = b;
    peerCtx->peer = client;
    peerCtx->ctx = client->context;
    peerCtx->update = client->update;
    peerCtx->settings = client->settings;

     if (g_rdp_backend_compositor_seat) {
        peerCtx->item.seat = g_rdp_backend_compositor_seat;
        wlr_log(WLR_INFO, "RDP: Peer %p using global compositor seat '%s'",
                (void*)client, g_rdp_backend_compositor_seat->name);
    } else {
        wlr_log(WLR_ERROR, "RDP: Global compositor seat (g_rdp_backend_compositor_seat) is NULL when initializing peer %p. Input will not work.", (void*)client);
        peerCtx->item.seat = NULL; // Explicitly set to NULL
        // You might consider this a fatal error and return -1,
        // or let it proceed without input capabilities.
        // For now, let's log and continue, but input won't function.
    }

    // Configure basic RDP settings
    rdpSettings *settings = client->context->settings;
    settings->ServerMode = TRUE;
    settings->ColorDepth = 32;
    settings->NlaSecurity = FALSE;
    settings->TlsSecurity = FALSE;
    settings->RdpSecurity = TRUE;
    settings->UseRdpSecurityLayer = TRUE;

    // Configure operating system settings
    settings->OsMajorType = OSMAJORTYPE_UNIX;
    settings->OsMinorType = OSMINORTYPE_PSEUDO_XSERVER;

    // Graphics and display settings
    settings->RefreshRect = TRUE;
    settings->RemoteFxCodec = FALSE;
    settings->NSCodec = TRUE;
    settings->FrameMarkerCommandEnabled = TRUE;
    settings->SurfaceFrameMarkerEnabled = TRUE;

    // Advanced features
    settings->RemoteApplicationMode = TRUE;
    settings->SupportGraphicsPipeline = TRUE;
    settings->SupportMonitorLayoutPdu = TRUE;
    settings->RedirectClipboard = TRUE;
    settings->HasExtendedMouseEvent = TRUE;
    settings->HasHorizontalWheel = TRUE;

    // Initialize client
    if (!client->Initialize(client)) {
        wlr_log(WLR_ERROR, "RDP: Peer initialization failed");
        freerdp_peer_context_free(client);
        return -1;
    }

    // Set up client callbacks
    client->Capabilities = xf_peer_capabilities;
    client->PostConnect = xf_peer_post_connect;
    client->Activate = rdp_peer_activate;
    client->AdjustMonitorsLayout = xf_peer_adjust_monitor_layout;

    // Update suppression callback
    client->context->update->SuppressOutput = (pSuppressOutput)xf_suppress_output;
    wlr_log(WLR_DEBUG, "Registered SuppressOutput callback: %p", client->context->update->SuppressOutput);

    // Set up input event handlers
    rdpInput *input = client->context->input;
    input->SynchronizeEvent = xf_input_synchronize_event;
    input->MouseEvent = xf_mouseEvent;
    input->ExtendedMouseEvent = xf_extendedMouseEvent;
    input->KeyboardEvent = xf_input_keyboard_event;
    input->UnicodeKeyboardEvent = xf_input_unicode_keyboard_event;

    // Retrieve and add client event handles to event loop
    HANDLE handles[MAX_FREERDP_FDS];
    int handle_count = client->GetEventHandles(client, handles, MAX_FREERDP_FDS);
    
    if (handle_count <= 0) {
        wlr_log(WLR_ERROR, "Unable to retrieve client handles");
        freerdp_peer_context_free(client);
        return -1;
    }

    // Get Wayland event loop
    struct wl_event_loop *loop = wl_display_get_event_loop(b->display);

    // Initialize event sources
    memset(peerCtx->events, 0, sizeof(peerCtx->events));

    // Add file descriptors to event loop
    for (int i = 0; i < handle_count; i++) {
        int fd = GetEventFileDescriptor(handles[i]);
        
        if (fd < 0) {
            wlr_log(WLR_ERROR, "Invalid file descriptor for handle %d", i);
            continue;
        }

        peerCtx->events[i] = wl_event_loop_add_fd(loop, fd, 
                                                  WL_EVENT_READABLE,
                                                  rdp_client_activity, 
                                                  client);
        
        if (!peerCtx->events[i]) {
            wlr_log(WLR_ERROR, "Failed to add file descriptor %d to event loop", fd);
        }
    }

    // Initialize peer item link
    wl_list_init(&peerCtx->item.link);
    peerCtx->item.peer = client;
    peerCtx->item.flags = RDP_PEER_ACTIVATED | RDP_PEER_OUTPUT_ENABLED;
    wlr_log(WLR_DEBUG, "Set peer flags to 0x%x (ACTIVATED | OUTPUT_ENABLED)", peerCtx->item.flags);

    // Add to peers list
    wl_list_insert(&b->peers, &peerCtx->item.link);

    // Optional: Rail (Remote Application Integrated Locally) initialization
    if (rdp_rail_peer_init(client, peerCtx) != 1) {
        wlr_log(WLR_ERROR, "RDP: RAIL peer initialization failed");
        // Consider whether to abort or continue
    }

    wlr_log(WLR_ERROR, "RDP: Peer Initialization COMPLETE");
    return 0;
}

*/
// In wlr_RDP_backend.c, near the top with other wlroots includes

#include <wlr/types/wlr_input_device.h> 
#include <wlr/backend/interface.h>
#include <wlr/types/wlr_input_device.h> // <<<< THIS IS THE IMPORTANT ONE
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_keyboard.h> // If you add virtual keyboard later
#include <wlr/types/wlr_seat.h>
#include <wlr/util/log.h>




int rdp_peer_init(freerdp_peer *client, struct wlr_RDP_backend *b)
{
    wlr_log(WLR_INFO, "RDP: Peer Initialization START for client %p", (void*)client);

    // Validate inputs
    if (!client) {
        wlr_log(WLR_ERROR, "RDP: Invalid client (NULL)");
        return -1;
    }
    if (!b) {
        wlr_log(WLR_ERROR, "RDP: Invalid backend (NULL) for client %p", (void*)client);
        return -1;
    }

    // Ensure peers list is initialized
    if (wl_list_empty(&b->peers)) {
        wlr_log(WLR_DEBUG, "RDP: Initializing backend peers list for client %p", (void*)client);
        wl_list_init(&b->peers);
    }

    // Set context size and callbacks
    client->ContextSize = sizeof(struct rdp_peer_context);
    client->ContextNew = (psPeerContextNew)rdp_peer_context_new;
    client->ContextFree = (psPeerContextFree)rdp_peer_context_free;

    // Create peer context
    if (!freerdp_peer_context_new(client)) {
        wlr_log(WLR_ERROR, "RDP: freerdp_peer_context_new failed for client %p", (void*)client);
        return -1;
    }

    struct rdp_peer_context *peerCtx = (struct rdp_peer_context *)client->context;
    if (!peerCtx) {
        wlr_log(WLR_ERROR, "RDP: client->context is NULL for client %p", (void*)client);
        return -1;
    }

    // Initialize context fields
    peerCtx->backend = b;
    peerCtx->peer = client;
    peerCtx->update = client->update;
    peerCtx->settings = client->settings;

    // Assign global compositor seat
    if (g_rdp_backend_compositor_seat) {
        peerCtx->item.seat = g_rdp_backend_compositor_seat;
        wlr_log(WLR_INFO, "RDP: Peer %p assigned seat '%s' (%p)",
                (void*)client, g_rdp_backend_compositor_seat->name, (void*)g_rdp_backend_compositor_seat);
    } else {
        wlr_log(WLR_INFO, "RDP: Global seat is NULL for peer %p", (void*)client);
        peerCtx->item.seat = NULL;
    }

    // Create wlr_pointer
    peerCtx->item.pointer = calloc(1, sizeof(struct wlr_pointer));
    if (!peerCtx->item.pointer) {
        wlr_log(WLR_ERROR, "RDP: Failed to allocate pointer for client %p", (void*)client);
        freerdp_peer_context_free(client);
        return -1;
    }

    // Initialize wlr_input_device (embedded in wlr_pointer)
    struct wlr_input_device *dev = &peerCtx->item.pointer->base;
    dev->type = WLR_INPUT_DEVICE_POINTER;
    dev->name = strdup("rdp-pointer");
    if (!dev->name) {
        wlr_log(WLR_ERROR, "RDP: Failed to allocate name for pointer device");
        free(peerCtx->item.pointer);
        peerCtx->item.pointer = NULL;
        freerdp_peer_context_free(client);
        return -1;
    }
    dev->data = peerCtx->item.pointer; // Optional, for wlr_pointer_from_input_device
    wl_signal_init(&dev->events.destroy);

    // Initialize wlr_pointer signals
    struct wlr_pointer *pointer = peerCtx->item.pointer;
    wl_signal_init(&pointer->events.motion);
    wl_signal_init(&pointer->events.motion_absolute);
    wl_signal_init(&pointer->events.button);
    wl_signal_init(&pointer->events.axis);
    wl_signal_init(&pointer->events.frame);
    wl_signal_init(&pointer->events.swipe_begin);
    wl_signal_init(&pointer->events.swipe_update);
    wl_signal_init(&pointer->events.swipe_end);
    wl_signal_init(&pointer->events.pinch_begin);
    wl_signal_init(&pointer->events.pinch_update);
    wl_signal_init(&pointer->events.pinch_end);
    wl_signal_init(&pointer->events.hold_begin);
    wl_signal_init(&pointer->events.hold_end);

    wlr_log(WLR_DEBUG, "RDP: Created pointer device %p for client %p", (void*)dev, (void*)client);
// Create virtual keyboard
struct wlr_keyboard *keyboard = calloc(1, sizeof(struct wlr_keyboard));
if (!keyboard) {
    wlr_log(WLR_ERROR, "RDP: Failed to allocate keyboard for client %p", (void*)client);
    free(dev->name);
    free(peerCtx->item.pointer);
    peerCtx->item.pointer = NULL;
    freerdp_peer_context_free(client);
    return -1;
}

// Initialize wlr_input_device for keyboard
struct wlr_input_device *keyboard_dev = &keyboard->base;
keyboard_dev->type = WLR_INPUT_DEVICE_KEYBOARD;
keyboard_dev->name = strdup("rdp-keyboard");
if (!keyboard_dev->name) {
    wlr_log(WLR_ERROR, "RDP: Failed to allocate name for keyboard device");
    free(keyboard);
    free(dev->name);
    free(peerCtx->item.pointer);
    peerCtx->item.pointer = NULL;
    freerdp_peer_context_free(client);
    return -1;
}
keyboard_dev->data = keyboard;
wl_signal_init(&keyboard_dev->events.destroy);

// Initialize wlr_keyboard signals
wl_signal_init(&keyboard->events.key);
wl_signal_init(&keyboard->events.modifiers);
wl_signal_init(&keyboard->events.keymap);
wl_signal_init(&keyboard->events.repeat_info); // Added to prevent crash

// Initialize XKB context and keymap
struct xkb_context *xkb_ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
if (!xkb_ctx) {
    wlr_log(WLR_ERROR, "RDP: Failed to create XKB context for keyboard");
    free(keyboard_dev->name);
    free(keyboard);
    free(dev->name);
    free(peerCtx->item.pointer);
    peerCtx->item.pointer = NULL;
    freerdp_peer_context_free(client);
    return -1;
}

struct xkb_keymap *xkb_keymap = xkb_keymap_new_from_names(xkb_ctx, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);
if (!xkb_keymap) {
    wlr_log(WLR_ERROR, "RDP: Failed to create XKB keymap for keyboard");
    xkb_context_unref(xkb_ctx);
    free(keyboard_dev->name);
    free(keyboard);
    free(dev->name);
    free(peerCtx->item.pointer);
    peerCtx->item.pointer = NULL;
    freerdp_peer_context_free(client);
    return -1;
}

// Set the keymap on the keyboard
wlr_keyboard_set_keymap(keyboard, xkb_keymap);
xkb_keymap_unref(xkb_keymap);
xkb_context_unref(xkb_ctx);

wlr_log(WLR_DEBUG, "RDP: Keyboard keymap initialized for device %p", (void*)keyboard_dev);
wlr_log(WLR_DEBUG, "RDP: Created keyboard device %p for client %p", (void*)keyboard_dev, (void*)client);

// Store keyboard in peer context
peerCtx->item.keyboard = keyboard;

// Emit new_input signal for keyboard
wl_signal_emit_mutable(&peerCtx->backend->backend.events.new_input, keyboard_dev);
wlr_log(WLR_INFO, "RDP: Emitted new_input for keyboard device '%s' (%p) for peer %p",
        keyboard_dev->name, (void*)keyboard_dev, (void*)client);

// Configure RDP settings
    rdpSettings *settings = peerCtx->settings;
    if (!settings) {
        wlr_log(WLR_ERROR, "RDP: peerCtx->settings is NULL for peer %p", (void*)client);
        free(dev->name);
        free(peerCtx->item.pointer);
        peerCtx->item.pointer = NULL;
        freerdp_peer_context_free(client);
        return -1;
    }

    settings->ServerMode = TRUE;
    settings->ColorDepth = 32;
    settings->NlaSecurity = FALSE;
    settings->TlsSecurity = FALSE;
    settings->RdpSecurity = TRUE;
    settings->UseRdpSecurityLayer = TRUE;
    settings->OsMajorType = OSMAJORTYPE_UNIX;
    settings->OsMinorType = OSMINORTYPE_PSEUDO_XSERVER;
    settings->RefreshRect = TRUE;
    settings->RemoteFxCodec = FALSE;
    settings->NSCodec = TRUE;
    settings->FrameMarkerCommandEnabled = TRUE;
    settings->SurfaceFrameMarkerEnabled = TRUE;
    settings->RemoteApplicationMode = FALSE;
    settings->SupportGraphicsPipeline = TRUE;
    settings->SupportMonitorLayoutPdu = TRUE;
    settings->RedirectClipboard = TRUE;
    settings->HasExtendedMouseEvent = TRUE;
    settings->HasHorizontalWheel = TRUE;
    

    // Initialize FreeRDP peer
    if (!client->Initialize(client)) {
        wlr_log(WLR_ERROR, "RDP: client->Initialize failed for peer %p", (void*)client);
        free(dev->name);
        free(peerCtx->item.pointer);
        peerCtx->item.pointer = NULL;
        freerdp_peer_context_free(client);
        return -1;
    }

    // Set FreeRDP callbacks
    client->Capabilities = xf_peer_capabilities;
    client->PostConnect = xf_peer_post_connect;
    client->Activate = rdp_peer_activate;
    client->AdjustMonitorsLayout = xf_peer_adjust_monitor_layout;
    if (client->update) {
        client->update->SuppressOutput = (pSuppressOutput)xf_suppress_output;
        wlr_log(WLR_DEBUG, "RDP: Registered SuppressOutput callback for peer %p", (void*)client);
    }

    // Set input handlers
    if (client->input) {
        rdpInput *rdp_input = client->input;
        rdp_input->SynchronizeEvent = xf_input_synchronize_event;
        rdp_input->MouseEvent = xf_mouseEvent;
        rdp_input->ExtendedMouseEvent = xf_extendedMouseEvent;
        rdp_input->KeyboardEvent = xf_input_keyboard_event;
        rdp_input->UnicodeKeyboardEvent = xf_input_unicode_keyboard_event;
    } else {
        wlr_log(WLR_ERROR, "RDP: client->input is NULL for peer %p", (void*)client);
    }

    // Add event handles to Wayland event loop
    HANDLE handles[MAX_FREERDP_FDS];
    int handle_count = client->GetEventHandles ? client->GetEventHandles(client, handles, MAX_FREERDP_FDS) : 0;
    if (handle_count <= 0) {
        wlr_log(WLR_ERROR, "RDP: Unable to retrieve event handles (count: %d) for peer %p", handle_count, (void*)client);
        free(dev->name);
        free(peerCtx->item.pointer);
        peerCtx->item.pointer = NULL;
        freerdp_peer_context_free(client);
        return -1;
    }

    struct wl_event_loop *loop = wl_display_get_event_loop(b->display);
    if (!loop) {
        wlr_log(WLR_ERROR, "RDP: Failed to get event loop for peer %p", (void*)client);
        free(dev->name);
        free(peerCtx->item.pointer);
        peerCtx->item.pointer = NULL;
        freerdp_peer_context_free(client);
        return -1;
    }

    memset(peerCtx->events, 0, sizeof(peerCtx->events));
    for (int i = 0; i < handle_count && i < MAX_FREERDP_FDS; i++) {
        int fd = GetEventFileDescriptor(handles[i]);
        if (fd < 0) {
            wlr_log(WLR_ERROR, "RDP: Invalid fd for handle %d for peer %p", i, (void*)client);
            continue;
        }
        peerCtx->events[i] = wl_event_loop_add_fd(loop, fd, WL_EVENT_READABLE, rdp_client_activity, client);
        if (!peerCtx->events[i]) {
            wlr_log(WLR_ERROR, "RDP: Failed to add fd %d to event loop for peer %p", fd, (void*)client);
            for (int j = 0; j < i; j++) {
                if (peerCtx->events[j]) {
                    wl_event_source_remove(peerCtx->events[j]);
                    peerCtx->events[j] = NULL;
                }
            }
            free(dev->name);
            free(peerCtx->item.pointer);
            peerCtx->item.pointer = NULL;
            freerdp_peer_context_free(client);
            return -1;
        }
    }

    // Finalize peer item
    wl_list_init(&peerCtx->item.link);
    peerCtx->item.peer = client;
    peerCtx->item.flags = RDP_PEER_ACTIVATED | RDP_PEER_OUTPUT_ENABLED;
    wl_list_insert(&b->peers, &peerCtx->item.link);

    // Optional RAIL initialization
    if (settings->RemoteApplicationMode) {
        if (rdp_rail_peer_init(client, peerCtx) != 1) {
            wlr_log(WLR_ERROR, "RDP: RAIL initialization failed for peer %p", (void*)client);
        }
    }

    // Emit new_input signal after all setup
    if (peerCtx->backend && peerCtx->item.pointer) {
        wl_signal_emit_mutable(&peerCtx->backend->backend.events.new_input, &peerCtx->item.pointer->base);
        wlr_log(WLR_INFO, "RDP: Emitted new_input for pointer device '%s' (%p) for peer %p",
                dev->name, (void*)dev, (void*)client);
    } else {
        wlr_log(WLR_ERROR, "RDP: Could not emit new_input for peer %p, backend=%p, pointer=%p",
                (void*)client, (void*)peerCtx->backend, (void*)peerCtx->item.pointer);
    }

    wlr_log(WLR_INFO, "RDP: Peer Initialization COMPLETE for client %p", (void*)client);
    return 0;
}

// Add event loop integration for the RDP listener
/**/

/* ------------------------------------------------------------------------
 * Utility & “is RDP backend?” checks
 * ------------------------------------------------------------------------ */

bool wlr_backend_is_RDP(struct wlr_backend *backend) {
    return (backend->impl == &rdp_backend_impl);
}





struct wlr_RDP_backend *RDP_backend_from_backend(struct wlr_backend *backend) {
    assert(wlr_backend_is_RDP(backend));
    return (struct wlr_RDP_backend *)backend;
}

static struct wlr_RDP_output *RDP_output_from_output(struct wlr_output *output) {
    assert(output->impl == &rdp_output_impl);
    return (struct wlr_RDP_output *)output;
}




/* ------------------------------------------------------------------------
 * Output Implementation
 * ------------------------------------------------------------------------ */

static bool rdp_output_test(struct wlr_output *wlr_output, const struct wlr_output_state *state) {
    struct wlr_RDP_output *output = RDP_output_from_output(wlr_output);
fprintf(stderr, "Output name: %s\n", wlr_output->name ? wlr_output->name : "(null)");
fprintf(stderr, "Backend started: %d\n", output->backend->started);
fprintf(stderr, "Committed state ptr: %p\n", state);



    if (state->committed & WLR_OUTPUT_STATE_MODE) {
        const struct wlr_output_mode *mode = state->mode;
        if (!mode) {
            wlr_log(WLR_ERROR, "No mode provided for output %s",
                    wlr_output->name ? wlr_output->name : "(null)");
            return false;
        }
        wlr_log(WLR_DEBUG, "Testing mode %dx%d@%d for output %s",
                mode->width, mode->height, mode->refresh/1000,
                wlr_output->name ? wlr_output->name : "(null)");
        if (mode->width <= 0 || mode->height <= 0) {
            wlr_log(WLR_ERROR, "Invalid mode dimensions %dx%d for output %s",
                    mode->width, mode->height, wlr_output->name ? wlr_output->name : "(null)");
            return false;
        }
    }
    wlr_log(WLR_DEBUG, "Output %s test passed", wlr_output->name ? wlr_output->name : "(null)");
    return true;
}






static BOOL rdp_suppress_output(rdpContext *context, BYTE allow, 
                              const RECTANGLE_16 *area) {
    struct rdp_peer_context *peerCtx = (struct rdp_peer_context *)context;
    
    wlr_log(WLR_DEBUG, "RDP: Output suppression changed: allow=%d", allow);
    wlr_output_send_frame(peerCtx->current_output);
    if (allow) {
        peerCtx->item.flags |= RDP_PEER_OUTPUT_ENABLED;
        
        // If we have a pending frame, send it now
        if (peerCtx->frame_ack_pending && peerCtx->current_output) {
            wlr_output_send_frame(peerCtx->current_output);
            peerCtx->frame_ack_pending = false;
        }
    } else {
        peerCtx->item.flags &= (~RDP_PEER_OUTPUT_ENABLED);
    }
    
    return TRUE;
}




static bool rdp_output_commit(struct wlr_output *wlr_output,
        const struct wlr_output_state *state) {
    struct wlr_RDP_output *output = RDP_output_from_output(wlr_output);
    struct wlr_RDP_backend *backend = output->backend;
    
    if (!backend->started) {
        return false;
    }

    // Handle bufferless commits for surfaceless mode
    if (!(state->committed & WLR_OUTPUT_STATE_BUFFER)) {
        wlr_log(WLR_DEBUG, "Bufferless commit on RDP output");
        return true;
    }
    
    // Get the current RDP peer
    freerdp_peer *peer = get_global_rdp_peer();
    if (!peer || !peer->context) {
        wlr_log(WLR_ERROR, "No active RDP peer for frame commit");
        return false;
    }
    
    struct rdp_peer_context *peerCtx = (struct rdp_peer_context *)peer->context;
    peerCtx->current_output = wlr_output;
    
    // Only send frame event if output is enabled
    if (peerCtx->item.flags & RDP_PEER_OUTPUT_ENABLED) {
        wlr_output_send_frame(wlr_output);
        peerCtx->frame_ack_pending = false;
    } else {
        peerCtx->frame_ack_pending = true;
    }
    
    return true;
}




static void rdp_output_destroy(struct wlr_output *wlr_output) {
    if (!wlr_output) {
        wlr_log(WLR_ERROR, "Attempted to destroy null output");
        return;
    }
    struct wlr_RDP_output *output = RDP_output_from_output(wlr_output);
  //  wlr_log(WLR_INFO, "RDP: destroying output '%s'", wlr_output->name ? wlr_output->name : "(null)");
    if (wl_list_empty(&output->link)) {
//        wlr_log(WLR_DEBUG, "Output %s link already removed", wlr_output->name ? wlr_output->name : "(null)");
    } else {
        wl_list_remove(&output->link);
        wl_list_init(&output->link); // Prevent double removal
    }
    wlr_output_destroy(wlr_output);
    free(output);
}

static size_t rdp_output_get_gamma_size(struct wlr_output *wlr_output) {
    return 0; /* No gamma-lut support in RDP skeleton */
}

static const struct wlr_drm_format_set *rdp_output_get_cursor_formats(
        struct wlr_output *o, uint32_t buffer_flags) {
    return NULL; /* Not supporting hardware cursors in this example */
}

static const struct wlr_output_impl rdp_output_impl = {
    .destroy = rdp_output_destroy,
    .test = rdp_output_test,
    .commit = rdp_output_commit,
    .get_gamma_size = rdp_output_get_gamma_size,
    .get_cursor_formats = rdp_output_get_cursor_formats,
};

// Function prototypes to add at the top of your file
//static int rdp_output_frame(void *data);
//static bool rdp_output_init_render(struct wlr_RDP_output *output, int width, int height);
/*
// Implementation of rdp_output_frame
static int rdp_output_frame(void *data) {
    struct wlr_RDP_output *output = data;
    
    // Don't render if we're not enabled
    if (!output->wlr_output.enabled) {
        wl_event_source_timer_update(output->frame_timer, 1000);
        return 0;
    }
    
    // Calculate refresh interval based on the current mode
    struct wlr_output_mode *mode = output->wlr_output.current_mode;
    int refresh_ms = 1000000 / (mode->refresh / 1000);
    
    // Schedule next frame
    wl_event_source_timer_update(output->frame_timer, refresh_ms);
    
    // Signal a new frame
    wlr_output_send_frame(&output->wlr_output);
    
    return 0;
}

// Implementation of rdp_output_init_render
static bool rdp_output_init_render(struct wlr_RDP_output *output, int width, int height) {
   // struct wlr_RDP_backend *backend = output->backend;
    
    wlr_log(WLR_DEBUG, "Initializing rendering resources for RDP output");
    
    // Initialize the rendering context for this output
    // This is a simplified implementation - you may need to add more setup based 
    // on your specific rendering needs
    
   // output->width = width;
 //   output->height = height;
    
    // Set up any additional rendering resources needed
    // ...
    
    return true;
}
*/
/* Creates one “virtual” RDP output. */
#include <wlr/render/swapchain.h> // Added for wlr_swapchain
// In your RDP backend (e.g., backend/RDP/backend.c)

// Make static if only used in this file.
// Add prototype if used from other .c files in this module.
static struct wlr_RDP_output *rdp_output_create(
        struct wlr_RDP_backend *backend,
        const char *name,
        int width, int height, int refresh_mhz) {

 // Initialize peer manager and clear global peer
    set_global_rdp_peer(NULL);
    rdp_connection_established = false;
    init_rdp_peer_manager();

    wlr_log(WLR_DEBUG, "RDP Backend: Creating RDP output: %s %dx%d@%dmHz",
            name, width, height, refresh_mhz);

    struct wlr_RDP_output *output_priv = calloc(1, sizeof(struct wlr_RDP_output));
    if (!output_priv) {
        wlr_log(WLR_ERROR, "Failed to allocate wlr_RDP_output");
        return NULL;
    }
    output_priv->backend = backend;

    struct wlr_output *wlr_output = &output_priv->wlr_output;

    struct wl_event_loop *event_loop = wl_display_get_event_loop(backend->display);
    if (!event_loop) {
        wlr_log(WLR_ERROR, "RDP Backend: Failed to get event loop for RDP output %s", name);
        free(output_priv);
        return NULL;
    }

    struct wlr_output_state initial_state;
    wlr_output_state_init(&initial_state);
    wlr_output_state_set_enabled(&initial_state, true);

    struct wlr_output_mode *current_mode_obj = calloc(1, sizeof(struct wlr_output_mode));
    if (!current_mode_obj) {
        wlr_log(WLR_ERROR, "RDP Backend: Failed to allocate wlr_output_mode");
        wlr_output_state_finish(&initial_state);
        free(output_priv);
        return NULL;
    }
    current_mode_obj->width = width;
    current_mode_obj->height = height;
    current_mode_obj->refresh = refresh_mhz;
    current_mode_obj->preferred = true;

    wlr_output_state_set_mode(&initial_state, current_mode_obj);

    wlr_output_init(wlr_output, &backend->backend, &rdp_output_impl, event_loop, &initial_state);
    wlr_output_state_finish(&initial_state);

    bool mode_was_copied_by_init = false;
    bool mode_is_correctly_current = false;

    if (wlr_output->current_mode) {
        if (wlr_output->current_mode->width == current_mode_obj->width &&
            wlr_output->current_mode->height == current_mode_obj->height &&
            wlr_output->current_mode->refresh == current_mode_obj->refresh) {
            mode_is_correctly_current = true;
            if (wlr_output->current_mode != current_mode_obj) {
                // current_mode has the right data, but it's not our original pointer.
                // This implies wlr_output_init (or a helper) made a copy and linked that.
                mode_was_copied_by_init = true;
            }
        }
    }

    if (mode_was_copied_by_init) {
        wlr_log(WLR_DEBUG, "RDP Backend: wlr_output_init used a copy of the mode. Freeing original mode object for %s.", name);
        free(current_mode_obj);
        current_mode_obj = NULL; // Mark as freed
    } else if (current_mode_obj != NULL && !mode_is_correctly_current) {
        // current_mode_obj is still around, but it's not (or not correctly) the current mode.
        // This implies wlr_output_init didn't fully set it up from the initial_state.
        wlr_log(WLR_INFO, "RDP Backend: Mode for %s not correctly set as current by init. Attempting to add to list and commit.", name);
        
        bool already_in_list = false;
        struct wlr_output_mode *iter_mode_check;
        wl_list_for_each(iter_mode_check, &wlr_output->modes, link) {
            if (iter_mode_check == current_mode_obj) {
                already_in_list = true;
                break;
            }
        }
        if (!already_in_list) {
            wl_list_insert(&wlr_output->modes,&current_mode_obj->link); // Corrected variable name
        }

        struct wlr_output_state temp_state;
        wlr_output_state_init(&temp_state);
        wlr_output_state_set_enabled(&temp_state, true);
        wlr_output_state_set_mode(&temp_state, current_mode_obj);
        if (!wlr_output_test_state(wlr_output, &temp_state) || !wlr_output_commit_state(wlr_output, &temp_state)) {
             wlr_log(WLR_INFO, "RDP Backend: Failed to make manually added/verified mode current for %s.", name);
        } else {
             wlr_log(WLR_INFO, "RDP Backend: Successfully made manually added/verified mode current for %s.", name);
        }
        wlr_output_state_finish(&temp_state);
    } else if (current_mode_obj != NULL && mode_is_correctly_current && wlr_output->current_mode == current_mode_obj) {
         wlr_log(WLR_DEBUG, "RDP Backend: Original mode object for %s was directly linked as current_mode.", name);
    }


    wlr_output_set_name(wlr_output, name);

    if (!backend->allocator) {
        wlr_log(WLR_DEBUG, "RDP Backend: Creating its specific RDP allocator.");
        backend->allocator = wlr_rdp_allocator_create(backend->renderer);
        if (!backend->allocator) {
            wlr_log(WLR_ERROR, "Failed to create RDP allocator for %s", name);
            wlr_output_destroy(wlr_output); // Frees modes in wlr_output->modes list.
                                            // If current_mode_obj was our original and linked, it's freed by destroy.
            if (current_mode_obj) { // If it's non-NULL, it means it wasn't the "copied by init" case.
                                    // And if it wasn't successfully linked, it needs freeing.
                                    // This specific scenario (allocated, not copied, not linked, then error) is tricky.
                                    // For now, rely on wlr_output_destroy to clean linked modes.
                                    // If current_mode_obj is non-NULL, it means it was not freed as a copy.
                                    // If it also didn't get linked to wlr_output->modes before destroy, it would leak.
                                    // This simplified logic assumes if current_mode_obj is not NULL, and an error occurs,
                                    // and wlr_output_destroy is called, any linked version of it is handled.
                                    // If it was never linked, this is where it would need explicit free.
                                    // However, the path where it's not linked and not copied and init fails is complex.
                                    // Let's assume if current_mode_obj is non-NULL here, it's because it wasn't a copy.
                                    // If it also didn't get into wlr_output->modes, we must free it.
               // bool was_linked = false;
               // struct wlr_output_mode *m;
                // Check against the LIVE list BEFORE destroy, if possible. But here we are after destroy.
                // This is difficult. The safest is: if current_mode_obj wasn't freed (because it wasn't a copy)
                // AND it wasn't the current_mode that wlr_output_destroy would take care of,
                // then it needs explicit free.
                // For simplicity now: if current_mode_obj is not NULL, it means it wasn't considered a copy by init.
                // wlr_output_destroy *should* handle it if it was linked.
                // The risk is if it was allocated, not copied, and never linked.
            }
            free(output_priv);
            return NULL;
        }
        wlr_log(WLR_INFO, "RDP Backend: Created custom RDP allocator: %p", (void*)backend->allocator);
    }

    wlr_log(WLR_INFO, "RDP Backend: Output '%s' created. Final active dimensions: %dx%d. Current mode: %p (%dx%d@%dmHz)",
            wlr_output->name, wlr_output->width, wlr_output->height,
            (void*)wlr_output->current_mode,
            wlr_output->current_mode ? wlr_output->current_mode->width : -1,
            wlr_output->current_mode ? wlr_output->current_mode->height : -1,
            wlr_output->current_mode ? wlr_output->current_mode->refresh : -1
            );

    wl_list_insert(&backend->outputs, &output_priv->link);
    wl_signal_emit_mutable(&backend->backend.events.new_output, wlr_output);

    return output_priv;
}


/* ------------------------------------------------------------------------
 * RDP backend Implementation
 * ------------------------------------------------------------------------ */
static bool rdp_backend_start(struct wlr_backend *wlr_backend) {
    struct wlr_RDP_backend *backend = RDP_backend_from_backend(wlr_backend);
    wlr_log(WLR_ERROR, "!!!!!!!!!! RDP_BACKEND.C: rdp_backend_start ENTER. Checking g_rdp_backend_compositor_seat: %p (%s). Addr of g_seat: %p. Setter call count was: %d", 
            (void*)g_rdp_backend_compositor_seat, 
            g_rdp_backend_compositor_seat ? g_rdp_backend_compositor_seat->name : "NULL_G_SEAT_AT_START",
            (void*)&g_rdp_backend_compositor_seat, // Log address of the global
            setter_call_count);

    if (!g_rdp_backend_compositor_seat) {
        wlr_log(WLR_ERROR, "RDP: [FATAL CHECK] Cannot start backend, compositor seat (g_rdp_backend_compositor_seat) is not set!");
        return false;
    }
    wlr_log(WLR_INFO, "Starting RDP backend");
    backend->started = true; // Set early to allow commits
    

  // CRITICAL: Ensure the compositor seat has been set by now.
    if (!g_rdp_backend_compositor_seat) {
        wlr_log(WLR_ERROR, "RDP: Cannot start backend, compositor seat is not set! Call wlr_RDP_backend_set_compositor_seat() first.");
        return false;
    }

    struct wlr_RDP_output *out = rdp_output_create(backend, "rdp-0", 1024, 768, 60000);
    if (!out) {
        wlr_log(WLR_ERROR, "Failed to create RDP output");
        backend->started = false; // Reset on failure
        return false;
    }
    struct wlr_output_state state = {0};
    state.committed = WLR_OUTPUT_STATE_ENABLED;
    state.enabled = true;
    if (!wlr_output_commit_state(&out->wlr_output, &state)) {
        wlr_log(WLR_ERROR, "Failed to enable RDP output");
        wlr_output_destroy(&out->wlr_output);
        backend->started = false; // Reset on failure
        return false;
    }
    wl_signal_emit_mutable(&backend->backend.events.new_output, &out->wlr_output);
    return true;
}

/*
static void rdp_backend_destroy(struct wlr_backend *wlr_backend) {
    struct wlr_RDP_backend *backend = RDP_backend_from_backend(wlr_backend);
    if (!backend) {
        return;
    }

    // Destroy outputs
    struct wlr_RDP_output *output, *tmp;
    wl_list_for_each_safe(output, tmp, &backend->outputs, link) {
        wlr_output_destroy(&output->wlr_output);
    }

    // Safely destroy renderer
    if (backend->renderer) {
        wlr_renderer_destroy(backend->renderer);
        backend->renderer = NULL;
    }

    wlr_backend_finish(wlr_backend);
    free(backend);
}*/
/*//working but check if memory is cache heavily
static void rdp_backend_destroy(struct wlr_backend *wlr_backend) {
    struct wlr_RDP_backend *backend = RDP_backend_from_backend(wlr_backend);
    if (!backend) {
        return;
    }

    // Remove all event sources
    for (int i = 0; i < backend->fd_count; i++) {
        if (backend->fd_event_sources[i]) {
            wl_event_source_remove(backend->fd_event_sources[i]);
            backend->fd_event_sources[i] = NULL;
        }
    }

    // Destroy outputs
    struct wlr_RDP_output *output, *tmp;
    wl_list_for_each_safe(output, tmp, &backend->outputs, link) {
        wlr_output_destroy(&output->wlr_output);
    }

    // Free FreeRDP listener
    if (backend->listener) {
        freerdp_listener_free(backend->listener);
    }

    // Cleanup peers
    struct rdp_peers_item *peer, *peer_tmp;
    wl_list_for_each_safe(peer, peer_tmp, &backend->peers, link) {
        wl_list_remove(&peer->link);
        // Peer cleanup handled by rdp_peer_context_free
    }

    wlr_backend_finish(wlr_backend);
    free(backend);
}*/
/*//was working before but has a bug
static void rdp_backend_destroy(struct wlr_backend *wlr_backend) {
    struct wlr_RDP_backend *backend = RDP_backend_from_backend(wlr_backend);
    if (!backend) {
        return;
    }

    wlr_log(WLR_DEBUG, "Destroying RDP backend");

    for (int i = 0; i < backend->fd_count; i++) {
        if (backend->fd_event_sources[i]) {
            wlr_log(WLR_DEBUG, "Removing event source %d", i);
            wl_event_source_remove(backend->fd_event_sources[i]);
            backend->fd_event_sources[i] = NULL;
        }
    }

    struct wlr_RDP_output *output, *tmp;
    wl_list_for_each_safe(output, tmp, &backend->outputs, link) {
        wlr_log(WLR_DEBUG, "Destroying output %s", output->wlr_output.name);
        wlr_output_destroy(&output->wlr_output);
    }

    if (backend->listener) {
        wlr_log(WLR_DEBUG, "Freeing FreeRDP listener at %p", (void*)backend->listener);
        freerdp_listener_free(backend->listener);
    }

    struct rdp_peers_item *peer, *peer_tmp;
    int peer_count = 0;
    wl_list_for_each_safe(peer, peer_tmp, &backend->peers, link) {
        peer_count++;
        wlr_log(WLR_DEBUG, "Removing peer %d at %p", peer_count, (void*)peer->peer);
        wl_list_remove(&peer->link);
        if (peer->peer) {
            freerdp_peer_context_free(peer->peer);
            freerdp_peer_free(peer->peer);
        }
    }
    wlr_log(WLR_DEBUG, "Total peers cleaned up: %d", peer_count);

    wlr_backend_finish(wlr_backend);
    free(backend);
    wlr_log(WLR_DEBUG, "RDP backend destroyed - check 'free' memory post-exit");
}*/


static void rdp_backend_destroy(struct wlr_backend *wlr_backend) {
    struct wlr_RDP_backend *backend = RDP_backend_from_backend(wlr_backend);
    if (!backend) {
        wlr_log(WLR_ERROR, "Null backend in rdp_backend_destroy");
        return;
    }
    wlr_log(WLR_INFO, "Destroying RDP backend");
    for (int i = 0; i < backend->fd_count; i++) {
        if (backend->fd_event_sources[i]) {
            wlr_log(WLR_DEBUG, "Removing event source %d", i);
            wl_event_source_remove(backend->fd_event_sources[i]);
            backend->fd_event_sources[i] = NULL;
        }
    }
    backend->fd_count = 0;
    struct wlr_RDP_output *output, *tmp;
    wl_list_for_each_safe(output, tmp, &backend->outputs, link) {
        wlr_log(WLR_DEBUG, "Destroying output %s", output->wlr_output.name ? output->wlr_output.name : "(null)");
        wlr_output_destroy(&output->wlr_output);
    }
    if (backend->listener) {
        wlr_log(WLR_DEBUG, "Freeing FreeRDP listener at %p", (void*)backend->listener);
        freerdp_listener_free(backend->listener);
        backend->listener = NULL;
    }
    struct rdp_peers_item *peer, *peer_tmp;
    int peer_count = 0;
    wl_list_for_each_safe(peer, peer_tmp, &backend->peers, link) {
        peer_count++;
        wlr_log(WLR_DEBUG, "Removing peer %d at %p", peer_count, (void*)peer->peer);
        wl_list_remove(&peer->link);
        if (peer->peer) {
            freerdp_peer_context_free(peer->peer);
            freerdp_peer_free(peer->peer);
        }
    }
    wlr_log(WLR_DEBUG, "Total peers cleaned up: %d", peer_count);
    if (backend->allocator) {
        wlr_allocator_destroy(backend->allocator);
        backend->allocator = NULL;
    }
    if (backend->renderer) {
        wlr_renderer_destroy(backend->renderer);
        backend->renderer = NULL;
    }
    wlr_backend_finish(wlr_backend);
    free(backend);
    wlr_log(WLR_INFO, "RDP backend destroyed");
}





static const struct wlr_backend_impl rdp_backend_impl = {
    .start = rdp_backend_start,
    .destroy = rdp_backend_destroy,

   
};
/* ------------------------------------------------------------------------
 * The Public Constructor for RDP Backend
 * ------------------------------------------------------------------------ */
/*
struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display) {
    wlr_log(WLR_INFO, "Creating RDP backend (surfaceless approach)");

    struct wlr_RDP_backend *backend = calloc(1, sizeof(*backend));
    if (!backend) {
        return NULL;
    }

    wlr_backend_init(&backend->backend, &rdp_backend_impl);
    backend->display = display;
    wl_list_init(&backend->outputs);

    // Set up environment for Zink and surfaceless rendering
    setenv("MESA_LOADER_DRIVER_OVERRIDE", "zink", 1);
    setenv("EGL_PLATFORM", "surfaceless", 1);
    setenv("WLR_RENDERER", "gles2", 1);  // Explicitly set to GLES2

    // Try to create renderer with more relaxed configuration
    struct wlr_renderer *renderer = wlr_renderer_autocreate(&backend->backend);
    if (!renderer) {
        wlr_log(WLR_ERROR, "Failed to create RDP renderer");
        free(backend);
        return NULL;
    }

    // Initialize renderer for Wayland display
    wlr_renderer_init_wl_display(renderer, display);

    wlr_log(WLR_INFO, "RDP backend created successfully with surfaceless renderer");
    return &backend->backend;
}*/


// Modify the backend creation function
/*
struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display) {
    wlr_log(WLR_INFO, "Creating RDP backend (surfaceless approach)");

    struct wlr_RDP_backend *backend = calloc(1, sizeof(*backend));
    if (!backend) {
        wlr_log(WLR_ERROR, "Failed to allocate RDP backend");
        return NULL;
    }

    wlr_backend_init(&backend->backend, &rdp_backend_impl);
    backend->display = display;
    wl_list_init(&backend->outputs);

    
    // Create and store renderer in the backend
    backend->renderer = wlr_renderer_autocreate(&backend->backend);
    if (!backend->renderer) {
        wlr_log(WLR_ERROR, "Failed to create RDP renderer");
        free(backend);
        return NULL;
    }

    // Initialize renderer for Wayland display
    wlr_renderer_init_wl_display(backend->renderer, display);

    wlr_log(WLR_INFO, "RDP backend created successfully with surfaceless renderer");
    return &backend->backend;
}*/
/*
struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display) {
    wlr_log(WLR_INFO, "Creating RDP backend (surfaceless approach)");

    struct wlr_RDP_backend *backend = calloc(1, sizeof(*backend));
    if (!backend) {
        wlr_log(WLR_ERROR, "Failed to allocate RDP backend");
        return NULL;
    }

    wlr_backend_init(&backend->backend, &rdp_backend_impl);
    backend->display = display;
    wl_list_init(&backend->outputs);

    // Create renderer with explicit surfaceless initialization
    backend->renderer = wlr_gles2_renderer_create_surfaceless();
    if (!backend->renderer) {
        wlr_log(WLR_ERROR, "Failed to create RDP renderer");
        free(backend);
        return NULL;
    }

    // Create allocator 
    backend->allocator = wlr_allocator_autocreate(&backend->backend, backend->renderer);
    if (!backend->allocator) {
        wlr_log(WLR_ERROR, "Failed to create RDP allocator");
        wlr_renderer_destroy(backend->renderer);
        free(backend);
        return NULL;
    }

    // Ensure EGL context is current before further operations
    struct wlr_egl *egl = wlr_gles2_renderer_get_egl(backend->renderer);
    if (!egl || !wlr_egl_make_current(egl)) {
        wlr_log(WLR_ERROR, "Failed to make EGL context current");
        wlr_renderer_destroy(backend->renderer);
        free(backend);
        return NULL;
    }

    // Initialize renderer for display
    wlr_renderer_init_wl_display(backend->renderer, display);

    wlr_log(WLR_INFO, "RDP backend created successfully with surfaceless renderer");
    return &backend->backend;
}*/

// In backend/RDP/backend.c

// In backend/RDP/backend.c
/*
struct wlr_backend *wlr_RDP_backend_create(struct wl_display *display) {
    wlr_log(WLR_INFO, "Creating RDP backend (using global renderer)");

    struct wlr_RDP_backend *backend = calloc(1, sizeof(*backend));
    if (!backend) {
        wlr_log(WLR_ERROR, "Failed to allocate wlr_RDP_backend");
        return NULL;
    }

    wlr_backend_init(&backend->backend, &rdp_backend_impl);
    backend->display = display;
    wl_list_init(&backend->outputs);

    // **Attach the global renderer to the backend**
   // backend->renderer = renderer;

    wlr_log(WLR_INFO, "RDP backend created successfully");
    return &backend->backend;
}*/



///////////////////////////////////////////////////////////