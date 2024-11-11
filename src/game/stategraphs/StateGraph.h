#pragma once

#include "../../engine/common/Types.h"

typedef struct StateGraph StateGraph;

void StateGraph__gotoState(StateGraph* sg, u32 state);
void StateGraph__tick(StateGraph* sg);
void StateGraph__addTag(StateGraph* sg, u64 tag);
void StateGraph__removeTag(StateGraph* sg, u64 tag);
bool StateGraph__hasTag(StateGraph* sg, u64 tag);
