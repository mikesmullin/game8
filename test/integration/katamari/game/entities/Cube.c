#include "Cube.h"

void Cube__init(Entity* entity) {
  Cube* cube = (Cube*)entity;
  Entity__init((Entity*)cube);

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));

  // preload
  entity->render->material = Preload__material(&g_engine->materials->red, sizeof(Material));
  entity->render->material->mesh = Preload__model(  //
      &g_engine->models->cube,
      "../test/fixtures/assets/models/",
      "cube.obj");
}