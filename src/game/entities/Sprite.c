#include "Sprite.h"

#include "../../../vendor/HandmadeMath/HandmadeMath.h"
#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/Color.h"
#include "../common/Dispatcher.h"
#include "../common/Log.h"
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
  entity->tform->pos.x = x;
  entity->tform->pos.y = -(1.0f / 8);
  entity->tform->pos.z = z;

  entity->engine->render = SPRITE__RENDER;

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));

  // preload assets
  entity->render->material = Preload__material(&logic->materials.cat);
  entity->render->material->mesh = Preload__model(  //
      &logic->models.plane2D,
      "../assets/models/plane2D.obj");
  entity->render->material->texture = Preload__texture(  //
      &logic->textures.atlas,
      "../assets/textures/atlas.bmp");
  entity->render->material->ts = ATLAS_SPRITE_SZ;
  entity->render->material->tx = 0;
  entity->render->material->ty = 1;
  entity->render->material->color = TRANSPARENT;
  entity->render->material->useMask = true;
  entity->render->material->mask = BLACK;

  self->billboard = true;
}

void Sprite__render(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  Sprite* self = (Sprite*)block;

  static f32 sx = 0, sz = 0;
  if (sx == 0) sx = self->base.tform->pos.x;
  if (sz == 0) sz = self->base.tform->pos.z;
  f32 s = Math__map(HMM_SinF(g_engine->now / 1000.f), -1, 1, -2, 2);
  f32 c = Math__map(HMM_CosF(g_engine->now / 1000.f), -1, 1, -2, 2);
  self->base.tform->pos.x = sx + s;
  self->base.tform->pos.z = sz + c;

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