#include "Logic.h"

// #include "../../../../src/game/common/Audio.h"
#include "Game.h"

// on init (data only)
void logic_oninit(Engine__State* state) {
  g_engine = state;

  // NOTICE: logging won't work in here

  Arena__Alloc(&g_engine->arena, 1024 * 1024 * 3);
  g_engine->logic = Arena__Push(g_engine->arena, sizeof(Logic__State));

  // Audio__init();
  Game__init();
}

void logic_onpreload(void) {
  // preload assets
  // Audio__preload();
  Game__preload();
}

// window, keyboard, mouse events
void logic_onevent(const sapp_event* event) {
  Logic__State* logic = g_engine->logic;

  logic->player->input.fwd = false;
  logic->player->input.left = false;
  logic->player->input.back = false;
  logic->player->input.right = false;
  logic->player->input.use = false;
  logic->player->input.up = false;
  logic->player->input.down = false;
  logic->player->input.reload = false;
  logic->player->input.esc = false;
  logic->player->input.use = false;

  logic->player->ptr.wheely = 0;
  logic->player->ptr.x = 0;
  logic->player->ptr.y = 0;
}

// on physics
void logic_onfixedupdate(void) {
  g_engine->sfetch_dowork();

  Game__tick();
}

// on draw
void logic_onupdate(void) {
  // 1st pass
  Game__render();
  Game__gui();

  // 2nd pass
  Game__postprocessing();
}

void logic_onshutdown(void) {
  Game__shutdown();
  // Audio__shutdown();
}