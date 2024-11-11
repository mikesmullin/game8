#include "Preloader.h"

#include "../../game/Logic.h"
#include "../Engine.h"
#include "Bmp.h"
#include "Wav.h"
#include "Wavefront.h"

Wavefront* Preload__model(Wavefront** saveSlot, const char* filePath) {
  if (0 == *saveSlot) {
    LOG_DEBUGF("Preloading model %s", filePath);
    (*saveSlot) = Wavefront__Read(filePath);
  }
  return *saveSlot;
}

BmpReader* Preload__texture(BmpReader** saveSlot, const char* filePath) {
  if (0 == *saveSlot) {
    LOG_DEBUGF("Preloading texture %s", filePath);
    (*saveSlot) = Bmp__Read(filePath);
  }
  return *saveSlot;
}

Material* Preload__material(Material** saveSlot, u64 sz) {
  if (0 == *saveSlot) {
    (*saveSlot) = Arena__Push(g_engine->arena, sz);
  }
  return *saveSlot;
}

WavReader* Preload__audio(WavReader** saveSlot, const char* filePath) {
  if (0 == *saveSlot) {
    LOG_DEBUGF("Preloading audio %s", filePath);
    (*saveSlot) = Wav__Read(filePath);
  }
  return *saveSlot;
}