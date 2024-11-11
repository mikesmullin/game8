#include "Net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Engine.h"

#ifdef _WIN32
#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

typedef enum WebsocketFnId /* : u32 */ {
  WS_CLIENT_CONNECTION_ERROR = 1,
  WS_CLIENT_ESTABLISHED = 2,
  WS_CLOSED = 3,
  WS_CLIENT_RECEIVE = 4,
  WS_WSI_DESTROY = 5,
} WebsocketFnId;
#endif

void Net__init() {
#ifdef _WIN32
  WSADATA wsaData;
  int r = WSAStartup(MAKEWORD(2, 2), &wsaData);
  ASSERT_CONTEXT(0 == r, "WSAStartup failed with error: %d", r);
#endif

#ifdef __EMSCRIPTEN__
  EM_ASM_({
    var libwebsocket = {};
    var ctx = 0;

    libwebsocket.sockets = new Map();
    libwebsocket.on_event = Module.cwrap(
        'libwebsocket_cb',
        'number',
        [ 'number', 'number', 'number', 'number', 'number', 'number', 'number' ]);
    libwebsocket.connect = function(url, user_data) {
      try {
        var socket = new WebSocket(url);
        socket.binaryType = "arraybuffer";
        socket.user_data = user_data;

        socket.onopen = this.on_connect;
        socket.onmessage = this.on_message;
        socket.onclose = this.on_close;
        socket.onerror = this.on_error;
        socket.destroy = this.destroy;

        socket.id = this.sockets.size + 1;
        this.sockets.set(socket.id, socket);
        return socket;
      } catch (e) {
        console.error("Socket creation failed:" + e);
        return 0;
      }
    };
    libwebsocket.on_connect = function() {
      var stack = stackSave();
      // WS_CLIENT_ESTABLISHED = 2
      ret = libwebsocket.on_event(this.protocol_id, ctx, this.id, 2, this.user_data, 0, 0);
      stackRestore(stack);
    };
    libwebsocket.on_message = function(event) {
      var stack = stackSave();
      var len = event.data.byteLength;
      var ptr = allocate(len, 'i8', ALLOC_STACK);

      var data = new Uint8Array(event.data);

      for (var i = 0, buf = ptr; i < len; ++i) {
        setValue(buf, data[i], 'i8');
        buf++;
      }

      // WS_CLIENT_RECEIVE = 4
      if (libwebsocket.on_event(this.protocol_id, ctx, this.id, 4, this.user_data, ptr, len)) {
        this.close();
      }
      stackRestore(stack);
    };
    libwebsocket.on_close = function() {
      // WS_CLOSED = 3
      libwebsocket.on_event(this.protocol_id, ctx, this.id, 3, this.user_data, 0, 0);
      this.destroy();
    };
    libwebsocket.on_error = function() {
      // WS_CLIENT_CONNECTION_ERROR = 1
      libwebsocket.on_event(this.protocol_id, ctx, this.id, 1, this.user_data, 0, 0);
      this.destroy();
    };
    libwebsocket.destroy = function() {
      libwebsocket.sockets.set(this.id, undefined);
      // WS_WSI_DESTROY = 5
      libwebsocket.on_event(this.protocol_id, ctx, this.id, 5, this.user_data, 0, 0);
    };

    Module.__libwebsocket = libwebsocket;
  });
#endif
}

Socket* Net__Socket__alloc() {
  Socket* socket = Arena__Push(g_engine->arena, sizeof(Socket));
  socket->buf.data[0] = '\0';  // null-terminated string
  return socket;
}

void Net__Socket__init(Socket* sock, char* addr, char* port, u32 opts) {
  sock->addr = addr;
  sock->port = port;

#ifdef _WIN32
  sock->_win_socket = INVALID_SOCKET;
  struct addrinfo* result = NULL;
  struct addrinfo hints;
  ZeroMemory(&hints, sizeof(hints));
  if (opts && SERVER_SOCKET) {
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
  } else if (opts && CLIENT_SOCKET) {
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
  }

  // resolve the address and port strings
  int r = getaddrinfo(addr, port, &hints, &result);
  ASSERT_CONTEXT(0 == r, "getaddrinfo failed. error: %d, address: %s, port: %s", r, addr, port);

  sock->_win_addr = result;

  // Create a SOCKET
  sock->_win_socket = socket(
      sock->_win_addr->ai_family,
      sock->_win_addr->ai_socktype,
      sock->_win_addr->ai_protocol);
  ASSERT_CONTEXT(
      sock->_win_socket != INVALID_SOCKET,
      "socket create failed with error: %ld",
      WSAGetLastError());

#endif
}

