#include "HotReload.h"

#include <stdio.h>

#include "Log.h"
#include "OS.h"

#define LOGIC_FILENAME "src\\game\\Logic.c.dll"

static void* logic = NULL;

logic_oninit_data_t logic_oninit_data;
logic_oninit_compute_t logic_oninit_compute;
logic_onreload_t logic_onreload;
logic_onfixedupdate_t logic_onfixedupdate;
logic_onupdate_t logic_onupdate;

#if OS_NIX == 1
#include <dlfcn.h>

#define LOAD_SYMBOL(name)                                                      \
  do {                                                                         \
    name = (name##_t)dlsym(logic, #name);                                      \
    if (name == NULL) {                                                        \
      fprintf(stderr, "ERROR could not find symbol %s: %s", #name, dlerror()); \
      return 1;                                                                \
    }                                                                          \
  } while (0)

int load_logic(void) {
  if (logic != NULL) {
    if (!unload_logic()) {
      return 1;
    }
  }

  logic = dlopen(LOGIC_FILENAME, RTLD_NOW);
  if (logic == NULL) {
    fprintf(stderr, "ERROR loading %s: %s\n", LOGIC_FILENAME, dlerror());
    return 1;
  }

  return 0;
}

int unload_logic(void) {
  if (dlclose(logic)) {
    fprintf(stderr, "ERROR unloading %s: %s\n", LOGIC_FILENAME, dlerror());
    return 1;
  }

  return 0;
}
#endif

// #if OS_WINDOWS == 1
#include <windows.h>

int load_logic(const char* file) {
  if (logic != NULL) {
    if (!unload_logic()) {
      return 0;
    }
  }
  LOG_DEBUGF("load lib %s", file);
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

  logic_oninit_data = (logic_oninit_data_t)GetProcAddress(logic, "logic_oninit_data");
  logic_oninit_compute = (logic_oninit_compute_t)GetProcAddress(logic, "logic_oninit_compute");
  logic_onreload = (logic_onreload_t)GetProcAddress(logic, "logic_onreload");
  logic_onfixedupdate = (logic_onfixedupdate_t)GetProcAddress(logic, "logic_onfixedupdate");
  logic_onupdate = (logic_onupdate_t)GetProcAddress(logic, "logic_onupdate");
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

// #endif