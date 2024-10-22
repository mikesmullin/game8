#include "Level.h"

#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/Bmp.h"
#include "../common/List.h"
#include "../common/Log.h"
#include "../common/Preloader.h"
// #include "../common/Math.h"
// #include "../blocks/BreakBlock.h"
// #include "../blocks/CatSpawnBlock.h"
#include "../common/Color.h"
#include "../common/Dispatcher.h"
#include "../common/Geometry.h"
#include "../common/QuadTree.h"
#include "../entities/blocks/SpawnBlock.h"
#include "../entities/blocks/WallBlock.h"

extern Engine__State* g_engine;

Level* Level__alloc() {
  Level* level = Arena__Push(g_engine->arena, sizeof(Level));
  level->bmp = NULL;
  level->world = NULL;
  level->entities = List__alloc(g_engine->arena);
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

static Block* Level__makeBlock(u32 col, f32 x, f32 y) {
  if (0xff000000 == col) {  // black; empty space
    return NULL;
  }
  if (0xffffffff == col) {  // white
    Block* block = WallBlock__alloc();
    WallBlock__init((Entity*)block, x, y);
    return block;
  }
  if (0xff00f2ff == col) {  // yellow
    Block* block = SpawnBlock__alloc();
    SpawnBlock__init((Entity*)block, x, y);
    return block;
  }
  // if (0xff241ced == col) {  // red
  //   Block* block = CatSpawnBlock__alloc();
  //   CatSpawnBlock__init(block, x, y);
  //   return block;
  // }
  // if (0xff7f7f7f == col) {  // dark gray
  //   Block* block = BreakBlock__alloc();
  //   BreakBlock__init(block, x, y);
  //   return block;
  // }

  LOG_DEBUGF("Unimplemented Level Block pixel color %08x", col);
  return NULL;
}

void Level__preload(Level* level) {
  // preload assets
  Preload__texture(&level->bmp, level->levelFile);
  // Preload__texture(&level->world, level->worldFile);
}

static void Level__loaded(Level* level) {
  if (level->loaded) return;
  if (!level->bmp->loaded) return;
  // if (!level->world->loaded) return;
  level->loaded = true;

  for (s32 y = 0; y < level->bmp->h; y++) {
    for (s32 x = 0; x < level->bmp->w; x++) {
      u32 color = Bmp__Get2DPixel(level->bmp, x, y, TRANSPARENT);
      Block* block = Level__makeBlock(color, x, y);
      if (NULL != block) {
        List__append(g_engine->arena, level->entities, block);
      }
    }
  }
}

void Level__render(Level* level) {
  Level__loaded(level);
  if (!level->loaded) return;

  List__Node* node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity* entity = node->data;
    Dispatcher__call1(entity->engine->render, entity);
    node = node->next;
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

void Level__tick(Level* level) {
  // build a QuadTree of all entities
  f32 W = (f32)level->width / 2, D = (f32)level->depth / 2;
  Rect boundary = {0.0f, 0.0f, W, D};  // Center (0,0), width/height 20x20
  if (NULL == level->qtArena) {
    // TODO: reduce size back to 1MB; instead don't store entities in the tree that can't collide
    level->qtArena = Arena__SubAlloc(g_engine->arena, 1024 * 1024 * 10);  // MB
  }
  Arena__Reset(level->qtArena);
  level->qt = QuadTreeNode_create(level->qtArena, boundary);
  List__Node* node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity* entity = node->data;
    node = node->next;
    QuadTreeNode_insert(
        level->qtArena,
        level->qt,
        (Point){entity->tform->pos.x, entity->tform->pos.z},
        entity);
  }

  node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity* entity = node->data;
    node = node->next;
    Dispatcher__call1(entity->engine->tick, entity);
  }
}