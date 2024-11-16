#pragma once

#include "../Game.h"  // IWYU pragma: keep

typedef struct Level {
  BmpReader* bmp;
  char* levelFile;
  bool loaded;
  List* entities;  // world entities
  List* nzentities;
  List* zentities;  // zsort world entities (ie. transparent entities only)
  Entity* cubemap;
  u32 wallTex;
  u32 ceilTex;
  u32 floorTex;
  u32 wallCol;
  u32 ceilCol;
  u32 floorCol;
  u32 width;
  u32 depth;
  u32 height;
  SpawnBlock* spawner;
  QuadTreeNode* qt;
} Level;

Level* Level__alloc();
void Level__init(Level* level);
void Level__preload(Level* level);
void Level__tick(Level* level);
void Level__render(Level* level);
void Level__gui(Level* level);

s32 Level__zsort(void* a, void* b);
Entity* Level__findEntity(Level* level, bool (*findCb)(Entity* e));