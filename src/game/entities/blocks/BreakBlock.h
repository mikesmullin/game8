#pragma once

typedef float f32;
typedef struct Entity Entity;
typedef struct StateGraph StateGraph;
typedef struct Action Action;

void BreakBlock__init(Entity* entity, f32 x, f32 y);
void BreakBlock__render(Entity* entity);
void BreakBlock__tick(Entity* entity);
void BreakBlock__callSGAction(StateGraph* sg, Action* action);
void BreakBlock__action(Entity* entity, void* _action);