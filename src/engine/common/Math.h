#pragma once

#include <fenv.h>

extern inline void Math__init() {
  fesetround(FE_TONEAREST);  // Set rounding mode
}

#include <stdio.h>

#include "Types.h"

#define Math__PI32 (3.14159265359f)
#define Math__PI64 (3.14159265358979323846)
#define Math__DEG2RAD32 (Math__PI32 / 180.0f)
#define Math__DEG2RAD64 (Math__PI64 / 180.0)
#define Math__RAD2DEG32 (180.0f / Math__PI32)
#define Math__RAD2DEG64 (180.0 / Math__PI64)

// min, max, clamp
#define Math__min(a, b) (((a) < (b)) ? (a) : (b))
#define Math__max(a, b) (((a) > (b)) ? (a) : (b))
#define Math__clamp(min, n, max) (((n) < (min)) ? (min) : ((max) < (n)) ? (max) : (n))
f32 Math__map(f32 n, f32 input_start, f32 input_end, f32 output_start, f32 output_end);
f32 Math__fmodf(f32 n, f32 d);
f32 Math__wrapaf(f32 n, f32 x, f32 m, f32 s);
u32 Math__ceil(f32 n);
u32 Math__floor(f32 n);

extern inline f32 Math__sign(f32 n) {
  return n < 0 ? -1.0f : +1.0f;
}

extern inline f32 Math__fabsf(f32 x) {
  return (x < 0.0f) ? -x : x;
}

extern inline f32 Math__pow4(f32 n) {
  return n * n * n * n;
}

extern inline f32 Math__scalef(f32 min, f32 n, f32 max) {
  return min + n * (max - min);
}

extern inline u32 Math__scaleu(u32 min, u32 n, u32 max) {
  return min + (n % (max - min + 1));
}

// sqrt
f32 Math__sqrtf(f32 n);

// trig
f32 Math__cosf(f32 rad);
f32 Math__sinf(f32 rad);
f32 Math__tanf(f32 rad);
f32 Math__acosf(f32 rad);
f32 Math__atanf(f32 rad);
f32 Math__atan2f(f32 y, f32 x);

// prng
u64 Math__randomNext(u64* state);
f32 Math__randomf(f32 min, f32 max, u64* state);
u32 Math__randomu(u32 min, u32 max, u64* state);

// vector structures (1D, 2D, 3D, and 4D)

typedef struct v1 {
  f32 x;
} v1;

typedef struct v2 {
  f32 x, y;
} v2;

typedef struct v3 {
  f32 x, y, z;
} v3;

typedef struct v4 {
  f32 x, y, z, w;
} v4;

// clang-format off
typedef struct m4 {
  // f32 rows[4][4]; // row-major
  // v4 cols[4]; // col-major

  // row-major
  // f32 ax, ay, az, aw,
  //     bx, by, bz, bw,
  //     cx, cy, cz, cw,
  //     dx, dy, dz, dw;

  // col-major (for GPU)
  f32 ax, bx, cx, dx,
      ay, by, cy, dy,
      az, bz, cz, dz,
      aw, bw, cw, dw;
} m4;
// clang-format on

// vector constants

static const v3 V3_ZERO = (v3){0.0f, 0.0f, 0.0f};
static const v3 V3_ONE = (v3){1.0f, 1.0f, 1.0f};
static const v3 V3_UP = (v3){0.0f, 1.0f, 0.0f};  // +Y_UP
static const v3 V3_FWD = (v3){0.0f, 0.0f, -1.0f};  // -Z_FWD
static const v3 V3_RIGHT = (v3){1.0f, 0.0f, 0.0f};  // +X_RIGHT
// clang-format off
static const m4 M4_IDENTITY = (m4){
  1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f,
};
// clang-format on

// constructors

extern inline v3 v3_new(const f32 x, const f32 y, const f32 z) {
  return (v3){x, y, z};
}

// clone

extern inline v1 v1_cp(const v1* a) {
  return *a;
}

