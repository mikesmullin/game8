#include "String.h"

#include "Arena.h"
#include "List.h"
#include "Utils.h"

// construction

// heap, uninitialized
String8* String8__alloc(Arena* arena, const u32 sz) {
  String8* s = Arena__Push(arena, sizeof(String8));
  s->str = Arena__Push(arena, sz + 1);
  s->str[sz] = '\0';  // null-terminated
  s->sz = sz;
  return s;
}

// heap, by copy
String8* String8__cp(Arena* arena, const String8* s) {
  String8* s2 = String8__alloc(arena, s->sz);
  mmemcp(s2->str, s->str, s->sz);
  return s2;
}

// heap, wrap struct, by ref
// WARNING: user needs to observe len, since null-terminator is not guaranteed!
String8* String8__slice(Arena* arena, const u8* start, const u8* end) {
  String8* s = Arena__Push(arena, sizeof(String8));
  s->str = (u8*)start;
  s->sz = (u32)(end - start);
  return s;
}

u64 String8__cstrlen(const u8* c) {
  u8* p;
  for (p = (u8*)c; *p != 0; p += 1);
  return p - c;
}

// heap, wrap c_str, by ref, size by null-terminator
String8* String8__cstr(Arena* arena, const char* c_str) {
  String8* s = Arena__Push(arena, sizeof(String8));
  s->str = (u8*)c_str;
  s->sz = String8__cstrlen((u8*)c_str);
  return s;
}

// slicing

// mutates input
void String8__skip(String8* s, u32 offset) {
  offset = Math__min(offset, s->sz);
  s->str += offset;
  s->sz -= offset;
}

// heap, by ref
String8* String8__substr(Arena* arena, const String8* str, u32 start, u32 end) {
  u32 min = Math__min(start, str->sz);
  u32 max = Math__min(end, str->sz);
  return String8__slice(arena, str->str + min, str->str + max - min);
}

// format

String8* String8__toUpper(Arena* arena, const String8* s) {
  String8* s2 = String8__alloc(arena, s->sz);
  for (u32 i = 0; i < s2->sz; i++) {
    s2->str[i] = String8__toUpperC(s->str[i]);
  }
  return s2;
}

String8* String8__toLower(Arena* arena, const String8* s) {
  String8* s2 = String8__alloc(arena, s->sz);
  for (u32 i = 0; i < s2->sz; i++) {
    s2->str[i] = String8__toLowerC(s->str[i]);
  }
  return s2;
}

String8* String8__format(Arena* arena, u32 len, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  u8 buf[len];
  vsprintf_s((char*)buf, len, fmt, args);
  u32 l = String8__cstrlen(buf);
  String8* s = String8__alloc(arena, l);
  mmemcp(s->str, buf, l);
  va_end(args);
  return s;
}

// comparison

// exact-match, case-sensitive
bool String8__cmp(const String8* a, const String8* b) {
  if (a->sz != b->sz) return false;
  for (u32 i = 0; i < a->sz; i++) {
    if (a->str[i] != b->str[i]) return false;
  }
  return true;
}

// match, non-case-sensitive
bool String8__ncmp(const String8* a, const String8* b) {
  if (a->sz != b->sz) return false;
  for (u32 i = 0; i < a->sz; i++) {
    if (String8__toUpperC(a->str[i]) != String8__toUpperC(b->str[i])) return false;
  }
  return true;
}

s32 String8__indexOf(const String8* haystack, u32 start, const String8* needle) {
  // ensure the needle can fit within the haystack starting from `start`
  if (needle->sz > haystack->sz - start) return -1;

  // search haystack
  for (u32 i = start; i <= haystack->sz - needle->sz; i++) {
    bool found = true;

    // check if the needle matches at this position
    for (u32 j = 0; j < needle->sz; j++) {
      if (haystack->str[i + j] != needle->str[j]) {
        found = false;
        break;
      }
    }

    if (found) return i;
  }
  return -1;  // not found
}

bool String8__startsWith(const String8* haystack, const String8* needle) {
  // ensure the needle can fit within the haystack
  if (needle->sz > haystack->sz) return false;

  // search haystack
  // check if the needle matches at this position
  for (u32 j = 0; j < needle->sz; j++) {
    if (haystack->str[j] != needle->str[j]) return false;
  }
  return true;
}

// parsing

List* String8__split(Arena* arena, const String8* str, u8 separator, u32 limit) {
  List* list = List__alloc(arena);
  u8* ptr = str->str;
  u8* len = str->str + str->sz;
  for (; ptr < len;) {
    u8* first = ptr;
    for (; ptr < len; ptr++) {
      if (separator == *ptr) break;
    }
    String8* chunk = String8__slice(arena, first, ptr);
    List__append(arena, list, chunk);
    ptr++;
    if (0 != limit && limit <= list->len) break;
  }
  return list;
}

String8* String8__join(Arena* arena, const List* list, u8 separator) {
  u32 len = 0;
  List__Node* c = list->head;
  for (u32 i = 0; i < list->len; i++) {
    String8* s = c->data;
    len += s->sz;
    c = c->next;
  }
  len += list->len - 1;  // separators
  String8* r = String8__alloc(arena, len);
  u32 ii = 0;
  c = list->head;
  for (u32 i = 0; i < list->len; i++) {
    String8* s = c->data;
    if (i > 0) r->str[ii++] = separator;
    for (u32 j = 0; j < s->sz; j++) {
      r->str[ii++] = s->str[j];
    }
    c = c->next;
  }
  return r;
}