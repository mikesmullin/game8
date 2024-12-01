#include "Level.h"

Level* Level__alloc() {
  Level* level = Arena__Push(g_engine->arena, sizeof(Level));
  level->bmp = 0;
  level->entities = List__alloc(g_engine->arena);

  // NOTICE: tune the size of this to fit anticipated max entity count (ie. adjust for load tests)
  g_engine->frameArena = Arena__SubAlloc(g_engine->arena, 1024 * 1024 * 500);  // MB

  return level;
}

void Level__init(Level* level) {
  level->wallTex = 0;
  level->ceilTex = 1;
  level->floorTex = 2;
  level->wallCol = 0;
  level->ceilCol = 0;
  level->floorCol = 0;
  level->spawner = 0;
}

static Entity* Level__makeEntity(u32 col, f32 x, f32 y) {
  if (0xff000000 == col) {  // black; empty space
    return 0;
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
  return 0;
}

void Level__preload(Level* level) {
  // preload assets
  Preload__texture(&level->bmp, level->levelFile);
  // Preload__texture(&level->world, level->worldFile);
}

s32 Level__zsort(void* a, void* b) {
  Player* player1 = (Player*)g_engine->players->head->data;
  Entity* ea = (Entity*)a;
  Entity* eb = (Entity*)b;

  // get position relative to player camera
  v3 adPos;
  v3_sub(&adPos, &player1->base.base.tform->pos, &ea->tform->pos);
  f32 alen = Math__fabsf(v3_mag2(&adPos));
  v3 bdPos;
  v3_sub(&bdPos, &player1->base.base.tform->pos, &eb->tform->pos);
  f32 blen = Math__fabsf(v3_mag2(&bdPos));

  return -(alen < blen ? -1 : alen > blen ? +1 : 0);  // nearest to screen in front
}

static void Level__loaded(Level* level) {
  if (level->loaded) return;
  if (!level->bmp->loaded) return;
  // if (!level->world->loaded) return;
  level->loaded = true;

  for (s32 y = 0; y < level->bmp->h; y++) {
    for (s32 x = 0; x < level->bmp->w; x++) {
      u32 color = Bmp__Get2DPixel(level->bmp, x, y, COLOR_TRANSPARENT);
      Entity* entity = Level__makeEntity(color, x, y);
      if (0 != entity) {
        List__append(g_engine->arena, level->entities, entity);
      }
    }
  }
}

static bool zsort_entities(void* data) {
  List__append(g_engine->frameArena, g_engine->game->level->zentities, data);
  return true;
}

void Level__tick(Level* level) {
  PROFILE__BEGIN(LEVEL__TICK);
  if (0 == level->entities || 0 == level->entities->len) return;
  g_engine->entity_count =  // for perf counters
      level->entities->len +  //
      g_engine->game->ui_entities->len;

  Dispatcher__call(level->cubemap->dispatch->tick, &(OnEntityParams){level->cubemap});

  // build a QuadTree of all entities
  Arena__Reset(g_engine->frameArena);
  f32 W = (f32)level->width / 2, D = (f32)level->depth / 2;
  Rect boundary = {0.0f, 0.0f, W, D};  // Center (0,0), width/height 20x20
  level->qt = QuadTree_create(g_engine->frameArena, boundary);
  level->nzentities = List__alloc(g_engine->frameArena);
  level->zentities = List__alloc(g_engine->frameArena);
  RBTree* tzentities = RBTree__alloc(g_engine->frameArena);
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
      QuadTree_insert(
          g_engine->frameArena,
          level->qt,
          (Point){entity->tform->pos.x, entity->tform->pos.z},
          entity);
      PROFILE__END(LEVEL__TICK__QUADTREE_CREATE);

      Dispatcher__call(entity->dispatch->tick, &(OnEntityParams){entity});

      if (0 != entity->render && entity->render->rg == WORLD_ZSORT_RG) {
        RBTree__insort(g_engine->frameArena, tzentities, entity, Level__zsort);
      } else {
        List__append(g_engine->frameArena, level->nzentities, entity);
      }
    }
  }

  RBTree__walk(tzentities, tzentities->root, zsort_entities);

  PROFILE__END(LEVEL__TICK);
}

void Level__render(Level* level) {
  PROFILE__BEGIN(LEVEL__RENDER);

  Level__loaded(level);
  if (!level->loaded) return;

  if (0 == level->entities || 0 == level->entities->len) return;

  List* sky = List__alloc(g_engine->frameArena);
  List__append(g_engine->frameArena, sky, level->cubemap);
  MeshRenderer__renderBatches(sky, Dispatcher__call);

  if (0 != level->nzentities) {
    List__Node* node = level->nzentities->head;
    for (u32 i = 0; i < level->nzentities->len; i++) {
      Entity* entity = node->data;
      node = node->next;

      if (0 == entity->render) continue;
      Dispatcher__call(entity->dispatch->render, &(OnEntityParams){entity});
    }
    MeshRenderer__renderBatches(level->nzentities, Dispatcher__call);
  }

  if (0 != level->zentities) {
    List__Node* node = level->zentities->head;
    for (u32 i = 0; i < level->zentities->len; i++) {
      Entity* entity = node->data;
      node = node->next;

      Dispatcher__call(entity->dispatch->render, &(OnEntityParams){entity});
    }
    MeshRenderer__renderBatches(level->zentities, Dispatcher__call);
  }

  PROFILE__END(LEVEL__RENDER);
}

void Level__gui(Level* level) {
  PROFILE__BEGIN(LEVEL__GUI);

  List__Node* node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity* entity = node->data;
    Dispatcher__call(entity->dispatch->gui, &(OnEntityParams){entity});

    node = node->next;
  }
  node = level->zentities->head;
  for (u32 i = 0; i < level->zentities->len; i++) {
    Entity* entity = node->data;
    Dispatcher__call(entity->dispatch->gui, &(OnEntityParams){entity});
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
