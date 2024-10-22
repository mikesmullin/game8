#pragma once

typedef float f32;
typedef struct Entity Entity;
typedef struct Block Block;

Block* SpawnBlock__alloc();
void SpawnBlock__init(Entity* entity, f32 x, f32 y);
void SpawnBlock__tick(Entity* entity);