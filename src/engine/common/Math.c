#include "Math.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "GLMShim.h"

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

f32 Math__ceil(f32 a) {
  return ceilf(a);
}

f32 Math__floor(f32 a) {
  return floorf(a);
}

// --- <math.h> wrapper ---

f32 Math__sqrtf(f32 n) {
  return sqrtf(n);
}

f32 Math__sinf(f32 n) {
  return sinf(n);
}

f32 Math__cosf(f32 n) {
  return cosf(n);
}

f32 Math__tanf(f32 n) {
  return tanf(n);
}

f32 Math__powf(f32 n, f32 e) {
  return powf(n, e);
}

f32 Math__fabsf(f32 n) {
  return fabsf(n);
}

// --- HandmadeMath.h shim ---

#ifdef HMM_SHIM

HMM_Vec3 HMM_V3(f32 X, f32 Y, f32 Z) {
  return (HMM_Vec3){.X = X, .Y = Y, .Z = Z};
}

HMM_Mat4 HMM_MulM4(HMM_Mat4 Left, HMM_Mat4 Right) {
  HMM_Mat4 Result;
  return Result;
}

HMM_Mat4 HMM_Rotate_LH(f32 Angle, HMM_Vec3 Axis) {
  HMM_Mat4 Result;
  return Result;
}

HMM_Vec3 HMM_SubV3(HMM_Vec3 Left, HMM_Vec3 Right) {
  HMM_Vec3 Result;
  return Result;
}

HMM_Vec3 HMM_NormV3(HMM_Vec3 A) {
  HMM_Vec3 Result;
  return Result;
}

HMM_Mat4 HMM_Translate(HMM_Vec3 Translation) {
  HMM_Mat4 Result;
  return Result;
}

f32 HMM_SinF(f32 Angle) {
  return 0.0f;
}

HMM_Vec3 HMM_MulV3F(HMM_Vec3 Left, f32 Right) {
  HMM_Vec3 Result;
  return Result;
}

f32 HMM_LenV3(HMM_Vec3 A) {
  return 0.0f;
}

HMM_Vec3 HMM_AddV3(HMM_Vec3 Left, HMM_Vec3 Right) {
  HMM_Vec3 Result;
  return Result;
}

HMM_Vec3 HMM_Cross(HMM_Vec3 Left, HMM_Vec3 Right) {
  HMM_Vec3 Result;
  return Result;
}

f32 HMM_CosF(f32 Angle) {
  return 0.0f;
}

HMM_Mat4 HMM_Scale(HMM_Vec3 Scale) {
  HMM_Mat4 Result;
  return Result;
}

HMM_Mat4 HMM_Rotate_RH(f32 Angle, HMM_Vec3 Axis) {
  HMM_Mat4 Result;
  return Result;
}

HMM_Vec3 HMM_RotateV3AxisAngle_LH(HMM_Vec3 V, HMM_Vec3 Axis, f32 Angle) {
  HMM_Vec3 Result;
  return Result;
}

HMM_Mat4 HMM_Perspective_LH_NO(f32 FOV, f32 AspectRatio, f32 Near, f32 Far) {
  HMM_Mat4 Result;
  return Result;
}

HMM_Mat4 HMM_Orthographic_LH_NO(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 Near, f32 Far) {
  HMM_Mat4 Result;
  return Result;
}

f32 HMM_DotV3(HMM_Vec3 Left, HMM_Vec3 Right) {
  return 0.0f;
}

#endif