#include "Screen.h"

void Screen__init(Entity* entity, u32 mpTexture) {
  Sprite* sprite = (Sprite*)entity;
  Sprite__init(entity, 0, 0);
  sprite->base.tform->pos.y = 0;
  sprite->base.tform->scale.x = sprite->base.tform->scale.y = SCREEN_SIZE;
  sprite->base.render->billboard = false;
  sprite->base.render->rg = SCREEN_RG;

  // preload assets
  sprite->base.render->material = Preload__material(&g_engine->materials->screen, sizeof(Material));
  sprite->base.render->material->mesh = Preload__model(  //
      &g_engine->models->screen2D,
      "../assets/models/",
      "screen2D.obj");
  sprite->base.render->material->shader = g_engine->shaders->atlas;
  sprite->base.render->ti = 0;
  sprite->base.render->tw = SCREEN_SIZE, sprite->base.render->th = SCREEN_SIZE;
  sprite->base.render->aw = SCREEN_SIZE, sprite->base.render->ah = SCREEN_SIZE;
  sprite->base.render->useMask = false;
  sprite->base.render->material->mpTexture = mpTexture;

  // entity->render->material->texture = Preload__texture(  //
  //     &logic->textures.atlas,
  //     "../assets/textures/atlas.bmp");
  // entity->render->ti = 1;
  // entity->render->tw = entity->render->th = 8;
  // entity->render->aw = entity->render->ah = 64;
  // entity->render->useMask = false;
  // entity->render->color = COLOR_TRANSPARENT;
}