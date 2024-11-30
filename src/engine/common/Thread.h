#pragma once

#include "Types.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>  // POSIX (Linux, macOS)
#endif

// TODO: add Emscripten WebWorker or WASM Threads support

typedef struct Mutex {
#ifdef _WIN32
  HANDLE _win;
#else
  pthread_mutex_t _nix;
#endif
} Mutex;

#ifdef _WIN32
#define THREAD_FN_RET DWORD WINAPI
#else
#define THREAD_FN_RET void*
#endif

#ifdef _WIN32
#define THREAD_FN_RET_VAL (0)
#else
#define THREAD_FN_RET_VAL (NULL)
#endif

#ifdef _WIN32
#define THREAD_FN_PARAM1 LPVOID
#else
#define THREAD_FN_PARAM1 void*
#endif

typedef THREAD_FN_RET (*thread_fn_t)(THREAD_FN_PARAM1);

typedef struct Thread {
#ifdef _WIN32
  HANDLE _win;
#else
  pthread_t _nix;
#endif
} Thread;

bool Thread__Mutex_create(Mutex* m);
void Thread__Mutex_lock(Mutex* m);
void Thread__Mutex_unlock(Mutex* m);
void Thread__Mutex_destroy(Mutex* m);

bool Thread__create(Thread* t, thread_fn_t fn, void* userdata);
void Thread__join(Thread t[], u32 len);
void Thread__destroy(Thread t[], u32 len);