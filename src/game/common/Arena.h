#pragma once

#include <stdint.h>
typedef uint64_t u64;

// Arena/Linear/Bump allocator
typedef struct Arena {
  void* buf;
  void* pos;
  void* end;
} Arena;

void Arena__Alloc(Arena** a, u64 sz);
Arena* Arena__SubAlloc(Arena* a, u64 sz);
void* Arena__Push(Arena* a, u64 sz);
void Arena__Free(Arena* a);
void Arena__Reset(Arena* a);
