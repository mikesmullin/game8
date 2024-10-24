#pragma once

#include <stdint.h>
typedef uint32_t u32;
typedef struct Entity Entity;

typedef enum DispatchFnId1 : u32 {
  DISPATCH_NONE1,

  CAT_SPAWN_BLOCK__TICK,
  CAT_SPAWN_BLOCK__GUI,
  SPAWN_BLOCK__TICK,
  WALL_BLOCK__RENDER,
  BREAK_BLOCK__RENDER,
  BREAK_BLOCK__TICK,

  SPRITE__RENDER,
  CAT_ENTITY__TICK,
  CAT_ENTITY__RENDER,
  CAT_ENTITY__GUI,
  RUBBLE_SPRITE__TICK,
  RUBBLE_SPRITE__RENDER,
  PLAYER_ENTITY__TICK,

  // ABOUT_MENU__TICK,
  // ABOUT_MENU__RENDER,
  // ABOUT_MENU__GUI,
  // HELP_MENU__TICK,
  // HELP_MENU__RENDER,
  // HELP_MENU__GUI,
  // TITLE_MENU__TICK,
  // TITLE_MENU__RENDER,
  // TITLE_MENU__GUI,
} DispatchFnId1;

typedef enum DispatchFnId2 : u32 {
  DISPATCH_NONE2,

  CAT_ENTITY__COLLIDE,
} DispatchFnId2;

void Dispatcher__call1(DispatchFnId1 id, Entity* inst);
void Dispatcher__call2(DispatchFnId2 id, Entity* inst, void* params);
