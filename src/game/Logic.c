#include "Logic.h"

#include "../lib/Arena.h"
#include "../lib/Log.h"

Engine__State_t* g_engine;

// on init (data only)
__declspec(dllexport) void logic_oninit_data(Engine__State_t* state) {
  state->local = Arena__Push(state->arena, sizeof(Logic__State_t));
  Logic__State_t* logic = state->local;

  logic->test1 = 2;
}

__declspec(dllexport) void logic_oninit_compute(Engine__State_t* state) {
  Logic__State_t* logic = state->local;
}

__declspec(dllexport) void logic_onreload(Engine__State_t* state) {
  g_engine = state;
  LOG_DEBUGF("Logic dll loaded state %p at %p", g_engine, &g_engine);
}

// on physics
__declspec(dllexport) void logic_onfixedupdate(Engine__State_t* state) {
  Logic__State_t* logic = state->local;
}

// on draw
__declspec(dllexport) void logic_onupdate(Engine__State_t* state) {
  Logic__State_t* logic = state->local;
}
