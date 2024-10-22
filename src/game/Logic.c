#include "Logic.h"

#include "../../vendor/sokol/sokol_fetch.h"
#include "../../vendor/sokol/sokol_gfx.h"
#include "Game.h"
#include "common/Arena.h"
#include "common/Audio.h"
#include "common/Log.h"

#ifdef __EMSCRIPTEN__
#define LOGIC_DECL
#else
#define LOGIC_DECL __declspec(dllexport)
#endif

Engine__State* g_engine;

// on init (data only)
LOGIC_DECL void logic_oninit(Engine__State* state) {
  g_engine = state;

  // NOTICE: logging won't work in here

  Arena__Alloc(&g_engine->arena, 1024 * 1024 * 12);  // MB (16mb is max for emscripten + firefox)
  g_engine->logic = Arena__Push(g_engine->arena, sizeof(Logic__State));

  Audio__init();
  Game__init();
}

LOGIC_DECL void logic_onpreload(void) {
  Logic__State* logic = g_engine->logic;

  logic->pass_action = Arena__Push(g_engine->arena, sizeof(sg_pass_action));

  // sokol_time.h
  g_engine->stm_setup();

  // sokol_fetch.h
  g_engine->sfetch_setup(&(sfetch_desc_t){
      .logger.func = g_engine->slog_func,
  });

  // sokol_gfx.h
  g_engine->sg_setup(&(sg_desc){
      .environment = g_engine->sglue_environment(),  //
      .logger.func = g_engine->slog_func,  //
  });

  Audio__preload();
  Game__preload();
}

LOGIC_DECL void logic_onreload(Engine__State* state) {
  g_engine = state;
  LOG_DEBUGF("Logic dll reloaded.");

  Audio__reload();
  Game__reload();
}

// on physics
LOGIC_DECL void logic_onfixedupdate(void) {
}

// on draw
LOGIC_DECL void logic_onupdate(void) {
  Logic__State* logic = g_engine->logic;

  g_engine->sfetch_dowork();

  // a pass action to clear framebuffer to black
  (*logic->pass_action) = (sg_pass_action){
      .colors[0] = {
          .load_action = SG_LOADACTION_CLEAR,  //
          .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},  //
      }};

  g_engine->sg_begin_pass(
      &(sg_pass){.action = *logic->pass_action, .swapchain = g_engine->sglue_swapchain()});

  // TODO: separate tick from render
  Game__tick();
  Game__render();
  Game__gui();

  g_engine->sg_end_pass();

  g_engine->sg_commit();
}

LOGIC_DECL void logic_onshutdown(void) {
  g_engine->sg_shutdown();
  Audio__shutdown();
  g_engine->sfetch_shutdown();
}