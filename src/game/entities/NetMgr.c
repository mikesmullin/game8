#include "NetMgr.h"

#include DEPINJ(GAME_H, "../Game.h")
#include "../messages/Message.h"

#define DEBUG_STR_LEN (2046)

static void onAccept(Socket* server, Socket* client);
static void onConnect(Socket* client);
static void ServerPump(Socket* client);
static void ClientPump(Socket* client);

void NetMgr__init() {
  g_engine->Net__init();

  NetMgr* self = g_engine->game->net;
  // #define self ((NetMgr*)g_engine->game->net)

  if (g_engine->isMaster) {
    // Server
    self->listener = g_engine->Net__Socket__alloc();
    g_engine->Net__Socket__init(
        self->listener,
        g_engine->listenHost,
        g_engine->listenPort,
        SERVER_SOCKET);
    LOG_DEBUGF("Server listen on %s:%s", self->listener->addr, self->listener->port);
    g_engine->Net__listen(self->listener);
  } else {
    // Client
    self->client = g_engine->Net__Socket__alloc();
    g_engine->Net__Socket__init(
        self->client,
        g_engine->connectHost,
        g_engine->connectPort,
        CLIENT_SOCKET);
    LOG_DEBUGF("Client connecting to %s:%s", self->client->addr, self->client->port);
    g_engine->Net__connect(self->client, onConnect);
  }

#undef self
}

void NetMgr__tick() {
// NetMgr* self = g_engine->game->net;
#define self ((NetMgr*)g_engine->game->net)

  if (g_engine->isMaster) {
    g_engine->Net__accept(self->listener, onAccept);

    for (u32 i = 0; i < self->client_count; i++) {
      ServerPump(self->clients[i]);
    }
  } else {
    ClientPump(self->client);
  }
#undef self
}

static void onAccept(Socket* server, Socket* client) {
// NetMgr* self = g_engine->game->net;
#define self ((NetMgr*)g_engine->game->net)

  client->state = SOCKET_CONNECTED;
  client->sessionState = SESSION_SERVER_HANDSHAKE_AWAIT;
  LOG_DEBUGF("Server %s:%s accept %s:%s", server->addr, server->port, client->addr, client->port);

  // remember all incoming connections
  self->clients[self->client_count++] = client;

#undef self
}

static void onConnect(Socket* client) {
// NetMgr* self = g_engine->game->net;
#define self ((NetMgr*)g_engine->game->net)

  client->state = SOCKET_CONNECTED;
  LOG_DEBUGF("Client connected to %s:%s", client->addr, client->port);

#undef self
}

static void ServerPump(Socket* client) {
// NetMgr* self = g_engine->game->net;
#define self ((NetMgr*)g_engine->game->net)

  if (SOCKET_CONNECTED != client->state) return;

  // Read data from client
  g_engine->Net__read(client);
  if (client->buf.len > 0) {
    char* debug = Arena__Push(g_engine->frameArena, DEBUG_STR_LEN);
    hexdump(client->buf.data, client->buf.len, debug, DEBUG_STR_LEN);
    LOG_DEBUGF("Server recv. len: %u, data:\n%s", client->buf.len, debug);

    if (SESSION_SERVER_HANDSHAKE_AWAIT == client->sessionState) {
      char key[256];
      Websocket__ParseHandshake(client->buf.data, key);
      char response[512];
      WebSocket__RespondHandshake(g_engine->frameArena, key, response);

      // Send response to client
      u32 len = strlen(response);
      g_engine->Net__write(client, len, (const u8*)response);
      hexdump(response, len, debug, DEBUG_STR_LEN);
      LOG_DEBUGF("Server send. len: %u, data:\n%s", len, debug);
      client->sessionState = SESSION_SERVER_HANDSHAKE_SENT;
    } else if (SESSION_SERVER_HANDSHAKE_SENT == client->sessionState) {
      u8* payload = NULL;
      u64 payload_length = 0;
      int r = WebSocket__ParseFrame(
          g_engine->frameArena,
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

#undef self
}

static void ClientPump(Socket* client) {
// NetMgr* self = g_engine->game->net;
#define self ((NetMgr*)g_engine->game->net)

  if (SOCKET_CONNECTED != client->state) return;

  char* debug = Arena__Push(g_engine->frameArena, DEBUG_STR_LEN);

  if (SESSION_NONE == client->sessionState) {
    // Send request to server
    MoveRequest msg = {.base.id = MSG_MOVE_REQ, .x = 1.0f, .y = 2.0f, .z = 3.0f};
    u32 len = sizeof(MoveRequest);
    g_engine->Net__write(client, len, (const u8*)&msg);
    hexdump(&msg, len, debug, DEBUG_STR_LEN);
    LOG_DEBUGF("Client send. len: %u, data:\n%s", len, debug);
    client->sessionState = SESSION_CLIENT_HELLO_SENT;
  }

  // Read data from server
  g_engine->Net__read(client);
  if (client->buf.len > 0) {
    hexdump(client->buf.data, client->buf.len, debug, DEBUG_STR_LEN);
    LOG_DEBUGF("Client recv. len: %u, data:\n%s", client->buf.len, debug);
  }
#undef self
}

void NetMgr__shutdown() {
// NetMgr* self = g_engine->game->net;
#define self ((NetMgr*)g_engine->game->net)

  if (g_engine->isMaster) {
    for (u32 i = 0; i < self->client_count; i++) {
      LOG_DEBUGF("close accepted client %u", i);
      g_engine->Net__shutdown(self->clients[i]);
      g_engine->Net__close(self->clients[i]);
      // g_engine->Net__free(clients[i]);
    }

    LOG_DEBUGF("close server");
    g_engine->Net__close(self->listener);
    g_engine->Net__free(self->listener);
  } else {
    LOG_DEBUGF("close client");
    g_engine->Net__shutdown(self->client);
    g_engine->Net__close(self->client);
    g_engine->Net__free(self->client);
  }

  LOG_DEBUGF("network shutdown");
  g_engine->Net__destroy();

#undef self
}