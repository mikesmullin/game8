#pragma once

#include "Types.h"

typedef struct BmpReader {
  u32 w, h;
  u16 bitsPerPixel;
  u32 rowSize, sz;
  u8* buf;
  bool loaded;
} BmpReader;

BmpReader* Bmp__Read(const char* filePath);
u32 Bmp__Get2DPixel(BmpReader* bmp, u32 x, u32 y, u32 def);
u32 Bmp__Get2DTiledPixel(BmpReader* bmp, u32 x, u32 y, u32 ts, u32 tx, u32 ty, u32 def);
char* Bmp__ToString(BmpReader* bmp, u32 sz);