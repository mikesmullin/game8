#include "Cube.h"

void Cube__init(Entity* entity) {
  Cube* cube = (Cube*)entity;
  Entity__init((Entity*)cube);

  cube->base.rb->mass = 10.0f;
  cube->friction = 0.5f;

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));

  // preload
  entity->render->material = Preload__material(&g_engine->materials->red, sizeof(Material));
  entity->render->material->mesh = Preload__model(  //
      &g_engine->models->cube,
      "../test/fixtures/assets/models/",
      "cube.obj");
  entity->render->material->shader = g_engine->shaders->pbr;
  // entity->render->material->shader = g_engine->shaders->atlas;
  entity->render->color = COLOR_GRAY;
  entity->render->material->texture0 = Preload__texture(  //
      &g_engine->textures->albedo,
      "../test/fixtures/assets/textures/test_A.bmp");
  entity->render->material->texture1 = Preload__texture(  //
      &g_engine->textures->normal,
      "../test/fixtures/assets/textures/test_N.bmp");
  entity->render->material->texture2 = Preload__texture(  //
      &g_engine->textures->metalness,
      "../test/fixtures/assets/textures/test_M.bmp");
  entity->render->material->texture3 = Preload__texture(  //
      &g_engine->textures->roughness,
      "../test/fixtures/assets/textures/test_R.bmp");
  entity->render->material->texture4 = Preload__texture(  //
      &g_engine->textures->specularBRDF_LUT,
      "../test/fixtures/assets/textures/test_S1.bmp");
  entity->render->material->texture5 = Preload__texture(  //
      &g_engine->textures->specular,
      "../test/fixtures/assets/textures/test_S2.bmp");
  entity->render->material->texture6 = Preload__texture(  //
      &g_engine->textures->irradiance,
      "../test/fixtures/assets/textures/test_I.bmp");
}