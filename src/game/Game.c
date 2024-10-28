#include "Game.h"

#include <string.h>

#include "Logic.h"
#include "common/Arena.h"
#include "common/Audio.h"
#include "common/Color.h"
#include "common/Dispatcher.h"
#include "common/List.h"
#include "common/Log.h"
#include "common/Preloader.h"
#include "common/Profiler.h"
#include "common/Utils.h"
#include "common/Wav.h"
#include "components/MeshRenderer.h"
#include "entities/DebugText.h"
#include "entities/Player.h"
#include "levels/Level.h"

extern Engine__State* g_engine;

void Game__init() {
  Logic__State* logic = g_engine->logic;

  memcpy(g_engine->window_title, "Retro", 6);
  u16 dims = 640;
  g_engine->window_width = dims;
  g_engine->window_height = dims;

  logic->level = 0;
  logic->player = 0;
  logic->lastUid = 0;
}

void Game__preload() {
  Logic__State* logic = g_engine->logic;

  logic->ui_entities = List__alloc(g_engine->arena);

  logic->player = Arena__Push(g_engine->arena, sizeof(Player));
  Player__init((Entity*)logic->player);
  logic->camera = logic->player;  // 1st player = main camera

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

  logic->dt = Arena__Push(g_engine->arena, sizeof(DebugText));
  char txt[40] = "Hello world!";
  DebugText__init((Entity*)logic->dt, 0, 0, 40, txt, LIME);
  List__insort(g_engine->arena, logic->ui_entities, logic->dt, Level__zsort);

  // preload assets
  Preload__audio(
      &logic->audio.pickupCoin,  //
      "../assets/audio/sfx/pickupCoin.wav");
}

void Game__reload() {
  Logic__State* logic = g_engine->logic;

  Audio__replay(logic->audio.pickupCoin);
}

void Game__tick() {
  Logic__State* logic = g_engine->logic;

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
  PROFILE__BEGIN(GAME__GUI);

  Logic__State* logic = g_engine->logic;

  // switch current camera to ortho cam at player pos
  logic->camera->proj.type = ORTHOGRAPHIC_PROJECTION;

  char* c = logic->dt->txt;
  mprintf(
      &c,
      "cam x %+06.1f y %+06.1f z %+06.1f r %+06.1f",
      logic->dt->glyphs->len,
      logic->player->base.tform->pos.x,
      logic->player->base.tform->pos.y,
      logic->player->base.tform->pos.z,
      logic->player->base.tform->rot.y);
  logic->dt->base.tform->pos.x = -55;
  logic->dt->base.tform->pos.y = -70;
  logic->dt->base.tform->scale.x = logic->dt->base.tform->scale.y = 5;

  List__Node* node = logic->ui_entities->head;
  for (u32 i = 0; i < logic->ui_entities->len; i++) {
    Entity* entity = node->data;
    node = node->next;

    Dispatcher__call1(entity->engine->tick, entity);

    if (0 == entity->render) continue;
    Dispatcher__call1(entity->engine->gui, entity);
    Dispatcher__call1(entity->engine->render, entity);
  }

  MeshRenderer__renderBatches(logic->ui_entities);

  // switch current camera to perspective cam at player pos
  logic->camera->proj.type = PERSPECTIVE_PROJECTION;

  // // TODO: draw debug cursor
  // Bitmap__Set2DPixel(
  //     &logic->screen,
  //     logic->CANVAS_DEBUG_X,
  //     logic->CANVAS_DEBUG_Y,
  //     Math__urandom() | 0xffff0000 + 0xff993399);

  PROFILE__END(GAME__GUI);
}
