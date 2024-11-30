#include "Math.h"

#include <math.h>
#include <stdlib.h>

#include "GLMShim.h"

f32 Math__random(f32 a, f32 b) {
  return a + (((f32)rand()) / (f32)RAND_MAX) * (b - a);
}

s32 Math__srandom(s32 a, s32 b) {
  return a + (((s32)rand()) / (s32)RAND_MAX) * (b - a);
}

u32 Math__urandom() {
  u32 low = rand() & 0xffff;  // 16 bits +
  u32 high = rand() & 0xffff;  // 16 bits =
  return (high << 16) | low;  // 32 bits
}

u32 Math__urandom2(u32 a, u32 b) {
  return a + (Math__urandom() % (b - a + 1));
}