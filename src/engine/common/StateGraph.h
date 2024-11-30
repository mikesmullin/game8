#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef enum ActionType {
  ACTION_NONE,  //
  ACTION_USE,
} ActionType;

typedef struct OnActionParams {
  ActionType type;
  Entity *actor, *target;
} OnActionParams;

typedef enum SGStateTags1 /* : u64 */ {
  SGST_NONE = 0,  //
  SGST_BUSY = 1 << 1,
} SGStateTags1;

typedef struct StateGraph StateGraph;
typedef struct SGState SGState;
typedef void (*SGStateFn)(StateGraph* sg);
typedef void (*SGActionFn)(StateGraph* sg, OnActionParams* action);
typedef SGState* (*SGGetStateFn)(u32 id);

typedef struct SGStateKeyframe {
  u32 id;
  SGStateFn cb;
} SGStateKeyframe;

typedef struct SGState {
  SGStateFn onEnter;
  SGStateFn onUpdate;
  SGStateFn onExit;
  u32 frameCount;
  u32 keyframeCount;
  SGStateKeyframe* keyframes;
  // eventListeners[];
} SGState;

typedef enum SGFSM_t {
  SGFSM_NULL,  //
  SGFSM_ENTERING,
  SGFSM_UPDATING,
  SGFSM_EXITING,
} SGFSM;

typedef struct StateGraph {
  Entity* entity;
  u32 currentState;
  SGFSM fsm;
  u32 frame;
  // SGActionFn actions;
  EventEmitter events;
  u64 tags1;  // SGStateTags1
} StateGraph;

void StateGraph__gotoState(StateGraph* sg, u32 state, SGGetStateFn getCb);
void StateGraph__tick(StateGraph* sg, SGGetStateFn getCb);
void StateGraph__addTag(StateGraph* sg, u64 tag);
void StateGraph__removeTag(StateGraph* sg, u64 tag);
bool StateGraph__hasTag(StateGraph* sg, u64 tag);
