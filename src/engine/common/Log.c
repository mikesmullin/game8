#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void Log__out(const char* line, ...) {
  va_list myargs;
  va_start(myargs, line);
  vfprintf(stdout, line, myargs);
  va_end(myargs);
}

void Log__abort(const char* line, ...) {
  va_list myargs;
  va_start(myargs, line);
  vfprintf(stderr, line, myargs);
  va_end(myargs);
  fflush(stderr);
  abort();
}