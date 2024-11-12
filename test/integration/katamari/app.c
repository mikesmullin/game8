#define ENGINE__MAIN
#ifdef ENGINE__COMPILE_HOT_RELOAD
#include "../../../src/engine/Engine.h"
#else
#include "game/Logic.h"
#endif

// @describe Gaffer Deterministic Lockstep Katamari demo
// see: https://gafferongames.com/post/deterministic_lockstep/
// @tag net
// @run -server
sapp_desc sokol_main(int argc, char* argv[]) {
  // use first (and only) engine
  g_engine = &engines[0];

#ifndef ENGINE__COMPILE_HOT_RELOAD
  g_engine->logic_oninit = logic_oninit;
  g_engine->logic_onpreload = logic_onpreload;
  // g_engine->logic_onreload = logic_onreload;
  g_engine->logic_onevent = logic_onevent;
  g_engine->logic_onfixedupdate = logic_onfixedupdate;
  g_engine->logic_onupdate = logic_onupdate;
  g_engine->logic_onshutdown = logic_onshutdown;
#endif

  return Engine__sokol_main(argc, argv);
}