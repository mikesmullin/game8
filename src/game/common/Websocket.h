#pragma once

#include <stddef.h>
#include <stdint.h>
typedef uint8_t u8;
typedef uint64_t u64;

typedef struct Arena Arena;

void Websocket__ParseHandshake(const char* handshake, char* key);
void WebSocket__RespondHandshake(Arena* arena, const char* key, char* response);
int WebSocket__ParseFrame(
    Arena* arena, const u8* buffer, size_t buffer_len, u8** payload_out, u64* payload_length_out);