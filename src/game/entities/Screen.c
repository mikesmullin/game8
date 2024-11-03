#include "Screen.h"

#include "../Logic.h"
#include "../common/Color.h"
#include "../common/Dispatcher.h"
#include "../common/Preloader.h"
#include "../levels/Level.h"
#include "Entity.h"
#include "Sprite.h"

extern Engine__State* g_engine;

void Screen__init(Entity* entity, u32 mpTexture) {
  Logic__State* logic = g_engine->logic;
  Sprite* sprite = (Sprite*)entity;
  Sprite__init(entity, 0, 0);
  sprite->base.tform->pos.y = 0;
  sprite->base.tform->scale.x = sprite->base.tform->scale.y = SCREEN_SIZE;
  sprite->base.render->billboard = false;
  sprite->base.render->rg = SCREEN_RG;

  // preload assets
  sprite->base.render->material = Preload__material(&logic->materials.screen);
  sprite->base.render->material->mesh = Preload__model(  //
      &logic->models.screen2D,
      "../assets/models/screen2D.obj");
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
  // entity->render->color = TRANSPARENT;
}