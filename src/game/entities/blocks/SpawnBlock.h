#pragma once

#include "../../../engine/common/Types.h"
typedef struct Entity Entity;

void SpawnBlock__init(Entity* entity, f32 x, f32 z);
void SpawnBlock__tick(Entity* entity);