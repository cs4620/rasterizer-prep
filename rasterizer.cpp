
// Sokol is a minimal cross-platform graphics library that wraps OpenGL and other APIs.
// These defines and includes set up Sokol for OpenGL rendering and logging.
#define SOKOL_IMPL
#define SOKOL_GLCORE
#include "./includes/sokol_app.h"   // Window and input management
#include "./includes/sokol_gfx.h"   // Graphics API (OpenGL abstraction)
#include "./includes/sokol_log.h"   // Logging
#include "./includes/sokol_glue.h"  // Platform glue code
#include "./includes/file_util.h"   // Utility for loading shader files
#include <cmath>                     // For math functions
#include <iostream>

using namespace std;


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Uniform block matching the vertex shader ---
// Two mat4s: model (world) matrix and view-projection matrix.
// Must match the GLSL uniform block layout exactly.
struct vs_params_t {
    float model[16];      // world / model matrix
    float view_proj[16];  // view-projection matrix
};

// --- STATE ---
struct State {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
    int index_count;
};
struct State state;

//Define a struct for Vector3
//Define a struct for Vector2
//Define a struct for a vertex
//Define a struct for a face


void init(void) {
    sg_desc desc = {};
    desc.environment = sglue_environment();
    desc.logger.func = slog_func;          // Hook up Sokol logger for debug output
    sg_setup(&desc);

    //Create a list of locations, uvs, normals, and faces
    
    //Load the obj file using load_file
    //Get the lines of the file using split

    //Go through each line...
    //Ignore lines that don't start with v, vt, vn, or f
    //Split the line on ' '
    //If its a v, vt, or vn, create the appropriate struct and push it on the appropriate list
    //If its a f, read each vertex definition. 
    //For each vertex definition in that face, get the index for the location, uv, and normal (remembering to subtract 1!)
    //... then compile the information for each face value using the three vertices
    //... then put the final face on the appropriate list

    //Create an array of floats for the vertices
    //Create an array of ints for the indices

    //Populate the vertices array with the information from the definition of your faces
    //Populate the indices array.

    //Tip: function stof converts a string to a float
    //Tip: function stoi converts a string to an int

    //Tip: You can't use SG_RANGE() with dynamically created arrays..
    //... instead use vbuf_desc.data = { vertices, (size_t)(x*sizeof(float))) };
    //... where x is the number of entries in your vertices array
    //... and the use ibuf_desc.data = { indices, (size_t)(y * sizeof(uint16_t)) };
    //... where y is the number of entries in your indices array


    // --- 1. Hardcoded cube mesh ---
    float vertices[] = {
        //  x,  y,  z,  nx, ny, nz,  u, v
        // Front face (z = +1), normal (0, 0, 1)
        -1, -1,  1,   0,  0,  1,   0, 0, // bottom-left
         1, -1,  1,   0,  0,  1,   1, 0, // bottom-right
         1,  1,  1,   0,  0,  1,   1, 1, // top-right
        -1,  1,  1,   0,  0,  1,   0, 1, // top-left
        // Back face (z = -1), normal (0, 0, -1)
         1, -1, -1,   0,  0, -1,   0, 0, // bottom-left  (looking at back from outside)
        -1, -1, -1,   0,  0, -1,   1, 0, // bottom-right
        -1,  1, -1,   0,  0, -1,   1, 1, // top-right
         1,  1, -1,   0,  0, -1,   0, 1, // top-left
        // Top face (y = +1), normal (0, 1, 0)
        -1,  1,  1,   0,  1,  0,   0, 0, // front-left
         1,  1,  1,   0,  1,  0,   1, 0, // front-right
         1,  1, -1,   0,  1,  0,   1, 1, // back-right
        -1,  1, -1,   0,  1,  0,   0, 1, // back-left
        // Bottom face (y = -1), normal (0, -1, 0)
        -1, -1, -1,   0, -1,  0,   0, 0, // back-left
         1, -1, -1,   0, -1,  0,   1, 0, // back-right
         1, -1,  1,   0, -1,  0,   1, 1, // front-right
        -1, -1,  1,   0, -1,  0,   0, 1, // front-left
        // Right face (x = +1), normal (1, 0, 0)
         1, -1,  1,   1,  0,  0,   0, 0, // bottom-front
         1, -1, -1,   1,  0,  0,   1, 0, // bottom-back
         1,  1, -1,   1,  0,  0,   1, 1, // top-back
         1,  1,  1,   1,  0,  0,   0, 1, // top-front
        // Left face (x = -1), normal (-1, 0, 0)
        -1, -1, -1,  -1,  0,  0,   0, 0, // bottom-back
        -1, -1,  1,  -1,  0,  0,   1, 0, // bottom-front
        -1,  1,  1,  -1,  0,  0,   1, 1, // top-front
        -1,  1, -1,  -1,  0,  0,   0, 1, // top-back
    };

    // 36 indices (6 faces * 2 triangles * 3 vertices)
    uint16_t indices[] = {
         0,  1,  2,   0,  2,  3,   // front
         4,  5,  6,   4,  6,  7,   // back
         8,  9, 10,   8, 10, 11,   // top
        12, 13, 14,  12, 14, 15,   // bottom
        16, 17, 18,  16, 18, 19,   // right
        20, 21, 22,  20, 22, 23,   // left
    };
    state.index_count = 36;

    // --- 2. Create vertex buffer ---
    sg_buffer_desc vbuf_desc = {};
    vbuf_desc.data = SG_RANGE(vertices);
    state.bind.vertex_buffers[0] = sg_make_buffer(&vbuf_desc);

    // --- 3. Create index buffer ---
    sg_buffer_desc ibuf_desc = {};
    ibuf_desc.usage.index_buffer = true;
    ibuf_desc.usage.vertex_buffer = false;
    ibuf_desc.data = SG_RANGE(indices);
    state.bind.index_buffer = sg_make_buffer(&ibuf_desc);

    // --- 4. Create shader ---
    char* vs_src = load_file("vertex.glsl");
    char* fs_src = load_file("fragment.glsl");

    sg_shader_desc shd_desc = {};
    shd_desc.vertex_func.source = vs_src;
    shd_desc.fragment_func.source = fs_src;

    // Tell the GL backend the GLSL attribute names so it can bind them by name
    shd_desc.attrs[0].glsl_name = "position";
    shd_desc.attrs[1].glsl_name = "normal";
    shd_desc.attrs[2].glsl_name = "uv";

    // Describe the uniform block for the vertex shader (slot 0)
    // Two mat4s: model and view_proj
    shd_desc.uniform_blocks[0].stage = SG_SHADERSTAGE_VERTEX;
    shd_desc.uniform_blocks[0].size = sizeof(vs_params_t);
    shd_desc.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_NATIVE;
    shd_desc.uniform_blocks[0].glsl_uniforms[0] = { SG_UNIFORMTYPE_MAT4, 0, "model" };
    shd_desc.uniform_blocks[0].glsl_uniforms[1] = { SG_UNIFORMTYPE_MAT4, 0, "view_proj" };

    sg_shader shd = sg_make_shader(&shd_desc);
    free(vs_src);
    free(fs_src);

    // --- 5. Create pipeline ---
    sg_pipeline_desc pip_desc = {};
    pip_desc.shader = shd;
    pip_desc.index_type = SG_INDEXTYPE_UINT16;   // We use 16-bit indices

    // Vertex layout: position (float3) + normal (float3) + uv (float2) = 32 bytes per vertex
    pip_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;  // position
    pip_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT3;  // normal
    pip_desc.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT2;  // uv

    // Enable depth testing so the cube renders correctly in 3D
    pip_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    pip_desc.depth.write_enabled = true;

    // Cull back faces for performance
    pip_desc.cull_mode = SG_CULLMODE_BACK;
    pip_desc.face_winding = SG_FACEWINDING_CCW;  // Our vertices are wound CCW when viewed from outside

    state.pip = sg_make_pipeline(&pip_desc);

    // --- 6. Set background color ---
    state.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    state.pass_action.colors[0].clear_value = { 0.2f, 0.2f, 0.2f, 1.0f };
    state.pass_action.depth.load_action = SG_LOADACTION_CLEAR;
    state.pass_action.depth.clear_value = 1.0f;
}


