#pragma once

#include <stdint.h>
typedef uint32_t u32;
typedef uint64_t u64;

#define BUFFER_SIZE 1024

typedef enum SocketOpts : u32 {
  SERVER_SOCKET,
  CLIENT_SOCKET,
} SocketOpts;

typedef struct SocketBuffer {
  u32 len;
  char data[BUFFER_SIZE];
} SocketBuffer;

typedef struct Socket {
  char *addr, *port;
  u32 opts;
#ifdef _WIN32
  u64 _win_socket;
  struct addrinfo* _win_addr;
#endif
  SocketBuffer buf;
} Socket;

void Net__init();
Socket* Net__Socket__alloc();
void Net__Socket__init(Socket* sock, char* addr, char* port, u32 opts);
void Net__listen(Socket* socket);
void Net__accept(Socket* socket, void (*acceptCb)(Socket*, Socket*));
void Net__connect(Socket* socket, void (*connectCb)(Socket*));
void Net__read(Socket* socket);
void Net__write(Socket* socket, u32 len, char* data);
void Net__shutdown(Socket* socket);
void Net__close(Socket* socket);
void Net__free(Socket* socket);
void Net__destroy();
