#include "Sprite.h"

#include "../../../vendor/HandmadeMath/HandmadeMath.h"
#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/Color.h"
#include "../common/Dispatcher.h"
#include "../common/Math.h"
#include "../common/Preloader.h"
#include "../components/MeshRenderer.h"
#include "Entity.h"

extern Engine__State* g_engine;

Sprite* Sprite__alloc() {
  return Arena__Push(g_engine->arena, sizeof(Sprite));
}

void Sprite__init(Entity* entity, f32 x, f32 z) {
  Logic__State* logic = g_engine->logic;
  Sprite* self = (Sprite*)entity;
  Entity__init(entity);
  entity->engine->render = SPRITE__RENDER;
  entity->tform->pos.x = x;
  entity->tform->pos.y = -(1.0f / 8);
  entity->tform->pos.z = z;
  self->billboard = true;

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));

  // preload assets
  entity->render->material = Preload__material(&logic->materials.sprite);
  entity->render->material->mesh = Preload__model(  //
      &logic->models.plane2D,
      "../assets/models/plane2D.obj");
  entity->render->material->texture = Preload__texture(  //
      &logic->textures.atlas,
      "../assets/textures/atlas.bmp");
  entity->render->ts = ATLAS_SPRITE_SZ;
  entity->render->tx = 0;
  entity->render->ty = 1;
  entity->render->useMask = true;
  entity->render->mask = BLACK;
  entity->render->color = TRANSPARENT;
}

void Sprite__render(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  Sprite* self = (Sprite*)block;

  if (self->billboard) {
    HMM_Vec3 cPos = HMM_V3(  //
        logic->player->base.tform->pos.x,
        0,  // logic->player->base.tform->pos.y,
        logic->player->base.tform->pos.z);
    HMM_Vec3 mPos = HMM_V3(  //
        self->base.tform->pos.x,
        0,  // self->base.tform->pos.y,
        self->base.tform->pos.z);
    HMM_Vec3 U = HMM_V3(0, 1.0f, 0);  // +Y_UP
    // direction from camera to model
    HMM_Vec3 look = HMM_NormV3(HMM_SubV3(cPos, mPos));
    // calculate angle (Y-axis aligned)
    self->base.tform->rot.y = HMM_ToDeg(-atan2f(look.X, look.Z));
  }

  MeshRenderer__render(entity);
}