#pragma once

#include "../common/Hash.h"
#include "../common/Sokol.h"
#include "Entity.h"

#define CONSOLE_LINE_LEN (255)
#define CONSOLE_HISTORY_COUNT (10)

typedef struct Console {
  Entity base;
  bool show;
  u32 len;
  char buf[CONSOLE_LINE_LEN];
  char render[CONSOLE_LINE_LEN];
  u8 history_offset;
  char* history[CONSOLE_HISTORY_COUNT][CONSOLE_LINE_LEN];
  HashTable* vtable;
} Console;

void Console__init(Entity* entity);
bool Console__event(Entity* entity, const sapp_event* event);
void Console__tick(Entity* entity);
