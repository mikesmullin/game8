// #define INTEGRATION_TEST

#include <stdbool.h>

#include "../../../src/game/Logic.h"
#include "../../../src/lib/Log.h"
#include "../mocks/Sokol.h"

// @mock "game/Logic.h"
// @mock "game/Game.h"
// @mock "game/common/Net.h"

// two app instances
Engine__State engines[2];

// no hot-reload
extern Engine__State* g_engine;
logic_oninit_t logic_oninit;
logic_onpreload_t logic_onpreload;
logic_onevent_t logic_onevent;
logic_onfixedupdate_t logic_onfixedupdate;
logic_onupdate_t logic_onupdate;
logic_onshutdown_t logic_onshutdown;

static void stream_cb(float* buffer, int num_frames, int num_channels) {
  // no audio
}

static void common_app(Engine__State* engine) {
  engine->log = logit;
  engine->abort = logabort;
  engine->stm_setup = stm_setup;
  engine->sg_setup = sg_setup;
  engine->sglue_environment = sglue_environment;
  engine->sglue_swapchain = sglue_swapchain;
  engine->slog_func = slog_func;
  engine->sg_make_buffer = sg_make_buffer;
  engine->sg_make_shader = sg_make_shader;
  engine->sg_query_backend = sg_query_backend;
  engine->sg_make_pipeline = sg_make_pipeline;
  engine->saudio_setup = saudio_setup;
  engine->saudio_sample_rate = saudio_sample_rate;
  engine->saudio_channels = saudio_channels;
  engine->stm_now = stm_now;
  engine->stm_ns = stm_ns;
  engine->stm_us = stm_us;
  engine->stm_ms = stm_ms;
  engine->stm_sec = stm_sec;
  engine->stm_laptime = stm_laptime;
  engine->sg_begin_pass = sg_begin_pass;
  engine->sg_apply_pipeline = sg_apply_pipeline;
  engine->sg_apply_bindings = sg_apply_bindings;
  engine->sg_draw = sg_draw;
  engine->sg_end_pass = sg_end_pass;
  engine->sg_commit = sg_commit;
  engine->sg_shutdown = sg_shutdown;
  engine->stream_cb1 = stream_cb;
  engine->saudio_shutdown = saudio_shutdown;
  engine->sfetch_setup = sfetch_setup;
  engine->sfetch_dowork = sfetch_dowork;
  engine->sfetch_shutdown = sfetch_shutdown;
  engine->sfetch_send = sfetch_send;
  engine->sg_alloc_image = sg_alloc_image;
  engine->sg_alloc_sampler = sg_alloc_sampler;
  engine->sg_init_sampler = sg_init_sampler;
  engine->sg_apply_uniforms = sg_apply_uniforms;
  engine->sg_init_image = sg_init_image;
  engine->sg_update_buffer = sg_update_buffer;
  engine->sg_update_image = sg_update_image;
  engine->sg_query_frame_stats = sg_query_frame_stats;
  engine->sg_enable_frame_stats = sg_enable_frame_stats;
  engine->sg_disable_frame_stats = sg_disable_frame_stats;
  engine->sg_frame_stats_enabled = sg_frame_stats_enabled;
  engine->sg_make_image = sg_make_image;
  engine->sg_make_attachments = sg_make_attachments;
  engine->sapp_lock_mouse = sapp_lock_mouse;
  engine->sapp_mouse_locked = sapp_mouse_locked;

  logic_oninit(engine);
  logic_onpreload();
}

static void common_app_pump(Engine__State* engine) {
  g_engine = engine;

  engine->now = stm_ms(stm_now());
  static u64 lastTick;
  engine->deltaTime = stm_sec(stm_laptime(&lastTick));

  logic_onfixedupdate();
  logic_onupdate();
  logic_onevent(&(sapp_event){});

  // TODO: could log perf stats to console
}

// @describe Gaffer Deterministic Lockstep Katamari demo
// see: https://gafferongames.com/post/deterministic_lockstep/
// @tag net
int main() {
  static bool quit = false;

  // spawn app1 -server
  engines[0].isMaster = true;
  common_app(&engines[0]);
  // spawn app2
  common_app(&engines[1]);

  while (!quit) {
    common_app_pump(&engines[0]);
    common_app_pump(&engines[1]);

    // TODO: wait for connect
    // TODO: macro player1 input
    // TODO: assert checksum game state on both apps (lockstep deterministic networking)
  }

  return 0;
}
