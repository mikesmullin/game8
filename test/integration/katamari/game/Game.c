#include "Game.h"

#include "Logic.h"

void Game__init() {
  mmemcp(g_engine->window_title, "Katamari", 9);
}

void Game__preload() {
  Logic__State* logic = g_engine->logic;

  // preload assets
  logic->coin = Wav__Read("../assets/audio/sfx/pickupCoin.wav");
}

void Game__reload() {
}

void Game__tick() {
  Logic__State* logic = g_engine->logic;

  if (logic->coin->loaded && !logic->playedSfxOnce) {
    logic->playedSfxOnce = true;
    Audio__replay(logic->coin);
  }
  if (logic->playedSfxOnce && NULL == g_engine->aSrc && !logic->testComplete) {
    // audio playback completed
    logic->testComplete = true;
    g_engine->sapp_request_quit();
  }
}

void Game__render() {
}

void Game__gui() {
}

void Game__postprocessing() {
}

void Game__shutdown() {
}