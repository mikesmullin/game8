#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef struct DispatchComponent {
  u32 /*DispatchFnId*/ tick;
  u32 /*DispatchFnId*/ render;
  u32 /*DispatchFnId*/ gui;
  u32 /*DispatchFnId2*/ action;
} DispatchComponent;