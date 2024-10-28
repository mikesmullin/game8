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
#include "../components/MeshRenderer.h"
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
  level->entities = List__alloc(g_engine->arena);
  level->zentities = List__alloc(g_engine->arena);

  // NOTICE: tune the size of this to fit anticipated max entity count (ie. adjust for load tests)
  g_engine->logic->frameArena = Arena__SubAlloc(g_engine->arena, 1024 * 1024 * 1);  // MB

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
        List__append(g_engine->arena, level->entities, entity);
      }
    }
  }
}

void Level__tick(Level* level) {
  PROFILE__BEGIN(LEVEL__TICK);
  if (NULL == level->entities || 0 == level->entities->len) return;
  g_engine->entity_count =  // for perf counters
      level->entities->len +  //
      level->zentities->len +  //
      g_engine->logic->ui_entities->len;
  Logic__State* logic = g_engine->logic;

  // build a QuadTree of all entities
  Arena__Reset(logic->frameArena);
  f32 W = (f32)level->width / 2, D = (f32)level->depth / 2;
  Rect boundary = {0.0f, 0.0f, W, D};  // Center (0,0), width/height 20x20
  level->qt = QuadTreeNode_create(logic->frameArena, boundary);
  List__Node* node = level->entities->head;
  u32 len = level->entities->len;  // cache, because loop will modify length as it goes
  for (u32 i = 0; i < len; i++) {
    if (0 == node) continue;  // TODO: find out how this happens
    Entity* entity = node->data;
    if (0 == entity) continue;  // TODO: find out how this happens
    List__Node* entityNode = node;  // cache, because it may get removed
    node = node->next;

    if (entity->removed) {  // gc
      List__remove(level->entities, entityNode);
    } else {
      PROFILE__BEGIN(LEVEL__TICK__QUADTREE_CREATE);
      if (TAG_WALL & entity->tags1) {
        QuadTreeNode_insert(
            logic->frameArena,
            level->qt,
            (Point){entity->tform->pos.x, entity->tform->pos.z},
            entity);
      }
      PROFILE__END(LEVEL__TICK__QUADTREE_CREATE);

      Dispatcher__call1(entity->engine->tick, entity);
    }
  }

  node = level->zentities->head;
  len = level->zentities->len;  // cache, because loop will modify length as it goes
  for (u32 i = 0; i < len; i++) {
    if (0 == node) continue;  // TODO: find out how this happens
    Entity* entity = node->data;
    if (0 == entity) continue;  // TODO: find out how this happens
    List__Node* entityNode = node;  // cache, because it may get removed
    node = node->next;

    if (entity->removed) {  // gc
      List__remove(level->zentities, entityNode);
    } else {
      PROFILE__BEGIN(LEVEL__TICK__QUADTREE_CREATE);
      if (TAG_WALL & entity->tags1) {
        QuadTreeNode_insert(
            logic->frameArena,
            level->qt,
            (Point){entity->tform->pos.x, entity->tform->pos.z},
            entity);
      }
      PROFILE__END(LEVEL__TICK__QUADTREE_CREATE);

      Dispatcher__call1(entity->engine->tick, entity);
    }
  }

  PROFILE__END(LEVEL__TICK);
}

void Level__render(Level* level) {
  PROFILE__BEGIN(LEVEL__RENDER);

  Level__loaded(level);
  if (!level->loaded) return;

  if (NULL == level->entities || 0 == level->entities->len) return;

  List__Node* node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity* entity = node->data;
    node = node->next;

    if (0 == entity->render) continue;
    Dispatcher__call1(entity->engine->render, entity);
  }
  node = level->zentities->head;
  for (u32 i = 0; i < level->zentities->len; i++) {
    Entity* entity = node->data;
    node = node->next;

    if (0 == entity->render) continue;
    Dispatcher__call1(entity->engine->render, entity);
  }

  MeshRenderer__renderBatches(level->entities);
  MeshRenderer__renderBatches(level->zentities);

  PROFILE__END(LEVEL__RENDER);
}

void Level__gui(Level* level) {
  PROFILE__BEGIN(LEVEL__GUI);

  List__Node* node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity* entity = node->data;
    Dispatcher__call1(entity->engine->gui, entity);
    node = node->next;
  }
  node = level->zentities->head;
  for (u32 i = 0; i < level->zentities->len; i++) {
    Entity* entity = node->data;
    Dispatcher__call1(entity->engine->gui, entity);
    node = node->next;
  }

  PROFILE__END(LEVEL__GUI);
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
