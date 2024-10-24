#pragma once

typedef float f32;

typedef struct CatSpawnBlock CatSpawnBlock;
typedef struct Entity Entity;

CatSpawnBlock* CatSpawnBlock__alloc();
void CatSpawnBlock__init(Entity* block, f32 x, f32 y);
void CatSpawnBlock__gui(Entity* block);
void CatSpawnBlock__tick(Entity* block);
