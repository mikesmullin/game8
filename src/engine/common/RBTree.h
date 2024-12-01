#pragma once

#include "Arena.h"
#include "Types.h"

// // Create red-black tree
// RBTree* tree = RBTree__alloc(g_engine->frameArena);

// // Insert entity into the tree
// RBTree__insert(g_engine->frameArena, tree, &entity, Level__zsort);

// walk entities in sorted order
// bool cb(void* data) {}
// RBTree__walk(tree, tree->root, cb);

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

typedef s32 (*RBTree__sortable_t)(void* a, void* b);
typedef bool (*RBTree__walker_t)(void* data);

RBTree* RBTree__alloc(const Arena* arena);
void RBTree__insort(const Arena* arena, RBTree* tree, const void* data, RBTree__sortable_t sortCb);
void RBTree__walk(const RBTree* tree, const RBTreeNode* node, const RBTree__walker_t eachCb);