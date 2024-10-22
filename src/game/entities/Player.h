#pragma once

typedef float f32;
typedef struct Player Player;

static const f32 PLAYER_BOB = 0.05f;
static const f32 PLAYER_HURT_ANIM_TIME = 0.33;

Player* Player__alloc();
void Player__init(Player* self);
void Player__render(Player* self);
void Player__tick(Player* self);