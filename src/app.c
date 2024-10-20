//------------------------------------------------------------------------------
//  triangle-sapp.c
//  Simple 2D rendering from vertex buffer.
//------------------------------------------------------------------------------
#define SOKOL_IMPL
/* this is only needed for the debug-inspection headers */
#define SOKOL_TRACE_HOOKS
/* sokol 3D-API defines are provided by build options */
#include "sokol_app.h"
#include "sokol_audio.h"
#include "sokol_fetch.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"
#include "sokol_time.h"

// #include "sokol_app.h"
// #include "sokol_gfx.h"
// #include "sokol_glue.h"
// #include "sokol_log.h"
#include "triangle-sapp.glsl.h"

//
#include "game/Logic.h"
#include "lib/Arena.h"
#include "lib/File.h"
#include "lib/HotReload.h"
#include "lib/Log.h"

static FileMonitor_t fm = {.directory = "src/game", .fileName = "Logic.c.dll"};
static Arena_t arena;
static Engine__State_t engine;

// application state
static struct {
  sg_pipeline pip;
  sg_bindings bind;
  sg_pass_action pass_action;
} state;

static void init(void) {
  stm_setup();

  Arena__Alloc(&arena, 1024 * 1024 * 50);  // MB
  engine.arena = &arena;

  File__StartMonitor(&fm);
  if (load_logic(LOGIC_FILENAME)) {
    logic_oninit_data(&engine);
    logic_oninit_compute(&engine);
  }

  sg_setup(&(sg_desc){
      .environment = sglue_environment(),  //
      .logger.func = slog_func,  //
  });

  // a vertex buffer with 3 vertices
  float vertices[] = {
      // positions                             // colors
      0.0f,  0.5f,  0.5f, /*    */ 1.0f, 0.0f, 0.0f, 1.0f,  //
      0.5f,  -0.5f, 0.5f, /*    */ 0.0f, 1.0f, 0.0f, 1.0f,  //
      -0.5f, -0.5f, 0.5f, /* */ 0.0f,    0.0f, 1.0f, 1.0f  //
  };
  state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
      .data = SG_RANGE(vertices),  //
      .label = "triangle-vertices"  //
  });

  // create shader from code-generated sg_shader_desc
  sg_shader shd = sg_make_shader(triangle_shader_desc(sg_query_backend()));

  // create a pipeline object (default render states are fine for triangle)
  state.pip = sg_make_pipeline(&(sg_pipeline_desc){
      .shader = shd,
      // if the vertex layout doesn't have gaps, don't need to provide strides and offsets
      .layout =
          {
              .attrs =
                  {
                      [ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,  //
                      [ATTR_vs_color0].format = SG_VERTEXFORMAT_FLOAT4,  //
                  }  //
          },
      .label = "triangle-pipeline",
  });

  // a pass action to clear framebuffer to black
  state.pass_action = (sg_pass_action){
      .colors[0] = {
          .load_action = SG_LOADACTION_CLEAR,  //
          .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},  //
      }};
}

void frame(void) {
  if (NULL != engine.local) {
    engine.local->now = stm_sec(stm_now());
    logic_onupdate(&engine);
    state.pass_action.colors[0].clear_value.r = engine.local->red;
  }

  sg_begin_pass(&(sg_pass){.action = state.pass_action, .swapchain = sglue_swapchain()});
  sg_apply_pipeline(state.pip);
  sg_apply_bindings(&state.bind);
  sg_draw(0, 3, 1);
  sg_end_pass();
  sg_commit();

  // check for fs changes
  char path[32] = "src/game/";
  char file[31];
  if (2 == File__CheckMonitor(&fm, file)) {
    LOG_DEBUGF("saw file %s", file);
    strcat_s(path, 32, file);
    LOG_DEBUGF("path %s", path);
    if (load_logic(path)) {
      logic_onreload(&engine);
    }
  }
}

void cleanup(void) {
  sg_shutdown();
  File__EndMonitor(&fm);
}

sapp_desc sokol_main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
  return (sapp_desc){
      .init_cb = init,
      .frame_cb = frame,
      .cleanup_cb = cleanup,
      .width = 640,
      .height = 480,
      .window_title = "Triangle",
      .icon.sokol_default = false,
      .logger.func = slog_func,
  };
}
