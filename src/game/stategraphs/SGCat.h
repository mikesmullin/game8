#pragma once

#include "../../engine/common/StateGraph.h"
#include "../entities/CatEntity.h"

// cat indexed palette
// 1 unused
// 2 spots
// 3 r eye
// 4 l eye
// 5 back
// 6 back foot
// 7 body
// 8 belly

static void subbedActions(StateGraph* sg, OnActionParams* action) {
  if (ACTION_USE == action->type) {
    CatEntity* self = (CatEntity*)sg->entity;
    self->interactingPlayer = (Entity*)action->actor;
    StateGraph__gotoState(sg, 3, CatEntity__getSGState);  // meow
  }
}

static void idleOnEnter(StateGraph* sg) {
  StateGraph__gotoState(sg, 1, CatEntity__getSGState);  // tail
}
static SGState SGidle = {
    .onEnter = idleOnEnter,
};

static void tailOnEnter(StateGraph* sg) {
  // StateGraph__addTag(sg, SGST_BUSY);
}
static void tailKF1(StateGraph* sg) {
  CatEntity* self = (CatEntity*)sg->entity;
  sg->entity->render->ti = 2 * 8 + 2;
}
static void tailKF2(StateGraph* sg) {
  sg->entity->render->ti = 2 * 8 + 3;
}
static void tailKF3(StateGraph* sg) {
  sg->entity->render->ti = 2 * 8 + 4;
}
static void tailKF4(StateGraph* sg) {
  sg->entity->render->ti = 2 * 8 + 5;
}
static void tailKF5(StateGraph* sg) {
  sg->entity->render->ti = 2 * 8 + 6;
}
static void tailKF6(StateGraph* sg) {
  if (Math__randomf(0, 10, &g_engine->seeds.sg) < 1)
    StateGraph__gotoState(sg, 2, CatEntity__getSGState);  // blink
}
static SGState SGtail = {
    .onEnter = tailOnEnter,
    .frameCount = 32,
    .keyframeCount = 6,
    .keyframes =
        (SGStateKeyframe[]){
            {0, tailKF1},  //
            {6, tailKF2},
            {12, tailKF3},
            {18, tailKF4},
            {24, tailKF5},
            {31, tailKF6},
        },
};

static void blinkKF1(StateGraph* sg) {  // b eyes open
  sg->entity->render->ti = 2 * 8 + 3;
}
// static void blinkKF2(StateGraph* sg) {  // l eye closed
//   sg->entity->render->ti = 3 * 8 + 1;
// }
// static void blinkKF3(StateGraph* sg) {  // r eye closed
//   sg->entity->render->ti = 3 * 8 + 2;
// }
static void blinkKF4(StateGraph* sg) {  // b eyes closed
  sg->entity->render->ti = 3 * 8 + 3;
}
static void blinkKF5(StateGraph* sg) {
  StateGraph__gotoState(
      sg,
      0,  //(u32)Math__randomf(0, 10, &g_engine->seeds.sg) < 1 ? 3 : 0, // meow or idle
      CatEntity__getSGState);
}

static SGState SGblink = {
    .frameCount = 12,
    .keyframeCount = 2,
    .keyframes =
        (SGStateKeyframe[]){
            // {12 * 0, blinkKF1},  //
            // {12 * 1, blinkKF2},
            // {12 * 2, blinkKF3},
            {0, blinkKF4},
            {11, blinkKF5},
        },
};

static void meowKF1(StateGraph* sg) {  // eyes open, mouth closed
  sg->entity->render->ti = 2 * 8 + 3;
}
static void meowKF2(StateGraph* sg) {  // eyes open, mouth open
  sg->entity->render->ti = 4 * 8 + 0;
}
static void meowKF3(StateGraph* sg) {  // eyes closed, mouth open
  CatEntity* self = (CatEntity*)sg->entity;
  sg->entity->render->ti = 4 * 8 + 3;
  AudioSource__play(g_engine->audio->meow, sg->entity, (Entity*)self->interactingPlayer);
}
static void meowKF4(StateGraph* sg) {
  StateGraph__gotoState(sg, 0, CatEntity__getSGState);  // idle
}

static SGState SGmeow = {
    .frameCount = 17,
    .keyframeCount = 4,
    .keyframes =
        (SGStateKeyframe[]){
            // {0, meowKF1},  // eyes open, mouth closed
            {0, meowKF3},  // eyes closed, mouth open
            {6, meowKF2},  // eyes open, mouth open (sound)
            {15, meowKF1},  // eyes open, mouth closed
            {16, meowKF4},
        },
};

// static SGState* states[] = {
//     &SGidle,  //
//     &SGtail,
// };