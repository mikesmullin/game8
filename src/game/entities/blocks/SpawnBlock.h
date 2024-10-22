#pragma once

typedef float f32;
typedef struct Block Block;

Block* SpawnBlock__alloc();
void SpawnBlock__init(Block* block, f32 x, f32 y);
void SpawnBlock__tick(Block* block);