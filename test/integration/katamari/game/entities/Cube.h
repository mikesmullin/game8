#pragma once

#include "../Game.h"

typedef struct Cube {
  Entity base;

  f32 friction;  // Friction factor for rolling/sliding
} Cube;

void Cube__init(Entity* entity);