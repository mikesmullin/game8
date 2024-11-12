#define ENGINE__MAIN
#include "game/Logic.h"

int main(int argc, char* argv[]) {
  g_engine->onbootstrap = logic_onbootstrap;
  return Engine__main(argc, argv);
}