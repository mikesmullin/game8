#include "Math.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

typedef int32_t s32;
typedef uint32_t u32;

f64 Math__map(f64 n, f64 input_start, f64 input_end, f64 output_start, f64 output_end) {
  f64 range = 1.0 * (output_end - output_start) / (input_end - input_start);
  return output_start + range * (n - input_start);
}

f64 Math__rclampf(f64 min, f64 n, f64 max) {
  while (n <= min) {
    n += max;
  }
  while (n >= max) {
    n -= max;
  }
  return n;
}

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