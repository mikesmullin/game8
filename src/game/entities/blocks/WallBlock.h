#pragma once

typedef float f32;
typedef struct Entity Entity;

void WallBlock__init(Entity* entity, f32 x, f32 z);
void WallBlock__render(Entity* entity);