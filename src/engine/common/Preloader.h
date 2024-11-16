#pragma once

#include "Types.h"

typedef struct Wavefront Wavefront;
typedef struct BmpReader BmpReader;
typedef struct Material Material;
typedef struct WavReader WavReader;

Wavefront* Preload__model(Wavefront** saveSlot, const char* path, const char* file);
BmpReader* Preload__texture(BmpReader** saveSlot, const char* filePath);
Material* Preload__material(Material** saveSlot, u64 sz);
WavReader* Preload__audio(WavReader** saveSlot, const char* filePath);