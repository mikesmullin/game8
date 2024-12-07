#include "Hash.h"

// #include "Log.h"

// NOTE: this implementation uses RBTree, so less memory, but O(logN) cpu
//   because it is used sparingly (faster than switch statement)
// see also: https://www.quora.com/Why-would-anyone-like-to-use-a-red-black-tree-when-a-hash-table-can-do-the-job-perfectly

HashTable* HashTable__alloc(Arena* arena) {
  HashTable* t = Arena__Push(arena, sizeof(HashTable));
  t->tree = RBTree__alloc(arena);
  return t;
}

static s8 HashTable__sort(const void* a, const void* b) {
  const HashTable_Node *an = a, *bn = b;
  s8 r = 0;
  u32 i = 0;
  for (; i < 1024; i++) {  // prevent infinite loop
    if (an->key[i] < bn->key[i]) {
      r = -1;
      break;
    } else if (an->key[i] > bn->key[i]) {
      r = +1;
      break;
    } else {  // equal
      r = 0;
      if (an->key[i] == '\0') break;
    }
  }

  // LOG_DEBUGF("HashTable__sort %s %s %u %d", an->key, bn->key, i, r);
  return r;
}

void HashTable__set(const Arena* arena, const HashTable* t, const HashTable_Node* node) {
  (void)RBTree__insort(arena, t->tree, node, HashTable__sort);
}

HashTable_Node* HashTable__get(const HashTable* t, const char* key) {
  HashTable_Node n = {.key = key};
  RBTreeNode* r = RBTree__search(t->tree, &n, HashTable__sort);
  if (r == t->tree->tnil) return NULL;
  HashTable_Node* rh = r->data;
  return rh;
}