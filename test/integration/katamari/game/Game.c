#include "Game.h"

void Game__init() {
  mmemcp(g_engine->window_title, "Katamari", 9);
  g_engine->window_width = WINDOW_SIZE;
  g_engine->window_height = WINDOW_SIZE;

  g_engine->game->w = g_engine->game->d = g_engine->game->h = 100.0f;

  g_engine->players = List__alloc(g_engine->arena);
  g_engine->audio = Arena__Push(g_engine->arena, sizeof(PreloadedAudio));
  g_engine->models = Arena__Push(g_engine->arena, sizeof(PreloadedModels));
  g_engine->textures = Arena__Push(g_engine->arena, sizeof(PreloadedTextures));
  g_engine->textures->albedo = 0;
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

  g_engine->game->entities = List__alloc(g_engine->arena);
}

void Game__preload() {
  Player* player1 = (Player*)Arena__Push(g_engine->arena, sizeof(Player));
  Player__init((Entity*)player1);
  player1->base.base.tform->pos.x = 0.0f;
  player1->base.base.tform->pos.y = 3.0f;
  player1->base.base.tform->pos.z = 10.0f;
  player1->base.camera.proj.type = PERSPECTIVE_PROJECTION;
  player1->base.camera.proj.fov = 45.0f;
  player1->base.camera.proj.nearZ = 0.1f;
  player1->base.camera.proj.farZ = 1000.0f;
  player1->base.camera.screenSize = SCREEN_SIZE;
  player1->base.camera.bobPhase = 0;
  List__append(g_engine->arena, g_engine->players, player1);

  Cube* cube = (Cube*)Arena__Push(g_engine->arena, sizeof(Cube));
  Cube__init((Entity*)cube);
  List__append(g_engine->arena, g_engine->game->entities, cube);
  cube->base.render->color = COLOR_RED;
  cube->base.tform->pos.y = 0.5f;
  player1->model = (Entity*)cube;

  u32 sq = 30;
  f32 s = 1.0f / 4, ss = s * 4;
  f32 hw = sq * ss / 2;
  for (u32 y = 0; y < sq; y++) {
    for (u32 x = 0; x < sq; x++) {
      Cube* cube = (Cube*)Arena__Push(g_engine->arena, sizeof(Cube));
      Cube__init((Entity*)cube);
      List__append(g_engine->arena, g_engine->game->entities, cube);
      cube->base.tform->scale.x = s;
      cube->base.tform->scale.y = s;
      cube->base.tform->scale.z = s;
      cube->base.tform->pos.x = -hw + (x * ss), cube->base.tform->pos.z = -hw + (y * ss);
      cube->base.tform->pos.y = s / 2;
      cube->base.render->color = COLOR_WHITE;
    }
  }

  // preload assets
  g_engine->audio->pickupCoin = Wav__Read("../assets/audio/sfx/pickupCoin.wav");

  // init network
  g_engine->game->net = Arena__Push(g_engine->arena, sizeof(NetMgr));
  NetMgr__init();
}

void Game__reload() {
  Audio__reload();
}

void Game__tick() {
  Arena__Reset(g_engine->frameArena);
  g_engine->entity_count = g_engine->game->entities->len;
  // CameraEntity* player1 = g_engine->players->head->data;

  NetMgr__tick();

  Player__tick(g_engine->players->head->data);

  Rect boundary = {0.0f, 0.0f, g_engine->game->w, g_engine->game->d};
  g_engine->game->qt = QuadTreeNode_create(g_engine->frameArena, boundary);

  if (g_engine->audio->pickupCoin->loaded && !g_engine->game->playedSfxOnce) {
    g_engine->game->playedSfxOnce = true;
    Audio__replay(g_engine->audio->pickupCoin);
  }
  if (g_engine->game->playedSfxOnce && NULL == g_engine->aSrc && !g_engine->game->testComplete) {
    g_engine->game->testComplete = true;
    // audio playback completed
    // g_engine->sapp_request_quit();
  }
}

void Game__render() {
  CameraEntity* player1 = (CameraEntity*)g_engine->players->head->data;
  // player1->base.tform->pos.x = 0.0f;  // mwave(1000, -5.0f, 5.0f);
  // player1->base.tform->pos.y = mwave(3000, 0.0f, 4.0f);
  // player1->base.tform->pos.z = mwave(7000, 2.0f, 10.0f);
  // player1->base.tform->rot.x = mwave(11000, -15.0f, 15.0f);
  // player1->base.tform->rot.y = mwave(11000, -15.0f, 15.0f);

  Cube* cube = g_engine->game->entities->head->data;
  // cube->base.tform->rot.x = mwave(1000, 0.0f, 180.0f);
  // cube->base.tform->rot.y = mwave(3000, 0.0f, 180.0f);
  // cube->base.tform->rot.z = mwave(7000, 0.0f, 180.0f);

  MeshRenderer__renderBatches(g_engine->game->entities, Dispatcher__call);
}

void Game__gui() {
}

void Game__postprocessing() {
}

void Game__shutdown() {
  NetMgr__shutdown();
}