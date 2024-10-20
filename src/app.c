//------------------------------------------------------------------------------
//  triangle-sapp.c
//  Simple 2D rendering from vertex buffer.
//------------------------------------------------------------------------------
#define SOKOL_IMPL
/* this is only needed for the debug-inspection headers */
// #define SOKOL_TRACE_HOOKS
/* sokol 3D-API defines are provided by build options */
#include "../vendor/sokol/sokol_app.h"
#include "../vendor/sokol/sokol_audio.h"
#include "../vendor/sokol/sokol_fetch.h"
#include "../vendor/sokol/sokol_gfx.h"
#include "../vendor/sokol/sokol_glue.h"
#include "../vendor/sokol/sokol_log.h"
#include "../vendor/sokol/sokol_time.h"
#include "game/Logic.h"
#include "lib/HotReload.h"
#include "lib/Log.h"

Engine__State engine;

static FileMonitor fm = {.directory = "src/game", .fileName = "Logic.c.dll"};
static void init(void);
static void frame(void);
static void cleanup(void);
static void stream_cb(float* buffer, int num_frames, int num_channels);

sapp_desc sokol_main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  engine.stm_setup = stm_setup;
  engine.sg_setup = sg_setup;
  engine.sglue_environment = sglue_environment;
  engine.sglue_swapchain = sglue_swapchain;
  engine.slog_func = slog_func;
  engine.sg_make_buffer = sg_make_buffer;
  engine.sg_make_shader = sg_make_shader;
  engine.sg_query_backend = sg_query_backend;
  engine.sg_make_pipeline = sg_make_pipeline;
  engine.saudio_setup = saudio_setup;
  engine.saudio_sample_rate = saudio_sample_rate;
  engine.saudio_channels = saudio_channels;
  engine.stm_ms = stm_ms;
  engine.stm_now = stm_now;
  engine.sg_begin_pass = sg_begin_pass;
  engine.sg_apply_pipeline = sg_apply_pipeline;
  engine.sg_apply_bindings = sg_apply_bindings;
  engine.sg_draw = sg_draw;
  engine.sg_end_pass = sg_end_pass;
  engine.sg_commit = sg_commit;
  engine.sg_shutdown = sg_shutdown;
  engine.stream_cb1 = stream_cb;

  ASSERT_CONTEXT(load_logic(LOGIC_FILENAME), "Failed to load Logic.dll");
  logic_oninit(&engine);

  return (sapp_desc){
      .init_cb = init,
      .frame_cb = frame,
      .cleanup_cb = cleanup,
      .width = engine.window_width,
      .height = engine.window_height,
      .window_title = engine.window_title,
      .icon.sokol_default = false,
      .logger.func = slog_func,
      .win32_console_utf8 = true,
      .win32_console_attach = true,
  };
}

static void init(void) {
  File__StartMonitor(&fm);
  logic_onpreload();
}

static void frame(void) {
  // check for fs changes
  char path[32] = "src/game/";
  char file[31];
  if (2 == File__CheckMonitor(&fm, file)) {
    LOG_DEBUGF("saw file %s", file);
    strcat_s(path, 32, file);
    LOG_DEBUGF("path %s", path);
    engine.stream_cb2 = NULL;
    if (load_logic(path)) {
      logic_onreload(&engine);
    }
  }

  logic_onupdate();
}

static void cleanup(void) {
  logic_onshutdown();
  File__EndMonitor(&fm);
}

static void stream_cb(float* buffer, int num_frames, int num_channels) {
  if (NULL != engine.stream_cb2) {
    engine.stream_cb2(buffer, num_frames, num_channels);
  }
}