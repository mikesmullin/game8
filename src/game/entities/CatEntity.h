#pragma once

#include <stdint.h>
typedef uint32_t u32;

typedef struct Arena Arena;
typedef struct Entity Entity;
typedef struct CatEntity CatEntity;
typedef struct Engine__State Engine__State;
typedef struct SGState SGState;
typedef struct StateGraph StateGraph;
typedef struct Action Action;

void CatEntity__init(Entity* entity);
void CatEntity__render(Entity* entity);
void CatEntity__gui(Entity* entity);
void CatEntity__tick(Entity* entity);
void CatEntity__collide(Entity* entity, void* params);
SGState* CatEntity__getSGState(u32 id);
void CatEntity__callSGAction(StateGraph* sg, Action* action);