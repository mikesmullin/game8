#pragma once

#include <stddef.h>  // NULL
#include <stdlib.h>  // IWYU pragma: keep  // malloc(), exit()

//
#include <stdbool.h>  // true, false
#include <stdint.h>  // fixed-width types

// convenience aliases
typedef int8_t s8;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t s32;
typedef int64_t s64;
typedef float f32;
typedef double f64;

// #ifdef _WIN32
// #ifdef __linux__
// #ifdef __APPLE__
// #ifdef __EMSCRIPTEN__
