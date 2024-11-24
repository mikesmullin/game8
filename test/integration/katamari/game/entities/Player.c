#include "Player.h"

static const f32 PLAYER_WALK_SPEED = 5.0f;  // per-second
static const f32 PLAYER_STRAFE_MOD = 0.5f;  // percent of walk
static const f32 PLAYER_FLY_SPEED = 3.0f;  // per-second
static const f32 PLAYER_LOOK_SPEED = 0.3f;  // deg/sec

static const f32 PLAYER_MOVE_FORCE = 0.50f;

void Player__init(Entity* entity) {
  Player* self = (Player*)entity;

  Entity__init(entity);

  entity->dispatch->tick = PLAYER__TICK;

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

  entity->hear = Arena__Push(g_engine->arena, sizeof(AudioListenerComponent));
}

static void ApplyRollingPhysics(Player* player) {
  Cube* cube = (Cube*)player->model;
  Rigidbody2DComponent* rb = cube->base.rb;

  v3 scaled;

  // Normalize joystick input to a direction
  v3 forceDirection = {player->input.joy.xAxis, 0.0f, -player->input.joy.zAxis};

  if (glms_v3_len(forceDirection) > 0.01f) {
    glms_v3_normalize(&forceDirection);

    // Apply force to the cube to start rolling
    v3 appliedForce;
    glms_v3_scale(
        forceDirection,
        PLAYER_MOVE_FORCE * rb->mass,
        &appliedForce);  // Scale by mass for realistic acceleration
    glms_v3_scale(appliedForce, g_engine->deltaTime, &scaled);
    glms_v3_add(rb->velocity, scaled, &rb->velocity);

    // Simulate angular velocity (rolling torque)
    v3 rollingTorque;
    glms_v3_cross(forceDirection, (v3){0.0f, 1.0f, 0.0f}, &rollingTorque);  // Assume Y is up
    glms_v3_scale(rollingTorque, g_engine->deltaTime, &scaled);
    glms_v3_add(rb->angularVelocity, scaled, &rb->angularVelocity);
  }

  // Apply friction to gradually slow down the cube
  glms_v3_scale(rb->velocity, 1.0f - cube->friction * g_engine->deltaTime, &rb->velocity);
  glms_v3_scale(
      rb->angularVelocity,
      1.0f - cube->friction * g_engine->deltaTime,
      &rb->angularVelocity);

  // Integrate velocity into position
  glms_v3_scale(rb->velocity, 0.2f * g_engine->deltaTime, &scaled);
  glms_v3_add(cube->base.tform->pos, scaled, &cube->base.tform->pos);

  // camera follows cube
  player->base.base.tform->pos.x = cube->base.tform->pos.x;
  player->base.base.tform->pos.y = cube->base.tform->pos.y + 3.0f;
  player->base.base.tform->pos.z = cube->base.tform->pos.z + 10.0f;

  // Integrate angular velocity into rotation
  v4 rotationDelta;
  glms_q_fromAxis(
      rb->angularVelocity,
      glms_v3_len(rb->angularVelocity) * g_engine->deltaTime,
      &rotationDelta);

  glms_q_mul(cube->base.tform->rot4, rotationDelta, &cube->base.tform->rot4);

  // LOG_DEBUGF(
  //     "Player Cube move %f %f %f rot %f %f %f %f",
  //     cube->base.tform->pos.x,
  //     cube->base.tform->pos.y,
  //     cube->base.tform->pos.z,

  //     cube->base.tform->rot4.x,
  //     cube->base.tform->rot4.y,
  //     cube->base.tform->rot4.z,
  //     cube->base.tform->rot4.w);
}

void Player__tick(void* _entity) {
  PROFILE__BEGIN(PLAYER__TICK);
  Entity* entity = (Entity*)_entity;
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
      self->base.base.tform->rot3.y = Math__rclampf(0, self->base.base.tform->rot3.y, 360.0f);
      self->input.ptr.x = 0;
    }

    if (0 != self->input.ptr.y) {  // pitch (rotate around X-axis)
      self->base.base.tform->rot3.x += self->input.ptr.y * PLAYER_LOOK_SPEED;
      self->base.base.tform->rot3.x = Math__clamp(-55.0f, self->base.base.tform->rot3.x, 55.0f);
      self->input.ptr.y = 0;
    }

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

    // apply up/down motion
    if (0 != self->input.joy.yAxis) {
      entity->tform->pos.y += self->input.joy.yAxis * PLAYER_FLY_SPEED * g_engine->deltaTime;

      entity->tform->pos.y = Math__clamp(0, entity->tform->pos.y, g_engine->game->h);
    }
  }

  ApplyRollingPhysics(self);

  Rigidbody2D__move(g_engine->game->qt, entity, Dispatcher__call);

  // LOG_DEBUGF(
  //     "Player move %f %f %f dT %f",
  //     self->model->tform->pos.x,
  //     self->model->tform->pos.y,
  //     self->model->tform->pos.z,
  //     g_engine->deltaTime);

  PROFILE__END(PLAYER__TICK);
}