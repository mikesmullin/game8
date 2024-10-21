#include "HotReload.h"

#include <stdio.h>
#include <windows.h>

#include "Log.h"

static void* logic = NULL;

logic_oninit_t logic_oninit;
logic_onpreload_t logic_onpreload;
logic_onreload_t logic_onreload;
logic_onfixedupdate_t logic_onfixedupdate;
logic_onupdate_t logic_onupdate;
logic_onshutdown_t logic_onshutdown;

int load_logic(const char* file) {
  if (logic != NULL) {
    if (!unload_logic()) {
      return 0;
    }
  }
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
    LOG_DEBUGF("ERROR loading %s: %s", LOGIC_FILENAME, (char*)errorMessage);

    // Free the buffer allocated by FormatMessage
    LocalFree(errorMessage);

    return 0;
  }

  logic_oninit = (logic_oninit_t)GetProcAddress(logic, "logic_oninit");
  logic_onpreload = (logic_onpreload_t)GetProcAddress(logic, "logic_onpreload");
  logic_onreload = (logic_onreload_t)GetProcAddress(logic, "logic_onreload");
  logic_onfixedupdate = (logic_onfixedupdate_t)GetProcAddress(logic, "logic_onfixedupdate");
  logic_onupdate = (logic_onupdate_t)GetProcAddress(logic, "logic_onupdate");
  logic_onshutdown = (logic_onshutdown_t)GetProcAddress(logic, "logic_onshutdown");
  return 1;
}

int unload_logic(void) {
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
    printf("ERROR freeing %s: %s\n", LOGIC_FILENAME, (char*)errorMessage);

    // Free the buffer allocated by FormatMessage
    LocalFree(errorMessage);

    return 0;
  }

  logic = NULL;

  return 1;
}

int File__StartMonitor(FileMonitor* fm) {
  // Open a handle to the directory
  // LOG_DEBUGF("watching dir: %s, file: %s", fm->directory, fm->fileName);
  fm->hDir = CreateFile(
      fm->directory,
      FILE_LIST_DIRECTORY,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
      NULL);

  if (fm->hDir == INVALID_HANDLE_VALUE) {
    LOG_DEBUGF("Failed to open directory. Error: %lu", GetLastError());
    return 1;
  }

  // Create an event for the OVERLAPPED structure
  fm->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (fm->hEvent == NULL) {
    LOG_DEBUGF("Failed to create event. Error: %lu", GetLastError());
    CloseHandle(fm->hDir);
    return 1;
  }

  // OVERLAPPED structure for asynchronous operations
  fm->overlapped = (OVERLAPPED){0};
  fm->overlapped.hEvent = fm->hEvent;

  // Read changes to the directory
  if (!ReadDirectoryChangesW(
          fm->hDir,
          &fm->buffer,
          sizeof(fm->buffer),
          FALSE,
          FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
          &fm->bytesReturned,
          &fm->overlapped,
          NULL)) {
    LOG_DEBUGF("Failed to read directory changes. Error: %lu", GetLastError());
    File__EndMonitor(fm);
    return 1;  // error
  }

  // LOG_DEBUGF("Monitoring for changes. directory: %s, file: %s", fm->directory, fm->fileName);

  return 0;
}

int File__CheckMonitor(FileMonitor* fm, char* file) {
  // Wait for the event to be signaled
  DWORD waitStatus = WaitForSingleObject(fm->hEvent, 0);

  int r = 0;

  if (waitStatus == WAIT_OBJECT_0) {
    // Event triggered by directory change

    FILE_NOTIFY_INFORMATION* pNotify;
    pNotify = (FILE_NOTIFY_INFORMATION*)fm->buffer;

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
    ResetEvent(fm->hEvent);
    if (!ReadDirectoryChangesW(
            fm->hDir,
            fm->buffer,
            sizeof(fm->buffer),
            FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE,
            &fm->bytesReturned,
            &fm->overlapped,
            NULL)) {
      LOG_DEBUGF("Failed to continue watching file. Error: %lu\n", GetLastError());
      return 1;
    }
  } else if (waitStatus == WAIT_TIMEOUT) {
    // this is expected most times
  } else {
    LOG_DEBUGF("Error: File watcher wait. Status: %X, Error: %lu\n", waitStatus, GetLastError());
    return 1;
  }

  return r;
}

int File__EndMonitor(const FileMonitor* fm) {
  // Close the directory handle
  CloseHandle(fm->hDir);
  CloseHandle(fm->hEvent);
  return 2;
}