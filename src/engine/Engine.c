#include "Engine.h"

#include <stdio.h>
#include <stdlib.h>

void Engine__init() {
  // prng seeds for deterministic math
  g_engine->seeds.main = 1234567ULL;
  // organized by subsystem for easy desync troubleshooting
  g_engine->seeds.nosync = Math__randomNext(&g_engine->seeds.main);
  g_engine->seeds.entityMove = Math__randomNext(&g_engine->seeds.main);
  g_engine->seeds.entityAnim = Math__randomNext(&g_engine->seeds.main);
  g_engine->seeds.bt = Math__randomNext(&g_engine->seeds.main);
  g_engine->seeds.sg = Math__randomNext(&g_engine->seeds.main);
  g_engine->seeds.uiAnim = Math__randomNext(&g_engine->seeds.main);

  // defaults
  g_engine->isMaster = false;
  g_engine->useVideo = true;
  g_engine->useAudio = true;
  g_engine->useInput = true;
  g_engine->useTime = true;
  g_engine->useHotReload = false;
  g_engine->useConsole = false;
  g_engine->usePerfLog = true;

  // wrappers
  g_engine->log = Log__out;
  g_engine->abort = Log__abort;

  g_engine->slog_func = wslog_func;

  g_engine->sapp_lock_mouse = wsapp_lock_mouse;
  g_engine->sapp_mouse_locked = wsapp_mouse_locked;
  g_engine->sapp_update_window_title = wsapp_update_window_title;
  g_engine->sapp_request_quit = wsapp_request_quit;

  g_engine->sg_setup = wsg_setup;
  g_engine->sglue_environment = wsglue_environment;
  g_engine->sglue_swapchain = wsglue_swapchain;
  g_engine->sg_make_buffer = wsg_make_buffer;
  g_engine->sg_make_shader = wsg_make_shader;
  g_engine->sg_query_backend = wsg_query_backend;
  g_engine->sg_make_pipeline = wsg_make_pipeline;
  g_engine->sg_begin_pass = wsg_begin_pass;
  g_engine->sg_apply_pipeline = wsg_apply_pipeline;
  g_engine->sg_apply_bindings = wsg_apply_bindings;
  g_engine->sg_draw = wsg_draw;
  g_engine->sg_end_pass = wsg_end_pass;
  g_engine->sg_commit = wsg_commit;
  g_engine->sg_shutdown = wsg_shutdown;
  g_engine->sg_alloc_image = wsg_alloc_image;
  g_engine->sg_alloc_sampler = wsg_alloc_sampler;
  g_engine->sg_init_sampler = wsg_init_sampler;
  g_engine->sg_apply_uniforms = wsg_apply_uniforms;
  g_engine->sg_init_image = wsg_init_image;
  g_engine->sg_update_buffer = wsg_update_buffer;
  g_engine->sg_update_image = wsg_update_image;
  g_engine->sg_query_frame_stats = wsg_query_frame_stats;
  g_engine->sg_enable_frame_stats = wsg_enable_frame_stats;
  g_engine->sg_disable_frame_stats = wsg_disable_frame_stats;
  g_engine->sg_frame_stats_enabled = wsg_frame_stats_enabled;
  g_engine->sg_make_image = wsg_make_image;
  g_engine->sg_make_attachments = wsg_make_attachments;

  g_engine->saudio_setup = wsaudio_setup;
  g_engine->saudio_sample_rate = wsaudio_sample_rate;
  g_engine->saudio_channels = wsaudio_channels;
  g_engine->saudio_shutdown = wsaudio_shutdown;

  g_engine->stm_setup = wstm_setup;
  g_engine->stm_now = wstm_now;
  g_engine->stm_ns = wstm_ns;
  g_engine->stm_us = wstm_us;
  g_engine->stm_ms = wstm_ms;
  g_engine->stm_sec = wstm_sec;
  g_engine->stm_laptime = wstm_laptime;

  g_engine->sfetch_setup = wsfetch_setup;
  g_engine->sfetch_dowork = wsfetch_dowork;
  g_engine->sfetch_shutdown = wsfetch_shutdown;
  g_engine->sfetch_send = wsfetch_send;
}

