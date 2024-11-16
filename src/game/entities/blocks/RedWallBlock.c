#include "RedWallBlock.h"

void RedWallBlock__init(Entity* entity, f32 x, f32 z) {
  Block* block = (Block*)entity;
  WallBlock* self = (WallBlock*)block;
  WallBlock__init(entity, x, z);
  entity->dispatch->action = RED_WALL_BLOCK__ACTION;

  entity->render->color = 0x660000ff;  // red

  // preload assets
  Preload__audio(
      &g_engine->audio->click,  //
      "../assets/audio/sfx/click.wav");
}

bool findCatBlock(Entity* entity) {
  return entity->tags1 & TAG_CATSPAWN;
}

void RedWallBlock__action(Entity* entity, void* _action) {
  CatEntity* self = (CatEntity*)entity;
  Action* action = (Action*)_action;
  Player* player1 = (Player*)g_engine->players->head->data;

  if (ACTION_USE == action->type) {
    CatSpawnBlock* csb = (CatSpawnBlock*)Level__findEntity(g_engine->game->level, findCatBlock);
    csb->maxSpawnCount += 100;

    AudioSource__play(g_engine->audio->click, entity, (Entity*)player1);
  }
}