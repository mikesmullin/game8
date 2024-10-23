#include "Sprite.h"

#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/Color.h"
#include "../common/Dispatcher.h"
#include "../common/Log.h"
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
  entity->tform->pos.y = 0;
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
  entity->render->material->tx = 0;
  entity->render->material->ty = 1;
  entity->render->material->color = TRANSPARENT;
  entity->render->material->ts = ATLAS_SPRITE_SZ;
}

void Sprite__render(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  Sprite* self = (Sprite*)block;

  MeshRenderer__render(entity);
}