#include "Triangle.h"

#include <math.h>

#include "../../../vendor/sokol/sokol_gfx.h"
#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/Math.h"

//
#include "../../../assets/shaders/triangle-sapp.glsl.h"

extern Engine__State* g_engine;

//------------------------------------------------------------------------------
//  triangle-sapp example
//  Simple 2D rendering from vertex buffer.
//------------------------------------------------------------------------------

void Triangle__preload() {
  Logic__State* logic = g_engine->logic;

  logic->pip = Arena__Push(g_engine->arena, sizeof(sg_pipeline));
  logic->bind = Arena__Push(g_engine->arena, sizeof(sg_bindings));
  logic->pass_action = Arena__Push(g_engine->arena, sizeof(sg_pass_action));

  // a vertex buffer of vertices
  f32 u = 0.5f, z = 0, o = 1.0f;
  float vertices[] = {
      // positions                   // colors
      -u, u,  z, /*   */ o, o, o, o,  //
      u,  u,  z, /*   */ o, z, z, o,  //
      u,  -u, z, /**/ z,    o, z, o,  //
      -u, u,  z, /*   */ o, o, o, o,  //
      u,  -u, z, /**/ z,    o, z, o,  //
      -u, -u, z, /**/ z,    z, o, o,  //
  };
  logic->bind->vertex_buffers[0] = g_engine->sg_make_buffer(&(sg_buffer_desc){
      .data = SG_RANGE(vertices),  //
      .label = "square-vertices"  //
  });

  // create shader from code-generated sg_shader_desc
  sg_shader shd = g_engine->sg_make_shader(triangle_shader_desc(g_engine->sg_query_backend()));

  // create a pipeline object (default render states are fine for triangle)
  (*logic->pip) = g_engine->sg_make_pipeline(&(sg_pipeline_desc){
      .shader = shd,
      // if the vertex layout doesn't have gaps, don't need to provide strides and offsets
      .layout =
          {
              .attrs =
                  {
                      [ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,  //
                      [ATTR_vs_color0].format = SG_VERTEXFORMAT_FLOAT4,  //
                  }  //
          },
      .label = "square-pipeline",
  });

  // a pass action to clear framebuffer to black
  (*logic->pass_action) = (sg_pass_action){
      .colors[0] = {
          .load_action = SG_LOADACTION_CLEAR,  //
          .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},  //
      }};
}

void Triangle__render() {
  Logic__State* logic = g_engine->logic;

  g_engine->logic->now = g_engine->stm_ms(g_engine->stm_now());
  f32 s1 = Math__map(sinf(logic->now / 500.0f), -1, 1, 0, 0.25f);
  logic->pass_action->colors[0].clear_value.r = s1;
  logic->pass_action->colors[0].clear_value.g = 0;
  logic->pass_action->colors[0].clear_value.b = 0;

  g_engine->sg_begin_pass(
      &(sg_pass){.action = *logic->pass_action, .swapchain = g_engine->sglue_swapchain()});
  g_engine->sg_apply_pipeline(*logic->pip);
  g_engine->sg_apply_bindings(logic->bind);
  g_engine->sg_draw(0, 6, 1);
  g_engine->sg_end_pass();
}