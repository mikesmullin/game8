#pragma once

typedef float f32;
typedef struct Entity Entity;

void RedWallBlock__init(Entity* entity, f32 x, f32 z);
void RedWallBlock__render(Entity* entity);
void RedWallBlock__action(Entity* entity, void* _action);