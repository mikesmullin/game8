#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef struct TransformComponent {
  v3 pos;  // (x, y, z)
  v3 rot3;  // (yaw, pitch, roll) Euler
  v4 rot4;  // Quaternion
  v3 scale;  // (sx, sy, sz)
} TransformComponent;