#pragma once

typedef float f32;
typedef struct Player Player;
typedef struct Entity Entity;

static const f32 PLAYER_BOB = 0.05f;
static const f32 PLAYER_HURT_ANIM_TIME = 0.33;

Player* Player__alloc();
void Player__init(Entity* entity);
void Player__render(Entity* entity);
void Player__tick(Entity* entity);