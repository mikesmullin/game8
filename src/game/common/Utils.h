#pragma once

#include <stdint.h>
typedef uint8_t u8;
typedef uint64_t u64;
typedef int64_t s64;

// typedef struct MRange {
//   size_t sz;
//   const void* p;
// } MRange;

// #define MRANGE(x)            \
//   (MRange) {                 \
//     .sz = sizeof(x), .p = &x \
//   }

// static const void* mnullptr = &mnullptr;

u64 mindexOf(u8* src, char byte, s64 smaxlen);
u64 mread(void* dst, s64 readlen, u8** srcCursor, s64 smaxlen);
u64 msscanf(const char* input, const char* format, ...);
void mprintf(char** dstCursor, const char* format, s64 maxLen, ...);
