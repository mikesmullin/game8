#include "Logic.h"

#include "../../vendor/sokol/sokol_app.h"
#include "../../vendor/sokol/sokol_fetch.h"
#include "../../vendor/sokol/sokol_gfx.h"
#include "Game.h"
#include "common/Arena.h"
#include "common/Audio.h"
#include "common/Log.h"
#include "common/Profiler.h"

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

  Arena__Alloc(&g_engine->arena, 1024 * 1024 * 11);  // MB (16mb is max for emscripten + firefox)
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

// window, keyboard, mouse events
LOGIC_DECL void logic_onevent(const sapp_event* event) {
  Logic__State* logic = g_engine->logic;

  // LOG_DEBUGF("event frame_count %llu", event->frame_count);

  if (SAPP_EVENTTYPE_KEY_DOWN == event->type) {
    if (KEYCODE_W == event->key_code) logic->player->input.fwd = true;
    if (KEYCODE_A == event->key_code) logic->player->input.left = true;
    if (KEYCODE_S == event->key_code) logic->player->input.back = true;
    if (KEYCODE_D == event->key_code) logic->player->input.right = true;
    if (KEYCODE_E == event->key_code) logic->player->input.use = true;
    if (KEYCODE_SPACE == event->key_code) logic->player->input.up = true;
    if (KEYCODE_TAB == event->key_code) logic->player->input.down = true;
    if (KEYCODE_R == event->key_code) logic->player->input.reload = true;
    if (KEYCODE_ESC == event->key_code) logic->player->input.esc = true;

    // LOG_DEBUGF(
    //     "event keydown"
    //     " char_code %u"
    //     " key_code %u"
    //     " key_repeat %u"
    //     " modifiers %u",
    //     event->char_code,
    //     event->key_code,
    //     event->key_repeat,
    //     event->modifiers);
  }
  if (SAPP_EVENTTYPE_KEY_UP == event->type) {
    if (KEYCODE_W == event->key_code) logic->player->input.fwd = false;
    if (KEYCODE_A == event->key_code) logic->player->input.left = false;
    if (KEYCODE_S == event->key_code) logic->player->input.back = false;
    if (KEYCODE_D == event->key_code) logic->player->input.right = false;
    if (KEYCODE_E == event->key_code) logic->player->input.use = false;
    if (KEYCODE_SPACE == event->key_code) logic->player->input.up = false;
    if (KEYCODE_TAB == event->key_code) logic->player->input.down = false;
    if (KEYCODE_R == event->key_code) logic->player->input.reload = false;
    if (KEYCODE_ESC == event->key_code) logic->player->input.esc = false;

    // LOG_DEBUGF(
    //     "event keyup"
    //     " char_code %u"
    //     " key_code %u"
    //     " key_repeat %u"
    //     " modifiers %u",
    //     event->char_code,
    //     event->key_code,
    //     event->key_repeat,
    //     event->modifiers);
  }

  // LOG_DEBUGF(
  //     "kbState "
  //     "w %u a %u s %u d %u e %u sp %u ctl %u "
  //     "r %u esc %u",
  //     logic->player->kb.fwd,
  //     logic->player->kb.left,
  //     logic->player->kb.back,
  //     logic->player->kb.right,
  //     logic->player->kb.use,
  //     logic->player->kb.up,
  //     logic->player->kb.down,
  //     logic->player->kb.reload,
  //     logic->player->kb.esc);

  if (SAPP_EVENTTYPE_MOUSE_DOWN == event->type) {
    if (SAPP_MOUSEBUTTON_LEFT == event->mouse_button) logic->player->input.use = true;

    // LOG_DEBUGF(
    //     "event mousedown"
    //     " mouse_button %u",
    //     event->mouse_button);
  }
  if (SAPP_EVENTTYPE_MOUSE_UP == event->type) {
    if (SAPP_MOUSEBUTTON_LEFT == event->mouse_button) logic->player->input.use = false;

    // LOG_DEBUGF(
    //     "event mouseup"
    //     " mouse_button %u",
    //     event->mouse_button);
  }
  if (SAPP_EVENTTYPE_MOUSE_SCROLL == event->type) {
    logic->player->ptr.wheely += event->scroll_y;

    // LOG_DEBUGF(
    //     "event mousescroll"
    //     " scroll_x %f"
    //     " scroll_y %f",
    //     event->scroll_x,
    //     event->scroll_y);
  }
  if (SAPP_EVENTTYPE_MOUSE_MOVE == event->type) {
    logic->player->ptr.x += event->mouse_dx;
    logic->player->ptr.y += event->mouse_dy;

    // LOG_DEBUGF(
    //     "event mousemove"
    //     " mouse_dx %f"
    //     " mouse_dy %f"
    //     " mouse_x %f"
    //     " mouse_y %f",
    //     event->mouse_dx,
    //     event->mouse_dy,
    //     event->mouse_x,
    //     event->mouse_y);
  }

  // switch (event->type) {
  //   case SAPP_EVENTTYPE_TOUCHES_BEGAN:
  //   case SAPP_EVENTTYPE_TOUCHES_MOVED:
  //   case SAPP_EVENTTYPE_TOUCHES_ENDED:
  //   case SAPP_EVENTTYPE_TOUCHES_CANCELLED:
  //     LOG_DEBUGF("event touch*. num_touches: %u", event->num_touches);
  //     break;
  // }

  // LOG_DEBUGF(
  //     "mState "
  //     "x %d y %d wheely %3.3f btn1 %u",
  //     logic->player->ptr.x,
  //     logic->player->ptr.y,
  //     logic->player->ptr.wheely,
  //     logic->player->ptr.btn1);

  switch (event->type) {
    case SAPP_EVENTTYPE_FOCUSED:
      LOG_DEBUGF("event focused");
      break;
    case SAPP_EVENTTYPE_UNFOCUSED:
      LOG_DEBUGF("event unfocused");
      break;
    case SAPP_EVENTTYPE_RESIZED:
      LOG_DEBUGF("event resized %u %u", event->window_width, event->window_height);
      g_engine->window_width = event->window_width;
      g_engine->window_height = event->window_height;
      break;
    case SAPP_EVENTTYPE_ICONIFIED:
      LOG_DEBUGF("event iconified/minimized");
      break;
    case SAPP_EVENTTYPE_RESTORED:
      LOG_DEBUGF("event restored");
      break;
    case SAPP_EVENTTYPE_SUSPENDED:
      LOG_DEBUGF("event suspended");
      break;
    case SAPP_EVENTTYPE_RESUMED:
      LOG_DEBUGF("event resumed");
      break;
    case SAPP_EVENTTYPE_QUIT_REQUESTED:
      LOG_DEBUGF("event quit_requested");
      break;
    default:
      break;
  }
}

// on physics
LOGIC_DECL void logic_onfixedupdate(void) {
  Logic__State* logic = g_engine->logic;

  g_engine->sfetch_dowork();

  Game__tick();

  static f32 counter = 0, wait = 5.0f;
  counter += g_engine->deltaTime;
  if (counter > wait) {
    counter -= wait;
    PROFILE__PRINT();
  }
}

// on draw
LOGIC_DECL void logic_onupdate(void) {
  Logic__State* logic = g_engine->logic;

  // a pass action to clear framebuffer to black
  (*logic->pass_action) = (sg_pass_action){
      .colors[0] = {
          .load_action = SG_LOADACTION_CLEAR,  //
          .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},  //
      }};

  g_engine->sg_begin_pass(
      &(sg_pass){.action = *logic->pass_action, .swapchain = g_engine->sglue_swapchain()});

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