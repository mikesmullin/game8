#include "GameInput.h"

#include <stdio.h>

#include "../common/BitFlag.h"
#include "../common/Date.h"
#include "../common/Log.h"
#include "../common/String.h"

void GameInput__serialize(const GameInputComponent* input, InputRecord* record) {
  if (input->key.fwd) BitFlag__set8(&record->btns, INPUT_FWD);
  if (input->key.back) BitFlag__set8(&record->btns, INPUT_BACK);
  if (input->key.left) BitFlag__set8(&record->btns, INPUT_LEFT);
  if (input->key.up) BitFlag__set8(&record->btns, INPUT_UP);
  if (input->key.down) BitFlag__set8(&record->btns, INPUT_DOWN);
  if (input->key.use) BitFlag__set8(&record->btns, INPUT_USE);
  if (input->key.reload) BitFlag__set8(&record->btns, INPUT_RELOAD);
  record->x = input->ptr.x;
  record->y = input->ptr.y;
}

void GameInput__deserialize(const InputRecord* record, GameInputComponent* input) {
  input->key.fwd = BitFlag__hasOne8(record->btns, INPUT_FWD);
  input->key.back = BitFlag__hasOne8(record->btns, INPUT_BACK);
  input->key.left = BitFlag__hasOne8(record->btns, INPUT_LEFT);
  input->key.up = BitFlag__hasOne8(record->btns, INPUT_UP);
  input->key.down = BitFlag__hasOne8(record->btns, INPUT_DOWN);
  input->key.use = BitFlag__hasOne8(record->btns, INPUT_USE);
  input->key.reload = BitFlag__hasOne8(record->btns, INPUT_RELOAD);
  input->ptr.x = record->x;
  input->ptr.y = record->y;
}

void GameInput__Demo__alloc(Arena* arena, DemoRecording** demo, String8* file) {
  *demo = Arena__Push(arena, sizeof(DemoRecording));
  (*demo)->file = NULL != file ? file : String8__format(arena, 255, "%llu.demo", Date__now());
  (*demo)->state = DEMO_RECORD;
  (*demo)->seed = g_engine->seeds.main;
}

bool GameInput__Demo__open(DemoRecording* demo, const DemoState mode) {
#ifndef __EMSCRIPTEN__
  errno_t err =
      fopen_s((FILE**)&demo->handle, (char*)demo->file->str, (DEMO_RECORD == mode ? "ab" : "rb"));
  if (0 != err) {
    LOG_DEBUGF("Error opening demo. code: %u, file: %s", err, (char*)demo->file->str);
    demo->state = DEMO_STOP;
    return false;
  }
  // u64 sz = ftell(demo->handle);
  // LOG_DEBUGF("file size is %u", sz);
  demo->state = mode;
  return true;
#endif
}

static void _debug_record(const char* mode, const InputRecord* record) {
  char a[11], b[11], c[11];
  u82bin(a, record->btns);
  u82bin(b, record->x);
  u82bin(c, record->y);
  LOG_DEBUGF("%s btns %u %s x %d %s y %d %s", mode, record->btns, a, record->x, b, record->y, c);
}

void GameInput__Demo__read(DemoRecording* demo, InputRecord* record) {
#ifndef __EMSCRIPTEN__
  if (feof(demo->handle)) {
    // fseek(demo->handle, 0, SEEK_SET);  // loop
    demo->state = DEMO_STOP;
    LOG_DEBUGF("Reached end of playback.");
  }
  size_t bytesRead = fread_s(record, sizeof(InputRecord), sizeof(InputRecord), 1, demo->handle);
#endif

  _debug_record("replay", record);
}

void GameInput__Demo__write(const DemoRecording* demo, InputRecord* record) {
#ifndef __EMSCRIPTEN__
  fwrite(record, sizeof(InputRecord), 1, demo->handle);
#endif

  _debug_record("record", record);
}

void GameInput__Demo__close(DemoRecording* demo) {
  demo->state = DEMO_STOP;

#ifndef __EMSCRIPTEN__
  fclose(demo->handle);
#endif
}
