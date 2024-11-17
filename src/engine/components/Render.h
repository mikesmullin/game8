#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef struct OnRenderParams1 {
  Entity* entity;
  Material* material;
} OnRenderParams1;

typedef struct OnRenderParams2 {
  Material* material;
} OnRenderParams2;

typedef struct OnRenderParams3 {
  u32 b;
  Entity* entity;
  Material* material;
  HMM_Mat4 model;
} OnRenderParams3;

typedef struct OnRenderParams4 {
  Entity* entity;
  Material* material;
  HMM_Mat4 view;
  HMM_Mat4 projection;
  HMM_Vec3 viewPos;
} OnRenderParams4;

typedef void (*onrender_load_t)(OnRenderParams1* params);
typedef void (*onrender_alloc_t)(OnRenderParams2* params);
typedef void (*onrender_entity_t)(OnRenderParams3* params);
typedef void (*onrender_material_t)(OnRenderParams4* params);

typedef struct Shader {
  u32 /*VTABLE_ENGINE3*/ onload, onalloc, onentity, onmaterial;
} Shader;

// TODO: kindof misnomer; contains mesh + texture + shader + bindings
typedef struct Material {
  bool loaded;
  Wavefront* mesh;
  BmpReader* texture;
  u32 mpTexture;  // resource id for gpu texture (ie. multi-pass rendering)
  sg_pipeline* pipe;
  sg_bindings* bind;
  void *vs_params, *fs_params;
  Shader* shader;
} Material;

typedef enum RenderGroup {
  WORLD_UNSORT_RG,  // ie. 3D (default)
  WORLD_ZSORT_RG,  // ie. 3D transparent
  UI_ZSORT_RG,  // ie. 2D transparent
  SKY_RG,  // skybox
  SCREEN_RG,  // screen
} RenderGroup;

typedef struct RendererComponent {
  RenderGroup rg;
  Material* material;
  u32 ti, pi, po,  // texture index, palette index, palette offset
      tw, th,  // texture width x  height
      aw, ah;  // atlas width x height
  bool useMask, billboard, indexedPalette;
  u32 mask, color;
} RendererComponent;
