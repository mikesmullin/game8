#define ENGINE__MAIN
#include "game/Logic.h"

sapp_desc sokol_main(int argc, char* argv[]) {
  g_engine->onbootstrap = logic_onbootstrap;

  return Engine__sokol_main(argc, argv);
}