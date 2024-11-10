#pragma once

void logit(const char* line, ...);
void logabort(const char* line, ...);

#define DEBUG_TRACE logit("*** TRACE %s:%u\n", __FILE__, __LINE__);

#define LOG_INFOF(s, ...) logit(s "\n", ##__VA_ARGS__);
#define LOG_DEBUGF(s, ...) logit(s "\n", ##__VA_ARGS__);

#define ASSERT(cond)                                                             \
  if (!(cond)) {                                                                 \
    logabort(("Assertion failed: " #cond "\n  at %s:%u\n"), __FILE__, __LINE__); \
  }
#define ASSERT_CONTEXT(cond, ctx, ...)                                     \
  if (!(cond)) {                                                           \
    logabort(                                                              \
        ("Assertion failed: " #cond "\n  at %s:%u\n  Context: " ctx "\n"), \
        __FILE__,                                                          \
        __LINE__,                                                          \
        ##__VA_ARGS__);                                                    \
  }
#define ASSERT_EQUAL(a, b, ctx, ...)                                                            \
  if (a != b) {                                                                                 \
    logabort(                                                                                   \
        ("Assertion failed: " #a " (0x%x) == " #b " (0x%x)\n  at %s:%u\n  Context: " ctx "\n"), \
        (unsigned int)a,                                                                        \
        (unsigned int)b,                                                                        \
        __FILE__,                                                                               \
        __LINE__,                                                                               \
        ##__VA_ARGS__);                                                                         \
  }
