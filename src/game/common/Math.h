#pragma once

typedef float f32;
typedef double f64;

// #define Math__PI32 (3.14159265359f)
// #define Math__PI64 (3.14159265358979323846)
// #define Math__DEG2RAD32 (Math__PI32 / 180.0f)
// #define Math__DEG2RAD64 (Math__PI64 / 180.0)

#define Math__min(a, b) (((a) < (b)) ? (a) : (b))
#define Math__max(a, b) (((a) > (b)) ? (a) : (b))

f64 Math__map(f64 n, f64 input_start, f64 input_end, f64 output_start, f64 output_end);
f64 Math__fmod(f64 n, f64 max);