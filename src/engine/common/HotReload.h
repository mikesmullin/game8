#pragma once

#include "Types.h"

#ifdef __EMSCRIPTEN__
#define HOT_RELOAD__EXPORT
#endif
#ifdef _WIN32
#define HOT_RELOAD__EXPORT __declspec(dllexport)
#else  // mac, nix
// TODO: figure out the correct value for import
#define HOT_RELOAD__EXPORT __attribute__((visibility("default")))
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
// TODO: there is also unimplemented nuance here for unit tests on non-Windows
#error "TODO: Finish HotReload support for macOS and Linux."
#endif

#ifdef _WIN32
typedef struct WinFileMonitor {
  char buffer[1024 * 5];  // may increase to detect all events (variable per frame)
  DWORD bytesReturned;
  HANDLE hDir;
  HANDLE hEvent;
  OVERLAPPED overlapped;
} WinFileMonitor;
#endif

typedef struct FileMonitor {
  const char* directory;
  const char* fileName;
  void (*cb)();

#ifdef _WIN32
  WinFileMonitor _win;
#endif
} FileMonitor;

u8 HotReload__load(const char* file);
u8 HotReload__unload(void);
u8 HotReload__StartMonitor(FileMonitor* fm);
u8 HotReload__CheckMonitor(FileMonitor* fm, char* file);
u8 HotReload__EndMonitor(const FileMonitor* fm);