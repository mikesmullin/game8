#include "Entity.h"

#include "../../engine/common/Dispatcher.h"
#include "../Logic.h"

void Entity__init(Entity* entity) {
  Logic__State* logic = g_engine->logic;

  entity->id = ++logic->lastUid;

  entity->tform = Arena__Push(g_engine->arena, sizeof(TransformComponent));
  entity->tform->pos.x = 0;
  entity->tform->pos.y = 0;
  entity->tform->pos.z = 0;
  entity->tform->rot.x = 0;
  entity->tform->rot.y = 0;
  entity->tform->rot.z = 0;
  entity->tform->scale.x = 1;
  entity->tform->scale.y = 1;
  entity->tform->scale.z = 1;

  entity->rb = Arena__Push(g_engine->arena, sizeof(Rigidbody2DComponent));
  entity->rb->xa = 0;
  entity->rb->za = 0;

  entity->engine = Arena__Push(g_engine->arena, sizeof(EngineComponent));
  entity->engine->tick = DISPATCH1_NONE;
  entity->engine->render = DISPATCH1_NONE;
  entity->engine->gui = DISPATCH1_NONE;
}
