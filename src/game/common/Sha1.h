#pragma once

#include <stddef.h>
#include <stdint.h>
typedef uint8_t u8;
typedef uint32_t u32;

void sha1(u8* hash, const u8* data, size_t len);