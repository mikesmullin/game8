#include "List.h"

#include "Arena.h"

List* List__alloc(Arena* arena) {
  List* list = Arena__Push(arena, sizeof(List));
  List__init(list);
  return list;
}

void List__init(List* list) {
  list->len = 0;
  list->head = 0;
  list->tail = 0;
}

List__Node* List__Node__alloc(Arena* arena) {
  return Arena__Push(arena, sizeof(List__Node));
}

void List__Node__init(List__Node* node, void* data) {
  node->data = data;
  node->next = 0;
}

void List__append(Arena* arena, List* list, void* data) {
  List__Node* node = List__Node__alloc(arena);
  List__Node__init(node, data);

  if (0 == list->len) {
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
  for (u32 i = 0; i < list->len; i++) {
    if (i == index) {
      return c->data;
    }
    c = c->next;
  }
  return 0;
}

void List__remove(List* list, List__Node* node) {
  List__Node* c = list->head;

  // special case for head
  if (c == node) {
    list->head = c->next;
    list->len--;
    return;
  }

  for (u32 i = 0; i < list->len; i++) {
    if (c->next == node) {
      c->next = c->next->next;
      list->len--;
      if (0 == c->next) list->tail = c;
      return;
    }
    c = c->next;
  }
}

void* List__pop(List* list) {
  List__Node* c = list->head;
  List__Node* prev;
  if (0 == list->len) return 0;
  if (1 == list->len) {
    list->head = list->tail = NULL;
    list->len = 0;
    return c->data;
  }
  for (u32 i = 0; i < list->len; i++) {
    if (i == list->len - 1) {
      list->tail = prev;
      list->tail->next = NULL;
      list->len--;  // update
      return c->data;  // return last
    }
    prev = c;
    c = c->next;
  }
  return 0;
}

// insert in sorted position
void List__insort(Arena* arena, List* list, void* data, List__sorter_t sortCb) {
  List__Node* node = List__Node__alloc(arena);
  List__Node__init(node, data);

  // special case for first node
  if (0 == list->head) {
    list->head = node;
    list->tail = node;
    list->len++;
    return;
  }

  // locate the node before the point of insertion
  List__Node* c = list->head;
  // while data is deeper than head (-Z_FWD = head -Z, tail +Z)
  while (0 != c->next && sortCb(node->data, c->next->data) == -1) {  // -1 a<b, 0 a==b, +1 a>b
    c = c->next;
  }

  // insert
  node->next = c->next;
  c->next = node;
  list->len++;
}