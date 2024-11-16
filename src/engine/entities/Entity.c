#include "Entity.h"

#include "../Engine.h"

void Entity__init(Entity* entity) {
  entity->id = ++g_engine->lastUid;

  entity->tform = Arena__Push(g_engine->arena, sizeof(TransformComponent));
  entity->tform->scale.x = 1;
  entity->tform->scale.y = 1;
  entity->tform->scale.z = 1;

  entity->rb = Arena__Push(g_engine->arena, sizeof(Rigidbody2DComponent));

  entity->dispatch = Arena__Push(g_engine->arena, sizeof(DispatchComponent));
}
