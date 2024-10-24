#include "Game.h"

#include "Logic.h"
#include "common/Audio.h"
#include "common/Dispatcher.h"
#include "common/Preloader.h"
#include "common/Wav.h"
#include "entities/Player.h"
#include "levels/Level.h"

extern Engine__State* g_engine;

void Game__init() {
  Logic__State* logic = g_engine->logic;

  g_engine->window_title = "Retro";
  u16 dims = 640;
  g_engine->window_width = dims;
  g_engine->window_height = dims;

  logic->level = 0;
  logic->player = 0;
  logic->lastUid = 0;
}

void Game__preload() {
  Logic__State* logic = g_engine->logic;

  // preload assets
  Preload__audio(
      &logic->audio.pickupCoin,  //
      "../assets/audio/sfx/pickupCoin.wav");

  Level* level = Level__alloc();
  Level__init(level);
  level->width = 300;
  level->depth = 300;
  level->height = 10;
  // DEBUG: UV ColorMap
  // level->wallTex = 0;
  // level->wallCol = TRANSPARENT;
  level->wallTex = 5;
  level->wallCol = 0x66ff0000;
  level->skybox = true;
  // level->ceilCol = 0x77000022;  // blood red
  level->ceilCol = 0xaa000000;  // darken
  // level->levelFile = "../assets/textures/level0.bmp";  // DEBUG: single block
  level->levelFile = "../assets/textures/level1.bmp";
  level->worldFile = "../assets/textures/sky.bmp";
  logic->level = level;
  Level__preload(logic->level);
}

void Game__reload() {
  Logic__State* logic = g_engine->logic;

  Audio__replay(logic->audio.pickupCoin);
}

void Game__tick() {
  Logic__State* logic = g_engine->logic;

  if (0 == logic->player) {
    logic->player = Player__alloc();
    Player__init((Entity*)logic->player);
  }

  // in-game
  Dispatcher__call1(logic->player->base.engine->tick, (Entity*)logic->player);
  Level__tick(logic->level);
}

void Game__render() {
  Logic__State* logic = g_engine->logic;

  // in-game
  Level__render(logic->level);
  // PostProcessing();
}

void Game__gui() {
  Logic__State* logic = g_engine->logic;

  // // draw debug cursor
  // Bitmap__Set2DPixel(
  //     &logic->screen,
  //     logic->CANVAS_DEBUG_X,
  //     logic->CANVAS_DEBUG_Y,
  //     Math__urandom() | 0xffff0000 + 0xff993399);

  // Bitmap__DebugText(
  //     &logic->screen,
  //     &logic->glyphs0,
  //     4,
  //     6 * 29,
  //     0xffffffff,
  //     0,
  //     "cam x %+06.1f y %+06.1f z %+06.1f r %+06.1f",
  //     logic->player.base->tform->pos.x,
  //     logic->player.base->tform->pos.y,
  //     logic->player.base->tform->pos.z,
  //     logic->player.base->tform->rot.y);
}
