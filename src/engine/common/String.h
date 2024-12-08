#pragma once

#include "Types.h"

typedef struct String8 {
  u32 sz;
  u8* str;
} String8;

typedef struct Arena Arena;
typedef struct List List;

// construction

// heap, uninitialized
String8* String8__alloc(Arena* arena, const u32 sz);

// heap, by copy
String8* String8__cp(Arena* arena, const String8* s);

// heap, wrap struct, by ref
// WARNING: user needs to observe len, since null-terminator is not guaranteed!
String8* String8__slice(Arena* arena, const u8* start, const u8* end);

u64 String8__cstrlen(const u8* c);

// heap, wrap c_str, by ref, size by null-terminator
String8* String8__cstr(Arena* arena, const char* c_str);

// slicing

// mutates input
void String8__skip(String8* s, u32 offset);

// heap, by ref
String8* String8__substr(Arena* arena, const String8* str, u32 start, u32 end);

// format

String8* String8__toUpper(Arena* arena, const String8* s);
String8* String8__toLower(Arena* arena, const String8* s);
String8* String8__printf(Arena* arena, u32 len, const char* fmt, ...);

// classification

extern inline bool String8__isSpaceC(u8 c) {
  return ' ' == c || '\n' == c || '\t' == c || '\r' == c || '\f' == c || '\v' == c;
}

extern inline bool String8__isUpperC(u8 c) {
  return 'A' <= c && c <= 'Z';
}

extern inline bool String8__isLowerC(u8 c) {
  return 'a' <= c && c <= 'z';
}

extern inline bool String8__isAlphaC(u8 c) {
  return String8__isUpperC(c) || String8__isLowerC(c);
}

extern inline bool String8__isNumericC(u8 c) {
  return '0' <= c && c <= '9';
}

extern inline bool String8__isDigitC(u8 c) {
  return String8__isNumericC(c) || '.' == c;
}

extern inline bool String8__isSlashC(u8 c) {
  return (c == '/' || c == '\\');
}

// format

extern inline u8 String8__toLowerC(u8 c) {
  return String8__isUpperC(c) ? c + ('a' - 'A') : c;
}

extern inline u8 String8__toUpperC(u8 c) {
  return String8__isLowerC(c) ? c + ('A' - 'a') : c;
}

extern inline u8 String8__toNixPathC(u8 c) {
  return String8__isSlashC(c) ? '/' : c;
}

// comparison

bool String8__cmp(String8* a, String8* b);
bool String8__ncmp(String8* a, String8* b);
s32 String8__indexOf(String8* haystack, u32 start, String8* needle);
bool String8__startsWith(String8* haystack, String8* needle);

// parsing

List* String8__split(Arena* arena, String8* str, u8 separator, u32 limit);
String8* String8__join(Arena* arena, List* list, u8 separator);
