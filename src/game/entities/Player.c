#include "Player.h"

#include <math.h>

#include "../../../vendor/HandmadeMath/HandmadeMath.h"
#include "../Logic.h"
#include "../behaviortrees/Action.h"
#include "../common/Arena.h"
#include "../common/Dispatcher.h"
#include "../common/Geometry.h"
#include "../common/List.h"
#include "../common/Log.h"
#include "../common/Math.h"
#include "../common/Profiler.h"
#include "../common/QuadTree.h"
#include "../components/Rigidbody2D.h"
#include "../levels/Level.h"
#include "CatEntity.h"
#include "Entity.h"
#include "RubbleSprite.h"
#include "blocks/BreakBlock.h"

extern Engine__State* g_engine;

static const f32 PLAYER_WALK_SPEED = 5.0f;  // per-second
static const f32 PLAYER_STRAFE_MOD = 0.5f;  // percent of walk
static const f32 PLAYER_FLY_SPEED = 3.0f;  // per-second
static const f32 PLAYER_LOOK_SPEED = 0.3f;  // deg/sec

void Player__init(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Player* self = (Player*)entity;

  Entity__init(entity);

  entity->engine->tick = PLAYER_ENTITY__TICK;

  self->joy.xAxis = 0.0f;
  self->joy.yAxis = 0.0f;
  self->joy.zAxis = 0.0f;

  self->proj.type = PERSPECTIVE_PROJECTION;
  self->proj.fov = 45.0f;
  self->proj.nearZ = 0.1f;
  self->proj.farZ = 1000.0f;

  CircleCollider2DComponent* collider =
      Arena__Push(g_engine->arena, sizeof(CircleCollider2DComponent));
  collider->base.type = CIRCLE_COLLIDER_2D;
  collider->r = 0.65f;
  entity->collider = (ColliderComponent*)collider;

  entity->health = Arena__Push(g_engine->arena, sizeof(HealthComponent));
  entity->health->hp = 100;
  entity->health->hurtTime = 0;

  entity->hear = Arena__Push(g_engine->arena, sizeof(AudioListenerComponent));
}

