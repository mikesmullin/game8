#ifdef ENGINE__COMPILING_DLL
#define ENGINE__DLL
#endif
#include "Logic.h"

#include "Game.h"

// on init (data only)
static void logic_oninit(void) {
  // NOTICE: logging won't work in here

  Arena__Alloc(&g_engine->arena, 1024 * 1024 * 1000);
  g_engine->game = Arena__Push(g_engine->arena, sizeof(Game));

  // NOTICE: tune the size of this to fit anticipated max entity count (ie. adjust for load tests)
  g_engine->frameArena = Arena__SubAlloc(g_engine->arena, 1024 * 1024 * 500);  // MB

  Audio__init();
  Game__init();
}

static void logic_onpreload(void) {
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
  g_engine->sg_enable_frame_stats();

  // multi-pass rendering
  sg_pass_action clear = {
      .colors[0] =
          {
              .load_action = SG_LOADACTION_CLEAR,  // always clear
              .clear_value = {0.1f, 0.1f, 0.1f, 1.0f},  // black
          },
  };

  g_engine->game->pass1 = Arena__Push(g_engine->arena, sizeof(sg_pass));
  (*g_engine->game->pass1) = (sg_pass){
      .action = clear,  // copy
      .label = "pass1",
  };

  // preload assets
  Audio__preload();
  Game__preload();
}

// window, keyboard, mouse events
void logic_onevent(const sapp_event* event) {
  // logic->player->input.fwd = false;
  // logic->player->input.left = false;
  // logic->player->input.back = false;
  // logic->player->input.right = false;
  // logic->player->input.use = false;
  // logic->player->input.up = false;
  // logic->player->input.down = false;
  // logic->player->input.reload = false;
  // logic->player->input.esc = false;
  // logic->player->input.use = false;

  // logic->player->ptr.wheely = 0;
  // logic->player->ptr.x = 0;
  // logic->player->ptr.y = 0;
}

// on physics
static void logic_onfixedupdate(void) {
  g_engine->sfetch_dowork();

  Game__tick();
}

// on draw
static void logic_onupdate(void) {
  // 1st pass
  g_engine->game->pass1->swapchain = g_engine->sglue_swapchain();
  g_engine->sg_begin_pass(g_engine->game->pass1);
  Game__render();
  g_engine->sg_end_pass();

  g_engine->sg_commit();

  sg_frame_stats stats = g_engine->sg_query_frame_stats();
  g_engine->draw_count = stats.num_draw;
}

static void logic_onshutdown(void) {
  Game__shutdown();
  g_engine->sg_shutdown();
  Audio__shutdown();
  g_engine->sfetch_shutdown();
  exit(0);
}

void logic_onbootstrap(Engine__State* engine) {
  // NOTICE: logging won't work in here (first time)

  g_engine = engine;
  g_engine->onbootstrap = logic_onbootstrap;
  g_engine->oninit = logic_oninit;
  g_engine->onpreload = logic_onpreload;
  g_engine->onevent = logic_onevent;
  g_engine->onfixedupdate = logic_onfixedupdate;
  g_engine->onupdate = logic_onupdate;
  g_engine->onshutdown = logic_onshutdown;

  if (NULL != g_engine->game) LOG_DEBUGF("Logic dll reloaded.");

  Game__reload();
}