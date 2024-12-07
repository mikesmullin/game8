#pragma once

#include "../common/Hash.h"
#include "../common/Sokol.h"
#include "Entity.h"

typedef struct Console {
  Entity base;
  bool show;
  u32 len;
  char buf[255];
  char render[255];
  HashTable* vtable;
} Console;

void Console__init(Entity* entity);
bool Console__event(Entity* entity, const sapp_event* event);
void Console__tick(Entity* entity);