extern inline v2 v2_cp(const v2* a) {
  return *a;
}

extern inline v3 v3_cp(const v3* a) {
  return *a;
}

extern inline v4 v4_cp(const v4* a) {
  return *a;
}

extern inline m4 m4_cp(const m4* a) {
  return *a;
}

// set

extern inline void v3_set(v3* dst, const v3* src) {
  dst->x = src->x;
  dst->y = src->y;
  dst->z = src->z;
}

extern inline void v3_set3(v3* dst, const f32 x, const f32 y, const f32 z) {
  dst->x = x;
  dst->y = y;
  dst->z = z;
}

// comparison

extern inline bool v3_equal(const v3* a, const v3* b) {  // ==
  return a->x == b->x && a->y == b->y && a->z == b->z;
}

// serialization

extern inline void v3_str(char* dst, const u32 len, const v3* a) {  // toString
  sprintf_s(dst, len, "%f %f %f", a->x, a->y, a->z);
}

extern inline void v4_str(char* dst, const u32 len, const v4* a) {  // toString
  sprintf_s(dst, len, "%f %f %f %f", a->x, a->y, a->z, a->w);
}

extern inline void m4_str(char* dst, const u32 len, const m4* a) {  // toString
  // clang-format off
  sprintf_s(dst, len, "\n"
    "  %f %f %f %f\n"
    "  %f %f %f %f\n"
    "  %f %f %f %f\n"
    "  %f %f %f %f"
    , 
    a->ax, a->ay, a->az, a->aw,
    a->bx, a->by, a->bz, a->bw,
    a->cx, a->cy, a->cz, a->cw,
    a->dx, a->dy, a->dz, a->dw
  );
  // clang-format on
}

// dot product (a⋅b)

extern inline f32 v1_dot(const v1* a, const v1* b) {
  return a->x * b->x;
}

extern inline f32 v2_dot(const v2* a, const v2* b) {
  return v1_dot((v1*)a, (v1*)b) + a->y * b->y;
}

// can think of it as the length of an (opposite) side of a triangle
extern inline f32 v3_dot(const v3* a, const v3* b) {
  return v2_dot((v2*)a, (v2*)b) + a->z * b->z;
}

extern inline f32 v4_dot(const v4* a, const v4* b) {
  return v3_dot((v3*)a, (v3*)b) + a->w * b->w;
}

// magnitude squared
// because avoiding sqrt(x)) is computationally cheaper
// and often sufficient (e.g., for checking relative distances)

extern inline f32 v1_mag2(const v1* a) {
  return v1_dot(a, a);
}

extern inline f32 v2_mag2(const v2* a) {
  return v2_dot(a, a);
}

extern inline f32 v3_mag2(const v3* a) {
  return v3_dot(a, a);
}

extern inline f32 v4_mag2(const v4* a) {
  return v4_dot(a, a);
}

// magnitude

extern inline f32 v1_mag(const v1* a) {
  return Math__sqrtf(v1_mag2(a));
}

extern inline f32 v2_mag(const v2* a) {
  return Math__sqrtf(v2_mag2(a));
}

extern inline f32 v3_mag(const v3* a) {
  return Math__sqrtf(v3_mag2(a));
}

extern inline f32 v4_mag(const v4* a) {
  return Math__sqrtf(v4_mag2(a));
}

// normalize
void v1_norm(v1* dst, const v1* a);
void v2_norm(v2* dst, const v2* a);
void v3_norm(v3* dst, const v3* a);
void v4_norm(v4* dst, const v4* a);

void v3_rotAA(v3* dst, const v3* v, f32 rad, const v3* axis);

// PEMDAS
// {mul,div,add,sub} *Scalar