void Engine__cli(int argc, char* argv[]) {
  // parse
  for (u32 i = 0; i < argc; i++) {
    if (String__isEqual("-server", argv[i])) {
      g_engine->isMaster = true;

      char* start = argv[++i];
      u32 len = strlen(start);
      u32 d = msindexOf(':', start, len);

      // TODO: stop using malloc in this file
      g_engine->listenHost = malloc(d + 1);
      mscp(g_engine->listenHost, start, d);

      g_engine->listenPort = malloc(len - d + 1);
      mscp(g_engine->listenPort, (char*)start + d + 1, len - 1 - d);
      LOG_DEBUGF("Will listen on %s:%s", g_engine->listenHost, g_engine->listenPort);
    } else if (String__isEqual("-connect", argv[i])) {
      g_engine->isMaster = false;

      char* start = argv[++i];
      u32 len = strlen(start);
      u32 d = msindexOf(':', start, len);

      g_engine->connectHost = malloc(d + 1);
      mscp(g_engine->connectHost, start, d);

      g_engine->connectPort = malloc(len - d + 1);
      mscp(g_engine->connectPort, (char*)start + d + 1, len - 1 - d);
      LOG_DEBUGF("Will connect to %s:%s", g_engine->connectHost, g_engine->connectPort);
    } else if (String__isEqual("-noconsole", argv[i])) {
      g_engine->useConsole = false;
    } else if (String__isEqual("-console", argv[i])) {
      g_engine->useConsole = true;
    } else if (String__isEqual("-novideo", argv[i])) {
      g_engine->useVideo = false;
    } else if (String__isEqual("-video", argv[i])) {
      g_engine->useVideo = true;
    } else if (String__isEqual("-noaudio", argv[i])) {
      g_engine->useAudio = false;
    } else if (String__isEqual("-audio", argv[i])) {
      g_engine->useAudio = true;
    } else if (String__isEqual("-noinput", argv[i])) {
      g_engine->useInput = false;
    } else if (String__isEqual("-input", argv[i])) {
      g_engine->useInput = true;
    } else if (String__isEqual("-notime", argv[i])) {
      g_engine->useTime = false;
    } else if (String__isEqual("-time", argv[i])) {
      g_engine->useTime = true;
    } else if (String__isEqual("-headless", argv[i])) {
      // dedicated server, running remotely in cloud
      g_engine->useVideo = false;
      g_engine->useAudio = false;
      g_engine->useInput = false;
    } else if (String__isEqual("-windowed", argv[i])) {
      // integration test, running visibly for fun
      g_engine->useVideo = true;
      // g_engine->useAudio = true;
    } else if (String__isEqual("-noreload", argv[i])) {
      g_engine->useHotReload = false;
    } else if (String__isEqual("-reload", argv[i])) {
      g_engine->useHotReload = true;
    } else if (String__isEqual("-noperf", argv[i])) {
      g_engine->usePerfLog = false;
    } else if (String__isEqual("-perf", argv[i])) {
      g_engine->usePerfLog = true;
    }
  }
}

// --- Engine + Sokol integration ---

int Engine__main(int argc, char* argv[]) {
  Math__init();
  Log__init();
  Engine__init();
  sapp_desc desc = Engine__sokol_main(argc, argv);
  if (g_engine->useVideo) {
    sapp_run(&desc);
  } else {
    Engine__sokol_init();
    while (!g_engine->quit) {
      Engine__sokol_frame();
      // TODO: constant must become dynamic, based on last lap
      SLEEP(1000 / 60);  // TODO: use fixedUpdate time step?
    }
    Engine__sokol_cleanup();
  }
  return 0;
}

sapp_desc Engine__sokol_main(int argc, char* argv[]) {
  Engine__cli(argc, argv);
  g_engine->stream_cb1 = Engine__sokol_stream_cb;

  // hot-reload support
  if (g_engine->useHotReload) {
    g_engine->fm = (FileMonitor){.directory = "./", .fileName = "Logic.dll"};
    ASSERT_CONTEXT(HotReload__load(g_engine->fm.fileName), "Failed to load Logic.dll");
  }

  g_engine->onbootstrap(g_engine);
  g_engine->oninit();

  // NOTICE: You can't log here--it's too early. The window + console aren't initialized, yet.

  g_engine->sapp_update_window_title(g_engine->window_title);
  return (sapp_desc){
      .init_cb = Engine__sokol_init,
      .frame_cb = Engine__sokol_frame,
      .event_cb = Engine__sokol_event_cb,
      .cleanup_cb = Engine__sokol_cleanup,
      .width = g_engine->window_width,
      .height = g_engine->window_height,
      .window_title = g_engine->window_title,
      .icon.sokol_default = false,
      .logger.func = wslog_func,
      .win32_console_utf8 = true,
      .win32_console_attach = true,
      // NOTICE: sokol doesn't seem to work as intended here
      .win32_console_create = g_engine->useConsole,
  };
}

