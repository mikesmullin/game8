#include "Game.h"

#include <string.h>

#include "Logic.h"

extern Engine__State* g_engine;

void Game__init() {
  Logic__State* logic = g_engine->logic;

  memcpy(g_engine->window_title, "Katamari", 9);
}

void Game__preload() {
}

void Game__tick() {
}

void Game__render() {
}

void Game__gui() {
}

void Game__postprocessing() {
}

void Game__shutdown() {
}