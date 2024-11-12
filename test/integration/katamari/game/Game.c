#include "Game.h"

#include "Logic.h"

void Game__init() {
  Logic__State* logic = g_engine->logic;

  mmemcp(g_engine->window_title, "Katamari", 9);
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