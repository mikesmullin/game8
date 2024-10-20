#include <stdarg.h>
#include <stdio.h>

void logit(const char* line, ...) {
  va_list myargs;
  va_start(myargs, line);
  vfprintf(stdout, line, myargs);
  va_end(myargs);
}