#include "Block.h"

#include "../../Logic.h"
#include "../../common/Arena.h"
#include "../Entity.h"

extern Engine__State* g_engine;

// blocks are 3d models instantiated from level RGB pixel data

void Block__init(Block* block, f32 x, f32 z) {
  Logic__State* logic = g_engine->logic;
  Entity__init((Entity*)block);
  block->masked = false;
  block->base.tform->pos.x = x;
  block->base.tform->pos.z = z;

  BoxCollider2DComponent* collider = Arena__Push(g_engine->arena, sizeof(BoxCollider2DComponent));
  collider->base.type = BOX_COLLIDER_2D;
  f32 sq_r = 0.5f;  // square radius
  collider->hw = sq_r, collider->hh = sq_r;
  block->base.collider = (ColliderComponent*)collider;
}