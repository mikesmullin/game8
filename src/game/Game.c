#include "Game.h"

#include "../engine/common/Script.h"
#include "../engine/common/String.h"

static void record(Arena* arena, const HashTable* vtable, List* stack) {
  Player* player1 = g_engine->players->head->data;
  String8* file = NULL;
  if (stack->len > 0) {
    Script__Token* t = List__pop(stack);
    if (TOKEN_CHAR_PTR != t->type) {
      LOG_DEBUGF("Error: record() expects 1st parameter to be a string value.");
      return;
    }
    file = String8__cstr(g_engine->arena, t->value);
  }
  GameInput__Demo__alloc(g_engine->arena, &player1->demo, file);
  if (GameInput__Demo__open(player1->demo, DEMO_RECORD)) {
    g_engine->console->show = false;  // auto-hide
    LOG_DEBUGF("Recording to %s", player1->demo->file->str);
  }
}

static void stop(Arena* arena, const HashTable* vtable, List* stack) {
  Player* player1 = g_engine->players->head->data;
  if (  //
      GameInput__Demo__isRecording(player1->demo) ||  //
      GameInput__Demo__isPlaying(player1->demo)  //
  ) {
    GameInput__Demo__close(player1->demo);
    LOG_DEBUGF("Stopped demo. file: %s", player1->demo->file->str);
  } else {
    LOG_DEBUGF("Error: No demo loaded.");
  }
}

static void playback(Arena* arena, const HashTable* vtable, List* stack) {
  Player* player1 = g_engine->players->head->data;
  String8* file = NULL;
  if (stack->len < 1) {
    LOG_DEBUGF("Error: playback() requires 1 parameter: filename.");
    return;
  }
  Script__Token* t = List__pop(stack);
  if (TOKEN_CHAR_PTR != t->type) {
    LOG_DEBUGF("Error: playback() expects 1st parameter to be a string value.");
    return;
  }
  file = String8__cstr(g_engine->arena, t->value);
  GameInput__Demo__alloc(g_engine->arena, &player1->demo, file);
  if (GameInput__Demo__open(player1->demo, DEMO_PLAY)) {
    g_engine->console->show = false;  // auto-hide
    LOG_DEBUGF("Playing demo %s", player1->demo->file->str);
  }
}

static void thanos(Arena* arena, const HashTable* vtable, List* stack) {
  // delete all the odd cats
  // TODO: free arena memory, as well
  u32 count = 0;

  List* l1 = List__alloc(g_engine->frameArena);
  List__Node* c = g_engine->game->level->entities->head;
  for (u32 i = 0; i < g_engine->game->level->entities->len; i++) {
    Entity* e = c->data;
    if (BitFlag__hasOne64(e->tags1, TAG_CAT)) {
      if (0 == count % 2) {
        List__append(g_engine->frameArena, l1, c);
      }
      count++;
    }
    c = c->next;
  }

  c = l1->head;
  for (u32 i = 0; i < l1->len; i++) {
    List__remove(g_engine->game->level->entities, c->data);
    c = c->next;
  }
}

void Game__init() {
  memcpy(g_engine->window_title, "Retro", 6);
  g_engine->window_width = WINDOW_SIZE;
  g_engine->window_height = WINDOW_SIZE;

  g_engine->players = List__alloc(g_engine->arena);
  g_engine->audio = Arena__Push(g_engine->arena, sizeof(PreloadedAudio));
  g_engine->models = Arena__Push(g_engine->arena, sizeof(PreloadedModels));
  g_engine->textures = Arena__Push(g_engine->arena, sizeof(PreloadedTextures));
  g_engine->materials = Arena__Push(g_engine->arena, sizeof(PreloadedMaterials));
  g_engine->shaders = Arena__Push(g_engine->arena, sizeof(PreloadedShaders));
  g_engine->shaders->atlas = Arena__Push(g_engine->arena, sizeof(Shader));
  (*g_engine->shaders->atlas) = (Shader){
      ATLAS__ONRENDER_ALLOC,
      ATLAS__ONRENDER_LOAD,
      ATLAS__ONRENDER_ENTITY,
      ATLAS__ONRENDER_MATERIAL};
  g_engine->shaders->pbr = Arena__Push(g_engine->arena, sizeof(Shader));
  (*g_engine->shaders->pbr) = (Shader){
      PBR__ONRENDER_ALLOC,
      PBR__ONRENDER_LOAD,
      PBR__ONRENDER_ENTITY,
      PBR__ONRENDER_MATERIAL};
}