void frame(void) {
    sg_pass pass = {};
    pass.action = state.pass_action;
    pass.swapchain = sglue_swapchain();

    sg_begin_pass(&pass);
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);

    // --- Build model and projection matrices each frame ---
    vs_params_t vs_params;

    float* m = vs_params.model;
    m[0] = 1.0f;  m[4] = 0.0f;  m[8]  = 0.0f;   m[12] = 0.0f;
    m[1] = 0.0f;  m[5] = 1.0f;  m[9]  = 0.0f;   m[13] = 0.0f;
    m[2] = 0.0f;  m[6] = 0.0f;  m[10] = 1.0f;   m[14] = -5.0f;
    m[3] = 0.0f;  m[7] = 0.0f;  m[11] = 0.0f;   m[15] = 1.0f;
    
    float* p = vs_params.view_proj;
    p[0] = 1.0f;  p[4] = 0.0f;  p[8]  = 0.0f;    p[12] = 0.0f;
    p[1] = 0.0f;  p[5] = 1.0f;  p[9]  = 0.0f;    p[13] = 0.0f;
    p[2] = 0.0f;  p[6] = 0.0f;  p[10] = -1.0f;   p[14] = 0;
    p[3] = 0.0f;  p[7] = 0.0f;  p[11] = -1.0f;   p[15] = 0.0f;


    // Upload uniforms to slot 0
    sg_range params_range = { &vs_params, sizeof(vs_params) };
    sg_apply_uniforms(0, &params_range);

    sg_draw(0, state.index_count, 1);
    sg_end_pass();
    sg_commit();
}


void cleanup(void) {
    sg_shutdown();
}


sapp_desc sokol_main(int argc, char* argv[]) {
    sapp_desc app = {};
    app.init_cb = init;
    app.frame_cb = frame;
    app.cleanup_cb = cleanup;
    app.width = 800;
    app.height = 800;
    app.window_title = "Rasterizer";
    app.logger.func = slog_func;
    return app;
}