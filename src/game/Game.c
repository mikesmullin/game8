#include "Game.h"

#include "Logic.h"
#include "entities/Cube.h"
#include "entities/Triangle.h"

extern Engine__State* g_engine;

void Game__init() {
  g_engine->window_title = "Retro";
  u16 dims = 640;
  g_engine->window_width = dims;
  g_engine->window_height = dims;
}

void Game__preload() {
  // Triangle__preload();
  Cube__preload();
}

void Game__render() {
  // Triangle__render();
  Cube__render();
}