#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef struct TransformComponent {
  v3 pos;  // (x, y, z)
  v3 rot;  // (yaw, pitch, roll)
  v3 scale;  // (sx, sy, sz)
} TransformComponent;