#pragma once

#define DEBUG_TRACE g_engine->log("*** TRACE %s:%u\n", __FILE__, __LINE__);

#define LOG_INFOF(s, ...) g_engine->log(s "\n", ##__VA_ARGS__);
#define LOG_DEBUGF(s, ...) g_engine->log(s "\n", ##__VA_ARGS__);

#include <stdlib.h>  // IWYU pragma: keep

#define ASSERT(cond)                                                                  \
  if (!(cond)) {                                                                      \
    g_engine->log(("Assertion failed: " #cond "\n  at %s:%u\n"), __FILE__, __LINE__); \
    abort();                                                                          \
  }
#define ASSERT_CONTEXT(cond, ctx, ...)                                     \
  if (!(cond)) {                                                           \
    g_engine->log(                                                         \
        ("Assertion failed: " #cond "\n  at %s:%u\n  Context: " ctx "\n"), \
        __FILE__,                                                          \
        __LINE__,                                                          \
        ##__VA_ARGS__);                                                    \
    abort();                                                               \
  }
#define ASSERT_EQUAL(a, b, ctx, ...)                                                            \
  if (a != b) {                                                                                 \
    g_engine->log(                                                                              \
        ("Assertion failed: " #a " (0x%x) == " #b " (0x%x)\n  at %s:%u\n  Context: " ctx "\n"), \
        (unsigned int)a,                                                                        \
        (unsigned int)b,                                                                        \
        __FILE__,                                                                               \
        __LINE__,                                                                               \
        ##__VA_ARGS__);                                                                         \
    abort();                                                                                    \
  }
