
#include "QuadTree.h"

#include "Arena.h"
#include "Geometry.h"

// Create a new quadtree node
QuadTree* QuadTree_create(Arena* arena, Rect boundary) {
  QuadTree* node = (QuadTree*)Arena__Push(arena, sizeof(QuadTree));
  node->boundary = boundary;
  node->point_count = 0;
  node->subdivided = false;
  node->northwest = 0;
  node->northeast = 0;
  node->southwest = 0;
  node->southeast = 0;
  return node;
}

// Subdivide the quadtree node into four quadrants
void QuadTree_subdivide(Arena* arena, QuadTree* node) {
  f32 x = node->boundary.x;
  f32 y = node->boundary.y;
  f32 w = node->boundary.w / 2;
  f32 h = node->boundary.h / 2;

  Rect ne = {x + w, y - h, w, h};
  Rect nw = {x - w, y - h, w, h};
  Rect se = {x + w, y + h, w, h};
  Rect sw = {x - w, y + h, w, h};

  node->northeast = QuadTree_create(arena, ne);
  node->northwest = QuadTree_create(arena, nw);
  node->southeast = QuadTree_create(arena, se);
  node->southwest = QuadTree_create(arena, sw);

  node->subdivided = true;
}

// Insert a point into the quadtree
bool QuadTree_insert(Arena* arena, QuadTree* node, Point point, void* data) {
  // Ignore points that do not belong in this node's boundary
  if (!Rect__containsPoint(node->boundary, point)) {
    return false;
  }

  // If there is space in this node, add the point here
  if (node->point_count < QUAD_TREE_MAX_CAPACITY) {
    node->points[node->point_count] = point;
    node->data[node->point_count] = data;
    node->point_count++;
    return true;
  }

  // Otherwise, subdivide the node if it's not already subdivided
  if (!node->subdivided) {
    QuadTree_subdivide(arena, node);
  }

  // Try to insert the point into one of the child nodes
  return QuadTree_insert(arena, node->northeast, point, data) ||
         QuadTree_insert(arena, node->northwest, point, data) ||
         QuadTree_insert(arena, node->southeast, point, data) ||
         QuadTree_insert(arena, node->southwest, point, data);
}

// Query the quadtree for points within a given range
// TODO: accept various shapes to search for intersections with
void QuadTree_query(
    const QuadTree* node,
    const Rect range,
    const u32 limit,
    const void* matchData[],
    u32* matchCount,
    const QuadTree__filterable_t filterCb) {
  // If the range does not intersect this node's boundary, return
  if (!Rect__intersectsRect(node->boundary, range)) {
    return;
  }

  // Check each point in this node to see if it's within the range
  for (u32 i = 0; i < node->point_count; i++) {
    if (*matchCount >= limit) return;
    if (Rect__containsPoint(range, node->points[i])) {
      if (filterCb(node->data[i])) {
        matchData[*matchCount] = node->data[i];
        (*matchCount)++;
      }
    }
  }

  // If the node is subdivided, search the children
  if (node->subdivided) {
    QuadTree_query(node->northeast, range, limit, matchData, matchCount, filterCb);
    QuadTree_query(node->northwest, range, limit, matchData, matchCount, filterCb);
    QuadTree_query(node->southeast, range, limit, matchData, matchCount, filterCb);
    QuadTree_query(node->southwest, range, limit, matchData, matchCount, filterCb);
  }
}