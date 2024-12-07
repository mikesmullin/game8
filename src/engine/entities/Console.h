#pragma once

#include "../common/Hash.h"
#include "../common/Sokol.h"
#include "Entity.h"

static const u32 LINE_LEN = 255;
static const u32 HISTORY_COUNT = 10;

typedef struct Console {
  Entity base;
  bool show;
  u32 len;
  char buf[LINE_LEN];
  char render[LINE_LEN];
  u8 history_offset;
  char* history[HISTORY_COUNT][LINE_LEN];
  HashTable* vtable;
} Console;

void Console__init(Entity* entity);
bool Console__event(Entity* entity, const sapp_event* event);
void Console__tick(Entity* entity);
