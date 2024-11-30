#include "Player.h"

static const f32 PLAYER_WALK_SPEED = 4.0f;  // per-second
static const f32 PLAYER_STRAFE_MOD = 0.5f;  // percent of walk
static const f32 PLAYER_FLY_SPEED = 3.0f;  // per-second
static const f32 PLAYER_LOOK_SPEED = 0.3f;  // deg/sec

void Player__init(Entity* entity) {
  Player* self = (Player*)entity;

  Entity__init(entity);

  entity->dispatch->tick = PLAYER_ENTITY__TICK;

  self->input.joy.xAxis = 0.0f;
  self->input.joy.yAxis = 0.0f;
  self->input.joy.zAxis = 0.0f;

  self->base.camera.proj.type = PERSPECTIVE_PROJECTION;
  self->base.camera.proj.fov = 45.0f;
  self->base.camera.proj.nearZ = 0.1f;
  self->base.camera.proj.farZ = 1000.0f;
  self->base.camera.screenSize = SCREEN_SIZE;

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

void Player__tick(void* _params) {
  PROFILE__BEGIN(PLAYER_ENTITY__TICK);
  OnEntityParams* params = _params;
  Entity* entity = params->entity;
  Player* self = (Player*)entity;

  if (self->input.key.use && !self->input.mouseCaptured) {
    self->input.key.use = false;
    LOG_DEBUGF("request mouse lock");
    g_engine->sapp_lock_mouse(true);
  }
  if (self->input.key.esc && self->input.mouseCaptured) {
    self->input.key.esc = false;
    LOG_DEBUGF("request mouse unlock");
    g_engine->sapp_lock_mouse(false);
  }
  bool ml = g_engine->sapp_mouse_locked();
  if (ml != self->input.mouseCaptured) {
    if (ml) LOG_DEBUGF("response mouse locked");
    if (!ml) LOG_DEBUGF("response mouse unlocked");
  }
  self->input.mouseCaptured = ml;

  if (!self->input.mouseCaptured) {
    self->input.ptr.x = 0;
    self->input.ptr.y = 0;
  } else {
    if (0 != self->input.ptr.x) {  // yaw (rotate around Y-axis)
      self->base.base.tform->rot3.y += self->input.ptr.x * PLAYER_LOOK_SPEED;
      self->base.base.tform->rot3.y =
          Math__wrapaf(0, self->base.base.tform->rot3.y, 360.0f, 360.0f);
      q_fromEuler(&self->base.base.tform->rot4, &self->base.base.tform->rot3);
      self->input.ptr.x = 0;
    }

    if (0 != self->input.ptr.y) {  // pitch (rotate around X-axis)
      self->base.base.tform->rot3.x += self->input.ptr.y * PLAYER_LOOK_SPEED;
      self->base.base.tform->rot3.x =
          Math__wrapaf(-55.0f, self->base.base.tform->rot3.x, 55.0f, 55.0f);
      q_fromEuler(&self->base.base.tform->rot4, &self->base.base.tform->rot3);
      self->input.ptr.y = 0;
    }

    if (true == self->input.key.reload) {  // R
      self->input.key.reload = false;
      g_engine->game->level->spawner->firstTick = true;  // tp to spawn
    }

    // TODO: could implement player camera zoom (also from 1P to 3P)
    // if (0 != self->input.ptr.wheely) {
    //   self->base.base.tform->pos.z +=
    //       -self->input.ptr.wheely * PLAYER_ZOOM_SPEED /* deltaTime*/;
    //   self->input.ptr.wheely = 0;
    // }

    // W-S Forward/Backward axis
    if (self->input.key.fwd) {
      self->input.joy.zAxis = +1.0f;
    } else if (self->input.key.back) {
      self->input.joy.zAxis = -1.0f;
    } else {
      self->input.joy.zAxis = 0.0f;
    }

    // A-D Left/Right axis
    if (self->input.key.right) {
      self->input.joy.xAxis = +1.0f;
    } else if (self->input.key.left) {
      self->input.joy.xAxis = -1.0f;
    } else {
      self->input.joy.xAxis = 0.0f;
    }

    if (0 == self->input.joy.zAxis && 0 == self->input.joy.xAxis) {
      self->lastInput = 0;
    } else {
      if (0 == self->lastInput) self->lastInput = g_engine->now;
    }

    // Q-E Up/Down axis
    if (self->input.key.up) {
      self->input.joy.yAxis = +1.0f;  // +Y_UP
    } else if (self->input.key.down) {
      self->input.joy.yAxis = -1.0f;
    } else {
      self->input.joy.yAxis = 0.0f;
    }

    // Direction vectors for movement
    HMM_Vec3 forward, right, front;

    // Convert yaw to radians for direction calculation
    f32 yaw_radians = HMM_AngleDeg(entity->tform->rot3.y);
    q_fromEuler(&entity->tform->rot4, &entity->tform->rot3);

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
    entity->rb->velocity.x = 0;
    entity->rb->velocity.z = 0;
    if (0 != self->input.joy.zAxis) {
      forward = HMM_MulV3F(front, self->input.joy.zAxis * PLAYER_WALK_SPEED);
      pos = HMM_AddV3(p1, forward);
      entity->rb->velocity.x += forward.X;  // pos.X - entity->tform->pos.x;
      entity->rb->velocity.z += forward.Z;  // pos.Z - entity->tform->pos.z;
    }

    // apply left/right motion
    if (0 != self->input.joy.xAxis) {
      forward = HMM_MulV3F(right, self->input.joy.xAxis * PLAYER_WALK_SPEED * PLAYER_STRAFE_MOD);
      pos = HMM_AddV3(p1, forward);
      entity->rb->velocity.x += forward.X;  // pos.X - entity->tform->pos.x;
      entity->rb->velocity.z += forward.Z;  // pos.Z - entity->tform->pos.z;
    }

    // apply up/down motion
    if (0 != self->input.joy.yAxis) {
      entity->tform->pos.y += self->input.joy.yAxis * PLAYER_FLY_SPEED * g_engine->deltaTime;

      entity->tform->pos.y = Math__clamp(0, entity->tform->pos.y, g_engine->game->level->height);
    }

    static const f32 SQRT_TWO = 1.414214f;

    f32 xm = self->input.joy.xAxis;
    f32 zm = self->input.joy.zAxis;
    f32 d = Math__sqrtf(xm * xm + zm * zm);
    if (0 != self->input.joy.zAxis && 0 != self->input.joy.xAxis) {
      // normalize diagonal movement, so it is not faster
      // entity->rb->velocity.x /= SQRT_TWO;
      // entity->rb->velocity.z /= SQRT_TWO;
    }

    // headbob
    if (d != 0) {
      if (xm != 0 && zm != 0) {
        // normalize diagonal movement, so it is not faster
        d /= SQRT_TWO;
      }
      // d *= PLAYER_WALK_SPEED * g_engine->deltaTime * 4.0f;
      // d *=
      self->base.camera.bobPhase = HMM_SinF(
          (g_engine->now - self->lastInput) / 1000.0f * 2.0f * 3.0f * (60 / 29.0f));  // 29bpm
    }

    // LOG_DEBUGF(
    //     "Player move %f %f %f dT %f",
    //     entity->tform->pos.x,
    //     entity->tform->pos.y,
    //     entity->tform->pos.z,
    //     g_engine->deltaTime);
    Rigidbody2D__move(g_engine->game->level->qt, entity, Dispatcher__call);

    if (self->input.key.use) {
      self->input.key.use = false;

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
      List__append(g_engine->arena, g_engine->game->level->entities, rs);

      Rect range = {pos.X, pos.Z, 0.5f, 0.5f};
      QuadTreeNode_query(g_engine->game->level->qt, range, 100, matchData, &matchCount);

      for (u32 i = 0; i < matchCount; i++) {
        Entity* other = (Entity*)matchData[i];
        if (entity == other) continue;

        if (0 != other->dispatch && 0 != other->dispatch->action &&
            HMM_ABS(entity->tform->pos.y - other->tform->pos.y) < 2.0f) {
          Dispatcher__call(
              other->dispatch->action,
              &(OnActionParams){.type = ACTION_USE, .actor = entity, .target = other});
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