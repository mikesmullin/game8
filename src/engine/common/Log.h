#pragma once

#include "../Engine.h"  // IWYU pragma: keep

void Log__init(void);
void Log__trace(const char* line, ...);
void Log__abort(const char* line, ...);

#define DEBUG_TRACE g_engine->log("*** TRACE %s:%u\n", __FILE__, __LINE__);
#define LOG_INFOF(s, ...) g_engine->log(s "\n", ##__VA_ARGS__);
#define LOG_DEBUGF(s, ...) g_engine->log(s "\n", ##__VA_ARGS__);

static void u82bin(char* c, u8 b) {
  c[0] = '0';
  c[1] = 'b';
  c[2] = (b & (1 << 0)) ? '1' : '0';
  c[3] = (b & (1 << 1)) ? '1' : '0';
  c[4] = (b & (1 << 2)) ? '1' : '0';
  c[5] = (b & (1 << 3)) ? '1' : '0';
  c[6] = (b & (1 << 4)) ? '1' : '0';
  c[7] = (b & (1 << 5)) ? '1' : '0';
  c[8] = (b & (1 << 6)) ? '1' : '0';
  c[9] = (b & (1 << 7)) ? '1' : '0';
  c[10] = '\0';  // null-terminated c_str
}

#define ASSERT(cond)                                                                    \
  if (!(cond)) {                                                                        \
    g_engine->abort(("Assertion failed: " #cond "\n  at %s:%u\n"), __FILE__, __LINE__); \
  }
#define ASSERT_CONTEXT(cond, ctx, ...)                                     \
  if (!(cond)) {                                                           \
    g_engine->abort(                                                       \
        ("Assertion failed: " #cond "\n  at %s:%u\n  Context: " ctx "\n"), \
        __FILE__,                                                          \
        __LINE__,                                                          \
        ##__VA_ARGS__);                                                    \
  }
#define ASSERT_EQUAL(a, b, ctx, ...)                                                            \
  if (a != b) {                                                                                 \
    g_engine->abort(                                                                            \
        ("Assertion failed: " #a " (0x%x) == " #b " (0x%x)\n  at %s:%u\n  Context: " ctx "\n"), \
        (unsigned int)a,                                                                        \
        (unsigned int)b,                                                                        \
        __FILE__,                                                                               \
        __LINE__,                                                                               \
        ##__VA_ARGS__);                                                                         \
  }
