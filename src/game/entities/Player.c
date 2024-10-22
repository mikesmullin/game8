#include "Player.h"

#include <math.h>

#include "../Logic.h"
#include "../behaviortrees/Action.h"
// #include "../blocks/BreakBlock.h"
#include "../common/Arena.h"
#include "../common/Log.h"
// #include "../common/Finger.h"
// #include "../common/Keyboard.h"
#include "../common/Math.h"
#include "../components/Rigidbody2D.h"
// #include "../entities/CatEntity.h"
#include "../common/Dispatcher.h"
#include "../common/Geometry.h"
#include "../common/QuadTree.h"
#include "Entity.h"

extern Engine__State* g_engine;

static const f32 PLAYER_WALK_SPEED = 5.0f;  // per-second
static const f32 PLAYER_STRAFE_MOD = 0.5f;  // percent of walk
static const f32 PLAYER_FLY_SPEED = 3.0f;  // per-second
static const f32 PLAYER_LOOK_SPEED = 0.3f;  // deg/sec

Player* Player__alloc() {
  return Arena__Push(g_engine->arena, sizeof(Player));
}

void Player__init(Player* self) {
  Logic__State* logic = g_engine->logic;
  Entity* entity = &self->base;

  Entity__init(entity);

  entity->engine->tick = PLAYER_ENTITY__TICK;

  self->input.xAxis = 0.0f;
  self->input.yAxis = 0.0f;
  self->input.zAxis = 0.0f;

  self->camera.fov = 45.0f;
  self->camera.nearZ = 0.1f;
  self->camera.farZ = 1000.0f;

  CircleCollider2DComponent* collider =
      Arena__Push(g_engine->arena, sizeof(CircleCollider2DComponent));
  collider->base.type = CIRCLE_COLLIDER_2D;
  collider->r = 0.60f;
  entity->collider = (ColliderComponent*)collider;

  // entity->health = Arena__Push(g_engine->arena, sizeof(HealthComponent));
  // entity->health->hp = 100;
  // entity->health->hurtTime = 0;

  // entity->hear = Arena__Push(g_engine->arena, sizeof(AudioListenerComponent));
}

