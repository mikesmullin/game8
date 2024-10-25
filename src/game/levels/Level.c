#include "Level.h"

#include "../../../vendor/HandmadeMath/HandmadeMath.h"
#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/Bmp.h"
#include "../common/Color.h"
#include "../common/Dispatcher.h"
#include "../common/Geometry.h"
#include "../common/List.h"
#include "../common/Log.h"
#include "../common/Math.h"
#include "../common/Preloader.h"
#include "../common/Profiler.h"
#include "../common/QuadTree.h"
#include "../entities/Sprite.h"
#include "../entities/blocks/BreakBlock.h"
#include "../entities/blocks/CatSpawnBlock.h"
#include "../entities/blocks/RedWallBlock.h"
#include "../entities/blocks/SpawnBlock.h"
#include "../entities/blocks/WallBlock.h"

extern Engine__State* g_engine;

Level* Level__alloc() {
  Level* level = Arena__Push(g_engine->arena, sizeof(Level));
  level->bmp = NULL;
  level->world = NULL;

  level->frames = 2;

  for (u8 i = 0; i < level->frames; i++) {
    if (NULL == level->frameScratch[i]) {
      // TODO: tune the size of this; it is large for load testing only
      level->frameScratch[i] = Arena__SubAlloc(g_engine->arena, 1024 * 1024 * 5);  // MB
    }
  }
  level->entities = List__alloc(level->frameScratch[level->frame]);

  return level;
}

void Level__init(Level* level) {
  level->skybox = false;
  level->wallTex = 0;
  level->ceilTex = 1;
  level->floorTex = 2;
  level->wallCol = 0;
  level->ceilCol = 0;
  level->floorCol = 0;
  level->spawner = NULL;
}

static Entity* Level__makeEntity(u32 col, f32 x, f32 y) {
  if (0xff000000 == col) {  // black; empty space
    return NULL;
  } else if (0xffffffff == col) {  // white
    WallBlock* block = Arena__Push(g_engine->arena, sizeof(WallBlock));
    WallBlock__init((Entity*)block, x, y);
    return (Entity*)block;
  } else if (0xff00f2ff == col) {  // yellow
    SpawnBlock* block = Arena__Push(g_engine->arena, sizeof(SpawnBlock));
    SpawnBlock__init((Entity*)block, x, y);
    return (Entity*)block;
  } else if (0xff4cb122 == col) {  // green
    CatSpawnBlock* block = Arena__Push(g_engine->arena, sizeof(CatSpawnBlock));
    CatSpawnBlock__init((Entity*)block, x, y);
    return (Entity*)block;
  } else if (0xff7f7f7f == col) {  // dark gray
    BreakBlock* block = Arena__Push(g_engine->arena, sizeof(BreakBlock));
    BreakBlock__init((Entity*)block, x, y);
    return (Entity*)block;
  } else if (0xff241ced == col) {  // red
    WallBlock* block = Arena__Push(g_engine->arena, sizeof(WallBlock));
    RedWallBlock__init((Entity*)block, x, y);
    return (Entity*)block;
  }

  LOG_DEBUGF("Unimplemented Level Block pixel color %08x", col);
  return NULL;
}

void Level__preload(Level* level) {
  // preload assets
  Preload__texture(&level->bmp, level->levelFile);
  // Preload__texture(&level->world, level->worldFile);
}

s32 Level__zsort(void* a, void* b) {
  Logic__State* logic = g_engine->logic;
  Entity* ea = (Entity*)a;
  Entity* eb = (Entity*)b;

  HMM_Vec3 cPos = HMM_V3(  //
      logic->player->base.tform->pos.x,
      logic->player->base.tform->pos.y,
      logic->player->base.tform->pos.z);
  HMM_Vec3 aPos = HMM_V3(  //
      ea->tform->pos.x,
      ea->tform->pos.y,
      ea->tform->pos.z);
  HMM_Vec3 bPos = HMM_V3(  //
      eb->tform->pos.x,
      eb->tform->pos.y,
      eb->tform->pos.z);

  // get position relative to player camera
  HMM_Vec3 adPos = HMM_SubV3(cPos, aPos);
  f32 alen = fabsf(HMM_LenV3(adPos));
  HMM_Vec3 bdPos = HMM_SubV3(cPos, bPos);
  f32 blen = fabsf(HMM_LenV3(bdPos));

  return alen < blen ? -1 : alen > blen ? +1 : 0;
}

