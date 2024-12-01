#include "CatSpawnBlock.h"

typedef int32_t s32;

void CatSpawnBlock__init(Entity* entity, f32 x, f32 y) {
  Block* block = (Block*)entity;
  CatSpawnBlock* self = (CatSpawnBlock*)block;
  Block__init(block, x, y);
  block->base.dispatch->tick = CAT_SPAWN_BLOCK__TICK;
  block->base.collider = 0;
  entity->tags1 |= TAG_CATSPAWN;

  self->animTime = 0;  // counter
  self->spawnedCount = 0;
  self->maxSpawnCount = 5;
}

void CatSpawnBlock__tick(void* _params) {
  PROFILE__BEGIN(CAT_SPAWN_BLOCK__TICK);
  OnEntityParams* params = _params;
  Entity* entity = params->entity;
  Block* block = (Block*)entity;
  CatSpawnBlock* self = (CatSpawnBlock*)block;

  self->spawnInterval = 0.1f;  // 1.0f = 1 sec (how often to inject SpawnCount more)

  self->animTime += g_engine->deltaTime;
  if (self->animTime > self->spawnInterval) {
    self->animTime = 0;
    // spawn entities (like a particle emitter)
    for (u32 i = 0, len = self->maxSpawnCount - self->spawnedCount; i < len; i++) {
      CatEntity* cat = Arena__Push(g_engine->arena, sizeof(CatEntity));
      CatEntity__init((Entity*)cat);
      cat->base.base.tform->pos.x =
          block->base.tform->pos.x + Math__randomf(-1, 1, &g_engine->seeds.entityMove);
      cat->base.base.tform->pos.y = -(1.0f / 8);
      cat->base.base.tform->pos.z =
          block->base.tform->pos.z + Math__randomf(-1, 1, &g_engine->seeds.entityMove);
      List__append(g_engine->arena, g_engine->game->level->entities, cat);
      self->spawnedCount++;
    }
  }
  PROFILE__END(CAT_SPAWN_BLOCK__TICK);
}