void Player__tick(Entity* entity) {
  PROFILE__BEGIN(PLAYER_ENTITY__TICK);

  Logic__State* logic = g_engine->logic;
  Player* self = (Player*)entity;

  if (logic->player->input.use && !logic->mouseCaptured) {
    logic->player->input.use = false;
    g_engine->sapp_lock_mouse(true);
    LOG_DEBUGF("request mouse lock");
  }
  if (logic->player->input.esc && logic->mouseCaptured) {
    logic->player->input.esc = false;
    g_engine->sapp_lock_mouse(false);
    LOG_DEBUGF("request mouse unlock");
  }
  bool ml = g_engine->sapp_mouse_locked();
  if (ml != logic->mouseCaptured) {
    if (ml) LOG_DEBUGF("response mouse locked");
    if (!ml) LOG_DEBUGF("response mouse unlocked");
  }
  logic->mouseCaptured = ml;

  if (!logic->mouseCaptured) {
    logic->player->ptr.x = 0;
    logic->player->ptr.y = 0;
  } else {
    if (0 != logic->player->ptr.x) {  // yaw (rotate around Y-axis)
      logic->player->base.tform->rot.y += logic->player->ptr.x * PLAYER_LOOK_SPEED;
      logic->player->base.tform->rot.y = Math__rclampf(0, logic->player->base.tform->rot.y, 360.0f);
      logic->player->ptr.x = 0;
    }

    if (0 != logic->player->ptr.y) {  // pitch (rotate around X-axis)
      logic->player->base.tform->rot.x += logic->player->ptr.y * PLAYER_LOOK_SPEED;
      logic->player->base.tform->rot.x =
          Math__clamp(-55.0f, logic->player->base.tform->rot.x, 55.0f);
      logic->player->ptr.y = 0;
    }

    if (true == logic->player->input.reload) {  // R
      logic->player->input.reload = false;
      logic->level->spawner->firstTick = true;  // tp to spawn
    }

    // TODO: could implement player camera zoom (also from 1P to 3P)
    // if (0 != logic->player->ptr.wheely) {
    //   logic->player->base.tform->pos.z +=
    //       -logic->player->ptr.wheely * PLAYER_ZOOM_SPEED /* deltaTime*/;
    //   logic->player->ptr.wheely = 0;
    // }

    // W-S Forward/Backward axis
    if (logic->player->input.fwd) {
      self->joy.zAxis = +1.0f;
    } else if (logic->player->input.back) {
      self->joy.zAxis = -1.0f;
    } else {
      self->joy.zAxis = 0.0f;
    }

    // A-D Left/Right axis
    if (logic->player->input.right) {
      self->joy.xAxis = +1.0f;
    } else if (logic->player->input.left) {
      self->joy.xAxis = -1.0f;
    } else {
      self->joy.xAxis = 0.0f;
    }

    if (0 == self->joy.zAxis && 0 == self->joy.xAxis) {
      logic->player->lastInput = 0;
    } else {
      if (0 == logic->player->lastInput) logic->player->lastInput = g_engine->now;
    }

    // Q-E Up/Down axis
    if (logic->player->input.up) {
      self->joy.yAxis = +1.0f;  // +Y_UP
    } else if (logic->player->input.down) {
      self->joy.yAxis = -1.0f;
    } else {
      self->joy.yAxis = 0.0f;
    }

    // Direction vectors for movement
    HMM_Vec3 forward, right, front;

    // Convert yaw to radians for direction calculation
    f32 yaw_radians = HMM_AngleDeg(entity->tform->rot.y);

    // Calculate the front vector based on yaw only (for movement along the XZ plane)
    //   front.X = HMM_SinF(yaw_radians);
    //   front.Y = 0.0f;
    //   front.Z = HMM_CosF(yaw_radians);
    //   front = HMM_NormV3(front);
    HMM_Vec3 F = HMM_V3(0.0f, 0.0f, -1.0f);  // -Z_FWD
    HMM_Vec3 U = HMM_V3(0.0f, 1.0f, 0.0f);  // +Y_UP
    front = HMM_RotateV3AxisAngle_LH(F, U, yaw_radians);

    // Calculate the right vector (perpendicular to the front vector)
    right = HMM_Cross(front, U);
    right = HMM_NormV3(right);

    // apply forward/backward motion
    HMM_Vec3 p1 = HMM_V3(entity->tform->pos.x, entity->tform->pos.y, entity->tform->pos.z);
    HMM_Vec3 pos;
    // TODO: can manipulate this to simulate slipping/ice
    entity->rb->xa = 0;
    entity->rb->za = 0;
    if (0 != self->joy.zAxis) {
      forward = HMM_MulV3F(front, self->joy.zAxis * PLAYER_WALK_SPEED * g_engine->deltaTime);
      pos = HMM_AddV3(p1, forward);
      entity->rb->xa += pos.X - entity->tform->pos.x;
      entity->rb->za += pos.Z - entity->tform->pos.z;
    }

    // apply left/right motion
    if (0 != self->joy.xAxis) {
      forward = HMM_MulV3F(
          right,
          self->joy.xAxis * PLAYER_WALK_SPEED * PLAYER_STRAFE_MOD * g_engine->deltaTime);
      pos = HMM_AddV3(p1, forward);
      entity->rb->xa += pos.X - entity->tform->pos.x;
      entity->rb->za += pos.Z - entity->tform->pos.z;
    }

    // apply up/down motion
    if (0 != self->joy.yAxis) {
      entity->tform->pos.y += self->joy.yAxis * PLAYER_FLY_SPEED * g_engine->deltaTime;

      entity->tform->pos.y = Math__clamp(0, entity->tform->pos.y, logic->level->height);
    }

    static const f32 SQRT_TWO = 1.414214f;

    f32 xm = self->joy.xAxis;
    f32 zm = self->joy.zAxis;
    f32 d = sqrtf(xm * xm + zm * zm);
    if (0 != self->joy.zAxis && 0 != self->joy.xAxis) {
      // normalize diagonal movement, so it is not faster
      entity->rb->xa /= SQRT_TWO;
      entity->rb->za /= SQRT_TWO;
    }

    // headbob
    if (d != 0) {
      if (xm != 0 && zm != 0) {
        // normalize diagonal movement, so it is not faster
        d /= SQRT_TWO;
      }
      // d *= PLAYER_WALK_SPEED * g_engine->deltaTime * 4.0f;
      // d *=
      self->bobPhase = HMM_SinF(
          (g_engine->now - logic->player->lastInput) / 1000.0f * 2.0f * 3.0f *
          (60 / 29.0f));  // 29bpm
    }

    // LOG_DEBUGF(
    //     "Player move %f %f %f dT %f",
    //     entity->tform->pos.x,
    //     entity->tform->pos.y,
    //     entity->tform->pos.z,
    //     g_engine->deltaTime);
    Rigidbody2D__move(entity);

    if (logic->player->input.use) {
      logic->player->input.use = false;

      LOG_DEBUGF("use");

      // find block immediately in front
      u32 matchCount = 0;
      void* matchData[100];
      forward = HMM_MulV3F(front, 1.0f);
      pos = HMM_AddV3(p1, forward);

      RubbleSprite* rs = Arena__Push(g_engine->arena, sizeof(RubbleSprite));
      RubbleSprite__init((Entity*)rs);
      rs->base.base.tform->pos.x = p1.X;
      rs->base.base.tform->pos.y = p1.Y - 1.0f;
      rs->base.base.tform->pos.z = p1.Z;
      rs->xa += forward.X, rs->ya = 1, rs->za += forward.Z;
      rs->base.base.render->color = 0x660000ff;
      rs->life = rs->lifeSpan = 4.0f;
      List__append(g_engine->arena, logic->level->entities, rs);

      Rect range = {pos.X, pos.Z, 0.5f, 0.5f};
      QuadTreeNode_query(logic->level->qt, range, 100, matchData, &matchCount);

      for (u32 i = 0; i < matchCount; i++) {
        Entity* other = (Entity*)matchData[i];
        if (entity == other) continue;

        if (NULL != other->engine && 0 != other->engine->action &&
            HMM_ABS(entity->tform->pos.y - other->tform->pos.y) < 2.0f) {
          Dispatcher__call2(
              other->engine->action,
              other,
              &(Action){.type = ACTION_USE, .actor = entity, .target = other});
          break;
        }
      }
    }
  }

  if (entity->health->hurtTime > 0) {
    entity->health->hurtTime -= g_engine->deltaTime;
    if (entity->health->hurtTime < 0) entity->health->hurtTime = 0;
  }
  PROFILE__END(PLAYER_ENTITY__TICK);
}