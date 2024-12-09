#pragma once

#include "Geometry.h"
#include "Types.h"

typedef struct Arena Arena;

#define QUAD_TREE_MAX_CAPACITY 4

typedef struct QuadTree QuadTree;
typedef struct QuadTree {
  Rect boundary;  // The boundary (2D space) of this node
  Point points[QUAD_TREE_MAX_CAPACITY];  // Points contained in this node
  void* data[QUAD_TREE_MAX_CAPACITY];  // data the points represent
  u32 point_count;  // Number of points in this node
  bool subdivided;  // Has this node been subdivided?
  QuadTree* northwest;  // Pointers to children
  QuadTree* northeast;
  QuadTree* southwest;
  QuadTree* southeast;
} QuadTree;

typedef bool (*QuadTree__filterable_t)(void* data);

QuadTree* QuadTree_create(Arena* arena, Rect boundary);
void QuadTree_subdivide(Arena* arena, QuadTree* node);
bool QuadTree_insert(Arena* arena, QuadTree* node, Point point, void* data);
void QuadTree_query(
    const QuadTree* node,
    const Rect range,
    const u32 limit,
    void* matchData[],
    u32* matchCount,
    const QuadTree__filterable_t filterCb);