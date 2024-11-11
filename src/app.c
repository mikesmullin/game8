#define ENGINE__MAIN
#define ENGINE__HOT_RELOAD
#include "engine/Engine.h"

sapp_desc sokol_main(int argc, char* argv[]) {
  // use first (and only) engine
  g_engine = &engines[0];

  // bootstrap Logic.c.dll
  return Engine__sokol_main(argc, argv);
}