#pragma once

#include "../Game.h"  // IWYU pragma: keep

typedef struct Player {
  CameraEntity base;
  u32 lastInput;
  GameInputComponent input;
} Player;

static const f32 PLAYER_HURT_ANIM_TIME = 0.33;

void Player__init(Entity* entity);
void Player__render(Entity* entity);
void Player__tick(Entity* entity);