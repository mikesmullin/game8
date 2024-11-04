#include "Net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Logic.h"
#include "Log.h"

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

extern Engine__State* g_engine;

void Net__init() {
#ifdef _WIN32
  WSADATA wsaData;
  int r = WSAStartup(MAKEWORD(2, 2), &wsaData);
  ASSERT_CONTEXT(0 == r, "WSAStartup failed with error: %d", r);
#endif
}

Socket* Net__Socket__alloc() {
  Socket* socket = malloc(sizeof(Socket));
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
}

void Net__accept(Socket* socket, void (*acceptCb)(Socket*, Socket*)) {
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
  if (r > 0 && FD_ISSET(socket->_win_socket, &readfds)) {
    // Accept a connection from a new client socket
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    Socket* csocket = Net__Socket__alloc();
    csocket->_win_socket =
        accept(socket->_win_socket, (struct sockaddr*)&clientAddr, &clientAddrLen);
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

    acceptCb(socket, csocket);
  }
#endif
}

void Net__connect(Socket* socket, void (*connectCb)(Socket*)) {
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

  connectCb(socket);
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
}

void Net__write(Socket* socket, u32 len, char* data) {
#ifdef _WIN32
  int r = send(socket->_win_socket, data, len, 0);
  ASSERT_CONTEXT(r != SOCKET_ERROR, "socket send failed with error: %d", WSAGetLastError());
#endif
}

// for non-listening sockets
void Net__shutdown(Socket* socket) {
#ifdef _WIN32
  int r = shutdown(socket->_win_socket, SD_BOTH);  // stop sending and receiving
  ASSERT_CONTEXT(r != SOCKET_ERROR, "socket shutdown failed with error: %d", WSAGetLastError());
#endif
}

// for all sockets
void Net__close(Socket* socket) {
#ifdef _WIN32
  closesocket(socket->_win_socket);
  socket->_win_socket = INVALID_SOCKET;
#endif
}

// for all sockets
void Net__free(Socket* socket) {
#ifdef _WIN32
  freeaddrinfo(socket->_win_addr);
#endif
  free(socket);
}

// once for all
void Net__destroy() {
#ifdef _WIN32
  WSACleanup();
#endif
}
