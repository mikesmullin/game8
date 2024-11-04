#include "NetMgr.h"

#include <string.h>

#include "../Logic.h"
#include "../common/Log.h"
#include "../common/Net.h"

extern Engine__State* g_engine;

static void onAccept(Socket* server, Socket* client);
static void onConnect(Socket* client);
static void ServerPump(Socket* client);
static void ClientPump(Socket* client);

void NetMgr__init() {
  Logic__State* logic = g_engine->logic;
  NetMgr* self = &logic->net;

  Net__init();

  if (g_engine->isMaster) {
    // Server
    self->listener = Net__Socket__alloc();
    Net__Socket__init(self->listener, LISTEN_ADDR, LISTEN_PORT, SERVER_SOCKET);
    LOG_DEBUGF("Server listen on %s:%s", self->listener->addr, self->listener->port);
    Net__listen(self->listener);
  } else {
    // Client
    self->client = Net__Socket__alloc();
    Net__Socket__init(self->client, CONNECT_ADDR, LISTEN_PORT, CLIENT_SOCKET);
    LOG_DEBUGF("Client connecting to %s:%s", self->client->addr, self->client->port);
    Net__connect(self->client, onConnect);
  }
}

void NetMgr__tick() {
  Logic__State* logic = g_engine->logic;
  NetMgr* self = &logic->net;

  if (g_engine->isMaster) {
    Net__accept(self->listener, onAccept);

    for (u32 i = 0; i < self->client_count; i++) {
      ServerPump(self->clients[i]);
    }
  } else {
    ClientPump(self->client);
  }
}

static void onAccept(Socket* server, Socket* client) {
  Logic__State* logic = g_engine->logic;
  NetMgr* self = &logic->net;

  LOG_DEBUGF("Server %s:%s accept %s:%s", server->addr, server->port, client->addr, client->port);

  // remember all incoming connections
  self->clients[self->client_count++] = client;

  // Send response to client
  char* data = "shello";
  u32 len = strlen(data);
  Net__write(client, len, data);
  LOG_DEBUGF("Server send. len: %u, data: %s", len, data);
}

static void onConnect(Socket* client) {
  Logic__State* logic = g_engine->logic;
  NetMgr* self = &logic->net;

  LOG_DEBUGF("Client connected to %s:%s", client->addr, client->port);

  // Send response to server
  char* data = "chello";
  u32 len = strlen(data);
  Net__write(client, len, data);
  LOG_DEBUGF("Client send. len: %u, data: %s", len, data);
}

static void ServerPump(Socket* client) {
  Logic__State* logic = g_engine->logic;
  NetMgr* self = &logic->net;

  // Read data from client
  Net__read(client);
  if (client->buf.len > 0) {
    LOG_DEBUGF("Server recv. len: %u, data: %s", client->buf.len, client->buf.data);

    char* expected = "chello";
    if (strcmp(client->buf.data, expected) == 0) {
      LOG_DEBUGF("server satisfied");
    }
  }
}

static void ClientPump(Socket* client) {
  Logic__State* logic = g_engine->logic;
  NetMgr* self = &logic->net;

  // Read data from server
  Net__read(client);
  if (client->buf.len > 0) {
    LOG_DEBUGF("Client recv. len: %u, data: %s", client->buf.len, client->buf.data);

    char* expected = "shello";
    if (strcmp(client->buf.data, expected) == 0) {
      LOG_DEBUGF("client satisfied");
    }
  }
}

void NetMgr__shutdown() {
  Logic__State* logic = g_engine->logic;
  NetMgr* self = &logic->net;

  if (g_engine->isMaster) {
    for (u32 i = 0; i < self->client_count; i++) {
      LOG_DEBUGF("close accepted client %u", i);
      Net__shutdown(self->clients[i]);
      Net__close(self->clients[i]);
      // Net__free(clients[i]);
    }

    LOG_DEBUGF("close server");
    Net__close(self->listener);
    Net__free(self->listener);
  } else {
    LOG_DEBUGF("close client");
    Net__shutdown(self->client);
    Net__close(self->client);
    Net__free(self->client);
  }

  LOG_DEBUGF("network shutdown");
  Net__destroy();
}