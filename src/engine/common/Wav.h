#pragma once

#include "Types.h"

typedef struct WavReader {
  u32 numChannels;
  u32 bitsPerSample;
  u32 totalSamples;
  uint8_t* data;
  bool loaded;
  u32 offset;
  f32 gain;
} WavReader;

WavReader* Wav__Read(const char* filePath);
void Wav__NextSample(WavReader* r, u32* buffer);