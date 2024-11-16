#pragma once

#include "../Engine.h"  // IWYU pragma: keep

// TODO: kindof misnomer; contains mesh + texture + shader + bindings
typedef struct Material {
  Wavefront* mesh;
  BmpReader* texture;
  u32 mpTexture;  // resource id for gpu texture (ie. multi-pass rendering)
  sg_pipeline* pipe;
  sg_bindings* bind;
  bool loaded;
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
