#define ENGINE__MAIN
#include "game/Logic.h"

// @describe Gaffer Deterministic Lockstep Katamari demo
// see: https://gafferongames.com/post/deterministic_lockstep/
// @tag net
// @run -novideo -noaudio -noperf -server 127.0.0.1:9000
// @run -video -noaudio -connect 127.0.0.1:9000
// @run -video -noaudio -connect 127.0.0.1:9000
int main(int argc, char* argv[]) {
  g_engine->onbootstrap = logic_onbootstrap;
  return Engine__main(argc, argv);
}