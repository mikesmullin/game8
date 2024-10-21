#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct WavReader {
  u32 numChannels;
  u32 bitsPerSample;
  u32 totalSamples;
  uint8_t* data;
  bool loaded;
  u32 offset;
} WavReader;

WavReader* Wav__Read(const char* filePath);
void Wav__NextSample(WavReader* r, u8* buffer);