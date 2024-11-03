#include "SkyBox.h"

#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/Color.h"
#include "../common/Dispatcher.h"
#include "../common/Log.h"
#include "../common/Preloader.h"
#include "../common/Profiler.h"
#include "Entity.h"

extern Engine__State* g_engine;

void SkyBox__init(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Entity__init((Entity*)entity);
  entity->tags1 |= TAG_SKY;
  entity->engine->tick = SKY_BOX__TICK;

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
  entity->render->material = Preload__material(&logic->materials.cubemap);
  entity->render->material->mesh = Preload__model(  //
      &logic->models.skybox,
      "../assets/models/skybox.obj");
  entity->render->material->texture = Preload__texture(  //
      &logic->textures.sky,
      "../assets/textures/sky.bmp");
}

void SkyBox__tick(Entity* entity) {
  PROFILE__BEGIN(SKY_BOX__TICK);
  Logic__State* logic = g_engine->logic;

  entity->tform->pos.x = logic->player->base.tform->pos.x;
  entity->tform->pos.z = logic->player->base.tform->pos.z;
  PROFILE__END(SKY_BOX__TICK);
}
