#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "Geometry.h"

typedef uint32_t u32;
typedef float f32;

typedef struct Arena Arena;

#define QUAD_TREE_MAX_CAPACITY 4

typedef struct QuadTreeNode QuadTreeNode;
typedef struct QuadTreeNode {
  Rect boundary;  // The boundary (2D space) of this node
  Point points[QUAD_TREE_MAX_CAPACITY];  // Points contained in this node
  void* data[QUAD_TREE_MAX_CAPACITY];  // data the points represent
  u32 point_count;  // Number of points in this node
  bool subdivided;  // Has this node been subdivided?
  QuadTreeNode* northwest;  // Pointers to children
  QuadTreeNode* northeast;
  QuadTreeNode* southwest;
  QuadTreeNode* southeast;
} QuadTreeNode;

QuadTreeNode* QuadTreeNode_create(Arena* arena, Rect boundary);
void QuadTreeNode_subdivide(Arena* arena, QuadTreeNode* node);
bool QuadTreeNode_insert(Arena* arena, QuadTreeNode* node, Point point, void* data);
void QuadTreeNode_query(
    QuadTreeNode* node, Rect range, u32 limit, void* matchData[], u32* matchCount);