// #metacode
// #macro MACRO1(T1)
//   {{~#for T1}}
//   // {{this.op}}
//   extern inline void v1_{{this.op}}(v1* dst, v1* a, v1* b) {
//     dst->x = a->x {{this.oper}} b->x;
//   }
//
//   extern inline void v2_{{this.op}}(v2* dst, v2* a, v2* b) {
//     v1_{{this.op}}((v1*)dst, (v1*)a, (v1*)b);
//     dst->y = a->y {{this.oper}} b->y;
//   }
//
//   extern inline void v3_{{this.op}}(v3* dst, v3* a, v3* b) {
//     v2_{{this.op}}((v2*)dst, (v2*)a, (v2*)b);
//     dst->z = a->z {{this.oper}} b->z;
//   }
//
//   extern inline void v4_{{this.op}}(v4* dst, v4* a, v4* b) {
//     v3_{{this.op}}((v3*)dst, (v3*)a, (v3*)b);
//     dst->w = a->w {{this.oper}} b->w;
//   }
//
//   // {{this.op}} scalar
//
//   extern inline void v1_{{this.op}}S(v1* dst, v1* a, f32 s) {
//     dst->x = a->x {{this.oper}} s;
//   }
//
//   extern inline void v2_{{this.op}}S(v2* dst, v2* a, f32 s) {
//     v1_{{this.op}}S((v1*)dst, (v1*)a, s);
//     dst->y = a->y {{this.oper}} s;
//   }
//
//   extern inline void v3_{{this.op}}S(v3* dst, v3* a, f32 s) {
//     v2_{{this.op}}S((v2*)dst, (v2*)a, s);
//     dst->z = a->z {{this.oper}} s;
//   }
//
//   extern inline void v4_{{this.op}}S(v4* dst, v4* a, f32 s) {
//     v3_{{this.op}}S((v3*)dst, (v3*)a, s);
//     dst->w = a->w {{this.oper}} s;
//   }
//   {{~/for~}}
//
// #table T_VARS
//   op  | oper |
//   mul | *    |
//   div | /    |
//   add | +    |
//   sub | -    |
//
// MACRO1(T_VARS)
// #metagen

// mul
extern inline void v1_mul(v1* dst, v1* a, v1* b) {
  dst->x = a->x * b->x;
}

extern inline void v2_mul(v2* dst, v2* a, v2* b) {
  v1_mul((v1*)dst, (v1*)a, (v1*)b);
  dst->y = a->y * b->y;
}

extern inline void v3_mul(v3* dst, v3* a, v3* b) {
  v2_mul((v2*)dst, (v2*)a, (v2*)b);
  dst->z = a->z * b->z;
}

extern inline void v4_mul(v4* dst, v4* a, v4* b) {
  v3_mul((v3*)dst, (v3*)a, (v3*)b);
  dst->w = a->w * b->w;
}

// mul scalar

extern inline void v1_mulS(v1* dst, v1* a, f32 s) {
  dst->x = a->x * s;
}

extern inline void v2_mulS(v2* dst, v2* a, f32 s) {
  v1_mulS((v1*)dst, (v1*)a, s);
  dst->y = a->y * s;
}

extern inline void v3_mulS(v3* dst, v3* a, f32 s) {
  v2_mulS((v2*)dst, (v2*)a, s);
  dst->z = a->z * s;
}

extern inline void v4_mulS(v4* dst, v4* a, f32 s) {
  v3_mulS((v3*)dst, (v3*)a, s);
  dst->w = a->w * s;
}

// div
extern inline void v1_div(v1* dst, v1* a, v1* b) {
  dst->x = a->x / b->x;
}

extern inline void v2_div(v2* dst, v2* a, v2* b) {
  v1_div((v1*)dst, (v1*)a, (v1*)b);
  dst->y = a->y / b->y;
}

extern inline void v3_div(v3* dst, v3* a, v3* b) {
  v2_div((v2*)dst, (v2*)a, (v2*)b);
  dst->z = a->z / b->z;
}

extern inline void v4_div(v4* dst, v4* a, v4* b) {
  v3_div((v3*)dst, (v3*)a, (v3*)b);
  dst->w = a->w / b->w;
}

// div scalar

extern inline void v1_divS(v1* dst, v1* a, f32 s) {
  dst->x = a->x / s;
}

