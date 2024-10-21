#include "List.h"

#include "Arena.h"

#ifdef __EMSCRIPTEN__
#define NULL (0)
#endif

List* List__alloc(Arena* arena) {
  List* list = Arena__Push(arena, sizeof(List));
  list->len = 0;
  list->head = NULL;
  list->tail = NULL;
  return list;
}

List__Node* List__Node__alloc(Arena* arena) {
  return Arena__Push(arena, sizeof(List__Node));
}

void List__Node__init(List__Node* node, void* data) {
  node->data = data;
  node->next = NULL;
}

void List__append(Arena* arena, List* list, void* data) {
  List__Node* node = List__Node__alloc(arena);
  List__Node__init(node, data);

  if (NULL == list->tail) {
    list->head = node;
    list->tail = node;
  } else {
    list->tail->next = node;
    list->tail = node;
  }
  list->len++;
}

void* List__get(List* list, u32 index) {
  List__Node* c = list->head;
  u32 i = 0;
  while (NULL != c) {
    if (i == index) {
      return c->data;
    }
    i++;
    c = c->next;
  }
  return NULL;
}