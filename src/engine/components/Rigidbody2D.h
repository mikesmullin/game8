#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef struct Rigidbody2DComponent {
  f32 mass;  // kg
  v3 velocity;  // Linear velocity
  v3 angularVelocity;  // Rotational velocity (radians/sec)
} Rigidbody2DComponent;

void Rigidbody2D__move(QuadTreeNode* qt, Entity* entity, Dispatcher__call_t cb);
