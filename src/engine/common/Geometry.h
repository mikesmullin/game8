#pragma once

#include "Types.h"

// Struct to represent a point in 2D space
typedef struct Point_t {
  f32 x;
  f32 y;
} Point;

// Struct to represent a rectangular boundary
typedef struct Rect_t {
  f32 x;  // center x
  f32 y;  // center y
  f32 w;  // half-width
  f32 h;  // half-height
} Rect;

bool Rect__containsPoint(Rect boundary, Point point);
bool Rect__intersectsRect(Rect a, Rect b);