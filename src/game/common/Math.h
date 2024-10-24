#pragma once

// #include "../../../vendor/HandmadeMath/HandmadeMath.h"

#include <stdint.h>
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
typedef double f64;

// #define Math__PI32 (3.14159265359f)
// #define Math__PI64 (3.14159265358979323846)
// #define Math__DEG2RAD32 (Math__PI32 / 180.0f)
// #define Math__DEG2RAD64 (Math__PI64 / 180.0)

#define Math__min(a, b) (((a) < (b)) ? (a) : (b))
#define Math__max(a, b) (((a) > (b)) ? (a) : (b))
#define Math__clamp(min, n, max) (((n) < (min)) ? (min) : ((max) < (n)) ? (max) : (n))

f64 Math__map(f64 n, f64 input_start, f64 input_end, f64 output_start, f64 output_end);
f64 Math__rclampf(f64 min, f64 n, f64 max);
f32 Math__random(f32 a, f32 b);
s32 Math__srandom(s32 a, s32 b);
u32 Math__urandom();
u32 Math__urandom2(u32 a, u32 b);