#include "Bmp.h"

#include <stdlib.h>
#include <string.h>

#include "../../../vendor/sokol/sokol_fetch.h"
#include "../Logic.h"
#include "Arena.h"
#include "Log.h"
#include "Utils.h"

#define MAX_FILE_SIZE 1024 * 1024 * 1  // 1MB
extern Engine__State* g_engine;
static void response_callback(const sfetch_response_t* response);

#pragma pack(push, 1)
typedef struct {
  char bfType[2];  // File type ("BM")
  u32 bfSize;  // File size
  u16 bfReserved1;  // Reserved, expected to be 0
  u16 bfReserved2;  // Reserved, expected to be 0
  u32 bfOffBits;  // Offset to start of pixel data
} BMPHeader;

typedef struct {
  u32 biSize;  // Size of the DIB header (40B)
  s32 biWidth;  // Image width
  s32 biHeight;  // Image height
  u16 biPlanes;  // Number of color planes (must be 1)
  u16 biBitCount;  // Bits per pixel (24 for RGB)
  u32 biCompression;  // Compression type (0 = none)
  u32 biSizeImage;  // Size of image data
  s32 biXPelsPerMeter;  // Horizontal resolution
  s32 biYPelsPerMeter;  // Vertical resolution
  u32 biClrUsed;  // Number of colors in the palette
  u32 biClrImportant;  // Important colors
} DIBHeader;
#pragma pack(pop)

BmpReader* Bmp__Read(const char* filePath) {
  BmpReader* r = Arena__Push(g_engine->arena, sizeof(BmpReader));
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
    u8* start = ptr;
    u8* end = ptr + response->data.size;
    BmpReader* r = (void*)*(size_t*)response->user_data;

    BMPHeader bmpHeader;
    mread(&bmpHeader, sizeof(BMPHeader), &ptr, end - ptr);

    ASSERT_CONTEXT(
        strncmp(bmpHeader.bfType, "BM", 2) == 0,
        "Invalid BMP format. file: %s,"
        " bfType: %04x %.2s",
        response->path,
        bmpHeader.bfType,
        bmpHeader.bfType);

    DIBHeader dibHeader;
    mread(&dibHeader, sizeof(DIBHeader), &ptr, end - ptr);

    r->w = dibHeader.biWidth;
    r->h = dibHeader.biHeight;
    r->bitsPerPixel = dibHeader.biBitCount;
    r->rowSize = (r->w * r->bitsPerPixel / 8 + 3) & ~3;  // Row size (padded to 4 bytes)
    r->sz = r->rowSize * r->h;
    r->buf = Arena__Push(g_engine->arena, r->sz);
    ptr = start + bmpHeader.bfOffBits;
    mread(r->buf, r->sz, &ptr, end - ptr);
    r->loaded = true;
  }
  if (response->finished) {
    free((void*)response->data.ptr);
    ASSERT_CONTEXT(
        !response->failed,
        "Failed to fetch BMP. file: %s,"
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

typedef struct Pixel {
  u8 r, g, b, a;
} Pixel;

u32 Bmp__Get2DPixel(BmpReader* bmp, u32 x, u32 y, u32 def) {
  if (x >= 0 && x < bmp->w && y >= 0 && y < bmp->h) {
    // Row size (padded to 4 bytes)
    u32 rowSize = (bmp->w * bmp->bitsPerPixel / 8 + 3) & ~3;
    // NOTICE: BMP rows are stored bottom to top
    u8* row = bmp->buf + (bmp->h - y - 1) * rowSize;
    u8* pixel = row + x * (bmp->bitsPerPixel / 8);
    Pixel p;
    if (24 == bmp->bitsPerPixel) {
      p = (Pixel){.r = pixel[2], .g = pixel[1], .b = pixel[0], .a = 255};
    } else if (32 == bmp->bitsPerPixel) {
      p = (Pixel){.r = pixel[3], .g = pixel[2], .b = pixel[1], .a = pixel[0]};
    }

    return *((u32*)&p);  // ABGR
  }
  return def;
}

char* Bmp__ToString(BmpReader* bmp, u32 sz) {
  char* out = malloc(sizeof(char) * sz);
  char* p = out;
  char* e = out + sizeof(char) * sz;

  mprintf(&p, "BMP Image Info:\n", e - p);
  mprintf(&p, "Width: %u\n", e - p, bmp->w);
  mprintf(&p, "Height: %u\n", e - p, bmp->h);
  mprintf(&p, "Bits Per Pixel: %u\n", e - p, bmp->bitsPerPixel);

  mprintf(&p, "\nPixel Data (first few rows):\n", e - p);
  // Row size (padded to 4 bytes)
  u32 rowSize = (bmp->w * bmp->bitsPerPixel / 8 + 3) & ~3;
  // Print up to the first 5 rows
  u32 numRowsToPrint = bmp->h < 5 ? bmp->h : 5;
  for (u32 y = 0; y < numRowsToPrint; y++) {
    // NOTICE: BMP rows are stored bottom to top
    u8* row = bmp->buf + (bmp->h - y - 1) * rowSize;
    for (u32 x = 0; x < bmp->w; x++) {
      u8* pixel = row + x * (bmp->bitsPerPixel / 8);
      mprintf(&p, "(%u, %u, %u) ", e - p, pixel[2], pixel[1], pixel[0]);  // RGB values
    }
    mprintf(&p, "\n", e - p);
  }

  mprintf(&p, "\nEnd of BMP Info\n", e - p);

  // user must call free()
  return out;
}

#undef MAX_FILE_SIZE