#include "Wav.h"

#include <stdlib.h>
#include <string.h>

#include "../../../vendor/sokol/sokol_fetch.h"
#include "../Logic.h"
#include "Arena.h"
#include "Log.h"
#include "Utils.h"

#define MAX_FILE_SIZE 1024 * 1024 * 1  // 1MB
extern Engine__State* g_engine;

#pragma pack(push, 1)
typedef struct RIFFHeader {
  char chunkID[4];  // "RIFF"
  u32 chunkSize;  // File size - 8 bytes
  char format[4];  // "WAVE"
} RIFFHeader;

typedef enum : u16 {
  NONE,
  PCM,
} WavType;

typedef struct FormatChunk {
  char subChunk1ID[4];  // "fmt "
  u32 subChunk1Size;  // Size of this subchunk (usually 16 for PCM)
  WavType audioFormat;  // Audio format (1 = PCM, others for compression)
  u16 numChannels;  // Number of channels (1 = Mono, 2 = Stereo)
  u32 sampleRate;  // Sampling rate (e.g., 44100 Hz)
  u32 byteRate;  // SampleRate * NumChannels * BitsPerSample / 8
  u16 blockAlign;  // NumChannels * BitsPerSample / 8
  u16 bitsPerSample;  // Bits per sample (8, 16, 24, 32)
} FormatChunk;

typedef struct DataChunk {
  char subChunk2ID[4];  // "data"
  u32 subChunk2Size;  // NumSamples * NumChannels * BitsPerSample / 8
} DataChunk;
#pragma pack(pop)

static void response_callback(const sfetch_response_t* response);

WavReader* Wav__Read(const char* filePath) {
  WavReader* r = Arena__Push(g_engine->arena, sizeof(WavReader));
  u8* buf = malloc(sizeof(u8[MAX_FILE_SIZE]));

  g_engine->sfetch_send(&(sfetch_request_t){
      .path = filePath,
      .user_data = {.ptr = &r, .size = sizeof(size_t)},
      .callback = response_callback,
      .buffer = {.ptr = buf, .size = sizeof(u8[MAX_FILE_SIZE])}});

  return r;
}

static void response_callback(const sfetch_response_t* response) {
  if (response->fetched) {
    u8* ptr = (void*)response->data.ptr;
    u8* end = ptr + response->data.size;
    WavReader* r = (void*)*(size_t*)response->user_data;

    // Read RIFF header
    RIFFHeader riff;
    mread(&riff, sizeof(RIFFHeader), &ptr, end - ptr);

    ASSERT_CONTEXT(
        strncmp(riff.chunkID, "RIFF", 4) == 0 &&  //
            strncmp(riff.format, "WAVE", 4) == 0,
        "Invalid WAV format. file: %s,"
        " chunkID: %.4s,"
        " format: %.4s.",
        response->path,
        riff.chunkID,
        riff.format);

    // Read Format Chunk
    FormatChunk fmt;
    mread(&fmt, sizeof(FormatChunk), &ptr, end - ptr);
    ASSERT_CONTEXT(
        strncmp(fmt.subChunk1ID, "fmt ", 4) == 0,  //
        "Invalid WAV format. file: %s,"
        " subChunk1ID: %.4s.",
        response->path,
        fmt.subChunk1ID);

    ASSERT_CONTEXT(fmt.audioFormat == PCM, "WAV not supported. audioFormat: %u", fmt.audioFormat);
    ASSERT_CONTEXT(
        fmt.bitsPerSample == 8,
        "WAV not supported. bitsPerSample: %u",
        fmt.bitsPerSample);
    ASSERT_CONTEXT(fmt.numChannels == 1, "WAV not supported. numChannels: %u", fmt.numChannels);
    ASSERT_CONTEXT(fmt.sampleRate == 44100, "WAV not supported. sampleRate: %u", fmt.sampleRate);

    // Skip any extra format data (subChunk1Size could be larger than expected)
    if (fmt.subChunk1Size > 16) {
      ptr += fmt.subChunk1Size - 16;
    }

    // Read Data Chunk
    DataChunk data;
    mread(&data, sizeof(DataChunk), &ptr, end - ptr);
    ASSERT_CONTEXT(
        strncmp(data.subChunk2ID, "data", 4) == 0,  //
        "Invalid WAV format. file: %s,"
        " subChunk2ID: %.4s.",
        response->path,
        data.subChunk2ID);

    ASSERT_CONTEXT(
        data.subChunk2Size <= MAX_FILE_SIZE,
        "WAV not supported. data size: %u > %u",
        data.subChunk2Size,
        MAX_FILE_SIZE);

    r->offset = 0;
    r->bitsPerSample = fmt.bitsPerSample;
    r->numChannels = fmt.numChannels;
    r->totalSamples = data.subChunk2Size / (r->numChannels * (r->bitsPerSample / 8));
    r->data = Arena__Push(g_engine->arena, data.subChunk2Size);
    mread(r->data, data.subChunk2Size, &ptr, end - ptr);
    r->loaded = true;
  }
  if (response->finished) {
    free((void*)response->data.ptr);
    ASSERT_CONTEXT(
        !response->failed,
        "Failed to fetch WAV. file: %s,"
        " error_code: %u,"  // see enum sfetch_error_t
        " failed: %u,"
        " finished: %u,"
        " cancelled: %u.",
        response->path,
        response->error_code,
        response->failed,
        response->finished,
        response->cancelled);
  }
}

// read one sample at a time
void Wav__NextSample(WavReader* r, uint8_t* buffer) {
  // if not loaded, or cursor past end
  if (!r->loaded || r->offset >= r->totalSamples) {
    // r->offset = 0;  // loop forever

    // return silence
    for (int ch = 0; ch < r->numChannels; ch++) {
      ((int*)buffer)[ch] = 128;
    }
    return;
  }

  // read the sample value for each channel
  uint8_t sbuf[(r->bitsPerSample / 8) * r->numChannels];
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

    ((int*)buffer)[ch] = sampleValue;
  }

  r->offset++;

  // return data should be read like:
  // u8 samples[(logic->wr->bitsPerSample / 8) * logic->wr->numChannels];
}

#undef MAX_FILE_SIZE