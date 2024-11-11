#define ENGINE__MAIN
#define ENGINE__COUNT (2)
#include "../../../src/engine/Engine.h"

static void common_app(Engine__State* engine);
static void common_app_pump(Engine__State* engine);

// @describe Gaffer Deterministic Lockstep Katamari demo
// see: https://gafferongames.com/post/deterministic_lockstep/
// @tag net
int main() {
  // spawn app1 -server
  engines[0].isMaster = true;
  common_app(&engines[0]);
  // spawn app2
  common_app(&engines[1]);

  while (!engines[0].quit && !engines[1].quit) {
    common_app_pump(&engines[0]);
    common_app_pump(&engines[1]);

    // TODO: wait for connect
    // TODO: macro player1 input
    // TODO: assert checksum game state on both apps (lockstep deterministic networking)
  }

  return 0;
}

static void common_app(Engine__State* engine) {
  g_engine = engine;

  Engine__sokol_main(0, NULL);
  g_engine->useVideo = false;
  g_engine->useAudio = false;
  g_engine->useInput = false;
  g_engine->useNet = false;

  Engine__sokol_init();
}

static void common_app_pump(Engine__State* engine) {
  g_engine = engine;

  Engine__sokol_frame();
}