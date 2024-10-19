#include "File.h"

#include <stdio.h>
#include <windows.h>

#include "Log.h"

int File__StartMonitor(FileMonitor_t* fm) {
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

  LOG_DEBUGF("Monitoring for changes. directory: %s, file: %s", fm->directory, fm->fileName);

  return 0;
}

int File__CheckMonitor(FileMonitor_t* fm, char* file) {
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

  return r;  // no matching changes
}

int File__EndMonitor(const FileMonitor_t* fm) {
  // Close the directory handle
  CloseHandle(fm->hDir);
  CloseHandle(fm->hEvent);
  return 2;
}
