#define SOKOL_IMPL
#include "../vendor/sokol/sokol_app.h"
#include "../vendor/sokol/sokol_audio.h"
#include "../vendor/sokol/sokol_fetch.h"
#include "../vendor/sokol/sokol_gfx.h"
#include "../vendor/sokol/sokol_glue.h"
#include "../vendor/sokol/sokol_log.h"
#include "../vendor/sokol/sokol_time.h"
#include "game/Logic.h"

#ifndef __EMSCRIPTEN__
#include "lib/HotReload.h"
#endif
#include "lib/Log.h"

Engine__State engine;

#ifndef __EMSCRIPTEN__
static FileMonitor fm = {.directory = "src/game", .fileName = "Logic.c.dll"};
#endif
static void init(void);
static void frame(void);
static void cleanup(void);
static void stream_cb(float* buffer, int num_frames, int num_channels);

sapp_desc sokol_main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  engine.log = logit;
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
  engine.stm_now = stm_now;
  engine.stm_sec = stm_sec;
  engine.stm_ms = stm_ms;
  engine.stm_sec = stm_sec;
  engine.sg_begin_pass = sg_begin_pass;
  engine.sg_apply_pipeline = sg_apply_pipeline;
  engine.sg_apply_bindings = sg_apply_bindings;
  engine.sg_draw = sg_draw;
  engine.sg_end_pass = sg_end_pass;
  engine.sg_commit = sg_commit;
  engine.sg_shutdown = sg_shutdown;
  engine.stream_cb1 = stream_cb;
  engine.sfetch_setup = sfetch_setup;
  engine.sfetch_dowork = sfetch_dowork;
  engine.sfetch_shutdown = sfetch_shutdown;
  engine.sfetch_send = sfetch_send;
  engine.sg_alloc_image = sg_alloc_image;
  engine.sg_alloc_sampler = sg_alloc_sampler;
  engine.sg_init_sampler = sg_init_sampler;
  engine.sg_apply_uniforms = sg_apply_uniforms;
  engine.sg_init_image = sg_init_image;

#ifndef __EMSCRIPTEN__
  ASSERT_CONTEXT(load_logic(LOGIC_FILENAME), "Failed to load Logic.dll");
#endif
  logic_oninit(&engine);

#ifdef __EMSCRIPTEN__
  EM_ASM({ document.title = UTF8ToString($0); }, engine.window_title);
#endif

  // NOTICE: You can't log here--it's too early. The window + console aren't initialized, yet.

  return (sapp_desc){
      .init_cb = init,
      .frame_cb = frame,
      .cleanup_cb = cleanup,
      .width = engine.window_width,
      .height = engine.window_height,
      .html5_canvas_resize = true,
      .window_title = engine.window_title,
      .icon.sokol_default = false,
      .logger.func = slog_func,
      .win32_console_utf8 = true,
      .win32_console_attach = true,
  };
}

static void init(void) {
  if (false) {
    stm_setup();
    u64 started_at = stm_now();
    while (stm_ms(stm_diff(stm_now(), started_at)) < 4000) {
      // allow time to attach debugger
    }
  }

#ifndef __EMSCRIPTEN__
  File__StartMonitor(&fm);
#endif
  logic_onpreload();
}

static void frame(void) {
#ifndef __EMSCRIPTEN__
  // check for fs changes
  char path[32] = "src/game/";
  char file[31];
  if (2 == File__CheckMonitor(&fm, file)) {
    strcat_s(path, 32, file);
    while (2 == File__CheckMonitor(&fm, file)) {
      // flush all pending events
    }
    engine.stream_cb2 = NULL;
    if (load_logic(path)) {
      logic_onreload(&engine);
    }
  }
#endif

  logic_onupdate();
}

static void cleanup(void) {
  logic_onshutdown();
#ifndef __EMSCRIPTEN__
  File__EndMonitor(&fm);
#endif
}

static void stream_cb(float* buffer, int num_frames, int num_channels) {
  if (NULL != engine.stream_cb2) {
    engine.stream_cb2(buffer, num_frames, num_channels);
  }
}