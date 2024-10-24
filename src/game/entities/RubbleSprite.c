#include "RubbleSprite.h"

#include "../../../vendor/HandmadeMath/HandmadeMath.h"
#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/Color.h"
#include "../common/Dispatcher.h"
#include "../common/Log.h"
#include "../common/Math.h"
#include "../common/Preloader.h"
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

  entity->render->tx = 7;
  entity->render->ty = 0;
  entity->render->useMask = true;
  entity->render->mask = BLACK;
  entity->render->color = logic->level->wallCol;
}

void RubbleSprite__tick(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Sprite* sprite = (Sprite*)entity;
  RubbleSprite* self = (RubbleSprite*)sprite;
  if (self->removed) return;

  self->base.base.tform->pos.x += self->xa * 0.2f;
  self->base.base.tform->pos.y += self->ya * 0.2f;
  self->base.base.tform->pos.z += self->za * 0.2f;
  self->ya -= 0.1f;
  if (self->base.base.tform->pos.y < 0) {
    self->base.base.tform->pos.y = 0;
    self->xa *= 0.8f;
    self->za *= 0.8f;
    if (Math__random(0, 1) < 0.04f) {
      self->removed = true;
    }
  }
}

void RubbleSprite__render(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Sprite* sprite = (Sprite*)entity;
  RubbleSprite* self = (RubbleSprite*)sprite;
  if (self->removed) return;

  Sprite__render((Entity*)self);
}
