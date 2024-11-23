#pragma once

#include "../Game.h"  // IWYU pragma: keep

#define MAX_CLIENTS (1)

typedef struct NetMgr {
  Socket* listener;
  Socket* clients[MAX_CLIENTS];
  u32 client_count;
  Socket* client;
} NetMgr;

void NetMgr__init();
void NetMgr__tick();
void NetMgr__shutdown();
