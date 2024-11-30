#pragma once

#include "../../Game.h"  // IWYU pragma: keep

typedef struct BreakBlock {
  Block base;
  StateGraph* sg;
} BreakBlock;

void BreakBlock__init(Entity* entity, f32 x, f32 y);
void BreakBlock__callSGAction(StateGraph* sg, OnActionParams* action);
void BreakBlock__action(void* _params);