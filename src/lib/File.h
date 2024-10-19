#pragma once

#include <windows.h>

typedef struct {
  const char* directory;
  const char* fileName;
  char buffer[1024 * 5];  // may increase to detect all events (variable per frame)
  DWORD bytesReturned;
  HANDLE hDir;
  HANDLE hEvent;
  OVERLAPPED overlapped;
  void (*const cb)();
} FileMonitor_t;

int File__StartMonitor(FileMonitor_t* fm);
int File__CheckMonitor(FileMonitor_t* fm, char* file);
int File__EndMonitor(const FileMonitor_t* fm);