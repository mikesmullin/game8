#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef struct Rigidbody2DComponent {
  f32 xa, za;  // movement deltas (pre-collision)
} Rigidbody2DComponent;

void Rigidbody2D__move(QuadTreeNode* qt, Entity* entity, Dispatcher__call_t cb);
