#pragma once

typedef float f32;
typedef struct Entity Entity;

void DebugText__init(Entity* entity, f32 x, f32 z);
void DebugText__render(Entity* entity);