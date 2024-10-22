#include "WallBlock.h"

#include "../../Logic.h"
#include "../../common/Arena.h"
#include "../../common/Dispatcher.h"
#include "../../common/Log.h"
#include "../../common/Preloader.h"
#include "../../components/MeshRenderer.h"
#include "Block.h"

extern Engine__State* g_engine;

Block* WallBlock__alloc() {
  return Arena__Push(g_engine->arena, sizeof(WallBlock));
}

void WallBlock__init(Entity* entity, f32 x, f32 y) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  WallBlock* self = (WallBlock*)block;
  Block__init(block, x, y);
  entity->engine->render = WALL_BLOCK__RENDER;
  entity->tags1 |= TAG_WALL;
  block->meshId = MODEL_BOX;

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));
  entity->render->type = MESH_RENDERER;
  entity->render->tx = logic->level->wallTex;
  entity->render->ty = 0;
  entity->render->color = logic->level->wallCol;

  // preload assets
  entity->render->mesh =  //
      Preload__model(
          &logic->models.box,  //
          "../assets/models/box.obj");
  entity->render->texture =  //
      Preload__texture(
          &logic->textures.atlas,  //
          "../assets/textures/atlas.bmp");
  entity->render->ts = ATLAS_SPRITE_SZ;

  LOG_DEBUGF("WallBlock %u pos %f %f", entity->id, entity->tform->pos.x, entity->tform->pos.z);
}

void WallBlock__render(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  WallBlock* self = (WallBlock*)block;

  MeshRenderer__render(entity);
}