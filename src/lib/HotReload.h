#pragma once

#include <windows.h>

#include "../game/Logic.h"

#define LOGIC_FILENAME "src\\game\\Logic.c.dll"

extern logic_oninit_t logic_oninit;
extern logic_onpreload_t logic_onpreload;
extern logic_onreload_t logic_onreload;
extern logic_onfixedupdate_t logic_onfixedupdate;
extern logic_onupdate_t logic_onupdate;
extern logic_onshutdown_t logic_onshutdown;

int load_logic(const char* file);
int unload_logic(void);

typedef struct FileMonitor {
  const char* directory;
  const char* fileName;
  char buffer[1024 * 5];  // may increase to detect all events (variable per frame)
  DWORD bytesReturned;
  HANDLE hDir;
  HANDLE hEvent;
  OVERLAPPED overlapped;
  void (*const cb)();
} FileMonitor;

int File__StartMonitor(FileMonitor* fm);
int File__CheckMonitor(FileMonitor* fm, char* file);
int File__EndMonitor(const FileMonitor* fm);