#pragma once

#include <stdint.h>
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;

typedef enum MessageId {
  MSG_NONE,
  MSG_MOVE_REQ,
} MessageId;

typedef struct Message {
  u32 id;
} Message;

typedef struct MoveRequest {
  Message base;
  f32 x, y, z;
} MoveRequest;

void Message__Parse(u32 len, u8** ptr);