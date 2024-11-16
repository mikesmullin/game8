#include "WallBlock.h"

void WallBlock__init(Entity* entity, f32 x, f32 z) {
  Block* block = (Block*)entity;
  WallBlock* self = (WallBlock*)block;
  Block__init(block, x, z);
  entity->tags1 |= TAG_WALL;

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));

  // preload assets
  entity->render->material = Preload__material(&g_engine->materials->wall, sizeof(Material));
  entity->render->material->mesh = Preload__model(  //
      &g_engine->models->box,
      "../assets/models/box.obj");
  entity->render->material->texture = Preload__texture(  //
      &g_engine->textures->atlas,
      "../assets/textures/atlas.bmp");
  entity->render->ti = g_engine->game->level->wallTex;
  entity->render->tw = entity->render->th = 8;
  entity->render->aw = entity->render->ah = 64;
  entity->render->color = g_engine->game->level->wallCol;
}
