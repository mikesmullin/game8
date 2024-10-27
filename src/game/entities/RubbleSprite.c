#include "RubbleSprite.h"

#include "../Logic.h"
#include "../common/Color.h"
#include "../common/Dispatcher.h"
#include "../common/Easing.h"
#include "../common/Log.h"
#include "../common/Math.h"
#include "../common/Profiler.h"
#include "../components/MeshRenderer.h"
#include "Entity.h"
#include "Sprite.h"

extern Engine__State* g_engine;

void RubbleSprite__init(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Sprite* sprite = (Sprite*)entity;
  RubbleSprite* self = (RubbleSprite*)sprite;
  Sprite__init(entity, 0, 0);
  entity->engine->tick = RUBBLE_SPRITE__TICK;
  entity->engine->render = RUBBLE_SPRITE__RENDER;

  self->xa = Math__random(0, 1) - 0.5;
  self->ya = Math__random(0, 1);
  self->za = Math__random(0, 1) - 0.5;
  self->base.billboard = true;
  self->life = self->lifeSpan = easeInQuart(Math__random(0, 1)) * 3.0f;

  entity->render->ti = 7;
  entity->render->useMask = true;
  entity->render->mask = BLACK;
  entity->render->color = logic->level->wallCol;
}

void RubbleSprite__tick(Entity* entity) {
  PROFILE__BEGIN(RUBBLE_SPRITE__TICK);
  if (entity->removed) return;

  RubbleSprite* self = (RubbleSprite*)entity;
  self->base.base.tform->pos.x += self->xa * 0.2f;
  self->base.base.tform->pos.y += self->ya * 0.2f;
  self->base.base.tform->pos.z += self->za * 0.2f;
  self->ya -= 0.1f;
  self->life -= g_engine->deltaTime;
  if (self->life < 0.0f) self->life = 0;
  // f32 r = self->life / self->lifeSpan;
  // self->base.base.render->color =
  //     self->base.base.render->color &
  //     (0xff000000 | (u8)(r * 255) << 16 | (u8)(r * 255) << 8 | (u8)(r * 255));
  if (self->life <= 0) {
    entity->removed = true;
  }
  static f32 floor = 0;
  if (self->base.base.tform->pos.y < floor) {
    self->base.base.tform->pos.y = floor;
    self->xa *= 0.8f;
    self->za *= 0.8f;
  }
  PROFILE__END(RUBBLE_SPRITE__TICK);
}

void RubbleSprite__render(Entity* entity) {
  if (entity->removed) return;

  Sprite__render(entity);
}