extern inline void v2_divS(v2* dst, v2* a, f32 s) {
  v1_divS((v1*)dst, (v1*)a, s);
  dst->y = a->y / s;
}

extern inline void v3_divS(v3* dst, v3* a, f32 s) {
  v2_divS((v2*)dst, (v2*)a, s);
  dst->z = a->z / s;
}

extern inline void v4_divS(v4* dst, v4* a, f32 s) {
  v3_divS((v3*)dst, (v3*)a, s);
  dst->w = a->w / s;
}

// add
extern inline void v1_add(v1* dst, v1* a, v1* b) {
  dst->x = a->x + b->x;
}

extern inline void v2_add(v2* dst, v2* a, v2* b) {
  v1_add((v1*)dst, (v1*)a, (v1*)b);
  dst->y = a->y + b->y;
}

extern inline void v3_add(v3* dst, v3* a, v3* b) {
  v2_add((v2*)dst, (v2*)a, (v2*)b);
  dst->z = a->z + b->z;
}

extern inline void v4_add(v4* dst, v4* a, v4* b) {
  v3_add((v3*)dst, (v3*)a, (v3*)b);
  dst->w = a->w + b->w;
}

// add scalar

extern inline void v1_addS(v1* dst, v1* a, f32 s) {
  dst->x = a->x + s;
}

extern inline void v2_addS(v2* dst, v2* a, f32 s) {
  v1_addS((v1*)dst, (v1*)a, s);
  dst->y = a->y + s;
}

extern inline void v3_addS(v3* dst, v3* a, f32 s) {
  v2_addS((v2*)dst, (v2*)a, s);
  dst->z = a->z + s;
}

extern inline void v4_addS(v4* dst, v4* a, f32 s) {
  v3_addS((v3*)dst, (v3*)a, s);
  dst->w = a->w + s;
}

// sub
extern inline void v1_sub(v1* dst, v1* a, v1* b) {
  dst->x = a->x - b->x;
}

extern inline void v2_sub(v2* dst, v2* a, v2* b) {
  v1_sub((v1*)dst, (v1*)a, (v1*)b);
  dst->y = a->y - b->y;
}

extern inline void v3_sub(v3* dst, v3* a, v3* b) {
  v2_sub((v2*)dst, (v2*)a, (v2*)b);
  dst->z = a->z - b->z;
}

extern inline void v4_sub(v4* dst, v4* a, v4* b) {
  v3_sub((v3*)dst, (v3*)a, (v3*)b);
  dst->w = a->w - b->w;
}

// sub scalar

extern inline void v1_subS(v1* dst, v1* a, f32 s) {
  dst->x = a->x - s;
}

extern inline void v2_subS(v2* dst, v2* a, f32 s) {
  v1_subS((v1*)dst, (v1*)a, s);
  dst->y = a->y - s;
}

extern inline void v3_subS(v3* dst, v3* a, f32 s) {
  v2_subS((v2*)dst, (v2*)a, s);
  dst->z = a->z - s;
}

extern inline void v4_subS(v4* dst, v4* a, f32 s) {
  v3_subS((v3*)dst, (v3*)a, s);
  dst->w = a->w - s;
}

// #metaend

// invert [XYZ]

extern inline void v2_invertX(v2* dst, v2* a) {
  dst->x = a->y;
}

extern inline void v2_invertY(v2* dst, v2* a) {
  dst->y = a->x;
}

extern inline void v2_invert(v2* dst, v2* a) {
  v2_invertX(dst, a);
  v2_invertY(dst, a);
}

// cross product (a×b)

// 2D cross differs in that it returns a scalar,
// representing the perpendicular vector's magnitude
// if extended into 3D space.
// We can also think of it as the area of a parallelogram.
extern inline f32 v2_cross(v2* a, v2* b) {
  return a->x * b->y - a->y * b->x;
}

