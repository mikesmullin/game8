#pragma once

#include <time.h>

#include "Types.h"

extern inline u64 Date__now() {
  return time(NULL);  // unix epoch
}