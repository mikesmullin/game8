#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef enum ColliderType {
  BOX_COLLIDER_2D,
  CIRCLE_COLLIDER_2D,
} ColliderType;

typedef struct OnCollideParams {
  Entity *source, *target;
  f32 x, y;
  bool before, after;
  bool noclip;
} OnCollideParams;

typedef struct ColliderComponent {
  ColliderType type;
  u32 /*DispatchFnId*/ collide;
} ColliderComponent;

typedef struct BoxCollider2DComponent {
  ColliderComponent base;
  //f32 x, y; // origin
  f32 hw, hh;  // half width/height (radius)
} BoxCollider2DComponent;

typedef struct CircleCollider2DComponent {
  ColliderComponent base;
  //f32 x, y; // origin
  f32 r;  // radius
} CircleCollider2DComponent;

bool CircleCollider2D__check(f32 x0, f32 y0, f32 r0, f32 x1, f32 y1, f32 r1);
bool BoxCollider2D__check(f32 x0, f32 y0, f32 r0, f32 x1, f32 y1, f32 r1);
bool Collider__check(QuadTreeNode* qt, Entity* entity, f32 x, f32 y, Dispatcher__call2_t cb);
