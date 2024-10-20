#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef int8_t s8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

typedef struct Arena_t Arena_t;
typedef struct Engine__State_t Engine__State_t;
typedef struct Logic__State_t Logic__State_t;

typedef struct Engine__State_t {
  Arena_t* arena;
  Logic__State_t* local;
} Engine__State_t;

typedef void (*logic_oninit_data_t)(Engine__State_t* state);
typedef void (*logic_oninit_compute_t)(Engine__State_t* state);
typedef void (*logic_onreload_t)(Engine__State_t* state);
typedef void (*logic_onfixedupdate_t)(Engine__State_t* state);
typedef void (*logic_onupdate_t)(Engine__State_t* state);

typedef struct Logic__State_t {
  u64 now;
  f32 red;
} Logic__State_t;