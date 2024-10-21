#pragma once

typedef float f32;
typedef double f64;

typedef struct v2 {
  f32 x, y;
} v2;

typedef struct v3 {
  f32 x, y, z;
} v3;

typedef struct v4 {
  f32 x, y, z, w;
} v4;

typedef struct m4 {
  v4 a, b, c, d;
} m4;

f64 Math__map(f64 n, f64 input_start, f64 input_end, f64 output_start, f64 output_end);

// #define Math__PI32 (3.14159265359f)
// #define Math__PI64 (3.14159265358979323846)
// #define Math__DEG2RAD32 (Math__PI32 / 180.0f)
// #define Math__DEG2RAD64 (Math__PI64 / 180.0)

#define Math__min(a, b) (((a) < (b)) ? (a) : (b))
#define Math__max(a, b) (((a) > (b)) ? (a) : (b))