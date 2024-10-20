#include "Arena.h"

#include <stdlib.h>

#include "../../lib/Log.h"

void Arena__Alloc(Arena** a, u64 sz) {
  *a = malloc(sizeof(Arena));
  // LOG_DEBUGF("arena malloc %llu", sz);
  void* p = malloc(sz);
  // LOG_DEBUGF("arena p %p", p);
  ASSERT_CONTEXT(NULL != p, "Arena malloc request rejected by OS.");
  (*a)->buf = p;
  (*a)->pos = p;
  (*a)->end = p + sz;
}

Arena* Arena__SubAlloc(Arena* a, u64 sz) {
  Arena* sa = Arena__Push(a, sizeof(Arena));
  sa->buf = Arena__Push(a, sz);
  sa->pos = sa->buf;
  sa->end = sa->buf + sz;
  return sa;
}

void* Arena__Push(Arena* a, u64 sz) {
  ASSERT_CONTEXT(
      a->pos + sz < a->end,
      "Arena exhausted. pos: %p, sz: %d, end: %p, cap: %d, ask: %d, over: %d",
      a->pos,
      sz,
      a->end,
      a->end - a->buf,
      sz,
      a->buf - a->pos - sz);
  // memset(a->pos, 0, sz);  // zero-fill
  a->pos += sz;
  return a->pos - sz;
}

void Arena__Free(Arena* a) {
  free(a->buf);
}

void Arena__Reset(Arena* a) {
  a->pos = a->buf;
}