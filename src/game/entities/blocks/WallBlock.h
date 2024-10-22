#pragma once

typedef float f32;
typedef struct Block Block;

Block* WallBlock__alloc();
void WallBlock__init(Block* block, f32 x, f32 y);
void WallBlock__render(Block* block);