#pragma once

#include "../Game.h"  // IWYU pragma: keep

typedef struct DebugText {
  Entity base;
  char* txt;
  List* glyphs;
} DebugText;

void DebugText__init(Entity* entity, f32 x, f32 y, u32 len, char* txt, u32 color);
void DebugText__tick(Entity* entity);