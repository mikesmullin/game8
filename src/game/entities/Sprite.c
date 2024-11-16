#include "Sprite.h"

#include "../Game.h"

void Sprite__init(Entity* entity, f32 x, f32 z) {
  Sprite* self = (Sprite*)entity;
  Entity__init(entity);
  entity->tform->pos.x = x;
  entity->tform->pos.z = z;

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));
  entity->render->rg = WORLD_ZSORT_RG;
  entity->render->billboard = true;

  // preload assets
  entity->render->material = Preload__material(&g_engine->materials->sprite, sizeof(Material));
  entity->render->material->mesh = Preload__model(  //
      &g_engine->models->plane2D,
      "../assets/models/plane2D.obj");
  entity->render->material->texture = Preload__texture(  //
      &g_engine->textures->atlas,
      "../assets/textures/atlas.bmp");
  entity->render->ti = 1;
  entity->render->tw = entity->render->th = 8;
  entity->render->aw = entity->render->ah = 64;
  entity->render->useMask = true;
  entity->render->mask = COLOR_BLACK;
  entity->render->color = COLOR_TRANSPARENT;
}
