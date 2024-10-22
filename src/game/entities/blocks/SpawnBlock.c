#include "SpawnBlock.h"

#include <stdint.h>

#include "../../Logic.h"
#include "../../common/Arena.h"
#include "../../common/Audio.h"
#include "../../common/Dispatcher.h"
#include "../../common/Log.h"
#include "../../common/Preloader.h"
#include "Block.h"

typedef int32_t s32;

extern Engine__State* g_engine;

Block* SpawnBlock__alloc() {
  return Arena__Push(g_engine->arena, sizeof(SpawnBlock));
}

void SpawnBlock__init(Entity* entity, f32 x, f32 y) {
  Block* block = (Block*)entity;
  SpawnBlock* self = (SpawnBlock*)block;
  Logic__State* logic = g_engine->logic;
  Block__init(block, x, y);
  block->base.engine->tick = SPAWN_BLOCK__TICK;
  block->base.collider = NULL;

  self->firstTick = true;
  logic->level->spawner = self;

  // preload assets
  Preload__audio(
      &logic->audio.powerUp,  //
      "../assets/audio/sfx/powerUp.wav");

  LOG_DEBUGF("SpawnBlock %u pos %f %f", entity->id, entity->tform->pos.x, entity->tform->pos.z);
}

void SpawnBlock__tick(Entity* entity) {
  Block* block = (Block*)entity;
  SpawnBlock* self = (SpawnBlock*)block;
  Logic__State* logic = g_engine->logic;

  if (self->firstTick) {
    self->firstTick = false;

    Audio__replay(logic->audio.powerUp);

    logic->player->base.tform->pos.x = block->base.tform->pos.x;
    logic->player->base.tform->pos.y = 0;
    logic->player->base.tform->pos.z = block->base.tform->pos.z;
    logic->player->base.tform->rot.x = 0;
    logic->player->base.tform->rot.y = 0;
  }
}
