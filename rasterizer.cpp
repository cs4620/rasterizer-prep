
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
#include <cstring>                   // For memset/memcpy

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

// --- Simple math helpers (column-major mat4) ---
static void mat4_identity(float* m) {
    memset(m, 0, 16 * sizeof(float));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

// Perspective projection (column-major)
static void mat4_perspective(float* m, float fovy_rad, float aspect, float znear, float zfar) {
    memset(m, 0, 16 * sizeof(float));
    float f = 1.0f / tanf(fovy_rad / 2.0f);
    m[0]  = f / aspect;
    m[5]  = f;
    m[10] = (zfar + znear) / (znear - zfar);
    m[11] = -1.0f;
    m[14] = (2.0f * zfar * znear) / (znear - zfar);
}

// --- STATE ---
struct State {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
    int index_count;
};
struct State state;


void init(void) {
    sg_desc desc = {};
    desc.environment = sglue_environment();
    desc.logger.func = slog_func;          // Hook up Sokol logger for debug output
    sg_setup(&desc);

    // --- 1. Hardcoded cube mesh ---
    // 24 vertices (4 per face, each with its own face normal and UVs)
    // Each vertex: position (3) + normal (3) + uv (2) = 8 floats
    float vertices[] = {
        // Front face (z = +1), normal (0, 0, 1)
        -1, -1,  1,   0,  0,  1,   0, 0,
         1, -1,  1,   0,  0,  1,   1, 0,
         1,  1,  1,   0,  0,  1,   1, 1,
        -1,  1,  1,   0,  0,  1,   0, 1,
        // Back face (z = -1), normal (0, 0, -1)
         1, -1, -1,   0,  0, -1,   0, 0,
        -1, -1, -1,   0,  0, -1,   1, 0,
        -1,  1, -1,   0,  0, -1,   1, 1,
         1,  1, -1,   0,  0, -1,   0, 1,
        // Top face (y = +1), normal (0, 1, 0)
        -1,  1,  1,   0,  1,  0,   0, 0,
         1,  1,  1,   0,  1,  0,   1, 0,
         1,  1, -1,   0,  1,  0,   1, 1,
        -1,  1, -1,   0,  1,  0,   0, 1,
        // Bottom face (y = -1), normal (0, -1, 0)
        -1, -1, -1,   0, -1,  0,   0, 0,
         1, -1, -1,   0, -1,  0,   1, 0,
         1, -1,  1,   0, -1,  0,   1, 1,
        -1, -1,  1,   0, -1,  0,   0, 1,
        // Right face (x = +1), normal (1, 0, 0)
         1, -1,  1,   1,  0,  0,   0, 0,
         1, -1, -1,   1,  0,  0,   1, 0,
         1,  1, -1,   1,  0,  0,   1, 1,
         1,  1,  1,   1,  0,  0,   0, 1,
        // Left face (x = -1), normal (-1, 0, 0)
        -1, -1, -1,  -1,  0,  0,   0, 0,
        -1, -1,  1,  -1,  0,  0,   1, 0,
        -1,  1,  1,  -1,  0,  0,   1, 1,
        -1,  1, -1,  -1,  0,  0,   0, 1,
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

    // Enable depth testing so the sphere renders correctly in 3D
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

    // Model (world) matrix: rotate 45° around X, then 45° around Y, then translate into -Z
    float angle = 45.0f * (float)M_PI / 180.0f;
    float c = cosf(angle);
    float s = sinf(angle);

    // Rotation around Y (column-major)
    float ry[16];
    memset(ry, 0, sizeof(ry));
    ry[0] = c;  ry[2] = s;
    ry[5] = 1.0f;
    ry[8] = -s; ry[10] = c;
    ry[15] = 1.0f;

    // Rotation around X (column-major)
    float rx[16];
    memset(rx, 0, sizeof(rx));
    rx[0] = 1.0f;
    rx[5] = c;  rx[6] = s;
    rx[9] = -s; rx[10] = c;
    rx[15] = 1.0f;

    // model = translate * ry * rx  (apply rx first, then ry, then translate)
    // First: rot = ry * rx
    float rot[16];
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++)
                sum += ry[k * 4 + row] * rx[col * 4 + k];
            rot[col * 4 + row] = sum;
        }
    }
    // Copy rotation into model, then add translation
    memcpy(vs_params.model, rot, sizeof(rot));
    vs_params.model[14] = -5.0f;  // tz

    // Projection matrix: 45-degree FOV, correct aspect ratio
    float aspect = (float)sapp_width() / (float)sapp_height();
    mat4_perspective(vs_params.view_proj, 45.0f * (float)M_PI / 180.0f, aspect, 0.1f, 100.0f);

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
    app.height = 600;
    app.window_title = "Sphere Rasterizer";
    app.logger.func = slog_func;
    return app;
}