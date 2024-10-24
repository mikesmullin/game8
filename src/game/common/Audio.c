#include "Audio.h"

#include "../../../vendor/sokol/sokol_audio.h"
#include "../Logic.h"
#include "../common/Log.h"
#include "../common/Wav.h"

extern Engine__State* g_engine;

#define SILENCE (0)

void Audio__init() {
  g_engine->stream_cb2 = Audio__stream_cb;
}

void Audio__reload() {
  g_engine->stream_cb2 = Audio__stream_cb;
}

void Audio__preload() {
  // sokol_audio.h
  g_engine->saudio_setup(&(saudio_desc){
      .stream_cb = g_engine->stream_cb1,
      .logger = {
          .func = g_engine->slog_func,
      }});
  // LOG_DEBUGF(
  //     "audio system init. sample_rate: %u, channels: %u",
  //     g_engine->saudio_sample_rate(),
  //     g_engine->saudio_channels());
}

void Audio__replay(WavReader* r) {
  Logic__State* logic = g_engine->logic;

  // TODO: support gain
  // TODO: support pan
  // TODO: support multiple voices

  r->offset = 0;  // replay the current sfx
  logic->aSrc = r;
}

void Audio__stop() {
  Logic__State* logic = g_engine->logic;

  logic->aSrc = NULL;
}

// the sample callback, running in audio thread
void Audio__stream_cb(f32* buffer, int num_frames, int num_channels) {
  ASSERT(1 == num_channels);
  Logic__State* logic = g_engine->logic;
  if (NULL == logic->aSrc) {  // no current audio source
    for (u32 i = 0; i < num_frames; i++) {
      buffer[i] = SILENCE;
    }
    return;
  }

  u32 samples[logic->aSrc->numChannels];
  for (u32 i = 0; i < num_frames; i++) {
    if (!logic->aSrc->loaded) {
      buffer[i] = SILENCE;
    } else {
      Wav__NextSample(logic->aSrc, samples);
      // assume 1 channel, 1 byte per sample
      buffer[i] = (((s32)(samples[0]) - 128) / 128.0f) * logic->aSrc->gain;

      if (logic->aSrc->offset == logic->aSrc->totalSamples) {
        // TODO: if not loop==true
        // logic->aSrc->offset = 0;
        logic->aSrc = NULL;
        return;
      }
    }

    // LOG_DEBUGF(
    //     "stream_cb aSrc %p offset %u bps %u c %u s %u data %p sample %u sF %f",
    //     logic->aSrc,
    //     logic->aSrc->offset,
    //     logic->aSrc->bitsPerSample,
    //     logic->aSrc->numChannels,
    //     logic->aSrc->totalSamples,
    //     logic->aSrc->data,
    //     samples[0],
    //     buffer[i]);
  }
}

void Audio__shutdown() {
  g_engine->saudio_shutdown();
}

#undef SILENCE
