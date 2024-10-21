#include "Cube.h"

#include <math.h>

#include "../../../vendor/HandmadeMath/HandmadeMath.h"
#include "../../../vendor/sokol/sokol_gfx.h"
#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/Bmp.h"
#include "../common/List.h"
#include "../common/Log.h"
#include "../common/Math.h"
#include "../common/Wavefront.h"

//
#include "../../../assets/shaders/cube.glsl.h"

extern Engine__State* g_engine;

static const sg_sampler_desc global_sampler_desc = {
    .wrap_u = SG_WRAP_REPEAT,
    .wrap_v = SG_WRAP_REPEAT,
    .min_filter = SG_FILTER_NEAREST,
    .mag_filter = SG_FILTER_NEAREST,
    .compare = SG_COMPAREFUNC_NEVER,
};

#define sg_alloc_image_smp(bindings, image_index, smp_index)   \
  bindings.images[image_index] = g_engine->sg_alloc_image();   \
  bindings.samplers[smp_index] = g_engine->sg_alloc_sampler(); \
  g_engine->sg_init_sampler(bindings.samplers[smp_index], &global_sampler_desc);

void Cube__preload() {
  Logic__State* logic = g_engine->logic;

  // preload assets
  logic->wf = Wavefront__Read("../assets/models/box.obj");
  logic->bmp = Bmp__Read("../assets/textures/test.bmp");
}

static void Cube__finish_preload() {
  Logic__State* logic = g_engine->logic;
  if (!logic->wf->loaded) return;
  if (!logic->bmp->loaded) return;
  if (logic->finishedPreload) return;
  logic->finishedPreload = true;

  logic->pip = Arena__Push(g_engine->arena, sizeof(sg_pipeline));
  logic->bind = Arena__Push(g_engine->arena, sizeof(sg_bindings));
  logic->pass_action = Arena__Push(g_engine->arena, sizeof(sg_pass_action));

  // Allocate an image handle, but don't actually initialize the image yet,
  // this happens later when the asynchronous file load has finished.
  // Any draw calls containing such an incomplete image handle
  // will be silently dropped.
  sg_alloc_image_smp(logic->bind->fs, SLOT__texture1, SLOT_texture1_smp);
  sg_alloc_image_smp(logic->bind->fs, SLOT__texture2, SLOT_texture2_smp);

  f32 vertices[logic->wf->faces->len * 3 /*points (tri)*/ * 5 /*v3 pos + v2 uv*/];
  u32 i = 0;
  List__Node* c = logic->wf->faces->head;
  for (u32 t = 0; t < logic->wf->faces->len; t++) {
    Wavefront__Face* f = c->data;
    c = c->next;

    // TODO: use indexed list in GL, instead
    for (u32 y = 0; y < 3; y++) {
      v3* p = List__get(logic->wf->vertices, f->vertex_idx[y] - 1);
      vertices[i++] = p->x;
      vertices[i++] = p->y;
      vertices[i++] = p->z;
      v2* uv = List__get(logic->wf->texcoords, f->texcoord_idx[y] - 1);
      vertices[i++] = uv->x;
      vertices[i++] = uv->y;
      // v3* n = List__get(logic->wf->normals, f->normal_idx[y]);
    }
  }
  logic->bind->vertex_buffers[0] = g_engine->sg_make_buffer(&(sg_buffer_desc){
      .data = SG_RANGE(vertices),  //
      .label = "cube-vertices"  //
  });

  // create shader from code-generated sg_shader_desc
  sg_shader shd = g_engine->sg_make_shader(simple_shader_desc(g_engine->sg_query_backend()));

  // create a pipeline object (default render states are fine for triangle)
  (*logic->pip) = g_engine->sg_make_pipeline(&(sg_pipeline_desc){
      .shader = shd,
      // if the vertex layout doesn't have gaps, don't need to provide strides and offsets
      .layout =
          {
              .attrs =
                  {
                      [ATTR_vs_aPos].format = SG_VERTEXFORMAT_FLOAT3,  //
                      [ATTR_vs_aTexCoord].format = SG_VERTEXFORMAT_FLOAT2,  //
                  },
          },
      .depth =
          {
              .compare = SG_COMPAREFUNC_LESS_EQUAL,
              .write_enabled = true,
          },
      .label = "cube-pipeline",
  });

  // a pass action to clear framebuffer to black
  (*logic->pass_action) = (sg_pass_action){
      .colors[0] = {
          .load_action = SG_LOADACTION_CLEAR,  //
          .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},  //
      }};

  sg_image image1 = logic->bind->fs.images[SLOT__texture1];
  sg_image image2 = logic->bind->fs.images[SLOT__texture2];

  u32 pixels[logic->bmp->w * logic->bmp->h * 4];  // ABGR
  u32 ii = 0;
  for (u32 y = logic->bmp->h - 1; y != (u32)-1; y--) {  // flip-h; GL texture is stored flipped
    // for (u32 y = 0; y < logic->bmp->h; y++) {
    for (u32 x = 0; x < logic->bmp->w; x++) {
      u32 color = Bmp__Get2DPixel(logic->bmp, x, y, 0xffff00ff);
      pixels[ii++] = color;
      LOG_DEBUGF("color(%u,%u) %08x ABGR", x, y, color);
    }
  }
  g_engine->sg_init_image(
      image1,
      &(sg_image_desc){
          .width = logic->bmp->w,
          .height = logic->bmp->h,
          .pixel_format = SG_PIXELFORMAT_RGBA8,
          .data.subimage[0][0] = {
              .ptr = pixels,
              .size = logic->bmp->w * logic->bmp->h * 4,
          }});
  g_engine->sg_init_image(
      image2,
      &(sg_image_desc){
          .width = logic->bmp->w,
          .height = logic->bmp->h,
          .pixel_format = SG_PIXELFORMAT_RGBA8,
          .data.subimage[0][0] = {
              .ptr = pixels,
              .size = logic->bmp->w * logic->bmp->h * 4,
          }});
}

void Cube__render() {
  Logic__State* logic = g_engine->logic;

  Cube__finish_preload();
  if (!logic->finishedPreload) return;

  f32 r = g_engine->stm_sec(g_engine->stm_now());
  HMM_Mat4 model = HMM_Rotate_RH(HMM_AngleRad(r), HMM_V3(0.5f, 1.0f, 0.0f));
  HMM_Mat4 view = HMM_Translate(HMM_V3(0.0f, 0.0f, -3.0f));
  f32 aspect = (f32)(g_engine->window_width) / (f32)(g_engine->window_height);
  HMM_Mat4 projection = HMM_Perspective_RH_NO(45.0f, aspect, 0.1f, 100.0f);

  g_engine->sg_begin_pass(
      &(sg_pass){.action = *logic->pass_action, .swapchain = g_engine->sglue_swapchain()});
  g_engine->sg_apply_pipeline(*logic->pip);
  g_engine->sg_apply_bindings(logic->bind);

  vs_params_t vs_params = {.model = model, .view = view, .projection = projection};
  g_engine->sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));

  g_engine->sg_draw(0, logic->wf->faces->len * 3 /*points(tri)*/, 1);
  g_engine->sg_end_pass();
}