void Player__tick(Player* self) {
  Logic__State* logic = g_engine->logic;
  Entity* entity = &self->base;

  f32 s = Math__map(sinf(g_engine->stm_ms(g_engine->stm_now()) / 3000), -1, 1, 0, 360 - 1);
  self->base.tform->rot.y = s;
  // self->base.tform->rot.y = 45;
  // self->base.tform->pos.x = -25;
  // self->base.tform->pos.z = -25;

  // if (!logic->mouseCaptured) {
  //   logic->mState->x = 0;
  //   logic->mState->y = 0;
  // } else {
  //   if (0 != logic->mState->x) {  // yaw (rotate around Y-axis)
  //     logic->player->base.tform->rot.y += -logic->mState->x * PLAYER_LOOK_SPEED;
  //     logic->player->base.tform->rot.y = Math__fmod(logic->player->base.tform->rot.y, 360.0f);
  //     logic->mState->x = 0;
  //   }

  //   if (0 != logic->mState->y) {  // pitch (rotate around X-axis)
  //     logic->player->base.tform->rot.x += -logic->mState->y * PLAYER_LOOK_SPEED;
  //     logic->player->base.tform->rot.x = Math__fmod(logic->player->base.tform->rot.x, 360.0f);
  //     logic->mState->y = 0;
  //   }

  //   if (true == logic->kbState->reload) {  // R
  //     logic->kbState->reload = false;
  //     logic->level->spawner->firstTick = true;  // tp to spawn
  //   }

  //   // W-S Forward/Backward axis
  //   if (logic->kbState->fwd && logic->kbState->back) {
  //     self->input.zAxis = 0.0f;
  //   } else if (logic->kbState->fwd) {
  //     self->input.zAxis = -1.0f;  // -Z_FWD
  //   } else if (logic->kbState->back) {
  //     self->input.zAxis = +1.0f;
  //   } else {
  //     self->input.zAxis = 0.0f;
  //   }

  //   // A-D Left/Right axis
  //   if (logic->kbState->left && logic->kbState->right) {
  //     self->input.xAxis = 0.0f;
  //   } else if (logic->kbState->left) {
  //     self->input.xAxis = -1.0f;
  //   } else if (logic->kbState->right) {
  //     self->input.xAxis = +1.0f;  // +X_RIGHT
  //   } else {
  //     self->input.xAxis = 0.0f;
  //   }

  //   // Q-E Up/Down axis
  //   if (logic->kbState->up && logic->kbState->down) {
  //     self->input.yAxis = 0.0f;
  //   } else if (logic->kbState->up) {
  //     self->input.yAxis = +1.0f;  // +Y_UP
  //   } else if (logic->kbState->down) {
  //     self->input.yAxis = -1.0f;
  //   } else {
  //     self->input.yAxis = 0.0f;
  //   }

  //   // Direction vectors for movement
  //   v3 forward, right, front;

  //   // Convert yaw to radians for direction calculation
  //   f32 yaw_radians = glms_rad(entity->tform->rot.y);

  //   // Calculate the front vector based on yaw only (for movement along the XZ plane)
  //   front.x = Math__sin(yaw_radians);
  //   front.y = 0.0f;
  //   front.z = Math__cos(yaw_radians);
  //   glms_v3_normalize(&front);

  //   // Calculate the right vector (perpendicular to the front vector)
  //   glms_v3_cross(front, (v3){0.0f, 1.0f, 0.0f}, &right);
  //   glms_v3_normalize(&right);

  //   // apply forward/backward motion
  //   v3 pos;
  //   // TODO: can manipulate this to simulate slipping/ice
  //   entity->rb->xa = 0;
  //   entity->rb->za = 0;
  //   if (0 != self->input.zAxis) {
  //     glms_v3_scale(front, self->input.zAxis * PLAYER_WALK_SPEED * state->deltaTime, &forward);
  //     glms_v3_add(entity->tform->pos, forward, &pos);
  //     entity->rb->xa += pos.x - entity->tform->pos.x;
  //     entity->rb->za += pos.z - entity->tform->pos.z;
  //   }

  //   // apply left/right motion
  //   if (0 != self->input.xAxis) {
  //     glms_v3_scale(
  //         right,
  //         -self->input.xAxis * PLAYER_WALK_SPEED * PLAYER_STRAFE_MOD * state->deltaTime,
  //         &forward);
  //     glms_v3_add(entity->tform->pos, forward, &pos);
  //     entity->rb->xa += pos.x - entity->tform->pos.x;
  //     entity->rb->za += pos.z - entity->tform->pos.z;
  //   }

  //   // apply up/down motion
  //   if (0 != self->input.yAxis) {
  //     entity->tform->pos.y += self->input.yAxis * PLAYER_FLY_SPEED * state->deltaTime;

  //     entity->tform->pos.y = MATH_CLAMP(0, entity->tform->pos.y, logic->level->height);
  //   }

  //   static const f32 SQRT_TWO = 1.414214f;

  //   f32 xm = self->input.xAxis;
  //   f32 zm = self->input.zAxis;
  //   f32 d = sqrtf(xm * xm + zm * zm);
  //   if (0 != self->input.zAxis && 0 != self->input.xAxis) {
  //     // normalize diagonal movement, so it is not faster
  //     entity->rb->xa /= SQRT_TWO;
  //     entity->rb->za /= SQRT_TWO;
  //   }

  //   // headbob
  //   if (d != 0) {
  //     if (xm != 0 && zm != 0) {
  //       // normalize diagonal movement, so it is not faster
  //       d /= SQRT_TWO;
  //     }
  //     d *= PLAYER_WALK_SPEED * state->deltaTime * 4.0f;
  //     self->bobPhase += d;
  //   }

  //   Rigidbody2D__move(entity, state);

  //   // Space (Use)
  //   if (logic->kbState->use) {
  //     logic->kbState->use = false;

  //     // find nearest cat
  //     // TODO: make into reusable FindNearestEntity() type of fn
  //     u32 matchCount = 0;
  //     void* matchData[40];  // TODO: don't limit search results?
  //     glms_v3_scale(front, -1, &forward);
  //     glms_v3_add(entity->tform->pos, forward, &pos);
  //     Rect range = {pos.x, pos.z, 0.5f, 0.5f};
  //     QuadTreeNode_query(logic->level->qt, range, 40, matchData, &matchCount);

  //     for (u32 i = 0; i < matchCount; i++) {
  //       Entity* other = (Entity*)matchData[i];
  //       if (entity == other) continue;

  //       if (TAG_CAT & other->tags1) {
  //         CatEntity* cat = (CatEntity*)other;
  //         Action action = {.type = ACTION_USE, .actor = entity, .target = other};

  //         // Action__PerformBuffered(other, &action);
  //         if (NULL != cat->sg) {
  //           CatEntity__callSGAction(cat->sg, &action);
  //         }
  //         break;
  //       }

  //       if (TAG_BRICK & other->tags1) {
  //         BreakBlock_t* brick = (BreakBlock_t*)other;
  //         Action action = {.type = ACTION_USE, .actor = entity, .target = other};

  //         if (!(TAG_BROKEN & other->tags1)) {
  //           if (NULL != brick->sg) {
  //             BreakBlock__callSGAction(brick->sg, &action);
  //             break;
  //           }
  //         }
  //       }
  //     }
  //   }
  // }

  // if (entity->health->hurtTime > 0) {
  //   entity->health->hurtTime -= state->deltaTime;
  //   if (entity->health->hurtTime < 0) entity->health->hurtTime = 0;
  // }
}