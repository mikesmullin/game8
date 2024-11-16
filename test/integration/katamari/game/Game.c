#include "Game.h"

void Game__init() {
  mmemcp(g_engine->window_title, "Katamari", 9);
  g_engine->window_width = WINDOW_SIZE;
  g_engine->window_height = WINDOW_SIZE;

  g_engine->players = List__alloc(g_engine->arena);
  g_engine->audio = Arena__Push(g_engine->arena, sizeof(PreloadedAudio));
  g_engine->models = Arena__Push(g_engine->arena, sizeof(PreloadedModels));
  g_engine->textures = Arena__Push(g_engine->arena, sizeof(PreloadedTextures));
  g_engine->materials = Arena__Push(g_engine->arena, sizeof(PreloadedMaterials));
}

void Game__preload() {
  // preload assets
  g_engine->audio->pickupCoin = Wav__Read("../assets/audio/sfx/pickupCoin.wav");

  Cube* cube = (Cube*)Arena__Push(g_engine->arena, sizeof(Cube));
  Cube__init((Entity*)cube);
}

void Game__reload() {
}

void Game__tick() {
  if (g_engine->audio->pickupCoin->loaded && !g_engine->game->playedSfxOnce) {
    g_engine->game->playedSfxOnce = true;
    Audio__replay(g_engine->audio->pickupCoin);
  }
  if (g_engine->game->playedSfxOnce && NULL == g_engine->aSrc && !g_engine->game->testComplete) {
    g_engine->game->testComplete = true;
    // audio playback completed
    // g_engine->sapp_request_quit();
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