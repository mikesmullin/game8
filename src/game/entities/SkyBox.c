#include "SkyBox.h"

void SkyBox__init(Entity* entity) {
  Entity__init((Entity*)entity);
  entity->tags1 |= TAG_SKY;
  entity->dispatch->tick = SKY_BOX__TICK;

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));
  entity->render->billboard = false;
  entity->render->indexedPalette = false;
  entity->render->ti = 0;
  entity->render->po = 0;
  entity->render->tw = 512;
  entity->render->th = 64;
  entity->render->aw = 512;
  entity->render->ah = 64;
  //entity->render->color = 0x44000066;  // red
  // entity->render->color = 0x22666666;  // grey
  entity->render->rg = SKY_RG;

  f32 s = 0.25f;
  entity->tform->pos.y = 64.0f * s;
  entity->tform->scale.x = 256.0f * s;
  entity->tform->scale.y = 64.0f * s;
  entity->tform->scale.z = 256.0f * s;

  // preload assets
  entity->render->material = Preload__material(&g_engine->materials->cubemap, sizeof(Material));
  entity->render->material->mesh = Preload__model(  //
      &g_engine->models->skybox,
      "../assets/models/",
      "skybox.obj");
  entity->render->material->shader = g_engine->shaders->atlas;
  entity->render->material->texture0 = Preload__texture(  //
      &g_engine->textures->sky,
      "../assets/textures/sky.bmp");
}

void SkyBox__tick(Entity* entity) {
  PROFILE__BEGIN(SKY_BOX__TICK);
  Player* player1 = (Player*)g_engine->players->head->data;

  entity->tform->pos.x = player1->base.base.tform->pos.x;
  entity->tform->pos.z = player1->base.base.tform->pos.z;
  PROFILE__END(SKY_BOX__TICK);
}
