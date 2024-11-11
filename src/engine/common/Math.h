#pragma once

#include "GLMShim.h"
#include "Types.h"

#define Math__PI32 (3.14159265359f)
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
f32 Math__ceil(f32 a);
f32 Math__floor(f32 a);

// --- <math.h> wrapper ---

f32 Math__sinf(f32 n);
f32 Math__sqrtf(f32 n);
f32 Math__tanf(f32 n);
f32 Math__powf(f32 n, f32 e);
f32 Math__fabsf(f32 n);

// --- HandmadeMath.h shim ---

#ifndef HMM_SHIM
#include "../../../vendor/HandmadeMath/HandmadeMath.h"
#else

#define HMM_PI Math__PI32
#define HMM_DEG180 180.0
#define HMM_DegToRad ((float)(HMM_PI / HMM_DEG180))
#define HMM_AngleDeg(a) ((a) * HMM_DegToRad)
#define HMM_AngleRad(a) (a)
#define HMM_ABS(a) ((a) > 0 ? (a) : -(a))
typedef struct HMM_Vec3 {
  f32 X, Y, Z;
} HMM_Vec3;

typedef struct HMM_Vec4 {
  f32 X, Y, Z, W;
} HMM_Vec4;

typedef struct HMM_Mat4 {
  f32 Elements[4][4];
} HMM_Mat4;

HMM_Vec3 HMM_V3(f32 X, f32 Y, f32 Z);
HMM_Mat4 HMM_MulM4(HMM_Mat4 Left, HMM_Mat4 Right);
HMM_Mat4 HMM_Rotate_LH(f32 Angle, HMM_Vec3 Axis);
HMM_Vec3 HMM_SubV3(HMM_Vec3 Left, HMM_Vec3 Right);
HMM_Vec3 HMM_NormV3(HMM_Vec3 A);
HMM_Mat4 HMM_Translate(HMM_Vec3 Translation);
f32 HMM_SinF(f32 Angle);
HMM_Vec3 HMM_MulV3F(HMM_Vec3 Left, f32 Right);
f32 HMM_LenV3(HMM_Vec3 A);
HMM_Vec3 HMM_AddV3(HMM_Vec3 Left, HMM_Vec3 Right);
HMM_Vec3 HMM_Cross(HMM_Vec3 Left, HMM_Vec3 Right);
f32 HMM_CosF(f32 Angle);
HMM_Mat4 HMM_Scale(HMM_Vec3 Scale);
HMM_Mat4 HMM_Rotate_RH(f32 Angle, HMM_Vec3 Axis);
HMM_Vec3 HMM_RotateV3AxisAngle_LH(HMM_Vec3 V, HMM_Vec3 Axis, f32 Angle);
HMM_Mat4 HMM_Perspective_LH_NO(f32 FOV, f32 AspectRatio, f32 Near, f32 Far);
HMM_Mat4 HMM_Orthographic_LH_NO(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 Near, f32 Far);
f32 HMM_DotV3(HMM_Vec3 Left, HMM_Vec3 Right);

#endif