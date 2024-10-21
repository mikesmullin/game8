#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct BmpReader {
  u32 w, h;
  u16 bitsPerPixel;
  u32 rowSize, sz;
  u8* buf;
  bool loaded;
} BmpReader;

BmpReader* Bmp__Read(const char* filePath);
u32 Bmp__Get2DPixel(BmpReader* bmp, u32 x, u32 y, u32 def);
char* Bmp__ToString(BmpReader* bmp, u32 sz);