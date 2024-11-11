#pragma once

#include "../../engine/common/Types.h"

typedef enum MessageId /* : u32 */ {
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