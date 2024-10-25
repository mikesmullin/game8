#pragma once

typedef float f32;
typedef struct Entity Entity;

void SpawnBlock__init(Entity* entity, f32 x, f32 z);
void SpawnBlock__tick(Entity* entity);