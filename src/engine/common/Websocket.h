#pragma once

#include "Types.h"

typedef struct Arena Arena;

void Websocket__ParseHandshake(const char* handshake, char* key);
void WebSocket__RespondHandshake(Arena* arena, const char* key, char* response);
int WebSocket__ParseFrame(
    Arena* arena, const u8* buffer, size_t buffer_len, u8** payload_out, u64* payload_length_out);