#pragma once

#include <stdint.h>
typedef uint32_t u32;

typedef struct Arena Arena;
typedef struct Entity Entity;
typedef struct SGState SGState;
typedef struct Action Action;

void CatEntity__init(Entity* entity);
void CatEntity__tick(Entity* entity);
void CatEntity__collide(Entity* entity, void* params);
SGState* CatEntity__getSGState(u32 id);
void CatEntity__action(Entity* entity, void* action);