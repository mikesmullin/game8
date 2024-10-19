#pragma once

#include <stdint.h>
typedef uint64_t u64;

// Arena/Linear/Bump allocator
typedef struct Arena_t {
  void* buf;
  void* pos;
  void* end;
} Arena_t;

void Arena__Alloc(Arena_t* a, u64 sz);
Arena_t* Arena__SubAlloc(Arena_t* a, u64 sz);
void* Arena__Push(Arena_t* a, u64 sz);
void Arena__Free(Arena_t* a);
void Arena__Reset(Arena_t* a);
