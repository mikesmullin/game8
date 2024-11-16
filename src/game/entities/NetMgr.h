#pragma once

#include "../Game.h"  // IWYU pragma: keep

#define LISTEN_ADDR "0.0.0.0"
#define LISTEN_PORT "9000"
#define CONNECT_ADDR "127.0.0.1"
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
