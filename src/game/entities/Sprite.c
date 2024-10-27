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

void Sprite__init(Entity* entity, f32 x, f32 z) {
  Logic__State* logic = g_engine->logic;
  Sprite* self = (Sprite*)entity;
  Entity__init(entity);
  entity->engine->render = SPRITE__RENDER;
  entity->tform->pos.x = x;
  entity->tform->pos.y = -(1.0f / 8);
  entity->tform->pos.z = z;

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));
  entity->render->rg = WORLD_ZSORT_RG;
  entity->render->billboard = true;

  // preload assets
  entity->render->material = Preload__material(&logic->materials.sprite);
  entity->render->material->mesh = Preload__model(  //
      &logic->models.plane2D,
      "../assets/models/plane2D.obj");
  entity->render->material->texture = Preload__texture(  //
      &logic->textures.atlas,
      "../assets/textures/atlas.bmp");
  entity->render->ti = 1;
  entity->render->tw = entity->render->th = 8;
  entity->render->aw = entity->render->ah = 64;
  entity->render->useMask = true;
  entity->render->mask = BLACK;
  entity->render->color = TRANSPARENT;
}

void Sprite__render(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  Sprite* self = (Sprite*)block;

  MeshRenderer__render(entity);
}