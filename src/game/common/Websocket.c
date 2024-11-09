#include "Websocket.h"

#include <stdio.h>
#include <string.h>

#include "../Logic.h"
#include "Arena.h"
#include "Base64.h"
#include "Log.h"
#include "Sha1.h"
#include "Utils.h"

extern Engine__State* g_engine;

static const char* WS_HANDSHAKE_MAGIC = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

void Websocket__ParseHandshake(const char* handshake, char* key) {
  // GET / HTTP/1.1
  // Host: 127.0.0.1:9000
  // Connection: Upgrade
  // Pragma: no-cache
  // Cache-Control: no-cache
  // User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36
  // Upgrade: websocket
  // Origin: http://localhost:9090
  // Sec-WebSocket-Version: 13
  // Accept-Encoding: gzip, deflate, br, zstd
  // Accept-Language: en-US,en;q=0.9
  // Sec-WebSocket-Key: qBfzQt2X4U6P+KKnL/BLRQ==
  // Sec-WebSocket-Extensions: permessage-deflate; client_max_wind

  u8* ptr = (void*)handshake;
  u8* end = ptr + strlen(handshake);
  char line[256];
  while (mread(line, sizeof(line), &ptr, mindexOf(ptr, '\n', end - ptr))) {
    if (strncmp(line, "Sec-WebSocket-Key: ", 19) == 0) {
      msscanf(line, "Sec-WebSocket-Key: %s\r\n", key);
    }
  }
}

void WebSocket__RespondHandshake(Arena* arena, const char* key, char* response) {
  // Concatenate the client's Sec-WebSocket-Key with the magic string
  char a[256];
  sprintf(a, "%s%s", key, WS_HANDSHAKE_MAGIC);

  // Compute the SHA-1 hash of this concatenated string
  u8 hash[20];
  sha1(hash, (const u8*)a, strlen(a));

  // Base64 encode the result of the hash
  char* encoded = base64_encode(arena, (const unsigned char*)hash, 20);

  sprintf(  // TODO: use mprintf()
      response,
      "HTTP/1.1 101 Switching Protocols\r\n"
      "Upgrade: websocket\r\n"
      "Connection: Upgrade\r\n"
      "Sec-WebSocket-Accept: %s\r\n"
      // "Sec-WebSocket-Extensions: permessage-deflate\r\n"
      "\r\n",
      encoded);
}

// Define a maximum payload size (adjust as needed)
#define MAX_PAYLOAD_SIZE 65536

// Function to unmask the payload
static void unmask_payload(u8* payload, u64 length, u8 masking_key[4]) {
  for (u64 i = 0; i < length; i++) {
    payload[i] ^= masking_key[i % 4];
  }
}

// Function to parse a WebSocket frame from a buffer
int WebSocket__ParseFrame(
    Arena* arena, const u8* buffer, size_t buffer_len, u8** payload_out, u64* payload_length_out) {
  if (buffer_len < 2) {
    return -1;  // Incomplete frame
  }

  // Read the first two bytes
  u8 byte1 = buffer[0];
  u8 byte2 = buffer[1];

  // Check FIN and Opcode (Optional: handle different opcodes as needed)
  int fin = (byte1 >> 7) & 0x01;
  int opcode = byte1 & 0x0F;
  int masked = (byte2 >> 7) & 0x01;

  // Ensure the message is masked (required for client-to-server frames)
  if (!masked) {
    LOG_DEBUGF("Error: Received unmasked frame from client.");
    return -1;
  }

  // Get the payload length (7 bits)
  u64 payload_length = byte2 & 0x7F;
  size_t offset = 2;  // Initial offset in the buffer

  // Determine the actual payload length
  if (payload_length == 126) {
    // Extended payload length (16 bits)
    if (buffer_len < offset + 2) {
      return -1;  // Incomplete frame
    }
    payload_length = (buffer[offset] << 8) | buffer[offset + 1];
    offset += 2;
  } else if (payload_length == 127) {
    // Extended payload length (64 bits)
    if (buffer_len < offset + 8) {
      return -1;  // Incomplete frame
    }
    payload_length = 0;
    for (int i = 0; i < 8; i++) {
      payload_length = (payload_length << 8) | buffer[offset + i];
    }
    offset += 8;
  }

  // Check for buffer overflow
  if (payload_length > MAX_PAYLOAD_SIZE) {
    LOG_DEBUGF("Error: Payload too large.");
    return -1;
  }

  // Read the masking key (4 bytes)
  if (buffer_len < offset + 4) {
    return -1;  // Incomplete frame
  }
  u8 masking_key[4];
  memcpy(masking_key, buffer + offset, 4);
  offset += 4;

  // Ensure we have enough data for the payload
  if (buffer_len < offset + payload_length) {
    return -1;  // Incomplete frame
  }

  // Allocate memory for the payload and copy it
  u8* payload = (u8*)Arena__Push(arena, payload_length);
  if (!payload) {
    LOG_DEBUGF("Error: Memory allocation failed.");
    return -1;
  }
  memcpy(payload, buffer + offset, payload_length);

  // Unmask the payload
  unmask_payload(payload, payload_length, masking_key);

  // Set the output values
  *payload_out = payload;
  *payload_length_out = payload_length;

  return 0;  // Success
}