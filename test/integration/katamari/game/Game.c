#include "Game.h"

void Game__init() {
  mmemcp(g_engine->window_title, "Katamari", 9);
}

void Game__preload() {
  // preload assets
  g_engine->game->coin = Wav__Read("../assets/audio/sfx/pickupCoin.wav");
}

void Game__reload() {
}

void Game__tick() {
  if (g_engine->game->coin->loaded && !g_engine->game->playedSfxOnce) {
    g_engine->game->playedSfxOnce = true;
    Audio__replay(g_engine->game->coin);
  }
  if (g_engine->game->playedSfxOnce && NULL == g_engine->aSrc && !g_engine->game->testComplete) {
    // audio playback completed
    g_engine->game->testComplete = true;
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