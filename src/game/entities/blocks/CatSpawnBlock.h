#pragma once

#include "../../../engine/common/Types.h"

typedef struct Entity Entity;

void CatSpawnBlock__init(Entity* block, f32 x, f32 y);
void CatSpawnBlock__tick(Entity* block);
