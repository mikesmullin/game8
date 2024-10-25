#include "CatSpawnBlock.h"

#include <stdint.h>

#include "../../Logic.h"
#include "../../common/Arena.h"
#include "../../common/Dispatcher.h"
#include "../../common/List.h"
#include "../../common/Math.h"
#include "../../common/Profiler.h"
#include "../../levels/Level.h"
#include "../CatEntity.h"
#include "Block.h"

extern Engine__State* g_engine;

typedef int32_t s32;

void CatSpawnBlock__init(Entity* entity, f32 x, f32 y) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  CatSpawnBlock* self = (CatSpawnBlock*)block;
  Block__init(block, x, y);
  block->base.engine->tick = CAT_SPAWN_BLOCK__TICK;
  block->base.engine->gui = CAT_SPAWN_BLOCK__GUI;
  block->base.collider = 0;
  entity->tags1 |= TAG_CATSPAWN;

  self->firstTick = true;
  self->spawnCount = 1;  // instances
  self->spawnInterval = 1.0f / 100;  // per sec
  self->animTime = 0;  // counter
  self->spawnedCount = 0;
  self->maxSpawnCount = 5;
}

void CatSpawnBlock__gui(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  CatSpawnBlock* self = (CatSpawnBlock*)block;

  // Bitmap__DebugText(
  //     &logic->screen,
  //     &logic->glyphs0,
  //     4,
  //     6 * 20,
  //     0xff00ff00,
  //     0xff000000,
  //     "cats %u",
  //     self->spawnedCount);
}

void CatSpawnBlock__tick(Entity* entity) {
  PROFILE__BEGIN(CAT_SPAWN_BLOCK__TICK);
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  CatSpawnBlock* self = (CatSpawnBlock*)block;

  if (self->firstTick) {
    self->firstTick = false;
  }

  self->animTime += g_engine->deltaTime;
  if (self->animTime > self->spawnInterval) {
    self->animTime -= self->spawnInterval;

    // spawn entities (like a particle emitter)
    for (u32 i = 0; i < self->spawnCount; i++) {
      if (self->spawnedCount >= self->maxSpawnCount) return;
      CatEntity* cat = Arena__Push(g_engine->arena, sizeof(CatEntity));
      CatEntity__init((Entity*)cat);
      cat->base.base.tform->pos.x = block->base.tform->pos.x + Math__random(-1, 1);
      cat->base.base.tform->pos.y = -0.25f;
      cat->base.base.tform->pos.z = block->base.tform->pos.z + Math__random(-1, 1);
      List__insort(
          logic->level->frameScratch[logic->level->frame],
          logic->level->entities,
          cat,
          Level__zsort);
      self->spawnedCount++;
    }
  }
  PROFILE__END(CAT_SPAWN_BLOCK__TICK);
}