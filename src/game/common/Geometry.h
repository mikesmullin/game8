#pragma once

#include <stdbool.h>

// Struct to represent a point in 2D space
typedef struct Point_t {
  float x;
  float y;
} Point;

// Struct to represent a rectangular boundary
typedef struct Rect_t {
  float x;  // center x
  float y;  // center y
  float w;  // half-width
  float h;  // half-height
} Rect;

bool Rect__containsPoint(Rect boundary, Point point);
bool Rect__intersectsRect(Rect a, Rect b);