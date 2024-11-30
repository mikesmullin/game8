#pragma once

#include "../Game.h"  // IWYU pragma: keep

typedef struct RubbleSprite {
  Sprite base;
  f32 xa, ya, za;
  f32 life, lifeSpan;
} RubbleSprite;

void RubbleSprite__init(Entity* entity);
void RubbleSprite__tick(void* _params);