void Engine__sokol_init(void) {
  if (g_engine->useHotReload) {
    HotReload__StartMonitor(&g_engine->fm);
  }

  g_engine->onpreload();
}

void Engine__sokol_frame(void) {
  if (g_engine->useHotReload) {
    // check for fs changes
    char path[32] = "";
    char file[31];
    if (2 == HotReload__CheckMonitor(&g_engine->fm, file)) {
      strcat_s(path, 32, file);
      while (2 == HotReload__CheckMonitor(&g_engine->fm, file)) {
        // flush all pending events
      }
      g_engine->stream_cb2 = NULL;
      if (HotReload__load(path)) {
        g_engine->onbootstrap(g_engine);
      }
    }
  }

  g_engine->now = g_engine->stm_ms(g_engine->stm_now());
  static u64 lastTick;
  g_engine->deltaTime = g_engine->stm_sec(g_engine->stm_laptime(&lastTick));

  if (!g_engine->usePerfLog) {
    g_engine->onfixedupdate();
    g_engine->onupdate();
  } else {
    u64 startPhysics, costPhysics;
    startPhysics = g_engine->stm_now();
    g_engine->onfixedupdate();
    costPhysics = g_engine->stm_ms(g_engine->stm_laptime(&startPhysics));

    u64 startRender, costRender;
    startRender = g_engine->stm_now();
    g_engine->onupdate();
    costRender = g_engine->stm_ms(g_engine->stm_laptime(&startRender));

    // performance stats
    static f64 accumulator2 = 0.0f;
    static const f32 FPS_LOG_TIME_STEP = 1.0f;  // every second
    static u16 frames = 0;
    accumulator2 += g_engine->deltaTime;
    frames++;
    if (accumulator2 >= FPS_LOG_TIME_STEP) {
      static char title[100];
      sprintf(
          title,
          "%s | FPS %u P %llu R %llu pFPS %llu A %llu/%lluMB E %u D %u",
          g_engine->window_title,
          frames,  // FPS = measured/counted frames per second
          costPhysics,  // P = cost of last physics in ms
          costRender,  // R = cost of last render in ms
          // pFPS = potential frames per second (if it wasn't fixed)
          1000 / (costPhysics + costRender + 1),  // +1 avoids div/0
          // A = Arena memory used/capacity
          ((u64)((g_engine->arena->pos - g_engine->arena->buf) -
                 (g_engine->frameArena->end - g_engine->frameArena->pos))) /
              1024 / 1024,
          ((u64)(g_engine->arena->end - g_engine->arena->buf)) / 1024 / 1024,
          g_engine->entity_count,
          g_engine->draw_count);

      if (g_engine->useVideo) {
        g_engine->sapp_update_window_title(title);
      } else {
        LOG_DEBUGF("%s", title);
      }
      frames = 0;

      while (accumulator2 >= FPS_LOG_TIME_STEP) {
        accumulator2 -= FPS_LOG_TIME_STEP;
      }
    }
  }
}

void Engine__sokol_cleanup(void) {
  g_engine->onshutdown();
  if (g_engine->useHotReload) {
    HotReload__EndMonitor(&g_engine->fm);
  }
}

void Engine__sokol_event_cb(const sapp_event* event) {
  if (!g_engine->useInput) return;
  g_engine->onevent(event);
}

void Engine__sokol_stream_cb(float* buffer, int num_frames, int num_channels) {
  if (!g_engine->useAudio) return;
  if (NULL != g_engine->stream_cb2) {
    g_engine->stream_cb2(buffer, num_frames, num_channels);
  }
}

// --- Engine utils ---

f32 mwave(f32 ms, f32 a, f32 b) {
  return Math__map(Math__sinf(g_engine->now / ms), -1, 1, a, b);
}