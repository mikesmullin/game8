#include "Audio.h"

#include "../Engine.h"
#include "Wav.h"

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
  // TODO: support gain
  // TODO: support pan
  // TODO: support multiple voices

  r->offset = 0;  // replay the current sfx
  g_engine->aSrc = r;
}

void Audio__stop() {
  g_engine->aSrc = NULL;
}

// the sample callback, running in audio thread
void Audio__stream_cb(f32* buffer, int num_frames, int num_channels) {
  if (!g_engine->useAudio) return;
  ASSERT(1 == num_channels);
  if (NULL == g_engine->aSrc) {  // no current audio source
    for (u32 i = 0; i < num_frames; i++) {
      buffer[i] = SILENCE;
    }
    return;
  }

  u32 samples[g_engine->aSrc->numChannels];
  for (u32 i = 0; i < num_frames; i++) {
    if (!g_engine->aSrc->loaded) {
      buffer[i] = SILENCE;
    } else {
      Wav__NextSample(g_engine->aSrc, samples);
      // assume 1 channel, 1 byte per sample
      buffer[i] = (((s32)(samples[0]) - 128) / 128.0f) * g_engine->aSrc->gain;

      if (g_engine->aSrc->offset == g_engine->aSrc->totalSamples) {
        // TODO: if not loop==true
        // g_engine->aSrc->offset = 0;
        g_engine->aSrc = NULL;
        return;
      }
    }

    // LOG_DEBUGF(
    //     "stream_cb aSrc %p offset %u bps %u c %u s %u data %p sample %u sF %f",
    //     g_engine->aSrc,
    //     g_engine->aSrc->offset,
    //     g_engine->aSrc->bitsPerSample,
    //     g_engine->aSrc->numChannels,
    //     g_engine->aSrc->totalSamples,
    //     g_engine->aSrc->data,
    //     samples[0],
    //     buffer[i]);
  }
}

void Audio__shutdown() {
  g_engine->saudio_shutdown();
}

#undef SILENCE
