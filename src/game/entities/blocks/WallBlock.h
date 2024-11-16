#pragma once

#include "../../Game.h"  // IWYU pragma: keep

typedef struct WallBlock {
  Block base;
} WallBlock;

void WallBlock__init(Entity* entity, f32 x, f32 z);