#pragma once

#include "Types.h"

typedef struct Arena Arena;

typedef struct List__Node {
  struct List__Node* next;
  void* data;
} List__Node;

typedef struct List {
  u32 len;
  List__Node* head;
  List__Node* tail;
} List;

List* List__alloc(Arena* arena);
void List__init(List* list);
List__Node* List__Node__alloc(Arena* arena);
void List__init(List* list);
void List__Node__init(List__Node* node, void* data);
void List__append(Arena* arena, List* list, void* data);
void* List__get(List* list, u32 index);
void List__remove(List* list, List__Node* node);
void List__insort(Arena* arena, List* list, void* data, s32 (*sortCb)(void* a, void* b));