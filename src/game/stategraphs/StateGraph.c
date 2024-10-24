#include "StateGraph.h"

// #include "../../lib/Log.h"
#include "../Logic.h"
#include "../common/Color.h"
#include "../entities/CatEntity.h"

void StateGraph__gotoState(StateGraph* sg, u32 state) {
  sg->fsm = SGFSM_EXITING;
  SGState* curState = CatEntity__getSGState(state);
  if (0 != curState->onExit) curState->onExit(sg);
  sg->currentState = state;
  sg->frame = 0;
}

void StateGraph__tick(StateGraph* sg) {
  SGState* curState;
  while (sg->fsm == SGFSM_EXITING || sg->fsm == SGFSM_NULL) {
    sg->fsm = SGFSM_ENTERING;
    curState = CatEntity__getSGState(sg->currentState);
    if (0 != curState->onEnter) curState->onEnter(sg);

    if (sg->fsm == SGFSM_UPDATING || sg->fsm == SGFSM_ENTERING) {
      sg->fsm = SGFSM_UPDATING;
      curState = CatEntity__getSGState(sg->currentState);
      if (0 != curState->onUpdate) curState->onUpdate(sg);
    }
  }

  curState = CatEntity__getSGState(sg->currentState);
  for (u32 i = 0; i < curState->keyframeCount; i++) {
    SGStateKeyframe* frame = &curState->keyframes[i];
    if (sg->frame == frame->id) {
      frame->cb(sg);
      break;
    }
  }
  if (sg->fsm == SGFSM_UPDATING || sg->fsm == SGFSM_ENTERING) {
    if (curState->frameCount > 0) {
      sg->frame = (sg->frame + 1) % curState->frameCount;
    }
  }
}

void StateGraph__addTag(StateGraph* sg, u64 tag) {
  sg->tags1 |= tag;
}

void StateGraph__removeTag(StateGraph* sg, u64 tag) {
  sg->tags1 &= ~(tag);
}

bool StateGraph__hasTag(StateGraph* sg, u64 tag) {
  return sg->tags1 & tag;  // NOTE: doesn't compare tag=0
}
