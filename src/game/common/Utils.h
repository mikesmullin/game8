#pragma once

#include <stdint.h>
typedef uint8_t u8;
typedef uint64_t u64;
typedef int64_t s64;

u64 mindexOf(u8* src, char byte, s64 smaxlen);
u64 mread(void* dst, s64 readlen, u8** srcCursor, s64 smaxlen);
u64 msscanf(const char* input, const char* format, ...);
void mprintf(char** dstCursor, const char* format, s64 maxLen, ...);
