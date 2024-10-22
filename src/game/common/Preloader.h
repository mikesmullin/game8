#pragma once

typedef struct Wavefront Wavefront;
typedef struct BmpReader BmpReader;
typedef struct WavReader WavReader;

Wavefront* Preload__model(Wavefront** saveSlot, const char* filePath);
BmpReader* Preload__texture(BmpReader** saveSlot, const char* filePath);
WavReader* Preload__audio(WavReader** saveSlot, const char* filePath);
