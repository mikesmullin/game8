#pragma once

typedef float f32;

typedef struct Block Block;

Block* Block__alloc();
void Block__init(Block* block, f32 x, f32 z);
