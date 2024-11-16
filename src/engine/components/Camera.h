#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef enum ProjectionType {
  PERSPECTIVE_PROJECTION,
  ORTHOGRAPHIC_PROJECTION,
} ProjectionType;

typedef struct Projection {
  ProjectionType type;
  f32 fov;  // field of view
  f32 nearZ;  // near plane
  f32 farZ;  // far plane
} Projection;

typedef struct CameraComponent {
  Projection proj;
  u32 screenSize;
  f32 bobPhase;
} CameraComponent;

typedef struct CameraEntity {
  Entity base;
  CameraComponent camera;
} CameraEntity;