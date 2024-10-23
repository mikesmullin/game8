#pragma once

typedef struct Wavefront Wavefront;
typedef struct BmpReader BmpReader;
typedef struct Material Material;
typedef struct WavReader WavReader;

Wavefront* Preload__model(Wavefront** saveSlot, const char* filePath);
BmpReader* Preload__texture(BmpReader** saveSlot, const char* filePath);
Material* Preload__material(Material** saveSlot);
WavReader* Preload__audio(WavReader** saveSlot, const char* filePath);