#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct StateGraph StateGraph;
// typedef enum SGStateTags1 : u64 SGStateTags1;

void StateGraph__gotoState(StateGraph* sg, u32 state);
void StateGraph__tick(StateGraph* sg);
void StateGraph__addTag(StateGraph* sg, u64 tag);
void StateGraph__removeTag(StateGraph* sg, u64 tag);
bool StateGraph__hasTag(StateGraph* sg, u64 tag);
