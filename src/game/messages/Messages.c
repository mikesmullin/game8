#include "Messages.h"

#include "../Logic.h"
#include "../common/Log.h"
#include "../common/Utils.h"

extern Engine__State* g_engine;

void Message__Parse(u32 len, u8** ptr) {
  u8* start = (*ptr);
  u8* end = start + len;

  Message msg;
  mread(&msg, sizeof(Message), ptr, end - (*ptr));

  if (MSG_MOVE_REQ == msg.id) {
    (*ptr) = start;
    MoveRequest self;
    mread(&self, sizeof(MoveRequest), ptr, end - (*ptr));
    LOG_DEBUGF("Move. id: %u, x: %f, y: %f, z: %f", self.base.id, self.x, self.y, self.z);
  } else {
    (*ptr) = start;
    LOG_DEBUGF("Unrecognized message. id: %02x", msg.id);
  }
}