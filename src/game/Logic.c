#include "Logic.h"

#include "../../vendor/sokol/sokol_app.h"
#include "../../vendor/sokol/sokol_audio.h"
#include "../../vendor/sokol/sokol_fetch.h"
#include "../../vendor/sokol/sokol_gfx.h"
#include "../../vendor/sokol/sokol_glue.h"
#include "../../vendor/sokol/sokol_log.h"
#include "../../vendor/sokol/sokol_time.h"
#include "../lib/Log.h"
#include "common/Arena.h"
#include "common/Wav.h"

//
#include "../../assets/shaders/triangle-sapp.glsl.h"

#define LOGIC_DECL __declspec(dllexport)
Engine__State* g_engine;
static void stream_cb(float* buffer, int num_frames, int num_channels);

// on init (data only)
LOGIC_DECL void logic_oninit(Engine__State* state) {
  g_engine = state;
  g_engine->window_title = "Retro";
  u16 dims = 640;
  g_engine->window_width = dims;
  g_engine->window_height = dims;

  Arena__Alloc(&g_engine->arena, 1024 * 1024 * 50);  // MB
  g_engine->logic = Arena__Push(g_engine->arena, sizeof(Logic__State));

  g_engine->stream_cb2 = stream_cb;
}

LOGIC_DECL void logic_onpreload(void) {
  Logic__State* logic = g_engine->logic;

  logic->wr = Arena__Push(g_engine->arena, sizeof(WavReader));

  logic->pip = Arena__Push(g_engine->arena, sizeof(sg_pipeline));
  logic->bind = Arena__Push(g_engine->arena, sizeof(sg_bindings));
  logic->pass_action = Arena__Push(g_engine->arena, sizeof(sg_pass_action));

  g_engine->stm_setup();

  g_engine->sg_setup(&(sg_desc){
      .environment = g_engine->sglue_environment(),  //
      .logger.func = g_engine->slog_func,  //
  });

  // a vertex buffer with 3 vertices
  float vertices[] = {
      // positions                             // colors
      0.0f,  0.5f,  0.5f, /*    */ 1.0f, 0.0f, 0.0f, 1.0f,  //
      0.5f,  -0.5f, 0.5f, /*    */ 0.0f, 1.0f, 0.0f, 1.0f,  //
      -0.5f, -0.5f, 0.5f, /* */ 0.0f,    0.0f, 1.0f, 1.0f  //
  };
  logic->bind->vertex_buffers[0] = g_engine->sg_make_buffer(&(sg_buffer_desc){
      .data = SG_RANGE(vertices),  //
      .label = "triangle-vertices"  //
  });

  // create shader from code-generated sg_shader_desc
  sg_shader shd = g_engine->sg_make_shader(triangle_shader_desc(g_engine->sg_query_backend()));

  // create a pipeline object (default render states are fine for triangle)
  (*logic->pip) = g_engine->sg_make_pipeline(&(sg_pipeline_desc){
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
  (*logic->pass_action) = (sg_pass_action){
      .colors[0] = {
          .load_action = SG_LOADACTION_CLEAR,  //
          .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},  //
      }};

  g_engine->saudio_setup(&(saudio_desc){
      .stream_cb = g_engine->stream_cb1,
      .logger = {
          .func = g_engine->slog_func,
      }});

  LOG_DEBUGF(
      "audio system init. sample_rate: %u, channels: %u",
      g_engine->saudio_sample_rate(),
      g_engine->saudio_channels());

  Wav__Read("../assets/audio/sfx/pickupCoin.wav", logic->wr);
}

LOGIC_DECL void logic_onreload(Engine__State* state) {
  g_engine = state;
  g_engine->stream_cb2 = stream_cb;
  Logic__State* logic = g_engine->logic;

  LOG_DEBUGF("Logic dll loaded state %p at %p", g_engine, &g_engine);
  logic->wr->offset = 0;  // replay the sfx
}

// on physics
LOGIC_DECL void logic_onfixedupdate(void) {
}

// on draw
LOGIC_DECL void logic_onupdate(void) {
  Logic__State* logic = g_engine->logic;

  g_engine->logic->now = g_engine->stm_ms(g_engine->stm_now());
  //f32 s = (sin(logic->now / 1000.0f) + 1.0f) * 0.5f;
  logic->pass_action->colors[0].clear_value.r = 0.5f;

  g_engine->sg_begin_pass(
      &(sg_pass){.action = *logic->pass_action, .swapchain = g_engine->sglue_swapchain()});
  g_engine->sg_apply_pipeline(*logic->pip);
  g_engine->sg_apply_bindings(logic->bind);
  g_engine->sg_draw(0, 3, 1);
  g_engine->sg_end_pass();
  g_engine->sg_commit();
}

// the sample callback, running in audio thread
static void stream_cb(float* buffer, int num_frames, int num_channels) {
  Logic__State* logic = g_engine->logic;

  ASSERT(1 == num_channels);
  // static uint32_t count = 0;
  // float amp = 0.05f;
  // uint32_t freq = 0;
  // if (NULL != g_engine->logic) {
  //   uint32_t factor = (uint32_t)((1.0f - g_engine->logic->red) * 4.0f) + 5.0f;
  //   freq = (1 << factor);
  // }
  uint8_t samples[logic->wr->bytesPerSample * logic->wr->numChannels];
  float sampleFloat;
  for (int i = 0; i < num_frames; i++) {
    Wav__NextSample(logic->wr, samples);
    sampleFloat = ((int)(samples[0]) - 128) / 128.0f;  // assume 1 channel, 1 byte per sample
    buffer[i] = sampleFloat;
    // buffer[i] = (count++ & freq) ? amp : -amp;
    // buffer[i] = 0;
    // printf(" %f", buffer[i]);
  }
}

LOGIC_DECL void logic_onshutdown(void) {
  g_engine->sg_shutdown();
  g_engine->saudio_shutdown();
}