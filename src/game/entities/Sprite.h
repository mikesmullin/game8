#pragma once

typedef float f32;
typedef struct Entity Entity;
typedef struct Sprite Sprite;

Sprite* Sprite__alloc();
void Sprite__init(Entity* entity, f32 x, f32 z);
void Sprite__render(Entity* entity);