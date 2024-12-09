#pragma once

#include "Types.h"

extern inline void BitFlag__set8(u8* collection, const u8 item) {
  *collection |= item;
}

extern inline bool BitFlag__hasOne8(const u8 collection, const u8 item) {
  return (collection & item) != 0;
}

extern inline bool BitFlag__hasOne64(const u64 collection, const u64 item) {
  return (collection & item) != 0;
}

// NOTE: include multiple flags using Bitwise-OR (|)
extern inline bool BitFlag__hasAll64(const u64 collection, const u64 item) {
  return (collection & item) == item;
}