// 3D cross returns a new vector
// orthogonal (perpendicular) to both a and b
extern inline void v3_cross(v3* dst, const v3* a, const v3* b) {
  dst->x = a->y * b->z - a->z * b->y;
  dst->y = a->z * b->x - a->x * b->z;
  dst->z = a->x * b->y - a->y * b->x;
}

extern inline void v3_limit(v3* dst, const v3* a, const f32 max) {
  v3_set(dst, a);
  if (a->x > max) dst->x = max;
  if (a->y > max) dst->y = max;
  if (a->z > max) dst->z = max;
}

extern inline void v3_clamp(v3* dst, const v3* a, f32 n, f32 m) {
  v3_set(dst, a);
  if (a->x > m) dst->x = m;
  if (a->x < n) dst->x = n;
  if (a->y > m) dst->y = m;
  if (a->y < n) dst->y = n;
  if (a->z > m) dst->z = m;
  if (a->z < n) dst->z = n;
}

extern inline void v3_dampenOver(v3* dst, const v3* a, f32 max, f32 factor) {
  if (a->x > max) dst->x = a->x * factor;
  if (a->y > max) dst->y = a->y * factor;
  if (a->z > max) dst->z = a->z * factor;
}

extern inline void v3_rand(v3* dst, const v3* tl, const v3* br, u64* prng) {
  dst->x = Math__randomf(tl->x, br->x, prng);
  dst->y = Math__randomf(tl->y, br->y, prng);
  dst->z = Math__randomf(tl->z, br->z, prng);
}

// len, dist [Abs] [2|Sqrt] [XYZ]
// rot [*|To|By] (LH) [Deg|Rad] [XYZ]
// projectOnto
// vAng, hAng [Deg|Rad]

// lerp/mix, slerp [XYZ]

// frame-rate independent interpolation, ie.
//   f32 lap = 0.0, t = interp(2.0f, &lap, g_engine->deltaTime); // over 2sec
extern inline f32 interp(const f32 duration, f32* lap, const f32 delta) {
  if (*lap >= duration) return 1.0f;  // ensure value reaches target
  f32 t = *lap / duration;  // normalize time 0..1
  *lap += delta;  // aggregate elapsed time
  return t;
}

// frame-rate independent Lerp, ie.
//   f32 lap = 0.0, value = lerp(0.0f, 360.0f, interp(2.0f, &lap, g_engine->deltaTime)); // 0..360 over 2sec
extern inline f32 lerp(const f32 a, const f32 b, const f32 t) {
  return (1.0f - t) * a + t * b;
}

// TODO: use Quaternion (v4) instead of Euler (v3) to avoid gimbal lock and control short vs. long path rotations.
//       v3 is only fine when only two axis are changing (ie. mouse motion)
// spherical lerp (along the surface of a sphere)
void v3_slerp(v3* dst, const v3* a, const v3* b, const f32 t);

// see also: https://cubic-bezier.com/#.17,1.56,.53,.71
extern inline f32 v2_bezier(const v4* b, const f32 t) {
  f32 u = 1.0f - t;  // complement of t
  f32 tt = t * t;  // t squared
  f32 uu = u * u;  // u squared
  f32 uuu = uu * u;  // u cubed
  f32 ttt = tt * t;  // t cubed

  // cubic Bézier value
  return uuu * b->x  // P0
         + 3.0f * uu * t * b->y  // P1
         + 3.0f * u * tt * b->z  // P2
         + ttt * b->w;  // P3
}

// matrix 4x4

extern inline void m4_set(m4* dst, const m4* a) {
  dst->ax = a->ax, dst->ay = a->ay, dst->az = a->az, dst->aw = a->aw;
  dst->bx = a->bx, dst->by = a->by, dst->bz = a->bz, dst->bw = a->bw;
  dst->cx = a->cx, dst->cy = a->cy, dst->cz = a->cz, dst->cw = a->cw;
  dst->dx = a->dx, dst->dy = a->dy, dst->dz = a->dz, dst->dw = a->dw;
}

