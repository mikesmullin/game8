#pragma once

typedef float f32;
typedef struct Block Block;
typedef struct Entity Entity;

Block* WallBlock__alloc();
void WallBlock__init(Entity* block, f32 x, f32 y);
void WallBlock__render(Entity* block);