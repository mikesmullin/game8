#include "Atlas.h"

//
#include "../../../assets/shaders/atlas.glsl.h"

void Atlas__onrender_load(void* _params) {
  OnRenderParams1* params = _params;
  Wavefront* mesh = params->material->mesh;
  BmpReader* texture = params->material->texture;

  // alloc memory once for all meshRenderer instances, since it would be the same for each
  params->material->pipe = Arena__Push(g_engine->arena, sizeof(sg_pipeline));
  params->material->bind = Arena__Push(g_engine->arena, sizeof(sg_bindings));

  // Allocate an image handle, but don't actually initialize the image yet,
  // this happens later when the asynchronous file load has finished.
  // Any draw calls containing such an incomplete image handle
  // will be silently dropped.
  if (0 == params->material->mpTexture) {
    params->material->bind->fs.images[SLOT__texture1] = g_engine->sg_alloc_image();
  } else {
    params->material->bind->fs.images[SLOT__texture1] =
        (sg_image){.id = params->material->mpTexture};
  }
  params->material->bind->fs.samplers[SLOT_texture1_smp] = g_engine->sg_alloc_sampler();
  g_engine->sg_init_sampler(
      params->material->bind->fs.samplers[SLOT_texture1_smp],
      &(sg_sampler_desc){
          .wrap_u = SG_WRAP_REPEAT,
          .wrap_v = SG_WRAP_REPEAT,
          .min_filter = SG_FILTER_NEAREST,
          .mag_filter = SG_FILTER_NEAREST,
          .compare = SG_COMPAREFUNC_NEVER,
      });

  // create shader from code-generated sg_shader_desc
  sg_shader shd = g_engine->sg_make_shader(atlas_shader_desc(g_engine->sg_query_backend()));

  // create a pipeline object (default render states are fine for triangle)
  (*params->material->pipe) = g_engine->sg_make_pipeline(&(sg_pipeline_desc){
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
              .write_enabled = params->entity->render->rg == WORLD_UNSORT_RG,
          },
      .colors[0] =
          {
              .blend =
                  {// Enable blending only for transparency layers
                   .enabled = params->entity->render->rg != WORLD_UNSORT_RG,
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
      .sample_count = 1,  // because rendering to texture
      .label = "atlas-pipeline",
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
  params->material->bind->vertex_buffers[0] = g_engine->sg_make_buffer(&(sg_buffer_desc){
      .data = SG_RANGE(vertices),  //
      .label = "atlas-vertices"  //
  });

  if (0 == params->material->mpTexture) {
    u32 pixels[params->material->texture->w * params->material->texture->h];  // ABGR
    u32 ii = 0, mask, color;
    for (u32 y = 0; y < params->material->texture->h; y++) {
      for (u32 x = 0; x < params->material->texture->w; x++) {
        mask = params->entity->render->useMask ? params->entity->render->mask : COLOR_PINK;
        color = Bmp__Get2DPixel(texture, x, y, mask);
        pixels[ii++] = color;
      }
    }

    g_engine->sg_init_image(
        params->material->bind->fs.images[SLOT__texture1],
        &(sg_image_desc){
            .width = params->material->texture->w,
            .height = params->material->texture->h,
            .pixel_format = SG_PIXELFORMAT_RGBA8,  // has transparency
            .data.subimage[0][0] = {
                .ptr = pixels,
                .size = params->material->texture->w * params->material->texture->h * 4,
            }});
  }

  // g_engine->sg_update_buffer(logic->meshRenderer.bind->vertex_buffers[0], &SG_RANGE(vertices));
  //   g_engine->sg_update_image(
  //       logic->meshRenderer.bind->fs.images[SLOT__texture1],
  //       &(sg_image_data){
  //           .subimage[0][0] = {
  //               .ptr = pixels,
  //               .size = params->material->texture->w * params->material->texture->h * 4,
  //           }});
}

void Atlas__onrender_alloc(void* _params) {
  OnRenderParams2* params = _params;
  vs_params_t* vs_params = params->material->vs_params =
      Arena__Push(g_engine->frameArena, sizeof(vs_params_t));
  fs_params_t* fs_params = params->material->fs_params =
      Arena__Push(g_engine->frameArena, sizeof(fs_params_t));

  g_engine->sg_apply_pipeline(*params->material->pipe);
  g_engine->sg_apply_bindings(params->material->bind);
}

void Atlas__onrender_entity(void* _params) {
  OnRenderParams3* params = _params;
  vs_params_t* vs_params = params->material->vs_params;
  fs_params_t* fs_params = params->material->fs_params;

  vs_params->models[params->b] = params->model;
  vs_params->batch[params->b][0] = params->entity->render->ti;
  vs_params->batch[params->b][1] = params->entity->render->color;
  vs_params->batch[params->b][2] = params->entity->render->po;
  vs_params->batch[params->b][3] = 0;
}

void Atlas__onrender_material(void* _params) {
  OnRenderParams4* params = _params;
  vs_params_t* vs_params = params->material->vs_params;
  fs_params_t* fs_params = params->material->fs_params;

  vs_params->view = params->view;
  vs_params->projection = params->projection;
  vs_params->billboard = params->entity->render->billboard ? 1 : 0;
  vs_params->camPos[0] = params->viewPos.X;
  vs_params->camPos[1] = params->viewPos.Y;
  vs_params->camPos[2] = params->viewPos.Z;

  fs_params->ip = params->entity->render->indexedPalette ? 1 : 0;
  fs_params->pi = params->entity->render->pi;
  fs_params->tw = params->entity->render->tw;
  fs_params->th = params->entity->render->th;
  fs_params->aw = params->entity->render->aw;  // material->texture->w
  fs_params->ah = params->entity->render->ah;  // material->texture->h
  fs_params->useMask = params->entity->render->useMask ? 1 : 0;
  fs_params->mask = params->entity->render->mask;
  fs_params->fog =  //
      params->entity->render->rg != UI_ZSORT_RG &&  //
              params->entity->render->rg != SKY_RG &&  //
              params->entity->render->rg != SCREEN_RG
          ? 1
          : 0;

  g_engine->sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(*vs_params));
  g_engine->sg_apply_uniforms(SG_SHADERSTAGE_FS, SLOT_fs_params, &SG_RANGE(*fs_params));
}