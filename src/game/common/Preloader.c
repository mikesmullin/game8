#include "Preloader.h"

#include "Bmp.h"
#include "Wav.h"
#include "Wavefront.h"

Wavefront* Preload__model(Wavefront** saveSlot, const char* filePath) {
  if (0 == *saveSlot) (*saveSlot) = Wavefront__Read(filePath);
  return *saveSlot;
}

BmpReader* Preload__texture(BmpReader** saveSlot, const char* filePath) {
  if (0 == *saveSlot) (*saveSlot) = Bmp__Read(filePath);
  return *saveSlot;
}

WavReader* Preload__audio(WavReader** saveSlot, const char* filePath) {
  if (0 == *saveSlot) (*saveSlot) = Wav__Read(filePath);
  return *saveSlot;
}