void Net__listen(Socket* socket) {
#ifdef _WIN32
  // Setup the TCP listening socket
  int r = bind(socket->_win_socket, socket->_win_addr->ai_addr, (int)socket->_win_addr->ai_addrlen);
  ASSERT_CONTEXT(r != SOCKET_ERROR, "socket bind failed with error: %d", WSAGetLastError());

  // Begin listening
  r = listen(socket->_win_socket, SOMAXCONN);
  ASSERT_CONTEXT(r != SOCKET_ERROR, "socket listen failed with error: %d", WSAGetLastError());

  // Set the listen socket to non-blocking
  u_long mode = 1;  // 1 to enable non-blocking
  ioctlsocket(socket->_win_socket, FIONBIO, &mode);
#endif

#ifdef __EMSCRIPTEN__
  EM_ASM(
      // TODO: implement javascript equivalent code here
  );
#endif
}

void Net__accept(Socket* socket, void (*acceptCb)(Socket*, Socket*)) {
  Socket* csocket;

#ifdef _WIN32
  // Initialize the file descriptor set
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(socket->_win_socket, &readfds);

  // Set a zero timeout for non-blocking behavior
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;  // 0 microseconds, makes select() non-blocking

  // Check if there's a new incoming connection
  int r = select(0, &readfds, NULL, NULL, &timeout);
  if (!(r > 0 && FD_ISSET(socket->_win_socket, &readfds))) return;
  // Accept a connection from a new client socket
  struct sockaddr_in clientAddr;
  int clientAddrLen = sizeof(clientAddr);
  csocket = Net__Socket__alloc();
  csocket->_win_socket = accept(socket->_win_socket, (struct sockaddr*)&clientAddr, &clientAddrLen);
  ASSERT_CONTEXT(
      socket->_win_socket != INVALID_SOCKET,
      "socket accept failed. error: %d",
      WSAGetLastError());

  // Convert the IP address and port to C strings
  char ipStr[INET_ADDRSTRLEN], portStr[6];
  inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, sizeof(ipStr));
  snprintf(portStr, sizeof(portStr), "%d", ntohs(clientAddr.sin_port));
  u32 len = strlen(ipStr) + 1;
  csocket->addr = malloc(len);
  memcpy(csocket->addr, ipStr, len);
  len = strlen(portStr) + 1;
  csocket->port = malloc(len);
  memcpy(csocket->port, portStr, len);

  // csocket->_win_addr = clientAddr; // TODO: must convert the type manually

  // Set the incoming socket to non-blocking
  u_long mode = 1;  // 1 to enable non-blocking
  ioctlsocket(csocket->_win_socket, FIONBIO, &mode);
#endif

#ifdef __EMSCRIPTEN__
  EM_ASM(
      // TODO: implement javascript equivalent code here
  );
#endif

  acceptCb(socket, csocket);
}

void Net__connect(Socket* socket, void (*connectCb)(Socket*)) {
  socket->connectCb = connectCb;

#ifdef _WIN32
  // Connect to a server
  int r =
      connect(socket->_win_socket, socket->_win_addr->ai_addr, (int)socket->_win_addr->ai_addrlen);
  ASSERT_CONTEXT(r != SOCKET_ERROR, "socket connect failed. error: %d", WSAGetLastError());
  ASSERT_CONTEXT(
      socket->_win_socket != INVALID_SOCKET,
      "Unable to connect to server. error: %d",
      WSAGetLastError());

  // Set the incoming socket to non-blocking
  u_long mode = 1;  // 1 to enable non-blocking
  ioctlsocket(socket->_win_socket, FIONBIO, &mode);

  // TODO: wait for connection async
  socket->connectCb(socket);
#endif

#ifdef __EMSCRIPTEN__
  int r2 = EM_ASM_INT(
      {
        var url = "ws://" + UTF8ToString($0) + ":" + UTF8ToString($1);
        var socket = Module.__libwebsocket.connect(url, $2);
        if (!socket) {
          return 0;
        }

        return socket.id;
      },
      socket->addr,
      socket->port,
      socket);
  ASSERT_CONTEXT(0 != r2, "socket connect failed.");
  socket->_web_socket = r2;
#endif
}

