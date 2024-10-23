#pragma once

typedef float f32;
typedef struct Entity Entity;
typedef struct SpawnBlock SpawnBlock;

SpawnBlock* SpawnBlock__alloc();
void SpawnBlock__init(Entity* entity, f32 x, f32 z);
void SpawnBlock__tick(Entity* entity);