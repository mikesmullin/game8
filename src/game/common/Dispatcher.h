#pragma once

#include <stdint.h>
typedef uint32_t u32;
typedef struct Entity Entity;

typedef enum DispatchFnId1 : u32 {
  DISPATCH1_NONE,
  CAT_ENTITY__TICK,
  CAT_SPAWN_BLOCK__TICK,
  DEBUG_TEXT__TICK,
  PLAYER_ENTITY__TICK,
  RUBBLE_SPRITE__TICK,
  SPAWN_BLOCK__TICK,

  // ABOUT_MENU__GUI,
  // ABOUT_MENU__RENDER,
  // ABOUT_MENU__TICK,
  // HELP_MENU__GUI,
  // HELP_MENU__RENDER,
  // HELP_MENU__TICK,
  // TITLE_MENU__GUI,
  // TITLE_MENU__RENDER,
  // TITLE_MENU__TICK,

  LEVEL__TICK,
  LEVEL__TICK__ARENA_RESET,
  LEVEL__TICK__QUADTREE_CREATE,
  LEVEL__RENDER,
  LEVEL__GUI,
  GAME__GUI,

  DISPATCH1__COUNT,
} DispatchFnId1;

typedef enum DispatchFnId2 : u32 {
  DISPATCH2_NONE,

  BREAK_BLOCK__ACTION,
  CAT_ENTITY__ACTION,
  CAT_ENTITY__COLLIDE,
  RED_WALL_BLOCK__ACTION,

  DISPATCH2__COUNT,
} DispatchFnId2;

void Dispatcher__call1(DispatchFnId1 id, Entity* inst);
void Dispatcher__call2(DispatchFnId2 id, Entity* inst, void* params);
