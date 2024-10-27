#include "DebugText.h"

#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/Color.h"
#include "../common/Dispatcher.h"
#include "../common/Math.h"
#include "../common/Preloader.h"
#include "../components/MeshRenderer.h"
#include "Entity.h"
#include "Sprite.h"

extern Engine__State* g_engine;

void DebugText__init(Entity* entity, f32 x, f32 z) {
  Logic__State* logic = g_engine->logic;
  Sprite* self = (Sprite*)entity;
  Sprite__init(entity, x, z);

  //self->billboard = false;  // TODO: turn off billboardr

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));
  entity->render->rg = WORLD_ZSORT_RG;

  // preload assets
  entity->render->material = Preload__material(&logic->materials.glyph);
  entity->render->material->mesh = Preload__model(  //
      &logic->models.plane2D,
      "../assets/models/plane2D.obj");
  entity->render->material->texture = Preload__texture(  //
      &logic->textures.glyphs0,
      "../assets/textures/glyphs0.bmp");
  entity->render->ti = (u8)'A';
  entity->render->tw = 4, entity->render->th = 6;  // glyph 4x6 pixels
  entity->render->aw = 128, entity->render->ah = 24;  // atlas 32x4 chars
  entity->render->useMask = false;
  entity->render->mask = WHITE;
  entity->render->color = TRANSPARENT;

  entity->tform->scale.x = 4.0f / 6;
  entity->tform->scale.y = 6.0f / 6;
  entity->tform->scale.z = 1;
}

void DebugText__render(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  Sprite* self = (Sprite*)block;

  MeshRenderer__render(entity);
}