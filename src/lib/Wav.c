#include "Wav.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Log.h"

// Define structures for WAV header

// RIFF Chunk
typedef struct {
  char chunkID[4];  // "RIFF"
  uint32_t chunkSize;  // File size - 8 bytes
  char format[4];  // "WAVE"
} RIFFHeader;

// Format Chunk
typedef struct {
  char subChunk1ID[4];  // "fmt "
  uint32_t subChunk1Size;  // Size of this subchunk (usually 16 for PCM)
  uint16_t audioFormat;  // Audio format (1 = PCM, others for compression)
  uint16_t numChannels;  // Number of channels (1 = Mono, 2 = Stereo)
  uint32_t sampleRate;  // Sampling rate (e.g., 44100 Hz)
  uint32_t byteRate;  // SampleRate * NumChannels * BitsPerSample / 8
  uint16_t blockAlign;  // NumChannels * BitsPerSample / 8
  uint16_t bitsPerSample;  // Bits per sample (8, 16, 24, 32)
} FormatChunk;

// Data Chunk
typedef struct {
  char subChunk2ID[4];  // "data"
  uint32_t subChunk2Size;  // NumSamples * NumChannels * BitsPerSample / 8
} DataChunk;

// Function to read the WAV file header
void Wav__Read(const char* filePath, WavReader* r) {
  RIFFHeader riff;
  FormatChunk fmt;
  DataChunk data;

  FILE* file;
  if (0 != fopen_s(&file, filePath, "r")) {
    return;
  }

  // Read RIFF header
  fread(&riff, sizeof(RIFFHeader), 1, file);
  if (strncmp(riff.chunkID, "RIFF", 4) != 0 || strncmp(riff.format, "WAVE", 4) != 0) {
    printf("This is not a valid WAV file.\n");
    return;
  }

  // Read Format Chunk
  fread(&fmt, sizeof(FormatChunk), 1, file);
  if (strncmp(fmt.subChunk1ID, "fmt ", 4) != 0) {
    printf("Invalid format chunk.\n");
    return;
  }

  printf("WAV File Information:\n");
  printf("----------------------\n");
  printf("Audio Format: %s\n", fmt.audioFormat == 1 ? "PCM" : "Compressed");
  printf("Number of Channels: %d\n", fmt.numChannels);
  printf("Sample Rate: %d Hz\n", fmt.sampleRate);
  printf("Bits per Sample: %d\n", fmt.bitsPerSample);

  // Skip any extra format data (subChunk1Size could be larger than expected)
  if (fmt.subChunk1Size > 16) {
    fseek(file, fmt.subChunk1Size - 16, SEEK_CUR);
  }

  // Read Data Chunk
  fread(&data, sizeof(DataChunk), 1, file);
  if (strncmp(data.subChunk2ID, "data", 4) != 0) {
    printf("Invalid data chunk.\n");
    return;
  }

  printf("Data Size: %d bytes\n", data.subChunk2Size);

  r->offset = 0;
  r->bitsPerSample = fmt.bitsPerSample;
  r->bytesPerSample = fmt.bitsPerSample / 8;
  r->numChannels = fmt.numChannels;
  r->totalSamples = data.subChunk2Size / (r->numChannels * r->bytesPerSample);
  r->data = malloc(data.subChunk2Size);
  fread(r->data, data.subChunk2Size, 1, file);
  // printf("bytes:");
  // for (int i = 0; i < data.subChunk2Size; i++) {
  //   printf(" %u", r->data[i]);
  // }
  // printf("\n");

  fclose(file);
}

// read one sample at a time
void Wav__NextSample(WavReader* r, uint8_t* buffer) {
  // read next sample
  if (r->offset >= r->totalSamples) {
    // r->offset = 0;  // loop forever

    // return silence
    for (int ch = 0; ch < r->numChannels; ch++) {
      ((int*)buffer)[ch] = 128;
    }
    return;
  }

  // read the sample value for each channel
  uint8_t sbuf[r->bytesPerSample * r->numChannels];
  for (int ch = 0; ch < r->numChannels; ch++) {
    int sampleValue = 0;
    int idx = r->offset + ch;
    sbuf[ch] = r->data[idx];

    // Convert the sample based on bitsPerSample
    if (r->bitsPerSample == 8) {
      sampleValue = sbuf[ch];  // 8-bit is unsigned
    } else if (r->bitsPerSample == 16) {
      sampleValue = ((int16_t*)sbuf)[ch];  // 16-bit is signed
    } else if (r->bitsPerSample == 24) {
      // 24-bit requires manual conversion (3 bytes)
      sampleValue = sbuf[3 * ch] | (sbuf[3 * ch + 1] << 8) | (sbuf[3 * ch + 2] << 16);
      // Adjust for sign extension in 24-bit PCM
      if (sampleValue & 0x800000) {
        sampleValue |= 0xFF000000;
      }
    } else if (r->bitsPerSample == 32) {
      sampleValue = ((int32_t*)sbuf)[ch];  // 32-bit is signed
    }

    // printf("Sample %d, Channel %d: %d\n", i + 1, ch + 1, sampleValue);

    ((int*)buffer)[ch] = sampleValue;
    // LOG_DEBUGF("offset %u ch %u sampleValue %u", r->offset, ch, buffer[ch]);
  }

  r->offset++;
}