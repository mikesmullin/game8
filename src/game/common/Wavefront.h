#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef uint32_t u32;
typedef float f32;

typedef struct Arena Arena;
typedef struct List List;

typedef struct Wavefront__Face {
  // assumes Triangles, not Quads
  u32 vertex_idx[3];
  u32 texcoord_idx[3];
  u32 normal_idx[3];
} Wavefront__Face;

typedef struct Wavefront {
  bool loaded;
  List* vertices;
  List* texcoords;
  List* normals;
  List* faces;
} Wavefront;

Wavefront* Wavefront__Read(const char* filePath);
char* Wavefront__ToString(Wavefront* obj, u32 sz);