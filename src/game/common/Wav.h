#pragma once

#include <stdint.h>

typedef struct WavReader {
  int bitsPerSample;
  int bytesPerSample;
  int totalSamples;
  int numChannels;
  int offset;
  uint8_t* data;
} WavReader;

void Wav__Read(const char* filePath, WavReader* r);
void Wav__NextSample(WavReader* r, uint8_t* buffer);