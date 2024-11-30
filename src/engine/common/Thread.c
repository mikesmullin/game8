#include "Thread.h"

bool Thread__Mutex_create(Mutex* m) {
#ifdef _WIN32
  m->_win = CreateMutex(NULL, FALSE, NULL);
  return m->_win != NULL;
#else
  return 0 != pthread_mutex_init(&m->_nix, NULL);
#endif
}

void Thread__Mutex_lock(Mutex* m) {
#ifdef _WIN32
  WaitForSingleObject(m->_win, INFINITE);
#else
  pthread_mutex_lock(&m->_nix);
#endif
}

void Thread__Mutex_unlock(Mutex* m) {
#ifdef _WIN32
  WaitForSingleObject(m->_win, INFINITE);
#else
  pthread_mutex_unlock(&m->_nix);
#endif
}

void Thread__Mutex_destroy(Mutex* m) {
#ifdef _WIN32
  CloseHandle(m->_win);
#else
  pthread_mutex_destroy(&m->nix);
#endif
}

bool Thread__create(Thread* t, thread_fn_t fn, void* userdata) {
#ifdef _WIN32
  t->_win = CreateThread(NULL, 0, fn, userdata, 0, NULL);
  return NULL != t->_win;
#else
  return 0 != pthread_create(&t->_nix, NULL, fn, userdata);
#endif
}

void Thread__join(Thread t[], u32 len) {
#ifdef _WIN32
  WaitForMultipleObjects(len, (const HANDLE*)t, TRUE, INFINITE);
#else
  for (u32 i = 0; i < len; i++) {
    pthread_join(t[i], NULL);
  }
#endif
}

void Thread__destroy(Thread t[], u32 len) {
#ifdef _WIN32
  for (int i = 0; i < len; i++) {
    CloseHandle(t[i]._win);
  }
#endif
}