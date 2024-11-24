#pragma once

#include "../Game.h"  // IWYU pragma: keep

typedef struct Player {
  CameraEntity base;
  u32 lastInput;
  GameInputComponent input;
} Player;

void Player__init(Entity* entity);
void Player__tick(void* entity);