// PBR.c
#include "PBR.h"

//
#include "../../../assets/shaders/pbr.glsl.h"

typedef struct {
  v3 position;
  v3 normal;
  v3 tangent;
  v3 bitangent;
  v2 texCoord;
} Vertex;

static void calculateTangents(Vertex* vertices, u32 indexCount) {
  for (u32 i = 0; i < indexCount; i += 3) {
    // Get the indices of the triangle's vertices
    u32 i0 = i;
    u32 i1 = i + 1;
    u32 i2 = i + 2;

    // Get the vertex positions
    v3 _v0 = vertices[i0].position;
    v3 _v1 = vertices[i1].position;
    v3 _v2 = vertices[i2].position;

    // Get the texture coordinates
    v2 uv0 = vertices[i0].texCoord;
    v2 uv1 = vertices[i1].texCoord;
    v2 uv2 = vertices[i2].texCoord;

    // Calculate the edges of the triangle
    v3 edge1, edge2;
    glms_v3_sub(_v1, _v0, &edge1);
    glms_v3_sub(_v2, _v0, &edge2);

    // Calculate the differences in UV coordinates
    f32 deltaU1 = uv1.x - uv0.x;
    f32 deltaV1 = uv1.y - uv0.y;
    f32 deltaU2 = uv2.x - uv0.x;
    f32 deltaV2 = uv2.y - uv0.y;

    // Calculate the tangent and bitangent
    f32 f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

    v3 tangent, bitangent;

    tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
    tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
    tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);
    glms_v3_normalize(&tangent);

    bitangent.x = f * (-deltaU2 * edge1.x + deltaU1 * edge2.x);
    bitangent.y = f * (-deltaU2 * edge1.y + deltaU1 * edge2.y);
    bitangent.z = f * (-deltaU2 * edge1.z + deltaU1 * edge2.z);
    glms_v3_normalize(&bitangent);

    // Add the tangent and bitangent to each vertex of the triangle
    glms_v3_add(vertices[i0].tangent, tangent, &vertices[i0].tangent);
    glms_v3_add(vertices[i1].tangent, tangent, &vertices[i1].tangent);
    glms_v3_add(vertices[i2].tangent, tangent, &vertices[i2].tangent);

    glms_v3_add(vertices[i0].bitangent, bitangent, &vertices[i0].bitangent);
    glms_v3_add(vertices[i1].bitangent, bitangent, &vertices[i1].bitangent);
    glms_v3_add(vertices[i2].bitangent, bitangent, &vertices[i2].bitangent);
  }

  // Normalize the tangents and bitangents for each vertex
  for (u32 i = 0; i < indexCount; i++) {
    glms_v3_normalize(&vertices[i].tangent);
    glms_v3_normalize(&vertices[i].bitangent);
  }
}

