#include "CatSpawnBlock.h"

#include "../../../engine/common/Dispatcher.h"
#include "../../../engine/common/List.h"
#include "../../../engine/common/Profiler.h"
#include "../../Logic.h"
#include "../../levels/Level.h"
#include "../CatEntity.h"
#include "Block.h"

typedef int32_t s32;

void CatSpawnBlock__init(Entity* entity, f32 x, f32 y) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  CatSpawnBlock* self = (CatSpawnBlock*)block;
  Block__init(block, x, y);
  block->base.engine->tick = CAT_SPAWN_BLOCK__TICK;
  block->base.collider = 0;
  entity->tags1 |= TAG_CATSPAWN;

  self->spawnCount = 100;  // spawn this many instances...
  self->spawnInterval = 1.0f / 10;  // every (sec)
  self->animTime = 0;  // counter
  self->spawnedCount = 0;
  self->maxSpawnCount = 5;  // 1000000;
}

void CatSpawnBlock__tick(Entity* entity) {
  PROFILE__BEGIN(CAT_SPAWN_BLOCK__TICK);
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  CatSpawnBlock* self = (CatSpawnBlock*)block;

  self->animTime += g_engine->deltaTime;
  if (self->animTime > self->spawnInterval) {
    self->animTime -= self->spawnInterval;

    // spawn entities (like a particle emitter)
    for (u32 i = 0; i < self->spawnCount; i++) {
      if (self->spawnedCount >= self->maxSpawnCount) return;
      CatEntity* cat = Arena__Push(g_engine->arena, sizeof(CatEntity));
      CatEntity__init((Entity*)cat);
      cat->base.base.tform->pos.x = block->base.tform->pos.x + Math__random(-1, 1);
      cat->base.base.tform->pos.y = -(1.0f / 8);
      cat->base.base.tform->pos.z = block->base.tform->pos.z + Math__random(-1, 1);
      List__append(g_engine->arena, logic->level->entities, cat);
      self->spawnedCount++;
    }
  }
  PROFILE__END(CAT_SPAWN_BLOCK__TICK);
}