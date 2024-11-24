#pragma once

#include "Game.h"  // IWYU pragma: keep

typedef enum DispatchFnId /* : u32 */ {
  DISPATCH_NONE,

  ATLAS__ONRENDER_LOAD,
  ATLAS__ONRENDER_ALLOC,
  ATLAS__ONRENDER_ENTITY,
  ATLAS__ONRENDER_MATERIAL,
  PBR__ONRENDER_LOAD,
  PBR__ONRENDER_ALLOC,
  PBR__ONRENDER_ENTITY,
  PBR__ONRENDER_MATERIAL,

  PLAYER__TICK,

  DISPATCH__COUNT,
} DispatchFnId;

void Dispatcher__call(u32 id, void* params);