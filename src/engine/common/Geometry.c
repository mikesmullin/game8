#include "Geometry.h"

// check if a point is inside a rectangle
bool Rect__containsPoint(Rect boundary, Point point) {
  return (
      point.x >= boundary.x - boundary.w &&  //
      point.x <= boundary.x + boundary.w &&  //
      point.y >= boundary.y - boundary.h &&  //
      point.y <= boundary.y + boundary.h);
}

// check if two rectangles overlap
bool Rect__intersectsRect(Rect a, Rect b) {
  return !(
      // check for overlap using AABB (Axis-Aligned Bounding Box)
      a.x - a.w > b.x + b.w ||  //
      a.x + a.w < b.x - b.w ||  //
      a.y - a.h > b.y + b.h ||  //
      a.y + a.h < b.y - b.h);
}