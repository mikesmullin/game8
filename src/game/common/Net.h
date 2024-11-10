#pragma once

#include <stdint.h>
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

// #ifndef __EMSCRIPTEN__
// #define __EMSCRIPTEN__  // DEBUG: enable for testing in editor
// #endif

#define BUFFER_SIZE 1024

typedef enum SocketOpts {
  SERVER_SOCKET,
  CLIENT_SOCKET,
} SocketOpts;

typedef struct SocketBuffer {
  u32 len;
  char data[BUFFER_SIZE];
} SocketBuffer;

typedef enum SocketState {
  SOCKET_NONE,
  SOCKET_CONNECTED,
} SocketState;

typedef enum SessionState {
  SESSION_NONE,
  SESSION_SERVER_HANDSHAKE_AWAIT,
  SESSION_SERVER_HANDSHAKE_SENT,
  SESSION_CLIENT_HELLO_SENT,
} SessionState;

typedef struct Socket Socket;
typedef struct Socket {
  char *addr, *port;
  u32 opts;
#ifdef _WIN32
  u64 _win_socket;
  struct addrinfo* _win_addr;
#endif
#ifdef __EMSCRIPTEN__
  u32 _web_socket;
#endif
  SocketBuffer buf;
  u32 state;
  void (*connectCb)(Socket*);
  u32 sessionState;
} Socket;

void Net__init();
Socket* Net__Socket__alloc();
void Net__Socket__init(Socket* sock, char* addr, char* port, u32 opts);
void Net__listen(Socket* socket);
void Net__accept(Socket* socket, void (*acceptCb)(Socket*, Socket*));
void Net__connect(Socket* socket, void (*connectCb)(Socket*));
void Net__read(Socket* socket);
void Net__write(Socket* socket, u32 len, const u8* data);
void Net__shutdown(Socket* socket);
void Net__close(Socket* socket);
void Net__free(Socket* socket);
void Net__destroy();
