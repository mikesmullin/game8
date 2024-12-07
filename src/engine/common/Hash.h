#pragma once

#include "RBTree.h"

typedef struct HashTable {
  RBTree* tree;
} HashTable;

typedef struct HashTable_Node {
  const char* key;
  void* value;
} HashTable_Node;

typedef struct Arena Arena;

HashTable* HashTable__alloc(Arena* arena);
void HashTable__set(const Arena* arena, const HashTable* t, const HashTable_Node* node);
HashTable_Node* HashTable__get(const HashTable* t, const char* key);
