#include "Level.h"

#include "../../../../../src/engine/common/Dispatcher.h"
#include "../../../../../src/engine/common/Geometry.h"
#include "../../../../../src/engine/common/List.h"
#include "../../../../../src/engine/common/Profiler.h"
#include "../../../../../src/engine/common/QuadTree.h"
#include "../Logic.h"

// #include "../../../../../src/game/components/MeshRenderer.h"
#include "../entities/Cube.h"

extern Engine__State* g_engine;

Level* Level__alloc() {
  Level* level = Arena__Push(g_engine->arena, sizeof(Level));
  level->entities = List__alloc(g_engine->arena);

  // NOTICE: tune the size of this to fit anticipated max entity count (ie. adjust for load tests)
  g_engine->frameArena = Arena__SubAlloc(g_engine->arena, 1024 * 1024 * 1);  // MB

  return level;
}

void Level__init(Level* level) {
}

void Level__preload(Level* level) {
  // preload assets
}

// TODO: make a reusable level_walk() iterator fn
static void Level__loaded(Level* level) {
  if (level->loaded) return;
  level->loaded = true;

  // List__append(g_engine->arena, level->entities, entity);
}

void Level__tick(Level* level) {
  PROFILE__BEGIN(LEVEL__TICK);
  if (0 == level->entities || 0 == level->entities->len) return;
  g_engine->entity_count =  // for perf counters
      level->entities->len +  //
      (0 == level->zentities ? 0 : level->zentities->len) +  //
      g_engine->logic->ui_entities->len;
  Logic__State* logic = g_engine->logic;

  Dispatcher__call1(level->cubemap->engine->tick, level->cubemap);

  // build a QuadTree of all entities
  Arena__Reset(g_engine->frameArena);
  f32 W = (f32)level->width / 2, D = (f32)level->depth / 2;
  Rect boundary = {0.0f, 0.0f, W, D};  // Center (0,0), width/height 20x20
  level->qt = QuadTreeNode_create(g_engine->frameArena, boundary);
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
      // if (TAG_WALL & entity->tags1) {
      QuadTreeNode_insert(
          g_engine->frameArena,
          level->qt,
          (Point){entity->tform->pos.x, entity->tform->pos.z},
          entity);
      // }
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

  if (0 == level->entities || 0 == level->entities->len) return;

  // Dispatcher__call1(entity->engine->render, entity);
  // MeshRenderer__renderBatches(level->entities);

  PROFILE__END(LEVEL__RENDER);
}

void Level__gui(Level* level) {
}
