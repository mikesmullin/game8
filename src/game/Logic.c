#ifdef ENGINE__COMPILING_DLL
#define ENGINE__DLL
#endif
#include "Logic.h"

#include "../engine/common/Audio.h"
#include "../engine/common/List.h"
#include "../engine/common/Profiler.h"
#include "Game.h"
#include "entities/Screen.h"

// on init (data only)
static void logic_oninit(void) {
  // NOTICE: logging won't work in here

  Arena__Alloc(&g_engine->arena, 1024 * 1024 * 3);
  g_engine->logic = Arena__Push(g_engine->arena, sizeof(Logic__State));

  Audio__init();
  Game__init();
}

static void logic_onpreload(void) {
  Logic__State* logic = g_engine->logic;

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
              .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},  // black
          },
  };
  sg_image_desc img_desc = {
      .render_target = true,
      .width = SCREEN_SIZE,
      .height = SCREEN_SIZE,
      .pixel_format = SG_PIXELFORMAT_RGBA8,
      .sample_count = 1,
      .label = "color-image"};
  sg_image color_img = g_engine->sg_make_image(&img_desc);
  img_desc.pixel_format = SG_PIXELFORMAT_DEPTH_STENCIL;
  img_desc.label = "depth-image";
  sg_image depth_img = g_engine->sg_make_image(&img_desc);

  logic->pass1 = Arena__Push(g_engine->arena, sizeof(sg_pass));
  (*logic->pass1) = (sg_pass){
      .action = clear,  // copy
      .attachments = g_engine->sg_make_attachments(&(sg_attachments_desc){
          .colors[0].image = color_img,
          .depth_stencil.image = depth_img,
          .label = "pass1-attachments",
      }),
      .label = "pass1",
  };

  logic->pass2 = Arena__Push(g_engine->arena, sizeof(sg_pass));
  (*logic->pass2) = (sg_pass){
      .action = clear,  // copy
      .label = "pass2",
  };

  // preload assets
  Audio__preload();
  Game__preload();

  Entity* screen = Arena__Push(g_engine->arena, sizeof(Sprite));
  Screen__init(screen, color_img.id);
  logic->screen = List__alloc(g_engine->arena);
  List__append(g_engine->arena, logic->screen, screen);
}

// window, keyboard, mouse events
static void logic_onevent(const sapp_event* event) {
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
static void logic_onfixedupdate(void) {
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
static void logic_onupdate(void) {
  Logic__State* logic = g_engine->logic;

  // 1st pass
  g_engine->sg_begin_pass(logic->pass1);
  Game__render();
  Game__gui();
  g_engine->sg_end_pass();

  // 2nd pass
  logic->pass2->swapchain = g_engine->sglue_swapchain();
  g_engine->sg_begin_pass(logic->pass2);
  Game__postprocessing();
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
  LOG_DEBUGF("Logic dll reloaded.");

  Audio__reload();
  Game__reload();
}