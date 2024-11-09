#pragma once

#include <stddef.h>
#include <stdint.h>
typedef uint8_t u8;
typedef uint32_t u32;

typedef struct Arena Arena;

char* base64_encode(Arena* arena, const unsigned char* input, size_t len);
unsigned char* base64_decode(Arena* arena, const char* input, size_t* out_len);