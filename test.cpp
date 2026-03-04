#define SOKOL_IMPL
#define SOKOL_GLCORE
#include "sokol_log.h"
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "file_util.h"

// --- STATE ---
struct State {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
};
static struct State state;

void init(void) {
    sg_desc desc;
    memset(&desc, 0, sizeof(desc));
    desc.environment = sglue_environment();
    sg_setup(&desc);

    // 1. Create Vertex Buffer (Triangle: Pos[X,Y,Z] + Color[R,G,B,A])
    float vertices[] = {
        // positions            // colors
         0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
    };
    sg_buffer_desc buf_desc;
    memset(&buf_desc, 0, sizeof(buf_desc));

    buf_desc.data = SG_RANGE(vertices);
    state.bind.vertex_buffers[0] = sg_make_buffer(&buf_desc);

    // 2. Create Shader from .glsl files
    char* vs_src = load_file("vertex.glsl");
    char* fs_src = load_file("fragment.glsl");

    sg_shader_desc shd_desc;
    memset(&shd_desc, 0, sizeof(shd_desc));

    shd_desc.vertex_func.source = vs_src;
    shd_desc.fragment_func.source = fs_src;
    sg_shader shd = sg_make_shader(&shd_desc);

    free(vs_src);
    free(fs_src);

    // 3. Create Pipeline (Defines how to interpret the buffer)
    sg_pipeline_desc pip_desc;
    memset(&pip_desc, 0, sizeof(pip_desc));
    pip_desc.shader = shd;
    pip_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3; // position
    pip_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT4; // color
    state.pip = sg_make_pipeline(&pip_desc);

    // Background color (light gray)
    state.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    state.pass_action.colors[0].clear_value.r = 0.2f;
    state.pass_action.colors[0].clear_value.g = 0.2f;
    state.pass_action.colors[0].clear_value.b = 0.2f;
    state.pass_action.colors[0].clear_value.a = 1.0f;
}

void frame(void) {
    sg_pass pass;
    memset(&pass, 0, sizeof(pass));
    pass.action = state.pass_action;
    pass.swapchain = sglue_swapchain();
    sg_begin_pass(&pass);
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 3, 1);
    sg_end_pass();
    sg_commit();
}

void cleanup(void) {
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    sapp_desc app;
    memset(&app, 0, sizeof(app));
    app.init_cb = init;
    app.frame_cb = frame;
    app.cleanup_cb = cleanup;
    app.width = 800;
    app.height = 600;
    app.window_title = "Sokol Triangle";
    return app;
}