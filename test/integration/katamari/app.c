#define ENGINE__MAIN
#include "game/Logic.h"

// @describe Gaffer Deterministic Lockstep Katamari demo
// see: https://gafferongames.com/post/deterministic_lockstep/
// @tag net
// @run -server -novideo
int main(int argc, char* argv[]) {
  g_engine->onbootstrap = logic_onbootstrap;
  return Engine__main(argc, argv);
}