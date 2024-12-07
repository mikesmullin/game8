#pragma once

#include "Arena.h"
#include "Types.h"

// Define red-black tree node colors
typedef enum RBTNColor /* : u8 */ {
  RBTN_RED,
  RBTN_BLACK,
} RBTNColor;

// Red-black tree node structure
typedef struct RBTreeNode RBTreeNode;
typedef struct RBTreeNode {
  void* data;  // user data associated with the node
  RBTNColor color;  // Node color (RED or BLACK)
  RBTreeNode *left, *right, *parent;  // Left and right children and parent
} RBTreeNode;

// Define the red-black tree structure
typedef struct RBTree {
  RBTreeNode* root;
  RBTreeNode* tnil;  // Sentinel node (used for leaves)
} RBTree;

typedef s8 (*RBTree__sorter_t)(const void* a, const void* b);
typedef bool (*RBTree__iterator_t)(const void* data);

RBTree* RBTree__alloc(const Arena* arena);
RBTreeNode* RBTree__insort(
    const Arena* arena, RBTree* tree, const void* data, RBTree__sorter_t sortCb);
void RBTree__each(const RBTree* tree, const RBTreeNode* node, const RBTree__iterator_t eachCb);
RBTreeNode* RBTree__search(const RBTree* tree, const void* needle, const RBTree__sorter_t sortCb);