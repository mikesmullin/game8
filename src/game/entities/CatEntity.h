#pragma once

#include "../Game.h"  // IWYU pragma: keep

typedef struct CatEntity {
  Sprite base;
  f32 xa, ya, za;
  StateGraph* sg;
  BTNode* brain;
} CatEntity;

void CatEntity__init(Entity* entity);
void CatEntity__tick(Entity* entity);
void CatEntity__collide(Entity* entity, void* params);
SGState* CatEntity__getSGState(u32 id);
void CatEntity__action(Entity* entity, void* action);