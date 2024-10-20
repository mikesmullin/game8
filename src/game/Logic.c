#include "Logic.h"

#include "../../vendor/sokol/sokol_audio.h"
#include "../../vendor/sokol/sokol_gfx.h"
#include "../lib/Log.h"
#include "Game.h"
#include "common/Arena.h"
#include "common/Wav.h"

#define LOGIC_DECL __declspec(dllexport)
Engine__State* g_engine;
static void stream_cb(float* buffer, int num_frames, int num_channels);

// on init (data only)
LOGIC_DECL void logic_oninit(Engine__State* state) {
  g_engine = state;

  Game__init();

  Arena__Alloc(&g_engine->arena, 1024 * 1024 * 50);  // MB
  g_engine->logic = Arena__Push(g_engine->arena, sizeof(Logic__State));

  g_engine->stream_cb2 = stream_cb;
}

LOGIC_DECL void logic_onpreload(void) {
  Logic__State* logic = g_engine->logic;

  g_engine->stm_setup();

  g_engine->sg_setup(&(sg_desc){
      .environment = g_engine->sglue_environment(),  //
      .logger.func = g_engine->slog_func,  //
  });

  Game__preload();

  logic->wr = Arena__Push(g_engine->arena, sizeof(WavReader));

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

  Game__render();
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