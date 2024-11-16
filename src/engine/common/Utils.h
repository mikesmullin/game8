#pragma once

#include <stdio.h>  // IWYU pragma: keep

#include "../Engine.h"  // IWYU pragma: keep

u64 mindexOf(u8* src, char byte, s64 smaxlen);
u64 mread(void* dst, s64 readlen, u8** srcCursor, s64 smaxlen);
u64 msscanf(const char* input, const char* format, ...);
void mprintf(char** dstCursor, const char* format, s64 maxLen, ...);
void hexdump(const void* data, u32 len, char* out, u32 maxLen);

bool String__isEqual(char* a, char* b);
void* mmemcp(void* dst, const void* src, size_t sz);