#include "MeshRenderer.h"

#include <math.h>

#include "../../../vendor/HandmadeMath/HandmadeMath.h"
#include "../../../vendor/sokol/sokol_gfx.h"
#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/Bmp.h"
#include "../common/Color.h"
#include "../common/List.h"
#include "../common/Math.h"
#include "../common/Wavefront.h"

//
#include "../../../assets/shaders/atlas.glsl.h"

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

static void MeshRenderer__loaded(Entity* entity) {
  if (NULL == entity->render) return;  // need component
  if (NULL == entity->render->material) return;  // need material
  Material* material = entity->render->material;
  if (material->loaded) return;  // only do once
  Wavefront* mesh = material->mesh;
  if (!mesh->loaded) return;  // need model
  BmpReader* texture = material->texture;
  if (!texture->loaded) return;  // need texture
  material->loaded = true;

  Logic__State* logic = g_engine->logic;

  // alloc memory once for all meshRenderer instances, since it would be the same for each
  material->pipe = Arena__Push(g_engine->arena, sizeof(sg_pipeline));
  material->bind = Arena__Push(g_engine->arena, sizeof(sg_bindings));

  // Allocate an image handle, but don't actually initialize the image yet,
  // this happens later when the asynchronous file load has finished.
  // Any draw calls containing such an incomplete image handle
  // will be silently dropped.
  sg_alloc_image_smp(material->bind->fs, SLOT__texture1, SLOT_texture1_smp);

  // create shader from code-generated sg_shader_desc
  sg_shader shd = g_engine->sg_make_shader(atlas_shader_desc(g_engine->sg_query_backend()));

  // create a pipeline object (default render states are fine for triangle)
  (*material->pipe) = g_engine->sg_make_pipeline(&(sg_pipeline_desc){
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
              // enable depth buffer only for non-transparent layers
              .write_enabled = entity->render->rg == WORLD_UNSORT_RG,
          },
      .colors[0] =
          {
              .blend =
                  {// Enable blending only for transparency layers
                   .enabled = entity->render->rg != WORLD_UNSORT_RG,
                   //  source color (the fragment's color) will be multiplied by its alpha.
                   .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
                   .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                   // destination color (the existing color in the framebuffer)
                   // will be multiplied by (1 - src_alpha),
                   // providing the transparency effect.
                   .src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA,
                   .dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                   // add the source and destination colors together
                   // after applying the blend factors.
                   .op_rgb = SG_BLENDOP_ADD,
                   .op_alpha = SG_BLENDOP_ADD},
          },
      .cull_mode = SG_CULLMODE_BACK,
      .label = "mesh-pipeline",
  });

  f32 vertices[mesh->faces->len * 3 /*points (tri)*/ * 5 /*v3 pos + v2 uv*/];
  u32 i = 0;
  List__Node* c = mesh->faces->head;
  for (u32 t = 0; t < mesh->faces->len; t++) {
    Wavefront__Face* f = c->data;
    c = c->next;

    // TODO: use indexed list in GL, instead
    for (u32 y = 0; y < 3; y++) {
      v3* p = List__get(mesh->vertices, f->vertex_idx[y] - 1);
      vertices[i++] = p->x;
      vertices[i++] = p->y;
      vertices[i++] = p->z;
      v2* uv = List__get(mesh->texcoords, f->texcoord_idx[y] - 1);
      vertices[i++] = uv->x;
      vertices[i++] = uv->y;
      // v3* n = List__get(mesh->normals, f->normal_idx[y]);
    }
  }
  material->bind->vertex_buffers[0] = g_engine->sg_make_buffer(&(sg_buffer_desc){
      .data = SG_RANGE(vertices),  //
      .label = "mesh-vertices"  //
  });

  u32 pixels[material->texture->w * material->texture->h];  // ABGR
  u32 ii = 0, mask, color;
  for (u32 y = 0; y < material->texture->h; y++) {
    for (u32 x = 0; x < material->texture->w; x++) {
      mask = entity->render->useMask ? entity->render->mask : PINK;
      color = Bmp__Get2DPixel(texture, x, y, mask);
      pixels[ii++] = color;
    }
  }

  g_engine->sg_init_image(
      material->bind->fs.images[SLOT__texture1],
      &(sg_image_desc){
          .width = material->texture->w,
          .height = material->texture->h,
          .pixel_format = SG_PIXELFORMAT_RGBA8,  // has transparency
          .data.subimage[0][0] = {
              .ptr = pixels,
              .size = material->texture->w * material->texture->h * 4,
          }});

  // g_engine->sg_update_buffer(logic->meshRenderer.bind->vertex_buffers[0], &SG_RANGE(vertices));
  //   g_engine->sg_update_image(
  //       logic->meshRenderer.bind->fs.images[SLOT__texture1],
  //       &(sg_image_data){
  //           .subimage[0][0] = {
  //               .ptr = pixels,
  //               .size = material->texture->w * material->texture->h * 4,
  //           }});
}