void Net__read(Socket* socket) {
#ifdef _WIN32
  // Initialize the file descriptor set
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(socket->_win_socket, &readfds);

  // Check if there is data to read from the client
  if (socket->_win_socket != INVALID_SOCKET && FD_ISSET(socket->_win_socket, &readfds)) {
    int bytesRead = recv(socket->_win_socket, socket->buf.data, BUFFER_SIZE, 0);
    socket->buf.len = bytesRead;

    if (-1 == bytesRead) {
      // nothing to read
      socket->buf.len = 0;
    } else if (bytesRead == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
      // error occurred
      LOG_DEBUGF("socket recv failed. error: %d", WSAGetLastError());
      socket->buf.len = 0;
    } else if (bytesRead > 0) {
      // null-terminate the received data
      socket->buf.data[bytesRead] = '\0';
    } else if (bytesRead == 0) {
      // connection has been closed by the client
      // socket->_win_socket = INVALID_SOCKET;
    }
  }
#endif

#ifdef __EMSCRIPTEN__
  EM_ASM(
      // TODO: implement javascript equivalent code here
  );
#endif
}

void Net__write(Socket* socket, u32 len, const u8* data) {
#ifdef _WIN32
  int r = send(socket->_win_socket, (const char*)data, len, 0);
  ASSERT_CONTEXT(r != SOCKET_ERROR, "socket send failed with error: %d", WSAGetLastError());
#endif

#ifdef __EMSCRIPTEN__
  int r2 = EM_ASM_INT(
      {
        var socket = Module.__libwebsocket.sockets.get($0);
        if (!socket) {
          return -1;
        }

        // alloc a Uint8Array backed by the incoming data.
        var data_in = new Uint8Array(Module.HEAPU8.buffer, $1, $2);
        // allow the dest array
        var data = new Uint8Array($2);
        // set the dest from the src
        data.set(data_in);

        socket.send(data);

        return $2;
      },
      socket->_web_socket,
      data,
      len);
  ASSERT_CONTEXT(-1 != r2, "socket send failed.");
#endif
}

// for non-listening sockets
void Net__shutdown(Socket* socket) {
#ifdef _WIN32
  int r = shutdown(socket->_win_socket, SD_BOTH);  // stop sending and receiving
  ASSERT_CONTEXT(r != SOCKET_ERROR, "socket shutdown failed with error: %d", WSAGetLastError());
#endif

#ifdef __EMSCRIPTEN__
  EM_ASM(
      // TODO: implement javascript equivalent code here
  );
#endif
}

// for all sockets
void Net__close(Socket* socket) {
#ifdef _WIN32
  closesocket(socket->_win_socket);
  socket->_win_socket = INVALID_SOCKET;
#endif

#ifdef __EMSCRIPTEN__
  EM_ASM(
      // TODO: implement javascript equivalent code here
  );
#endif
}

// for all sockets
void Net__free(Socket* socket) {
#ifdef _WIN32
  freeaddrinfo(socket->_win_addr);
#endif
  free(socket);

#ifdef __EMSCRIPTEN__
  EM_ASM(
      // TODO: implement javascript equivalent code here
  );
#endif
}

// once for all
void Net__destroy() {
#ifdef _WIN32
  WSACleanup();
#endif

#ifdef __EMSCRIPTEN__
  EM_ASM({
      // TODO: implement javascript equivalent code here
  });
#endif
}

// -- Websockets --
#ifdef __EMSCRIPTEN__

int EMSCRIPTEN_KEEPALIVE libwebsocket_cb(
    int protocol,
    Socket* context,
    int socketId,
    int callbackFnId,
    Socket* socket,
    void* in,
    size_t len) {
  LOG_DEBUGF(
      "libwebsocket_cb() "
      "protocol %d, "
      "context %p, "
      "socketId %d, "
      "callbackFnId %d, "
      "socket %p, "
      "in %p, "
      "len %d",
      protocol,
      context,
      socketId,
      callbackFnId,
      socket,
      in,
      len);

  if (WS_CLIENT_ESTABLISHED == callbackFnId) {
    socket->connectCb(socket);
  }

  // if (reason == WS_WSI_DESTROY) {
  //   context->protocols[protocol].callback(context, wsi, reason, user, in, len);
  //   // TODO See if we need to destroy the user_data..currently we dont allocate it, so we never would need to free it.
  //   return 0;
  // } else {
  //   return context->protocols[protocol].callback(context, wsi, reason, user, in, len);
  // }

  return 0;
}

#endif