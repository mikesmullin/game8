#include "NetMgr.h"

#include <string.h>

#include "../Logic.h"
#include "../common/Arena.h"
#include "../common/Log.h"
#include "../common/Net.h"
#include "../common/Utils.h"
#include "../common/Websocket.h"
#include "../messages/Messages.h"

#define DEBUG_STR_LEN (2046)

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

  client->state = SOCKET_CONNECTED;
  client->sessionState = SESSION_SERVER_HANDSHAKE_AWAIT;
  LOG_DEBUGF("Server %s:%s accept %s:%s", server->addr, server->port, client->addr, client->port);

  // remember all incoming connections
  self->clients[self->client_count++] = client;
}

static void onConnect(Socket* client) {
  Logic__State* logic = g_engine->logic;
  NetMgr* self = &logic->net;

  client->state = SOCKET_CONNECTED;
  LOG_DEBUGF("Client connected to %s:%s", client->addr, client->port);
}

static void ServerPump(Socket* client) {
  if (SOCKET_CONNECTED != client->state) return;

  Logic__State* logic = g_engine->logic;
  NetMgr* self = &logic->net;

  // Read data from client
  Net__read(client);
  if (client->buf.len > 0) {
    char* debug = Arena__Push(g_engine->logic->frameArena, DEBUG_STR_LEN);
    hexdump(client->buf.data, client->buf.len, debug, DEBUG_STR_LEN);
    LOG_DEBUGF("Server recv. len: %u, data:\n%s", client->buf.len, debug);

    if (SESSION_SERVER_HANDSHAKE_AWAIT == client->sessionState) {
      char key[256];
      Websocket__ParseHandshake(client->buf.data, key);
      char response[512];
      WebSocket__RespondHandshake(g_engine->logic->frameArena, key, response);

      // Send response to client
      u32 len = strlen(response);
      Net__write(client, len, (const u8*)response);
      hexdump(response, len, debug, DEBUG_STR_LEN);
      LOG_DEBUGF("Server send. len: %u, data:\n%s", len, debug);
      client->sessionState = SESSION_SERVER_HANDSHAKE_SENT;
    } else if (SESSION_SERVER_HANDSHAKE_SENT == client->sessionState) {
      u8* payload = NULL;
      u64 payload_length = 0;
      int r = WebSocket__ParseFrame(
          g_engine->logic->frameArena,
          (const u8*)client->buf.data,
          client->buf.len,
          &payload,
          &payload_length);
      if (r == 0) {
        hexdump(payload, payload_length, debug, DEBUG_STR_LEN);
        LOG_DEBUGF("Websocket frame. len: %u, data:\n%s", payload_length, debug);

        u8* ptr = payload;
        Message__Parse(payload_length, &ptr);
        // TODO: replace client->buf with ring buffer, and advance start/end cursors
      }
    }
  }
}

static void ClientPump(Socket* client) {
  if (SOCKET_CONNECTED != client->state) return;

  Logic__State* logic = g_engine->logic;
  NetMgr* self = &logic->net;
  char* debug = Arena__Push(g_engine->logic->frameArena, DEBUG_STR_LEN);

  if (SESSION_NONE == client->sessionState) {
    // Send request to server
    MoveRequest msg = {.base.id = MSG_MOVE_REQ, .x = 1.0f, .y = 2.0f, .z = 3.0f};
    u32 len = sizeof(MoveRequest);
    Net__write(client, len, (const u8*)&msg);
    hexdump(&msg, len, debug, DEBUG_STR_LEN);
    LOG_DEBUGF("Client send. len: %u, data:\n%s", len, debug);
    client->sessionState = SESSION_CLIENT_HELLO_SENT;
  }

  // Read data from server
  Net__read(client);
  if (client->buf.len > 0) {
    hexdump(client->buf.data, client->buf.len, debug, DEBUG_STR_LEN);
    LOG_DEBUGF("Client recv. len: %u, data:\n%s", client->buf.len, debug);
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