#include "RedWallBlock.h"

#include "../../Logic.h"
#include "../../common/Dispatcher.h"
#include "../../common/Log.h"
#include "../../common/Preloader.h"
#include "../../components/AudioSource.h"
#include "../../levels/Level.h"
#include "Block.h"
#include "WallBlock.h"

extern Engine__State* g_engine;

void RedWallBlock__init(Entity* entity, f32 x, f32 z) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  WallBlock* self = (WallBlock*)block;
  WallBlock__init(entity, x, z);
  entity->engine->action = RED_WALL_BLOCK__ACTION;

  entity->render->color = 0x660000ff;  // red

  // preload assets
  Preload__audio(
      &logic->audio.click,  //
      "../assets/audio/sfx/click.wav");
}

bool findCatBlock(Entity* entity) {
  return entity->tags1 & TAG_CATSPAWN;
}

void RedWallBlock__action(Entity* entity, void* _action) {
  Logic__State* logic = g_engine->logic;
  CatEntity* self = (CatEntity*)entity;
  Action* action = (Action*)_action;

  if (ACTION_USE == action->type) {
    CatSpawnBlock* csb = (CatSpawnBlock*)Level__findEntity(logic->level, findCatBlock);
    csb->maxSpawnCount += 100;

    AudioSource__play(entity, g_engine->logic->audio.click);
  }
}