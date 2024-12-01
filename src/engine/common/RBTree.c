#include "RBTree.h"

#include "Log.h"

RBTree* RBTree__alloc(const Arena* arena) {
  RBTree* tree = (RBTree*)Arena__Push(arena, sizeof(RBTree));
  tree->tnil = (RBTreeNode*)Arena__Push(arena, sizeof(RBTreeNode));
  tree->tnil->color = RBTN_BLACK;
  tree->tnil->left = tree->tnil->right = tree->tnil;
  tree->root = tree->tnil;
  return tree;
}

static RBTreeNode* createNode(const Arena* arena, RBTree* tree, const void* data) {
  RBTreeNode* node = (RBTreeNode*)Arena__Push(arena, sizeof(RBTreeNode));
  node->data = data;
  node->color = RBTN_RED;  // New nodes are always red
  node->left = node->right = node->parent = tree->tnil;
  return node;
}

static void leftRotate(RBTree* tree, RBTreeNode* x) {
  RBTreeNode* y = x->right;
  x->right = y->left;
  if (y->left != tree->tnil) {
    y->left->parent = x;
  }
  y->parent = x->parent;
  if (x->parent == tree->tnil) {
    tree->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

static void rightRotate(RBTree* tree, RBTreeNode* y) {
  RBTreeNode* x = y->left;
  y->left = x->right;
  if (x->right != tree->tnil) {
    x->right->parent = y;
  }
  x->parent = y->parent;
  if (y->parent == tree->tnil) {
    tree->root = x;
  } else if (y == y->parent->right) {
    y->parent->right = x;
  } else {
    y->parent->left = x;
  }
  x->right = y;
  y->parent = x;
}

static void insertFixUp(RBTree* tree, RBTreeNode* z) {
  while (z->parent->color == RBTN_RED) {
    if (z->parent == z->parent->parent->left) {
      RBTreeNode* y = z->parent->parent->right;
      if (y->color == RBTN_RED) {
        z->parent->color = RBTN_BLACK;
        y->color = RBTN_BLACK;
        z->parent->parent->color = RBTN_RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->right) {
          z = z->parent;
          leftRotate(tree, z);
        }
        z->parent->color = RBTN_BLACK;
        z->parent->parent->color = RBTN_RED;
        rightRotate(tree, z->parent->parent);
      }
    } else {
      RBTreeNode* y = z->parent->parent->left;
      if (y->color == RBTN_RED) {
        z->parent->color = RBTN_BLACK;
        y->color = RBTN_BLACK;
        z->parent->parent->color = RBTN_RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left) {
          z = z->parent;
          rightRotate(tree, z);
        }
        z->parent->color = RBTN_BLACK;
        z->parent->parent->color = RBTN_RED;
        leftRotate(tree, z->parent->parent);
      }
    }
  }
  tree->root->color = RBTN_BLACK;
}

void RBTree__insort(const Arena* arena, RBTree* tree, const void* data, RBTree__sortable_t sortCb) {
  RBTreeNode* z = createNode(arena, tree, data);
  RBTreeNode* y = tree->tnil;
  RBTreeNode* x = tree->root;

  // Perform a standard binary search insert
  while (x != tree->tnil) {
    y = x;
    if (sortCb(z->data, x->data) < 0) {
      x = x->left;
    } else {
      x = x->right;
    }
  }

  z->parent = y;
  if (y == tree->tnil) {
    tree->root = z;
  } else if (sortCb(z->data, y->data) < 0) {
    y->left = z;
  } else {
    y->right = z;
  }
  z->left = tree->tnil;
  z->right = tree->tnil;

  insertFixUp(tree, z);
}

void RBTree__walk(const RBTree* tree, const RBTreeNode* node, const RBTree__walker_t eachCb) {
  if (node != tree->tnil) {
    RBTree__walk(tree, node->left, eachCb);
    bool r = eachCb(node->data);
    if (!r) return;
    RBTree__walk(tree, node->right, eachCb);
  }
}
