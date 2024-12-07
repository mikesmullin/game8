#pragma once

#include "RBTree.h"

typedef struct HashTable {
  RBTree* tree;
} HashTable;

typedef struct HashTable_Node {
  const char* key;
  const void* value;
} HashTable_Node;

typedef struct Arena Arena;

u32 Hash__djb2(const char* str, u32 len);
u32 Hash__fmix32(u32 h);
HashTable* HashTable__alloc(Arena* arena);
void HashTable__set(const Arena* arena, const HashTable* t, const char* key, const void* value);
HashTable_Node* HashTable__get(const HashTable* t, const char* key);