void MeshRenderer__render(Entity* entity) {
  Logic__State* logic = g_engine->logic;

  MeshRenderer__loaded(entity);
  if (!entity->render->material->loaded) return;  // no draw until assets load
  Material* material = entity->render->material;

  // TODO: move to OrbitalCameraComponent
  // f32 r = g_engine->stm_sec(g_engine->stm_now());
  // HMM_Mat4 model = HMM_Rotate_RH(HMM_AngleRad(r), HMM_V3(0.5f, 1.0f, 0.0f));

  // TODO: Move to 3DCameraComponent
  // +Y_UP
  HMM_Vec3 U = HMM_V3(0.0f, 1.0f, 0.0f);
  // -Z_FWD forward vector (normalized)
  // HMM_Vec3 F = HMM_V3(0.0f, 0.0f, -1.0f);
  // Identity
  HMM_Mat4 I = HMM_Translate(HMM_V3(0.0f, 0.0f, 0.0f));

  // TODO: use Quaternions to avoid gimbal locking

  // f32 r = g_engine->stm_ms(g_engine->stm_now()) / 40;

  // Model
  HMM_Vec3 modelPos = HMM_V3(  //
      -entity->tform->pos.x,  // move object to camera
      -entity->tform->pos.y,
      -entity->tform->pos.z);
  // modelPos = HMM_V3(0.0f, 0.0f, 0.0f);
  HMM_Vec3 modelRot = HMM_V3(  // Yaw, Pitch, Roll
      HMM_AngleDeg(entity->tform->rot.x),
      HMM_AngleDeg(entity->tform->rot.y),
      HMM_AngleDeg(entity->tform->rot.z));
  HMM_Mat4 model = I;
  // apply translation to model
  model = HMM_MulM4(model, HMM_Translate(modelPos));
  // apply rotation to model
  model = HMM_MulM4(model, HMM_Rotate_LH(modelRot.X, HMM_V3(1.0f, 0.0f, 0.0f)));
  model = HMM_MulM4(model, HMM_Rotate_LH(modelRot.Y, HMM_V3(0.0f, 1.0f, 0.0f)));
  model = HMM_MulM4(model, HMM_Rotate_LH(modelRot.Z, HMM_V3(0.0f, 0.0f, 1.0f)));
  // apply scale to model
  model = HMM_MulM4(
      model,
      HMM_Scale(HMM_V3(  //
          entity->tform->scale.x,
          entity->tform->scale.y,
          entity->tform->scale.z)));

  // View (Camera)
  HMM_Vec3 viewPos = HMM_V3(  //
      logic->player->base.tform->pos.x,
      logic->player->base.tform->pos.y,
      logic->player->base.tform->pos.z);
  if (0 == viewPos.Y) {  //  grounded
    viewPos.Y = Math__map(logic->player->bobPhase, -1, 1, 0, -1.0f / 8);
  }

  // viewPos = HMM_V3(0.0f, 0.0f, +3.0f);  // -Z_FWD
  HMM_Vec3 viewRot = HMM_V3(  // Yaw, Pitch, Roll
      HMM_AngleDeg(logic->player->base.tform->rot.x),
      HMM_AngleDeg(logic->player->base.tform->rot.y),
      // TODO: Why do I have to rotate cam Z?
      HMM_AngleDeg(logic->player->base.tform->rot.z + 180));
  HMM_Mat4 view = I;
  // apply rotation to view
  view = HMM_MulM4(view, HMM_Rotate_LH(viewRot.X, HMM_V3(1.0f, 0.0f, 0.0f)));
  view = HMM_MulM4(view, HMM_Rotate_LH(viewRot.Y, HMM_V3(0.0f, 1.0f, 0.0f)));
  view = HMM_MulM4(view, HMM_Rotate_LH(viewRot.Z, HMM_V3(0.0f, 0.0f, 1.0f)));
  // apply translation to view
  view = HMM_MulM4(view, HMM_Translate(viewPos));

  // LOG_DEBUGF(
  //     "modelPos %f %f %f rot %f viewPos %f %f %f rot %f",  //
  //     modelPos.X,
  //     modelPos.Y,
  //     modelPos.Z,
  //     entity->tform->rot.y,
  //     viewPos.X,
  //     viewPos.Y,
  //     viewPos.Z,
  //     logic->player->base.tform->rot.y);

  f32 aspect = (f32)(g_engine->window_width) / (f32)(g_engine->window_height);
  HMM_Mat4 projection = HMM_Perspective_LH_NO(  // LH = -Z_FWD, NO = -1..1 (GL)
      logic->player->proj.fov,
      aspect,
      logic->player->proj.nearZ,
      logic->player->proj.farZ);

  g_engine->sg_apply_pipeline(*material->pipe);
  g_engine->sg_apply_bindings(material->bind);

  vs_params_t vs_params = {.model = model, .view = view, .projection = projection};
  g_engine->sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));

  fs_params_t fs_params = {
      .ti = entity->render->ti,
      .tw = entity->render->tw,
      .th = entity->render->th,
      .aw = material->texture->w,
      .ah = material->texture->h,
      .useMask = entity->render->useMask ? 1 : 0,
      .mask = entity->render->mask,
      .color = entity->render->color};
  g_engine->sg_apply_uniforms(SG_SHADERSTAGE_FS, SLOT_fs_params, &SG_RANGE(fs_params));

  g_engine->sg_draw(0, material->mesh->faces->len * 3 /*points(tri)*/, 1);
}
