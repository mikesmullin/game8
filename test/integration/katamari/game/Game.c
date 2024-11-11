#include "Game.h"

#include <string.h>

#include "Logic.h"
// #include "../../../../src/game/common/Audio.h"
#include "../../../../src/engine/common/Dispatcher.h"
#include "../../../../src/engine/common/List.h"
// #include "../../../../src/engine/common/Preloader.h"
// #include "../../../../src/engine/common/Profiler.h"
#include "../../../../src/engine/common/Wav.h"
// #include "../../../../src/game/components/MeshRenderer.h"
// #include "../../../../src/game/entities/NetMgr.h"
// #include "../../../../src/game/entities/Player.h"
#include "levels/Level.h"

extern Engine__State* g_engine;

void Game__init() {
  Logic__State* logic = g_engine->logic;

  memcpy(g_engine->window_title, "Katamari", 6);
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
  logic->level = level;
  Level__preload(logic->level);

  // init network
  NetMgr__init();
}

void Game__tick() {
  Logic__State* logic = g_engine->logic;

  // in-game
  NetMgr__tick();
  Dispatcher__call1(logic->player->base.engine->tick, (Entity*)logic->player);
  Level__tick(logic->level);
}

void Game__render() {
  Logic__State* logic = g_engine->logic;

  // in-game
  // Level__render(logic->level);

  // PostProcessing();
}

void Game__gui() {
  Logic__State* logic = g_engine->logic;

  // switch current camera to ortho cam at player pos
  logic->camera->proj.type = ORTHOGRAPHIC_PROJECTION;

  List__Node* node = logic->ui_entities->head;
  for (u32 i = 0; i < logic->ui_entities->len; i++) {
    Entity* entity = node->data;
    node = node->next;

    Dispatcher__call1(entity->engine->tick, entity);

    if (0 == entity->render) continue;
    // Dispatcher__call1(entity->engine->gui, entity);
    // Dispatcher__call1(entity->engine->render, entity);
  }

  // MeshRenderer__renderBatches(logic->ui_entities);

  // switch current camera to perspective cam at player pos
  logic->camera->proj.type = PERSPECTIVE_PROJECTION;
}

void Game__postprocessing() {
  Logic__State* logic = g_engine->logic;

  // apply pixelized post-processing effect
  logic->camera->proj.type = ORTHOGRAPHIC_PROJECTION;
  // ...
  logic->camera->proj.type = PERSPECTIVE_PROJECTION;
}

void Game__shutdown() {
  NetMgr__shutdown();
}