extern inline void swap(f32* a, f32* b) {
  f32 c = *a;
  *a = *b;
  *b = c;
}

extern inline void m4_flip(m4* a) {
  swap(&a->bx, &a->ay);
  swap(&a->cx, &a->az);
  swap(&a->dx, &a->aw);
  swap(&a->cy, &a->bz);
  swap(&a->dy, &a->bw);
  swap(&a->dz, &a->cw);
}

// clang-format off
extern inline void m4_set4x4(m4* dst,
  f32 c00, f32 c01, f32 c02, f32 c03,
  f32 c10, f32 c11, f32 c12, f32 c13,
  f32 c20, f32 c21, f32 c22, f32 c23,
  f32 c30, f32 c31, f32 c32, f32 c33
) {
  dst->ax = c00, dst->ay = c01, dst->az = c02, dst->aw = c03;
  dst->bx = c10, dst->by = c11, dst->bz = c12, dst->bw = c13;
  dst->cx = c20, dst->cy = c21, dst->cz = c22, dst->cw = c23;
  dst->dx = c30, dst->dy = c31, dst->dz = c32, dst->dw = c33;
}
// clang-format on

extern inline void m4_trans(m4* dst, const v3* a) {
  // clang-format off
  m4_set4x4(dst,
    1.0f, 0.0f, 0.0f, a->x,
    0.0f, 1.0f, 0.0f, a->y,
    0.0f, 0.0f, 1.0f, a->z,
    0.0f, 0.0f, 0.0f, 1.0f
  );
  // clang-format on
}

extern inline void m4_scale(m4* dst, const v3* a) {
  // clang-format off
  m4_set4x4(dst,
    a->x, 0.0f, 0.0f, 0.0f,
    0.0f, a->y, 0.0f, 0.0f,
    0.0f, 0.0f, a->z, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );
  // clang-format on
}

extern inline void m4_rot(m4* dst, const v3* a, f32 rad) {
  // rad = -rad; // LH
  v3 b;
  v3_norm(&b, a);

  f32 SinTheta = Math__sinf(rad);
  f32 CosTheta = Math__cosf(rad);
  f32 CosValue = 1.0f - CosTheta;

  dst->ax = (a->x * a->x * CosValue) + CosTheta;
  dst->ay = (a->x * a->y * CosValue) + (a->z * SinTheta);
  dst->az = (a->x * a->z * CosValue) - (a->y * SinTheta);
  dst->aw = 0.0f;

  dst->bx = (a->y * a->x * CosValue) - (a->z * SinTheta);
  dst->by = (a->y * a->y * CosValue) + CosTheta;
  dst->bz = (a->y * a->z * CosValue) + (a->x * SinTheta);
  dst->bw = 0.0f;

  dst->cx = (a->z * a->x * CosValue) + (a->y * SinTheta);
  dst->cy = (a->z * a->y * CosValue) - (a->x * SinTheta);
  dst->cz = (a->z * a->z * CosValue) + CosTheta;
  dst->cw = 0.0f;

  dst->dx = 0.0f;
  dst->dy = 0.0f;
  dst->dz = 0.0f;
  dst->dw = 1.0f;
}

extern inline void m4_rotX(m4* dst, f32 rad) {  // LH
  f32 s = Math__sinf(rad), c = Math__cosf(rad);
  // clang-format off
  m4_set4x4(dst,
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, c, s, 0.0f,
    0.0f, -s, c, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );
  // clang-format on
}

extern inline void m4_rotY(m4* dst, f32 rad) {  // LH
  f32 s = Math__sinf(rad), c = Math__cosf(rad);
  // clang-format off
  m4_set4x4(dst,
    c, 0.0f, -s, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    s, 0.0f, c, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );
  // clang-format on
}

extern inline void m4_rotZ(m4* dst, f32 rad) {  // LH
  f32 s = Math__sinf(rad), c = Math__cosf(rad);
  // clang-format off
  m4_set4x4(dst,
    c, s, 0.0f, 0.0f,
    -s, c, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );
  // clang-format on
}