// TODO: make a reusable level_walk() iterator fn
static void Level__loaded(Level* level) {
  if (level->loaded) return;
  if (!level->bmp->loaded) return;
  // if (!level->world->loaded) return;
  level->loaded = true;

  for (s32 y = 0; y < level->bmp->h; y++) {
    for (s32 x = 0; x < level->bmp->w; x++) {
      u32 color = Bmp__Get2DPixel(level->bmp, x, y, TRANSPARENT);
      Entity* entity = Level__makeEntity(color, x, y);
      if (NULL != entity) {
        List__insort(level->frameScratch[level->frame], level->entities, entity, Level__zsort);
      }
    }
  }
}

void Level__tick(Level* level) {
  PROFILE__BEGIN(LEVEL__TICK);
  if (NULL == level->entities || 0 == level->entities->len) return;
  g_engine->entity_count = level->entities->len;

  PROFILE__BEGIN(LEVEL__TICK__ARENA_RESET);
  // cpu-side swapchain for arena memory management
  level->frame = (level->frame + 1) % 2;
  Arena* scratch = level->frameScratch[level->frame];
  Arena* lastScratch = level->frameScratch[(level->frame + 1) % 2];

  // perform swap (and update z-order)
  Arena__Reset(scratch);
  List* entities2 = List__alloc(scratch);
  List__Node* node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity* entity = node->data;
    node = node->next;

    if (entity->removed) continue;
    List__insort(scratch, entities2, entity, Level__zsort);
  }
  Arena__Reset(lastScratch);
  level->entities = entities2;
  PROFILE__END(LEVEL__TICK__ARENA_RESET);

  PROFILE__BEGIN(LEVEL__TICK__QUADTREE_CREATE);
  // rebuild a QuadTree of all entities
  f32 W = (f32)level->width / 2, D = (f32)level->depth / 2;
  Rect boundary = {0.0f, 0.0f, W, D};  // Center (0,0), width/height 20x20
  level->qt = QuadTreeNode_create(scratch, boundary);
  node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity* entity = node->data;
    node = node->next;

    if (entity->removed) continue;
    QuadTreeNode_insert(
        scratch,
        level->qt,
        (Point){entity->tform->pos.x, entity->tform->pos.z},
        entity);
  }
  PROFILE__END(LEVEL__TICK__QUADTREE_CREATE);

  node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    if (NULL == node) continue;  // TODO: how does len get > list?
    Entity* entity = node->data;
    node = node->next;

    if (entity->removed) continue;
    Dispatcher__call1(entity->engine->tick, entity);
  }
  PROFILE__END(LEVEL__TICK);
}

void Level__render(Level* level) {
  Level__loaded(level);
  if (!level->loaded) return;

  if (NULL == level->entities || 0 == level->entities->len) return;

  List__Node* node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity* entity = node->data;
    node = node->next;

    Dispatcher__call1(entity->engine->render, entity);
  }
}

void Level__gui(Level* level) {
  List__Node* node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity* entity = node->data;
    Dispatcher__call1(entity->engine->gui, entity);
    node = node->next;
  }
}

Entity* Level__findEntity(Level* level, bool (*findCb)(Entity* e)) {
  if (level->loaded) {
    List__Node* node = level->entities->head;
    for (u32 i = 0; i < level->entities->len; i++) {
      Entity* entity = node->data;
      node = node->next;

      if (findCb(entity)) return entity;
    }
  }
  return 0;
}
