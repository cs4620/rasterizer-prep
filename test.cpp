
// Sokol is a minimal cross-platform graphics library that wraps OpenGL and other APIs.
// These defines and includes set up Sokol for OpenGL rendering and logging.
#define SOKOL_IMPL
#define SOKOL_GLCORE
#include "sokol_app.h"   // Window and input management
#include "sokol_gfx.h"   // Graphics API (OpenGL abstraction)
#include "sokol_glue.h"  // Platform glue code
#include "file_util.h"   // Utility for loading shader files


// --- STATE ---
// This struct holds all the GPU state needed for rendering.
// 'pip' is the pipeline (shaders and how vertex data is interpreted).
// 'bind' holds buffers and textures to use for drawing.
// 'pass_action' describes what happens at the start of each frame (e.g., clear color).
struct State {
    sg_pipeline pip;         // Pipeline object (shaders, vertex layout)
    sg_bindings bind;        // Vertex buffer bindings
    sg_pass_action pass_action; // Framebuffer clear and actions
};
static struct State state;   // Global state instance


// This function is called once at startup to initialize graphics resources.
void init(void) {
    // Describe the graphics context and initialize Sokol (and OpenGL under the hood).
    sg_desc desc;
    memset(&desc, 0, sizeof(desc)); // Zero out the struct
    desc.environment = sglue_environment(); // Platform-specific setup
    sg_setup(&desc); // Initialize graphics system

    // --- 1. Create Vertex Buffer ---
    // This array defines a triangle with 3 vertices.
    // Each vertex has a position (x, y, z) and a color (r, g, b, a).
    float vertices[] = {
        // positions         // colors
         0.0f,  0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f, // Top vertex (red)
         0.5f, -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 1.0f, // Right vertex (green)
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 1.0f  // Left vertex (blue)
    };
    sg_buffer_desc buf_desc;
    memset(&buf_desc, 0, sizeof(buf_desc)); // Zero out buffer descriptor
    buf_desc.data = SG_RANGE(vertices); // Point to vertex data
    // Create a GPU buffer and store its handle in our state
    state.bind.vertex_buffers[0] = sg_make_buffer(&buf_desc);

    // --- 2. Create Shader from .glsl files ---
    // Shaders are small programs that run on the GPU.
    // Vertex shader: transforms each vertex
    // Fragment shader: colors each pixel
    char* vs_src = load_file("vertex.glsl");   // Load vertex shader source code
    char* fs_src = load_file("fragment.glsl"); // Load fragment shader source code

    sg_shader_desc shd_desc;
    memset(&shd_desc, 0, sizeof(shd_desc)); // Zero out shader descriptor
    shd_desc.vertex_func.source = vs_src;   // Set vertex shader source
    shd_desc.fragment_func.source = fs_src; // Set fragment shader source
    sg_shader shd = sg_make_shader(&shd_desc); // Create shader object

    free(vs_src); // Free loaded source code
    free(fs_src);

    // --- 3. Create Pipeline ---
    // The pipeline tells the GPU how to interpret vertex data and which shaders to use.
    sg_pipeline_desc pip_desc;
    memset(&pip_desc, 0, sizeof(pip_desc)); // Zero out pipeline descriptor
    pip_desc.shader = shd; // Attach our shader

    // Describe the layout of each vertex: first 3 floats are position, next 4 are color
    pip_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3; // position (x, y, z)
    pip_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT4; // color (r, g, b, a)
    
    state.pip = sg_make_pipeline(&pip_desc); // Create pipeline object

    // --- 4. Set Background Color ---
    // This tells Sokol to clear the screen to light gray before drawing each frame.
    state.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR; // Clear framebuffer
    state.pass_action.colors[0].clear_value.r = 0.2f;
    state.pass_action.colors[0].clear_value.g = 0.2f;
    state.pass_action.colors[0].clear_value.b = 0.2f;
    state.pass_action.colors[0].clear_value.a = 1.0f;
}


// This function is called every frame to draw the triangle.
void frame(void) {
    sg_pass pass;
    memset(&pass, 0, sizeof(pass)); // Zero out pass descriptor
    pass.action = state.pass_action; // Use our clear color
    pass.swapchain = sglue_swapchain(); // Get the current framebuffer

    sg_begin_pass(&pass); // Start drawing
    sg_apply_pipeline(state.pip); // Use our pipeline (shaders, layout)
    sg_apply_bindings(&state.bind); // Bind vertex buffer
    sg_draw(0, 3, 1); // Draw 3 vertices (1 triangle)
    sg_end_pass(); // Finish drawing
    sg_commit(); // Present the frame to the window
}


// This function is called when the app exits to clean up resources.
void cleanup(void) {
    sg_shutdown(); // Shut down graphics system
}


// Entry point for Sokol. Sets up the window and callbacks.
sapp_desc sokol_main(int argc, char* argv[]) {
    sapp_desc app;
    memset(&app, 0, sizeof(app)); // Zero out app descriptor
    app.init_cb = init;       // Called once at startup
    app.frame_cb = frame;     // Called every frame
    app.cleanup_cb = cleanup; // Called at exit
    app.width = 800;          // Window width
    app.height = 600;         // Window height
    app.window_title = "Sokol Triangle"; // Window title
    return app;
}