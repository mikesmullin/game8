#include "Preloader.h"

#include "../Logic.h"
#include "Arena.h"
#include "Bmp.h"
#include "Log.h"
#include "Wav.h"
#include "Wavefront.h"

extern Engine__State* g_engine;

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

Material* Preload__material(Material** saveSlot) {
  if (0 == *saveSlot) {
    (*saveSlot) = Arena__Push(g_engine->arena, sizeof(Material));
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