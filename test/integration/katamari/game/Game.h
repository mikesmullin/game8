#pragma once

#include "Logic.h"  // IWYU pragma: keep

// Globals ---------------------------------------------

#define WINDOW_SIZE (640)
#define SCREEN_SIZE ((u32)(WINDOW_SIZE / 4))

// Entities ----------------------------------------------

#include "Dispatcher.h"  // IWYU pragma: keep
#include "entities/Cube.h"  // IWYU pragma: keep

typedef struct PreloadedAudio {
  WavReader* pickupCoin;
} PreloadedAudio;

typedef struct PreloadedModels {
  Wavefront* cube;
} PreloadedModels;

typedef struct PreloadedTextures {
  BmpReader *albedo, *normal, *metalness, *roughness, *specularBRDF_LUT, *specular, *irradiance;
} PreloadedTextures;

typedef struct PreloadedShaders {
  Shader *atlas, *pbr;
} PreloadedShaders;

typedef struct PreloadedMaterials {
  Material* red;
} PreloadedMaterials;

typedef struct Game {
  bool playedSfxOnce, testComplete;
  List* entities;
  sg_pass* pass1;
} Game;

void Game__init();
void Game__preload();
void Game__reload();
void Game__tick();
void Game__render();
void Game__gui();
void Game__postprocessing();
void Game__shutdown();