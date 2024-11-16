#pragma once

#include "../../../engine/Engine.h"  // IWYU pragma: keep

typedef struct Block {
  Entity base;
  bool masked;
} Block;

void Block__init(Block* block, f32 x, f32 z);
