#pragma once

#include "../Game.h"  // IWYU pragma: keep

typedef struct CatEntity {
  Sprite base;
  f32 xa, ya, za;
  StateGraph* sg;
  BTNode* brain;
  Player* interactingPlayer;
} CatEntity;

void CatEntity__init(Entity* entity);
void CatEntity__tick(void* _params);
void CatEntity__collide(void* _params);
SGState* CatEntity__getSGState(u32 id);
void CatEntity__action(void* _params);