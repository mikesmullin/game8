#pragma once

#include "Types.h"

// NOTE: include multiple flags using Bitwise-OR (|)

// must have at least one flag
extern inline bool BitFlag__some(u64 value, u64 flags) {
  return (value & flags) != 0;
}

// must have all flags
extern inline bool BitFlag__every(u64 value, u64 flags) {
  return (value & flags) == flags;
}