void Game__preload() {
  g_engine->game->ui_entities = List__alloc(g_engine->arena);

  Player* player = (Player*)Arena__Push(g_engine->arena, sizeof(Player));
  Player__init((Entity*)player);
  List__append(g_engine->arena, g_engine->players, player);

  Level* level = Level__alloc();
  Level__init(level);
  level->width = 300;
  level->depth = 300;
  level->height = 10;
  // DEBUG: UV ColorMap
  // level->wallTex = 0;
  // level->wallCol = COLOR_TRANSPARENT;
  level->wallTex = 5;
  level->wallCol = 0x66ff0000;
  // level->ceilCol = 0x77000022;  // blood red
  level->ceilCol = 0xaa000000;  // darken
  // level->levelFile = "../assets/textures/level0.bmp";  // DEBUG: single block
  level->levelFile = "../assets/textures/level1.bmp";
  g_engine->game->level = level;
  Level__preload(g_engine->game->level);

  level->cubemap = Arena__Push(g_engine->arena, sizeof(Entity));
  SkyBox__init(level->cubemap);

  g_engine->game->dt = Arena__Push(g_engine->arena, sizeof(DebugText));
  char txt[40] = "Hello world!";
  DebugText__init((Entity*)g_engine->game->dt, 0, 0, 40, txt, COLOR_WHITE);
  List__insort(g_engine->arena, g_engine->game->ui_entities, g_engine->game->dt, Level__zsort);

  // preload assets
  Preload__audio(
      &g_engine->audio->pickupCoin,  //
      "../assets/audio/sfx/pickupCoin.wav");

  // init network
  g_engine->game->net = Arena__Push(g_engine->arena, sizeof(NetMgr));
  NetMgr__init();

  // init console
  g_engine->console = Arena__Push(g_engine->arena, sizeof(Console));
  Console__init((Entity*)g_engine->console);
  HashTable__set(g_engine->arena, g_engine->console->vtable, "thanos", &thanos);
  HashTable__set(g_engine->arena, g_engine->console->vtable, "record", &record);
  HashTable__set(g_engine->arena, g_engine->console->vtable, "stop", &stop);
  HashTable__set(g_engine->arena, g_engine->console->vtable, "playback", &playback);
}

void Game__reload() {
  if (NULL == g_engine->game) return;  // first time will not be defined

  Audio__replay(g_engine->audio->pickupCoin);
}

void Game__tick() {
  Player* player1 = (Player*)g_engine->players->head->data;

  // in-game
  NetMgr__tick();
  Dispatcher__call(player1->base.base.dispatch->tick, &(OnEntityParams){(Entity*)player1});
  Level__tick(g_engine->game->level);
  Console__tick((Entity*)g_engine->console);
}

void Game__render() {
  // in-game
  Level__render(g_engine->game->level);

  // PostProcessing();
}

void Game__gui() {
  PROFILE__BEGIN(GAME__GUI);
  Player* player1 = (Player*)g_engine->players->head->data;

  // switch current camera to ortho cam at player pos
  player1->base.camera.proj.type = ORTHOGRAPHIC_PROJECTION;

  char* c = g_engine->game->dt->txt;
  memcpy(c, g_engine->console->render, g_engine->game->dt->glyphs->len);
  g_engine->game->dt->base.tform->pos.x = -77;
  g_engine->game->dt->base.tform->pos.y = -75;
  // TODO: fix text scaling on window resize
  g_engine->game->dt->base.tform->scale.x = g_engine->game->dt->base.tform->scale.y = 6;

  List__Node* node = g_engine->game->ui_entities->head;
  for (u32 i = 0; i < g_engine->game->ui_entities->len; i++) {
    Entity* entity = node->data;
    node = node->next;

    Dispatcher__call(entity->dispatch->tick, &(OnEntityParams){entity});

    if (0 == entity->render) continue;
    Dispatcher__call(entity->dispatch->gui, &(OnEntityParams){entity});
    Dispatcher__call(entity->dispatch->render, &(OnEntityParams){entity});
  }

  MeshRenderer__renderBatches(g_engine->game->ui_entities, Dispatcher__call);

  // switch current camera to perspective cam at player pos
  player1->base.camera.proj.type = PERSPECTIVE_PROJECTION;

  // // TODO: draw debug cursor
  // Bitmap__Set2DPixel(
  //     &logic->screen,
  //     logic->CANVAS_DEBUG_X,
  //     logic->CANVAS_DEBUG_Y,
  //     (u32)Math__randomNext() | 0xffff0000 + 0xff993399, &g_engine->seeds.nosync);

  PROFILE__END(GAME__GUI);
}

void Game__postprocessing() {
  Player* player1 = (Player*)g_engine->players->head->data;

  // apply pixelized post-processing effect
  player1->base.camera.proj.type = ORTHOGRAPHIC_PROJECTION;
  Sprite* screen = (Sprite*)g_engine->game->screen->head->data;
  // f32 w = g_engine->window_width, h = g_engine->window_height;
  // f32 sw = w / SCREEN_SIZE, sh = h / SCREEN_SIZE;
  // f32 u = Math__min(sw, sh);
  screen->base.tform->scale.x = screen->base.tform->scale.y = SCREEN_SIZE * 0.95f;
  screen->base.tform->rot3.x = 0;
  screen->base.tform->rot3.y = 0;
  screen->base.tform->rot3.z = 180;
  q_fromEuler(&screen->base.tform->rot4, &screen->base.tform->rot3);
  MeshRenderer__renderBatches(g_engine->game->screen, Dispatcher__call);
  player1->base.camera.proj.type = PERSPECTIVE_PROJECTION;
}

void Game__shutdown() {
  NetMgr__shutdown();
}