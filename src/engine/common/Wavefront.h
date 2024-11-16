#pragma once

#include "Math.h"
#include "Types.h"

typedef struct Arena Arena;
typedef struct List List;

typedef struct Wavefront__Material_Library {
  bool loaded;
  char *path, *file;
  List* materials;
} Wavefront__Material_Library;

typedef struct Wavefront__Material {
  char* name;
  v3 kd, ks, ke;
  f32 ni, d, pr, pm, ps, pc, pcr, aniso, anisor;
  u8 illum;
} Wavefront__Material;

typedef struct Wavefront__Face {
  // assumes Triangles, not Quads
  u32 vertex_idx[3];
  u32 texcoord_idx[3];
  u32 normal_idx[3];
  bool smoothing;
  char* material_id;
} Wavefront__Face;

typedef struct Wavefront {
  bool loaded;
  char *path, *file, *name;
  List* materials;
  List* vertices;
  List* texcoords;
  List* normals;
  List* faces;
} Wavefront;

Wavefront* Wavefront__Read(const char* path, const char* file);
void Wavefront__ReadMat(Wavefront__Material_Library* wml);
char* Wavefront__ToString(Wavefront* obj, u32 sz);