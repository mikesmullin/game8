#pragma once

// #define CGLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <cglm/cglm.h>

typedef float f32;
typedef f32 vec2[2];
typedef f32 vec3[3];
typedef f32 vec4[4];
typedef vec4 mat4[4];

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
  vec4 a, b, c, d;
} m4;

f32 glms_v3_dot(v3 a, v3 b);
void glms_v3_add(v3 a, v3 b, v3* dest);
void glms_v3_sub(v3 a, v3 b, v3* dest);
void glms_v3_scale(v3 v, f32 scale, v3* dest);
void glms_v3_normalize(v3* dest);
void glms_v3_cross(v3 a, v3 b, v3* dest);
f32 glms_v3_distance(v3* a, v3* b);

f32 glms_rad(f32 degrees);
void glms_vec3_copy(vec3 src, vec3 dest);
void glms_vec2_copy(vec2 src, vec2 dest);
void glms_vec3_normalize(vec3 v);
void glms_vec3_cross(vec3 a, vec3 b, vec3 result);
void glms_vec3_scale(vec3 v, f32 scale, vec3 result);
void glms_vec3_add(vec3 a, vec3 b, vec3 result);
void glms_vec3_sub(vec3 a, vec3 b, vec3 result);
void glms_lookat(vec3 eye, vec3 center, vec3 up, mat4 result);
void glms_perspective(f32 fov, f32 aspect, f32 near, f32 far, mat4 result);
void glms_mat4_mulv(mat4 mat, vec4 v, vec4 dest);
