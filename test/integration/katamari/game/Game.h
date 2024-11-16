#pragma once

#include "Logic.h"  // IWYU pragma: keep

// Globals ---------------------------------------------

#define WINDOW_SIZE (640)
#define SCREEN_SIZE ((u32)(WINDOW_SIZE / 4))

// Entities ----------------------------------------------

typedef struct Game {
  WavReader* coin;
  bool playedSfxOnce, testComplete;
} Game;

void Game__init();
void Game__preload();
void Game__reload();
void Game__tick();
void Game__render();
void Game__gui();
void Game__postprocessing();
void Game__shutdown();