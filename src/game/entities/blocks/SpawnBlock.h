#pragma once

#include "../../Game.h"  // IWYU pragma: keep

typedef struct SpawnBlock {
  Block base;
  bool firstTick;
} SpawnBlock;

void SpawnBlock__init(Entity* entity, f32 x, f32 z);
void SpawnBlock__tick(Entity* entity);