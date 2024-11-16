#pragma once

#include "../../engine/Engine.h"  // IWYU pragma: keep

typedef struct Sprite {
  Entity base;
} Sprite;

void Sprite__init(Entity* entity, f32 x, f32 z);