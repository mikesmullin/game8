#pragma once

#include "../../engine/common/Types.h"

typedef struct Entity Entity;

void DebugText__init(Entity* entity, f32 x, f32 y, u32 len, char* txt, u32 color);
void DebugText__tick(Entity* entity);