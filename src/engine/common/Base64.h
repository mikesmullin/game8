#pragma once

#include "Types.h"

typedef struct Arena Arena;

char* base64_encode(Arena* arena, const unsigned char* input, size_t len);
unsigned char* base64_decode(Arena* arena, const char* input, size_t* out_len);