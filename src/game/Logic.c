#include "Logic.h"

#include <stdio.h>

#include "../../vendor/sokol/sokol_audio.h"
#include "../../vendor/sokol/sokol_fetch.h"
#include "../../vendor/sokol/sokol_gfx.h"
#include "Game.h"
#include "common/Arena.h"
#include "common/Log.h"
#include "common/Wav.h"

#ifdef __EMSCRIPTEN__
#define LOGIC_DECL
#else
#define LOGIC_DECL __declspec(dllexport)
#endif

Engine__State* g_engine;
static void stream_cb(float* buffer, int num_frames, int num_channels);

// on init (data only)
LOGIC_DECL void logic_oninit(Engine__State* state) {
  g_engine = state;

  // NOTICE: logging won't work from here

  Arena__Alloc(&g_engine->arena, 1024 * 1024 * 12);  // MB (16mb is max for emscripten + firefox)
  g_engine->logic = Arena__Push(g_engine->arena, sizeof(Logic__State));
  g_engine->stream_cb2 = stream_cb;

  Game__init();
}

LOGIC_DECL void logic_onpreload(void) {
  Logic__State* logic = g_engine->logic;

  g_engine->stm_setup();

  g_engine->sfetch_setup(&(sfetch_desc_t){.logger.func = g_engine->slog_func});

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

  // LOG_DEBUGF(
  //     "audio system init. sample_rate: %u, channels: %u",
  //     g_engine->saudio_sample_rate(),
  //     g_engine->saudio_channels());

  Wav__Read("../assets/audio/sfx/pickupCoin.wav", logic->wr);
}

LOGIC_DECL void logic_onreload(Engine__State* state) {
  g_engine = state;
  g_engine->stream_cb2 = stream_cb;
  Logic__State* logic = g_engine->logic;

  LOG_DEBUGF("Logic dll reloaded.");
  logic->wr->offset = 0;  // replay the sfx
}

// on physics
LOGIC_DECL void logic_onfixedupdate(void) {
}

// on draw
LOGIC_DECL void logic_onupdate(void) {
  Logic__State* logic = g_engine->logic;

  g_engine->sfetch_dowork();

  Game__render();
  g_engine->sg_commit();
}

// the sample callback, running in audio thread
static void stream_cb(float* buffer, int num_frames, int num_channels) {
  Logic__State* logic = g_engine->logic;

  ASSERT(1 == num_channels);

  u8 samples[(logic->wr->bitsPerSample / 8) * logic->wr->numChannels];
  f32 sampleFloat;
  for (u32 i = 0; i < num_frames; i++) {
    Wav__NextSample(logic->wr, samples);
    sampleFloat = ((s32)(samples[0]) - 128) / 128.0f;  // assume 1 channel, 1 byte per sample

    // if (logic->wr->offset == logic->wr->totalSamples) logic->wr->offset = 0;

    // LOG_DEBUGF(
    //     "stream_cb wr %p offset %u bps %u c %u s %u data %p sample %u sF %f",
    //     logic->wr,
    //     logic->wr->offset,
    //     logic->wr->bitsPerSample,
    //     logic->wr->numChannels,
    //     logic->wr->totalSamples,
    //     logic->wr->data,
    //     samples[0],
    //     sampleFloat);

    buffer[i] = sampleFloat;
  }
}

LOGIC_DECL void logic_onshutdown(void) {
  g_engine->sg_shutdown();
  g_engine->saudio_shutdown();
  g_engine->sfetch_shutdown();
}