#include "CatEntity.h"

// #include "../behaviortrees/BTCat.h"
#include "../stategraphs/SGCat.h"

static const f32 CAT_MOVE_SPEED = 1.00f;  // per-second
static f32 lastTurnWait = 2.0f;
static f32 sinceLastTurn = 0;

void CatEntity__init(Entity* entity) {
  CatEntity* self = (CatEntity*)entity;
  Sprite__init(entity, 0, 0);
  entity->dispatch->tick = CAT_ENTITY__TICK;
  entity->dispatch->action = CAT_ENTITY__ACTION;
  entity->tags1 |= TAG_CAT;

  self->xa = Math__random(-1, 1);
  self->za = Math__random(-1, 1);

  CircleCollider2DComponent* collider =
      Arena__Push(g_engine->arena, sizeof(CircleCollider2DComponent));
  collider->base.type = CIRCLE_COLLIDER_2D;
  collider->r = 1.0f;
  collider->base.collide = CAT_ENTITY__COLLIDE;
  entity->collider = (ColliderComponent*)collider;

  self->sg = Arena__Push(g_engine->arena, sizeof(StateGraph));
  self->sg->currentState = 0;
  self->sg->entity = entity;
  // self->sg->actions = subbedActions;

  // self->brain = (BTNode*)&BTroot;
  // self->brain->entity = entity;

  entity->render->indexedPalette = true;
  entity->render->pi = 0;
  entity->render->po = Math__urandom2(0, 7);
  entity->render->ti = 2 * 8 + 4;
  entity->render->useMask = true;
  entity->render->mask = COLOR_BLACK;
  entity->render->color = COLOR_TRANSPARENT;

  // preload assets
  Preload__audio(
      &g_engine->audio->meow,  //
      "../assets/audio/sfx/meow.wav");
}

SGState* CatEntity__getSGState(u32 id) {
  // TODO: just define them as array ints too, instead of pointers
  // or move this inside the SGCat.h like with subbedActions()
  if (0 == id) return &SGidle;
  if (1 == id) return &SGtail;
  if (2 == id) return &SGblink;
  if (3 == id) return &SGmeow;
  return &SGidle;
}

void CatEntity__action(void* _params) {
  OnActionParams* params = _params;
  CatEntity* self = (CatEntity*)params->entity;

  subbedActions(self->sg, params);
}

void CatEntity__tick(void* _params) {
  PROFILE__BEGIN(CAT_ENTITY__TICK);
  OnEntityParams* params = _params;
  Entity* entity = params->entity;
  CatEntity* self = (CatEntity*)entity;

  entity->rb->velocity.x = self->xa * CAT_MOVE_SPEED;
  entity->rb->velocity.z = self->za * CAT_MOVE_SPEED;

  // self->brain->tick(self->brain);
  StateGraph__tick(self->sg, CatEntity__getSGState);
  // static u32 last_ti = 0;
  // if (last_ti != entity->render->ti) {
  //   LOG_DEBUGF("ti %u", entity->render->ti);
  // }
  // last_ti = entity->render->ti;

  Rigidbody2D__move(g_engine->game->level->qt, entity, Dispatcher__call);
  PROFILE__END(CAT_ENTITY__TICK);
}

void CatEntity__collide(void* _params) {
  OnCollideParams* params = _params;
  CatEntity* self = (CatEntity*)params->entity;

  if (params->after) {  // onenter, onstay
    if (TAG_CAT & params->target->tags1) {
      params->noclip = true;
    } else {
      self->xa = Math__random(-1, 1);
      self->za = Math__random(-1, 1);
    }
  }
}