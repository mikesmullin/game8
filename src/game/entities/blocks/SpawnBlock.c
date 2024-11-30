#include "SpawnBlock.h"

typedef int32_t s32;

void SpawnBlock__init(Entity* entity, f32 x, f32 z) {
  Block* block = (Block*)entity;
  SpawnBlock* self = (SpawnBlock*)block;
  Block__init(block, x, z);
  block->base.dispatch->tick = SPAWN_BLOCK__TICK;
  block->base.collider = 0;

  self->firstTick = true;
  g_engine->game->level->spawner = self;

  // preload assets
  Preload__audio(
      &g_engine->audio->powerUp,  //
      "../assets/audio/sfx/powerUp.wav");
}

void SpawnBlock__tick(void* _params) {
  PROFILE__BEGIN(SPAWN_BLOCK__TICK);
  OnEntityParams* params = _params;
  Entity* entity = params->entity;
  Block* block = (Block*)entity;
  SpawnBlock* self = (SpawnBlock*)block;
  Player* player1 = (Player*)g_engine->players->head->data;

  if (self->firstTick) {
    self->firstTick = false;

    Audio__replay(g_engine->audio->powerUp);

    player1->base.base.tform->pos.x = block->base.tform->pos.x;
    player1->base.base.tform->pos.y = 0.0f;
    player1->base.base.tform->pos.z = block->base.tform->pos.z;
    player1->base.base.tform->rot3.x = 0.0f;
    player1->base.base.tform->rot3.y = 0.0f;
    q_fromEuler(&player1->base.base.tform->rot4, &player1->base.base.tform->rot3);
  }
  PROFILE__END(SPAWN_BLOCK__TICK);
}
