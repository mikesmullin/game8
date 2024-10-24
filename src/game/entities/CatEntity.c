#include "CatEntity.h"

#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/List.h"
#include "../common/Math.h"

// #include "../behaviortrees/BTCat.h"
#include "../common/Color.h"
#include "../common/Dispatcher.h"
#include "../common/Preloader.h"
#include "../common/QuadTree.h"
#include "../components/Collider.h"
#include "../components/MeshRenderer.h"
#include "../components/Rigidbody2D.h"
#include "../stategraphs/SGCat.h"
#include "../stategraphs/StateGraph.h"
#include "Entity.h"
#include "Sprite.h"

extern Engine__State* g_engine;

static const f32 CAT_MOVE_SPEED = 0.01f;  // per-second
static f32 lastTurnWait = 2.0f;
static f32 sinceLastTurn = 0;

void CatEntity__init(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  CatEntity* self = (CatEntity*)entity;
  Sprite__init(entity, 0, 0);
  entity->engine->tick = CAT_ENTITY__TICK;
  entity->engine->render = CAT_ENTITY__RENDER;
  entity->engine->gui = CAT_ENTITY__GUI;
  entity->tags1 |= TAG_CAT;

  entity->tform->pos.x = 0.0f;
  entity->tform->pos.y = 0.0f;
  entity->tform->pos.z = 0.0f;
  entity->tform->rot.x = 0.0f;
  entity->tform->rot.y = 0.0f;
  entity->tform->rot.z = 0.0f;
  self->xa = Math__random(-1, 1);
  self->za = Math__random(-1, 1);

  CircleCollider2DComponent* collider =
      Arena__Push(g_engine->arena, sizeof(CircleCollider2DComponent));
  collider->base.type = CIRCLE_COLLIDER_2D;
  collider->r = 0.5f;
  collider->base.collide = CAT_ENTITY__COLLIDE;
  entity->collider = (ColliderComponent*)collider;

  self->sg = Arena__Push(g_engine->arena, sizeof(StateGraph));
  self->sg->currentState = 0;
  self->sg->entity = entity;
  // self->sg->actions = subbedActions;

  // self->brain = (BTNode*)&BTroot;
  // self->brain->entity = entity;

  entity->render->tx = Math__urandom2(0, 7);
  entity->render->ty = 1;
  entity->render->useMask = true;
  entity->render->mask = BLACK;
  entity->render->color = TRANSPARENT;

  // preload assets
  Preload__audio(
      &logic->audio.meow,  //
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

void CatEntity__callSGAction(StateGraph* sg, Action* action) {
  subbedActions(sg, action);
}

void CatEntity__render(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  CatEntity* self = (CatEntity*)entity;

  Sprite__render(entity);
}

void CatEntity__gui(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  CatEntity* self = (CatEntity*)entity;

  // Bitmap__DebugText2(
  //     state,
  //     6,
  //     6 * 8,
  //     WHITE,
  //     TRANSPARENT,
  //     "frame %d tx %d ty %d",
  //     self->sg->frame,
  //     entity->render->tx,
  //     entity->render->ty);
}

void CatEntity__tick(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  CatEntity* self = (CatEntity*)entity;

  sinceLastTurn += g_engine->deltaTime;
  if (sinceLastTurn > lastTurnWait) {
    sinceLastTurn = 0;
  }

  entity->rb->xa = self->xa * CAT_MOVE_SPEED;
  entity->rb->za = self->za * CAT_MOVE_SPEED;

  // self->brain->tick(self->brain);
  StateGraph__tick(self->sg);

  Rigidbody2D__move(entity);
}

void CatEntity__collide(Entity* entity, void* _params) {
  OnCollideParams* params = _params;
  CatEntity* self = (CatEntity*)entity;

  if (params->after) {  // onenter, onstay
    if (TAG_CAT & params->target->tags1) {
      params->noclip = true;
    } else {
      self->xa = Math__random(-1, 1);
      self->za = Math__random(-1, 1);
    }
  }
}