#include "Entity.h"

#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/Dispatcher.h"

extern Engine__State* g_engine;

Entity* Entity__alloc() {
  return Arena__Push(g_engine->arena, sizeof(Entity));
}

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

  entity->rb = Arena__Push(g_engine->arena, sizeof(Rigidbody2DComponent));
  entity->rb->xa = 0;
  entity->rb->za = 0;

  entity->engine = Arena__Push(g_engine->arena, sizeof(EngineComponent));
  entity->engine->tick = DISPATCH_NONE;
  entity->engine->render = DISPATCH_NONE;
  entity->engine->gui = DISPATCH_NONE;
}
