#include "Utils.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Logic.h"
#include "Math.h"

extern Engine__State* g_engine;

u64 mindexOf(u8* src, char byte, s64 smaxlen) {
  if (smaxlen < 1) return 0;  // not possible to find
  u8* start = src;
  u8* end = src + smaxlen;
  while (src < end) {
    if (*src == byte) {
      return src - start + 1;  // found
    }
    src++;
  }
  return 0;  // not found
}

u64 mread(void* dst, s64 readlen, u8** srcCursor, s64 smaxlen) {
  // validate
  s64 r = Math__min(readlen, smaxlen);
  if (r < 1 || NULL == srcCursor) return 0;

  // copy requested bytes up to maxlen
  memcpy(dst, *srcCursor, r);
  // advance pointer similar to fread()
  (*srcCursor) += r;
  // return count of bytes read
  return r;
}

void mprintf(char** dstCursor, const char* format, s64 maxLen, ...) {
  if (maxLen < 1) return;

  va_list args;
  va_start(args, maxLen);
  const char* arg = NULL;
  vsnprintf(*dstCursor, maxLen, format, args);
  va_end(args);

  // advance pointer similar to fread()
  u32 r = strlen(*dstCursor);
  (*dstCursor) += r;
}

u64 msscanf(const char* input, const char* format, ...) {
  va_list args;
  va_start(args, format);

  u32 matched_items = 0;
  const char* p_format = format;
  const char* p_input = input;

  while (*p_format && *p_input) {
    if (*p_format == '%') {
      p_format++;  // Move past '%'

      if (*p_format == 'd') {  // Handle integer
        u32* p_int = va_arg(args, u32*);
        u32 val = 0;
        u32 sign = 1;

        // Skip whitespace
        while (isspace(*p_input)) p_input++;

        // Handle optional sign
        if (*p_input == '-') {
          sign = -1;
          p_input++;
        }

        // Parse integer
        if (isdigit(*p_input)) {
          while (isdigit(*p_input)) {
            val = val * 10 + (*p_input - '0');
            p_input++;
          }
          *p_int = val * sign;
          p_format++;  // Move past 'd'
          matched_items++;
        } else {
          break;  // Failed to match an integer
        }

      } else if (*p_format == 'f') {  // Handle float
        f32* p_float = va_arg(args, f32*);
        f32 val = 0.0f;
        f32 sign = 1.0f;
        f32 fraction = 0.0f;
        f32 divisor = 1.0f;

        // Skip whitespace
        while (isspace(*p_input)) p_input++;

        // Handle optional sign
        if (*p_input == '-') {
          sign = -1.0f;
          p_input++;
        }

        // Parse integer part of the float
        if (isdigit(*p_input)) {
          while (isdigit(*p_input)) {
            val = val * 10.0f + (*p_input - '0');
            p_input++;
          }
        }

        // Parse fractional part
        if (*p_input == '.') {
          p_input++;  // Skip '.'
          while (isdigit(*p_input)) {
            fraction = fraction * 10.0f + (*p_input - '0');
            divisor *= 10.0f;
            p_input++;
          }
          val += fraction / divisor;
        }

        *p_float = val * sign;
        p_format++;  // Move past 'f'
        matched_items++;

      } else if (*p_format == 's') {  // Handle string
        char* p_str = va_arg(args, char*);
        size_t buf_size = va_arg(args, size_t);

        // Skip whitespace
        while (isspace(*p_input)) p_input++;

        size_t i = 0;
        while (*p_input && !isspace(*p_input) && i < buf_size - 1) {
          p_str[i++] = *p_input++;
        }
        p_str[i] = '\0';  // Null-terminate the string
        p_format++;  // Move past 's'
        matched_items++;
      } else {
        // Unsupported format specifier
        break;
      }
    } else if (isspace(*p_format)) {
      // Skip spaces in format
      while (isspace(*p_format)) p_format++;
      while (isspace(*p_input)) p_input++;
    } else {
      // Literal character match in the format
      if (*p_format == *p_input) {
        p_format++;
        p_input++;
      } else {
        break;  // Mismatch between input and format
      }
    }
  }

  va_end(args);
  return matched_items;
}

f32 mwave(f32 ms, f32 a, f32 b) {
  return Math__map(Math__sinf(g_engine->now / ms), -1, 1, a, b);
}

void hexdump(const void* data, u32 len, char* out, u32 maxLen) {
  const u8* byte = (const u8*)data;
  for (u32 i = 0; i < len; i += 16) {
    // Print the offset
    mprintf(&out, "%08x  ", maxLen, i);

    // Print the hexadecimal representation
    for (u32 j = 0; j < 16; j++) {
      if (i + j < len)
        mprintf(&out, "%02x ", maxLen, byte[i + j]);
      else
        mprintf(&out, "   ", maxLen);  // for padding
    }

    // Print the ASCII representation
    mprintf(&out, " |", maxLen);
    for (u32 j = 0; j < 16; j++) {
      if (i + j < len) {
        char c = byte[i + j];
        mprintf(&out, "%c", maxLen, isprint(c) ? c : '.');
      } else {
        mprintf(&out, " ", maxLen);
      }
    }
    mprintf(&out, "|\n", maxLen);
  }
}