// left-handed orthographic projection matrix
// with Z ranging from -1 to 1 (GL convention)
void m4_ortho(m4* dst, f32 l, f32 r, f32 b, f32 t, f32 znear, f32 zfar);

// left-handed perspective projection matrix
// with Z ranging from -1 to 1 (GL convention)
void m4_persp(m4* dst, f32 fov, f32 aspect, f32 znear, f32 zfar);

// quaternions

void q_fromEuler(v4* dst, const v3* rot);
void q_fromAxis(v4* dst, const v3* axis, const f32 angle);

extern inline void q_mul(v4* dst, v4* q1, v4* q2) {
  dst->x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y;
  dst->y = q1->w * q2->y - q1->x * q2->z + q1->y * q2->w + q1->z * q2->x;
  dst->z = q1->w * q2->z + q1->x * q2->y - q1->y * q2->x + q1->z * q2->w;
  dst->w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z;
}

void m4_fromQ(m4* dst, const v4* left);

extern inline void m4_mul(m4* dst, const m4* l, const m4* r) {
  // a
  dst->ax = l->ax * r->ax;
  dst->ay = l->ax * r->ay;
  dst->az = l->ax * r->az;
  dst->aw = l->ax * r->aw;

  dst->ax += l->ay * r->bx;
  dst->ay += l->ay * r->by;
  dst->az += l->ay * r->bz;
  dst->aw += l->ay * r->bw;

  dst->ax += l->az * r->cx;
  dst->ay += l->az * r->cy;
  dst->az += l->az * r->cz;
  dst->aw += l->az * r->cw;

  dst->ax += l->aw * r->dx;
  dst->ay += l->aw * r->dy;
  dst->az += l->aw * r->dz;
  dst->aw += l->aw * r->dw;

  // b
  dst->bx = l->bx * r->ax;
  dst->by = l->bx * r->ay;
  dst->bz = l->bx * r->az;
  dst->bw = l->bx * r->aw;

  dst->bx += l->by * r->bx;
  dst->by += l->by * r->by;
  dst->bz += l->by * r->bz;
  dst->bw += l->by * r->bw;

  dst->bx += l->bz * r->cx;
  dst->by += l->bz * r->cy;
  dst->bz += l->bz * r->cz;
  dst->bw += l->bz * r->cw;

  dst->bx += l->bw * r->dx;
  dst->by += l->bw * r->dy;
  dst->bz += l->bw * r->dz;
  dst->bw += l->bw * r->dw;

  // c
  dst->cx = l->cx * r->ax;
  dst->cy = l->cx * r->ay;
  dst->cz = l->cx * r->az;
  dst->cw = l->cx * r->aw;

  dst->cx += l->cy * r->bx;
  dst->cy += l->cy * r->by;
  dst->cz += l->cy * r->bz;
  dst->cw += l->cy * r->bw;

  dst->cx += l->cz * r->cx;
  dst->cy += l->cz * r->cy;
  dst->cz += l->cz * r->cz;
  dst->cw += l->cz * r->cw;

  dst->cx += l->cw * r->dx;
  dst->cy += l->cw * r->dy;
  dst->cz += l->cw * r->dz;
  dst->cw += l->cw * r->dw;

  // d
  dst->dx = l->dx * r->ax;
  dst->dy = l->dx * r->ay;
  dst->dz = l->dx * r->az;
  dst->dw = l->dx * r->aw;

  dst->dx += l->dy * r->bx;
  dst->dy += l->dy * r->by;
  dst->dz += l->dy * r->bz;
  dst->dw += l->dy * r->bw;

  dst->dx += l->dz * r->cx;
  dst->dy += l->dz * r->cy;
  dst->dz += l->dz * r->cz;
  dst->dw += l->dz * r->cw;

  dst->dx += l->dw * r->dx;
  dst->dy += l->dw * r->dy;
  dst->dz += l->dw * r->dz;
  dst->dw += l->dw * r->dw;
}