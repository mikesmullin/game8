#pragma once

#include "../../../engine/common/Types.h"
typedef struct Entity Entity;

void RedWallBlock__init(Entity* entity, f32 x, f32 z);
void RedWallBlock__render(Entity* entity);
void RedWallBlock__action(Entity* entity, void* _action);