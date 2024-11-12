#define ENGINE__MAIN
#include "game/Logic.h"

// @describe Gaffer Deterministic Lockstep Katamari demo
// see: https://gafferongames.com/post/deterministic_lockstep/
// @tag net
// @run -server
sapp_desc sokol_main(int argc, char* argv[]) {
  g_engine->onbootstrap = logic_onbootstrap;

  return Engine__sokol_main(argc, argv);
}