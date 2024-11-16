#include "Game.h"

void Game__init() {
  memcpy(g_engine->window_title, "Retro", 6);
  g_engine->window_width = WINDOW_SIZE;
  g_engine->window_height = WINDOW_SIZE;

  g_engine->players = List__alloc(g_engine->arena);
  g_engine->audio = Arena__Push(g_engine->arena, sizeof(PreloadedAudio));
  g_engine->models = Arena__Push(g_engine->arena, sizeof(PreloadedModels));
  g_engine->textures = Arena__Push(g_engine->arena, sizeof(PreloadedTextures));
  g_engine->materials = Arena__Push(g_engine->arena, sizeof(PreloadedMaterials));
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
}

void Game__reload() {
  if (NULL == g_engine->game) return;  // first time will not be defined

  Audio__replay(g_engine->audio->pickupCoin);
}

void Game__tick() {
  Player* player1 = (Player*)g_engine->players->head->data;

  // in-game
  NetMgr__tick();
  Dispatcher__call1(player1->base.base.dispatch->tick, (Entity*)player1);
  Level__tick(g_engine->game->level);
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
  char c2[255];
  sprintf(
      c2,
      "cam x %+06.1f y %+06.1f z %+06.1f r %+06.1f",
      player1->base.base.tform->pos.x,
      player1->base.base.tform->pos.y,
      player1->base.base.tform->pos.z,
      player1->base.base.tform->rot.y);
  memcpy(c, c2, g_engine->game->dt->glyphs->len);
  // TODO: fix this fn (somehow only works on mousedown titlebar)
  // mprintf(
  //     &c,
  //     "cam x %+06.1f y %+06.1f z %+06.1f r %+06.1f",
  //     g_engine->game->dt->glyphs->len,
  //     player1->base.base.tform->pos.x,
  //     player1->base.base.tform->pos.y,
  //     player1->base.base.tform->pos.z,
  //     logic->player->base.tform->rot.y);
  g_engine->game->dt->base.tform->pos.x = -77;
  g_engine->game->dt->base.tform->pos.y = -75;
  // TODO: fix text scaling on window resize
  g_engine->game->dt->base.tform->scale.x = g_engine->game->dt->base.tform->scale.y = 6;

  List__Node* node = g_engine->game->ui_entities->head;
  for (u32 i = 0; i < g_engine->game->ui_entities->len; i++) {
    Entity* entity = node->data;
    node = node->next;

    Dispatcher__call1(entity->dispatch->tick, entity);

    if (0 == entity->render) continue;
    Dispatcher__call1(entity->dispatch->gui, entity);
    Dispatcher__call1(entity->dispatch->render, entity);
  }

  MeshRenderer__renderBatches(g_engine->game->ui_entities);

  // switch current camera to perspective cam at player pos
  player1->base.camera.proj.type = PERSPECTIVE_PROJECTION;

  // // TODO: draw debug cursor
  // Bitmap__Set2DPixel(
  //     &logic->screen,
  //     logic->CANVAS_DEBUG_X,
  //     logic->CANVAS_DEBUG_Y,
  //     Math__urandom() | 0xffff0000 + 0xff993399);

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
  screen->base.tform->rot.x = 0;
  screen->base.tform->rot.y = 0;
  screen->base.tform->rot.z = 180;
  MeshRenderer__renderBatches(g_engine->game->screen);
  player1->base.camera.proj.type = PERSPECTIVE_PROJECTION;
}

void Game__shutdown() {
  NetMgr__shutdown();
}