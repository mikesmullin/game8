#pragma once

typedef float f32;

typedef struct Entity Entity;

void CatSpawnBlock__init(Entity* block, f32 x, f32 y);
void CatSpawnBlock__tick(Entity* block);
