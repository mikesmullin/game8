#include "Arena.h"

#include <stdlib.h>

#include "Log.h"

Arena* Arena__Alloc(u64 sz) {
  Arena* a = malloc(sizeof(Arena));
  // LOG_DEBUGF("arena malloc %llu", sz);
  void* p = malloc(sz);
  // LOG_DEBUGF("arena p %p", p);
  ASSERT_CONTEXT(NULL != p, "Arena malloc request rejected by OS.");
  a->buf = p;
  a->pos = p;
  a->end = p + sz;
  return a;
}

Arena* Arena__SubAlloc(Arena* a, u64 sz) {
  Arena* sa = Arena__Push(a, sizeof(Arena));
  sa->buf = Arena__Push(a, sz);
  sa->pos = sa->buf;
  sa->end = sa->buf + sz;
  return sa;
}

static inline void* align_forward(void* p, size_t alignBytes) {
  uintptr_t addr = (uintptr_t)p;
  uintptr_t aligned = (addr + (alignBytes - 1)) & ~(alignBytes - 1);
  return (void*)aligned;
}

void* Arena__Push(Arena* a, u64 sz) {
  // align arena position to next byte boundary
  // this is required because not doing so creates undefined behavior
  // ie. during lookup, when casting void* to a struct
  a->pos = align_forward(a->pos, 8);

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
  void* p = a->pos - sz;
  return p;
}

void Arena__Free(Arena* a) {
  free(a->buf);
}

void Arena__Reset(Arena* a) {
  a->pos = a->buf;
}