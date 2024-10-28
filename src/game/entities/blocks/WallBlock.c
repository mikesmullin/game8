#include "WallBlock.h"

#include "../../Logic.h"
#include "../../common/Arena.h"
#include "../../common/Dispatcher.h"
#include "../../common/Log.h"
#include "../../common/Preloader.h"
#include "../../components/MeshRenderer.h"
#include "Block.h"

extern Engine__State* g_engine;

void WallBlock__init(Entity* entity, f32 x, f32 z) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  WallBlock* self = (WallBlock*)block;
  Block__init(block, x, z);
  entity->engine->render = WALL_BLOCK__RENDER;
  entity->tags1 |= TAG_WALL;

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));

  // preload assets
  entity->render->material = Preload__material(&logic->materials.wall);
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

void WallBlock__render(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  WallBlock* self = (WallBlock*)block;

  // MeshRenderer__render(entity);
}