#include "Cube.h"

#include "../../../../../src/engine/common/Dispatcher.h"
#include "../../../../../src/engine/common/Preloader.h"
// #include "../../../../../src/game/entities/Entity.h"
#include "../Logic.h"

extern Engine__State* g_engine;

void Cube__init(Entity* entity, f32 x, f32 z) {
  Logic__State* logic = g_engine->logic;

  entity->tform->pos.x = x;
  entity->tform->pos.z = z;

  BoxCollider2DComponent* collider = Arena__Push(g_engine->arena, sizeof(BoxCollider2DComponent));
  collider->base.type = BOX_COLLIDER_2D;
  f32 sq_r = 0.5f;  // square radius
  collider->hw = sq_r, collider->hh = sq_r;
  entity->collider = (ColliderComponent*)collider;

  entity->tags1 |= TAG_WALL;

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));

  // preload assets
  entity->render->material = Preload__material(&logic->materials.wall, sizeof(Material));
  entity->render->material->mesh = Preload__model(  //
      &logic->models.box,
      "../assets/models/box.obj");
  entity->render->material->texture = Preload__texture(  //
      &logic->textures.atlas,
      "../assets/textures/atlas.bmp");
  entity->render->ti = logic->level->wallTex;
  entity->render->tw = entity->render->th = 8;
  entity->render->aw = entity->render->ah = 64;
  entity->render->color = logic->level->wallCol;
}
