#define ENGINE__MAIN
#include "../../../src/engine/Engine.h"
#include "game/Logic.h"

extern logic_oninit_t logic_oninit;
extern logic_onpreload_t logic_onpreload;
extern logic_onreload_t logic_onreload;
extern logic_onevent_t logic_onevent;
extern logic_onfixedupdate_t logic_onfixedupdate;
extern logic_onupdate_t logic_onupdate;
extern logic_onshutdown_t logic_onshutdown;

// @describe Gaffer Deterministic Lockstep Katamari demo
// see: https://gafferongames.com/post/deterministic_lockstep/
// @tag net
// @run -server
sapp_desc sokol_main(int argc, char* argv[]) {
  // use first (and only) engine
  g_engine = &engines[0];

  g_engine->logic_oninit = logic_oninit;
  g_engine->logic_onpreload = logic_onpreload;
  g_engine->logic_onreload = logic_onreload;
  g_engine->logic_onevent = logic_onevent;
  g_engine->logic_onfixedupdate = logic_onfixedupdate;
  g_engine->logic_onupdate = logic_onupdate;
  g_engine->logic_onshutdown = logic_onshutdown;

  return Engine__sokol_main(argc, argv);
}