void PBR__onrender_load(void* _params) {
  OnRenderParams1* params = _params;
  Wavefront* mesh = params->material->mesh;

  // alloc memory once for all meshRenderer instances, since it would be the same for each
  params->material->pipe = Arena__Push(g_engine->arena, sizeof(sg_pipeline));
  params->material->bind = Arena__Push(g_engine->arena, sizeof(sg_bindings));

  // textures & samplers
  params->material->bind->fs.images[SLOT__albedoTexture] = g_engine->sg_alloc_image();
  params->material->bind->fs.samplers[SLOT_albedoTexture_smp] = g_engine->sg_alloc_sampler();
  g_engine->sg_init_sampler(
      params->material->bind->fs.samplers[SLOT_albedoTexture_smp],
      &(sg_sampler_desc){
          .wrap_u = SG_WRAP_REPEAT,
          .wrap_v = SG_WRAP_REPEAT,
          .min_filter = SG_FILTER_NEAREST,
          .mag_filter = SG_FILTER_NEAREST,
          .compare = SG_COMPAREFUNC_NEVER,
      });

  params->material->bind->fs.images[SLOT_normalTexture_tex] = g_engine->sg_alloc_image();
  params->material->bind->fs.samplers[SLOT_normalTexture_smp] = g_engine->sg_alloc_sampler();
  g_engine->sg_init_sampler(
      params->material->bind->fs.samplers[SLOT_normalTexture_smp],
      &(sg_sampler_desc){
          .wrap_u = SG_WRAP_REPEAT,
          .wrap_v = SG_WRAP_REPEAT,
          .min_filter = SG_FILTER_NEAREST,
          .mag_filter = SG_FILTER_NEAREST,
          .compare = SG_COMPAREFUNC_NEVER,
      });

  params->material->bind->fs.images[SLOT_metalnessTexture_tex] = g_engine->sg_alloc_image();
  params->material->bind->fs.samplers[SLOT_metalnessTexture_smp] = g_engine->sg_alloc_sampler();
  g_engine->sg_init_sampler(
      params->material->bind->fs.samplers[SLOT_metalnessTexture_smp],
      &(sg_sampler_desc){
          .wrap_u = SG_WRAP_REPEAT,
          .wrap_v = SG_WRAP_REPEAT,
          .min_filter = SG_FILTER_NEAREST,
          .mag_filter = SG_FILTER_NEAREST,
          .compare = SG_COMPAREFUNC_NEVER,
      });

  params->material->bind->fs.images[SLOT_roughnessTexture_tex] = g_engine->sg_alloc_image();
  params->material->bind->fs.samplers[SLOT_roughnessTexture_smp] = g_engine->sg_alloc_sampler();
  g_engine->sg_init_sampler(
      params->material->bind->fs.samplers[SLOT_roughnessTexture_smp],
      &(sg_sampler_desc){
          .wrap_u = SG_WRAP_REPEAT,
          .wrap_v = SG_WRAP_REPEAT,
          .min_filter = SG_FILTER_NEAREST,
          .mag_filter = SG_FILTER_NEAREST,
          .compare = SG_COMPAREFUNC_NEVER,
      });

  params->material->bind->fs.images[SLOT_specularBRDF_LUT_tex] = g_engine->sg_alloc_image();
  params->material->bind->fs.samplers[SLOT_specularBRDF_LUT_smp] = g_engine->sg_alloc_sampler();
  g_engine->sg_init_sampler(
      params->material->bind->fs.samplers[SLOT_specularBRDF_LUT_smp],
      &(sg_sampler_desc){
          .wrap_u = SG_WRAP_REPEAT,
          .wrap_v = SG_WRAP_REPEAT,
          .min_filter = SG_FILTER_NEAREST,
          .mag_filter = SG_FILTER_NEAREST,
          .compare = SG_COMPAREFUNC_NEVER,
      });

  params->material->bind->fs.images[SLOT_specularTexture_tex] = g_engine->sg_alloc_image();
  params->material->bind->fs.samplers[SLOT_specularTexture_smp] = g_engine->sg_alloc_sampler();
  g_engine->sg_init_sampler(
      params->material->bind->fs.samplers[SLOT_specularTexture_smp],
      &(sg_sampler_desc){
          .wrap_u = SG_WRAP_REPEAT,
          .wrap_v = SG_WRAP_REPEAT,
          .min_filter = SG_FILTER_NEAREST,
          .mag_filter = SG_FILTER_NEAREST,
          .compare = SG_COMPAREFUNC_NEVER,
      });

  params->material->bind->fs.images[SLOT_irradianceTexture_tex] = g_engine->sg_alloc_image();
  params->material->bind->fs.samplers[SLOT_irradianceTexture_smp] = g_engine->sg_alloc_sampler();
  g_engine->sg_init_sampler(
      params->material->bind->fs.samplers[SLOT_irradianceTexture_smp],
      &(sg_sampler_desc){
          .wrap_u = SG_WRAP_REPEAT,
          .wrap_v = SG_WRAP_REPEAT,
          .min_filter = SG_FILTER_NEAREST,
          .mag_filter = SG_FILTER_NEAREST,
          .compare = SG_COMPAREFUNC_NEVER,
      });

  // create shader from code-generated sg_shader_desc
  sg_shader shd = g_engine->sg_make_shader(pbr_shader_desc(g_engine->sg_query_backend()));

  // create a pipeline object (default render states are fine for triangle)
  (*params->material->pipe) = g_engine->sg_make_pipeline(&(sg_pipeline_desc){
      .shader = shd,
      // if the vertex layout doesn't have gaps, don't need to provide strides and offsets
      .layout =
          {
              .attrs =
                  {
                      [ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
                      [ATTR_vs_normal].format = SG_VERTEXFORMAT_FLOAT3,
                      [ATTR_vs_tangent].format = SG_VERTEXFORMAT_FLOAT3,
                      [ATTR_vs_bitangent].format = SG_VERTEXFORMAT_FLOAT3,
                      [ATTR_vs_texcoord].format = SG_VERTEXFORMAT_FLOAT2,
                  },
          },
      .depth =
          {
              .compare = SG_COMPAREFUNC_LESS_EQUAL,
              .write_enabled = true,
          },
      // .colors[0] =
      //     {
      //         .blend =
      //             {// Enable blending only for transparency layers
      //              .enabled = params->entity->render->rg != WORLD_UNSORT_RG,
      //              //  source color (the fragment's color) will be multiplied by its alpha.
      //              .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
      //              .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
      //              // destination color (the existing color in the framebuffer)
      //              // will be multiplied by (1 - src_alpha),
      //              // providing the transparency effect.
      //              .src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA,
      //              .dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
      //              // add the source and destination colors together
      //              // after applying the blend factors.
      //              .op_rgb = SG_BLENDOP_ADD,
      //              .op_alpha = SG_BLENDOP_ADD},
      //     },
      .cull_mode = SG_CULLMODE_BACK,
      // .sample_count = 1,  // because rendering to texture
      .label = "pbr-pipeline",
  });

  f32 vertices
      [mesh->faces->len * 3 /*points (tri)*/ *
       14 /*v3 pos + v3 normal + v3 tangent + v3 bitangent + v2 uv*/];
  u32 i = 0;
  List__Node* c = mesh->faces->head;
  for (u32 t = 0; t < mesh->faces->len; t++) {
    Wavefront__Face* f = c->data;
    c = c->next;

    Vertex _vertices[3];
    for (u32 y = 0; y < 3; y++) {
      // vec3 position
      v3* p = List__get(mesh->vertices, f->vertex_idx[y] - 1);
      _vertices[y].position.x = p->x;
      _vertices[y].position.y = p->y;
      _vertices[y].position.z = p->z;
      // vec3 normal
      v3* n = List__get(mesh->normals, f->normal_idx[y] - 1);
      _vertices[y].normal.x = n->x;
      _vertices[y].normal.y = n->y;
      _vertices[y].normal.z = n->z;
      // vec3 tangent
      _vertices[y].tangent.x = 0.0f;
      _vertices[y].tangent.y = 0.0f;
      _vertices[y].tangent.z = 0.0f;
      // vec3 bitangent
      _vertices[y].bitangent.x = 0.0f;
      _vertices[y].bitangent.y = 0.0f;
      _vertices[y].bitangent.z = 0.0f;
      // vec2 texcoord
      v2* uv = List__get(mesh->texcoords, f->texcoord_idx[y] - 1);
      _vertices[y].texCoord.x = uv->x;
      _vertices[y].texCoord.y = uv->y;
    }
    calculateTangents(_vertices, 3);

    // TODO: send indexed list to GPU
    for (u32 y = 0; y < 3; y++) {
      // vec3 position
      vertices[i++] = _vertices[y].position.x;
      vertices[i++] = _vertices[y].position.y;
      vertices[i++] = _vertices[y].position.z;
      // vec3 normal
      vertices[i++] = _vertices[y].normal.x;
      vertices[i++] = _vertices[y].normal.y;
      vertices[i++] = _vertices[y].normal.z;
      // vec3 tangent
      vertices[i++] = _vertices[y].tangent.x;
      vertices[i++] = _vertices[y].tangent.y;
      vertices[i++] = _vertices[y].tangent.z;
      // vec3 bitangent
      vertices[i++] = _vertices[y].bitangent.x;
      vertices[i++] = _vertices[y].bitangent.y;
      vertices[i++] = _vertices[y].bitangent.z;
      // vec2 texcoord
      vertices[i++] = _vertices[y].texCoord.x;
      vertices[i++] = _vertices[y].texCoord.y;
    }
  }
  params->material->bind->vertex_buffers[0] = g_engine->sg_make_buffer(&(sg_buffer_desc){
      .data = SG_RANGE(vertices),  //
      .label = "pbr-vertices"  //
  });

  // textures

  {
    u32 plen = params->material->texture0->w * params->material->texture0->h * sizeof(u32);
    u32* pixels = Arena__Push(g_engine->frameArena, plen);  // ABGR
    u32 ii = 0, color;
    for (u32 y = 0; y < params->material->texture0->h; y++) {
      for (u32 x = 0; x < params->material->texture0->w; x++) {
        color = Bmp__Get2DPixel(params->material->texture0, x, y, COLOR_PINK);
        pixels[ii++] = color;
      }
    }
    g_engine->sg_init_image(
        params->material->bind->fs.images[SLOT__albedoTexture],
        &(sg_image_desc){
            .width = params->material->texture0->w,
            .height = params->material->texture0->h,
            .pixel_format = SG_PIXELFORMAT_RGBA8,  // has transparency
            .data.subimage[0][0] = {
                .ptr = pixels,
                .size = plen,
            }});
  }

  {
    u32 plen = params->material->texture1->w * params->material->texture1->h * sizeof(u32);
    u32* pixels = Arena__Push(g_engine->frameArena, plen);  // ABGR
    u32 ii = 0, color;
    for (u32 y = 0; y < params->material->texture1->h; y++) {
      for (u32 x = 0; x < params->material->texture1->w; x++) {
        color = Bmp__Get2DPixel(params->material->texture1, x, y, COLOR_PINK);
        pixels[ii++] = color;
      }
    }
    g_engine->sg_init_image(
        params->material->bind->fs.images[SLOT_normalTexture_tex],
        &(sg_image_desc){
            .width = params->material->texture1->w,
            .height = params->material->texture1->h,
            .pixel_format = SG_PIXELFORMAT_RGBA8,  // has transparency
            .data.subimage[0][0] = {
                .ptr = pixels,
                .size = plen,
            }});
  }

  {
    u32 plen = params->material->texture2->w * params->material->texture2->h * sizeof(u32);
    u32* pixels = Arena__Push(g_engine->frameArena, plen);  // ABGR
    u32 ii = 0, color;
    for (u32 y = 0; y < params->material->texture2->h; y++) {
      for (u32 x = 0; x < params->material->texture2->w; x++) {
        color = Bmp__Get2DPixel(params->material->texture2, x, y, COLOR_PINK);
        pixels[ii++] = color;
      }
    }

    g_engine->sg_init_image(
        params->material->bind->fs.images[SLOT_metalnessTexture_tex],
        &(sg_image_desc){
            .width = params->material->texture2->w,
            .height = params->material->texture2->h,
            .pixel_format = SG_PIXELFORMAT_RGBA8,  // has transparency
            .data.subimage[0][0] = {
                .ptr = pixels,
                .size = plen,
            }});
  }

  {
    u32 plen = params->material->texture3->w * params->material->texture3->h * sizeof(u32);
    u32* pixels = Arena__Push(g_engine->frameArena, plen);  // ABGR
    u32 ii = 0, color;
    for (u32 y = 0; y < params->material->texture3->h; y++) {
      for (u32 x = 0; x < params->material->texture3->w; x++) {
        color = Bmp__Get2DPixel(params->material->texture3, x, y, COLOR_PINK);
        pixels[ii++] = color;
      }
    }
    g_engine->sg_init_image(
        params->material->bind->fs.images[SLOT_roughnessTexture_tex],
        &(sg_image_desc){
            .width = params->material->texture3->w,
            .height = params->material->texture3->h,
            .pixel_format = SG_PIXELFORMAT_RGBA8,  // has transparency
            .data.subimage[0][0] = {
                .ptr = pixels,
                .size = plen,
            }});
  }

  {
    u32 plen = params->material->texture4->w * params->material->texture4->h * sizeof(u32);
    u32* pixels = Arena__Push(g_engine->frameArena, plen);  // ABGR
    u32 ii = 0, color;
    for (u32 y = 0; y < params->material->texture4->h; y++) {
      for (u32 x = 0; x < params->material->texture4->w; x++) {
        color = Bmp__Get2DPixel(params->material->texture4, x, y, COLOR_PINK);
        pixels[ii++] = color;
      }
    }
    g_engine->sg_init_image(
        params->material->bind->fs.images[SLOT_specularBRDF_LUT_tex],
        &(sg_image_desc){
            .width = params->material->texture4->w,
            .height = params->material->texture4->h,
            .pixel_format = SG_PIXELFORMAT_RGBA8,  // has transparency
            .data.subimage[0][0] = {
                .ptr = pixels,
                .size = plen,
            }});
  }

  {
    u32 plen = params->material->texture5->w * params->material->texture5->h * sizeof(u32);
    u32* pixels = Arena__Push(g_engine->frameArena, plen);  // ABGR
    u32 ii = 0, color;
    for (u32 y = 0; y < params->material->texture5->h; y++) {
      for (u32 x = 0; x < params->material->texture5->w; x++) {
        color = Bmp__Get2DPixel(params->material->texture5, x, y, COLOR_PINK);
        pixels[ii++] = color;
      }
    }
    g_engine->sg_init_image(
        params->material->bind->fs.images[SLOT_specularTexture_tex],
        &(sg_image_desc){
            .type = SG_IMAGETYPE_CUBE,
            .width = params->material->texture5->w,
            .height = params->material->texture5->h,
            .pixel_format = SG_PIXELFORMAT_RGBA8,  // has transparency
            .data.subimage[0][0] =
                {
                    .ptr = pixels,
                    .size = plen,
                },
            .data.subimage[1][0] =
                {
                    .ptr = pixels,
                    .size = plen,
                },
            .data.subimage[2][0] =
                {
                    .ptr = pixels,
                    .size = plen,
                },
            .data.subimage[3][0] =
                {
                    .ptr = pixels,
                    .size = plen,
                },
            .data.subimage[4][0] =
                {
                    .ptr = pixels,
                    .size = plen,
                },
            .data.subimage[5][0] =
                {
                    .ptr = pixels,
                    .size = plen,
                },
        });
  }

  {
    u32 plen = params->material->texture6->w * params->material->texture6->h * sizeof(u32);
    u32* pixels = Arena__Push(g_engine->frameArena, plen);  // ABGR
    u32 ii = 0, color;
    for (u32 y = 0; y < params->material->texture6->h; y++) {
      for (u32 x = 0; x < params->material->texture6->w; x++) {
        color = Bmp__Get2DPixel(params->material->texture6, x, y, COLOR_PINK);
        pixels[ii++] = color;
      }
    }
    g_engine->sg_init_image(
        params->material->bind->fs.images[SLOT_irradianceTexture_tex],
        &(sg_image_desc){
            .type = SG_IMAGETYPE_CUBE,
            .width = params->material->texture0->w,
            .height = params->material->texture0->h,
            .pixel_format = SG_PIXELFORMAT_RGBA8,  // has transparency
            .data.subimage[0][0] =
                {
                    .ptr = pixels,
                    .size = plen,
                },
            .data.subimage[1][0] =
                {
                    .ptr = pixels,
                    .size = plen,
                },
            .data.subimage[2][0] =
                {
                    .ptr = pixels,
                    .size = plen,
                },
            .data.subimage[3][0] =
                {
                    .ptr = pixels,
                    .size = plen,
                },
            .data.subimage[4][0] =
                {
                    .ptr = pixels,
                    .size = plen,
                },
            .data.subimage[5][0] =
                {
                    .ptr = pixels,
                    .size = plen,
                },
        });
  }
}

void PBR__onrender_alloc(void* _params) {
  OnRenderParams2* params = _params;
  TransformUniforms_t* TransformUniforms = params->material->uniforms1 =
      Arena__Push(g_engine->frameArena, sizeof(TransformUniforms_t));
  ShadingUniforms_t* ShadingUniforms = params->material->uniforms2 =
      Arena__Push(g_engine->frameArena, sizeof(ShadingUniforms_t));

  g_engine->sg_apply_pipeline(*params->material->pipe);
  g_engine->sg_apply_bindings(params->material->bind);
}

void PBR__onrender_entity(void* _params) {
  OnRenderParams3* params = _params;
  TransformUniforms_t* TransformUniforms = params->material->uniforms1;
  ShadingUniforms_t* ShadingUniforms = params->material->uniforms2;

  TransformUniforms->models[params->b] = params->model;
  TransformUniforms->batch[params->b][0] = params->entity->render->color;
}

void PBR__onrender_material(void* _params) {
  OnRenderParams4* params = _params;
  TransformUniforms_t* TransformUniforms = params->material->uniforms1;
  ShadingUniforms_t* ShadingUniforms = params->material->uniforms2;

  TransformUniforms->viewMatrix = params->view;
  TransformUniforms->projectionMatrix = params->projection;

  f32 s1 = mwave(1000.0f, -1.0f, 1.0f);
  f32 s2 = mwave(3000.0f, -1.0f, 1.0f);
  f32 s3 = mwave(7000.0f, -1.0f, 1.0f);
  // LOG_DEBUGF("s %f %f %f", s1, s2, s3);
  // s 0.669210 0.719315 0.337315
  ShadingUniforms->lights_direction[0][0] = 0.669210f;  // -X_RIGHT
  ShadingUniforms->lights_direction[0][1] = 0.719315f;  // -Y_UP
  ShadingUniforms->lights_direction[0][2] = 0.337315f;  // -Z_FWD
  ShadingUniforms->lights_radiance[0][0] = 1.0f;
  ShadingUniforms->lights_radiance[0][1] = 1.0f;
  ShadingUniforms->lights_radiance[0][2] = 1.0f;

  ShadingUniforms->lights_direction[1][0] = 0.0f;
  ShadingUniforms->lights_direction[1][1] = 0.0f;
  ShadingUniforms->lights_direction[1][2] = 0.0f;
  ShadingUniforms->lights_radiance[1][0] = 0.0f;
  ShadingUniforms->lights_radiance[1][1] = 0.0f;
  ShadingUniforms->lights_radiance[1][2] = 0.0f;

  ShadingUniforms->lights_direction[2][0] = 0.0f;
  ShadingUniforms->lights_direction[2][1] = 0.0f;
  ShadingUniforms->lights_direction[2][2] = 0.0f;
  ShadingUniforms->lights_radiance[2][0] = 0.0f;
  ShadingUniforms->lights_radiance[2][1] = 0.0f;
  ShadingUniforms->lights_radiance[2][2] = 0.0f;

  ShadingUniforms->eyePosition[0] = params->viewPos.X;
  ShadingUniforms->eyePosition[1] = params->viewPos.Y;
  ShadingUniforms->eyePosition[2] = params->viewPos.Z;

  g_engine->sg_apply_uniforms(
      SG_SHADERSTAGE_VS,
      SLOT_TransformUniforms,
      &SG_RANGE(*TransformUniforms));
  g_engine->sg_apply_uniforms(SG_SHADERSTAGE_FS, SLOT_ShadingUniforms, &SG_RANGE(*ShadingUniforms));
}