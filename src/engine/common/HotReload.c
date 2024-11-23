#include "HotReload.h"

#include <stdio.h>

#include "../Engine.h"

static void* logic = NULL;

u8 HotReload__load(const char* file) {
  if (!g_engine->useHotReload) return 1;
  if (logic != NULL) {
    if (!HotReload__unload()) {
      return 0;  // fail
    }
  }

#ifdef _WIN32
  // LOG_DEBUGF("load lib %s", file);
  logic = LoadLibrary(file);
  if (!logic) {
    // Retrieve the error code
    DWORD errorCode = GetLastError();

    // Print the error code
    LOG_DEBUGF("LoadLibrary failed with error code: %lu", errorCode);

    // Optionally, format the error message into a readable string
    LPVOID errorMessage;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&errorMessage,
        0,
        NULL);

    // Print the formatted error message
    LOG_DEBUGF("ERROR loading %s: %s", file, (char*)errorMessage);

    // Free the buffer allocated by FormatMessage
    LocalFree(errorMessage);

    return 0;  // fail
  }

  g_engine->onbootstrap = (void*)GetProcAddress(logic, "logic_onbootstrap");
#endif

  return 1;  // success
}

u8 HotReload__unload(void) {
  if (!g_engine->useHotReload) return 1;

#ifdef _WIN32
  if (FreeLibrary(logic) == 0) {
    // Retrieve the error code
    DWORD errorCode = GetLastError();

    // Print the error code
    printf("FreeLibrary failed with error code: %lu\n", errorCode);

    // Optionally, format the error message into a readable string
    LPVOID errorMessage;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&errorMessage,
        0,
        NULL);

    // Print the formatted error message
    printf("ERROR freeing logic dll: %s\n", (char*)errorMessage);

    // Free the buffer allocated by FormatMessage
    LocalFree(errorMessage);

    return 0;  // fail
  }
#endif

  logic = NULL;
  return 1;  // success
}

u8 HotReload__StartMonitor(FileMonitor* fm) {
  if (!g_engine->useHotReload) return 0;

#ifdef _WIN32
  // Open a handle to the directory
  // LOG_DEBUGF("watching dir: %s, file: %s", fm->directory, fm->fileName);
  fm->_win.hDir = CreateFile(
      fm->directory,
      FILE_LIST_DIRECTORY,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
      NULL);

  if (fm->_win.hDir == INVALID_HANDLE_VALUE) {
    LOG_DEBUGF("Failed to open directory. Error: %lu", GetLastError());
    return 1;  // fail
  }

  // Create an event for the OVERLAPPED structure
  fm->_win.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (fm->_win.hEvent == NULL) {
    LOG_DEBUGF("Failed to create event. Error: %lu", GetLastError());
    CloseHandle(fm->_win.hDir);
    return 1;  // fail
  }

  // OVERLAPPED structure for asynchronous operations
  fm->_win.overlapped = (OVERLAPPED){0};
  fm->_win.overlapped.hEvent = fm->_win.hEvent;

  // Read changes to the directory
  if (!ReadDirectoryChangesW(
          fm->_win.hDir,
          &fm->_win.buffer,
          sizeof(fm->_win.buffer),
          FALSE,
          FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
          &fm->_win.bytesReturned,
          &fm->_win.overlapped,
          NULL)) {
    LOG_DEBUGF("Failed to read directory changes. Error: %lu", GetLastError());
    HotReload__EndMonitor(fm);
    return 1;  // fail
  }

  // LOG_DEBUGF("Monitoring for changes. directory: %s, file: %s", fm->directory, fm->fileName);
#endif

  return 0;  // success
}

u8 HotReload__CheckMonitor(FileMonitor* fm, char* file) {
  if (!g_engine->useHotReload) return 0;

  u8 r = 0;

#ifdef _WIN32
  // Wait for the event to be signaled
  DWORD waitStatus = WaitForSingleObject(fm->_win.hEvent, 0);

  if (waitStatus == WAIT_OBJECT_0) {
    // Event triggered by directory change

    FILE_NOTIFY_INFORMATION* pNotify;
    pNotify = (FILE_NOTIFY_INFORMATION*)fm->_win.buffer;

    do {
      // Convert wide character file name to multibyte (UTF-8)
      char changedFileName[MAX_PATH];
      int len = WideCharToMultiByte(
          CP_UTF8,
          0,
          pNotify->FileName,
          pNotify->FileNameLength / sizeof(WCHAR),
          changedFileName,
          MAX_PATH,
          NULL,
          NULL);
      changedFileName[len] = '\0';  // Null terminate

      // Check if the changed file is the one we are monitoring
      // if (strcmp(changedFileName, fm->fileName) == 0) {
      const char* suffix = changedFileName + strlen(changedFileName) - 4;
      // LOG_DEBUGF("changed: %s", changedFileName);
      if (strcmp(suffix, ".dll") == 0) {
        // fm->cb();
        strcpy_s(file, (pNotify->FileNameLength / sizeof(WCHAR)) + 1, changedFileName);
        r = 2;
      }

      pNotify = (FILE_NOTIFY_INFORMATION*)((char*)pNotify + pNotify->NextEntryOffset);
    } while (pNotify->NextEntryOffset != 0);

    // Reset the event and continue monitoring
    ResetEvent(fm->_win.hEvent);
    if (!ReadDirectoryChangesW(
            fm->_win.hDir,
            fm->_win.buffer,
            sizeof(fm->_win.buffer),
            FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE,
            &fm->_win.bytesReturned,
            &fm->_win.overlapped,
            NULL)) {
      LOG_DEBUGF("Failed to continue watching file. Error: %lu\n", GetLastError());
      return 1;  // fail
    }
  } else if (waitStatus == WAIT_TIMEOUT) {
    // this is expected most times
  } else {
    LOG_DEBUGF("Error: File watcher wait. Status: %X, Error: %lu\n", waitStatus, GetLastError());
    return 1;  // fail
  }
#endif

  return r;  // 0 = success, 1 = fail
}

u8 HotReload__EndMonitor(const FileMonitor* fm) {
  if (!g_engine->useHotReload) return 0;

#ifdef _WIN32
  // Close the directory handle
  CloseHandle(fm->_win.hDir);
  CloseHandle(fm->_win.hEvent);
#endif

  return 0;  // success (always)
}