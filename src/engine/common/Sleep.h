#pragma once

// NOTICE: Only for temporary debug. DO NOT use in final build.

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define SLEEP(ms) Sleep(ms);
#endif

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#define SLEEP(ms) sleep(ms);
#endif
