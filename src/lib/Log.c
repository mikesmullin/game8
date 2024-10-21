#include <stdarg.h>
#include <stdio.h>

// TODO: consider using sokol log fn?
// g_engine->slog_func("test tag", 1, 1, "test msg", 28, "Logic.c", NULL);

void logit(const char* line, ...) {
  va_list myargs;
  va_start(myargs, line);
  vfprintf(stdout, line, myargs);
  va_end(myargs);
}