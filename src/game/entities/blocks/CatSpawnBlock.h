#pragma once

#include "../../Game.h"  // IWYU pragma: keep

typedef struct CatSpawnBlock {
  Block base;
  u32 spawnCount;
  f32 spawnInterval;
  f32 animTime;
  u32 spawnedCount;
  u32 maxSpawnCount;
} CatSpawnBlock;
void CatSpawnBlock__init(Entity* block, f32 x, f32 y);
void CatSpawnBlock__tick(void* _params);
