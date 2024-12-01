#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef struct Rigidbody2DComponent {
  f32 mass;  // kg (mass x velocity = momentum)
  v3 velocity;  // Linear velocity (Force x deltaTime = impulse; also = delta mometum; represents the transfer/conservation of momentum)
  v3 angularVelocity;  // Rotational velocity (radians/sec)
} Rigidbody2DComponent;

void Rigidbody2D__move(QuadTree* qt, Entity* entity, Dispatcher__call_t cb);
