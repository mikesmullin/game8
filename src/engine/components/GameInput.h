#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef struct String8 String8;

typedef struct InputState {
  bool fwd;
  bool back;
  bool left;
  bool right;
  bool up;
  bool down;
  bool use;
  bool reload;
  bool esc;
} InputState;

typedef struct PointerState {
  s8 x, y;
  f32 wheely;
} PointerState;

typedef u8 ButtonRecord;
enum {
  INPUT_FWD = 1 << 0,  // Bit 0
  INPUT_BACK = 1 << 1,  // Bit 1
  INPUT_LEFT = 1 << 2,  // Bit 2
  INPUT_RIGHT = 1 << 3,  // Bit 3
  INPUT_UP = 1 << 4,  // Bit 4
  INPUT_DOWN = 1 << 5,  // Bit 5
  INPUT_USE = 1 << 6,  // Bit 6
  INPUT_RELOAD = 1 << 7,  // Bit 7
};

typedef struct InputRecord {  // 2 bytes per frame
  ButtonRecord btns;
  // TODO: pad 8 bits to align hex view
  s8 x, y;
} InputRecord;

typedef struct VirtualJoystick {
  f32 xAxis, yAxis, zAxis;
} VirtualJoystick;

typedef struct GameInputComponent {
  bool mouseCaptured;
  VirtualJoystick joy;
  PointerState ptr;
  InputState key;
} GameInputComponent;

typedef u8 DemoState;
enum {
  DEMO_STOP,
  DEMO_PLAY,
  DEMO_PAUSE,
  DEMO_RECORD,
};

typedef struct DemoRecording {
  DemoState state;
  String8* file;
  void* handle;
  u64 seed;
} DemoRecording;

extern inline bool GameInput__Demo__isRecording(const DemoRecording* demo) {
  return NULL != demo && DEMO_RECORD == demo->state;
}

extern inline bool GameInput__Demo__isPlaying(const DemoRecording* demo) {
  return NULL != demo && DEMO_PLAY == demo->state;
}

void GameInput__serialize(const GameInputComponent* input, InputRecord* record);
void GameInput__deserialize(const InputRecord* record, GameInputComponent* input);
void GameInput__Demo__alloc(Arena* arena, DemoRecording** demo, String8* file);
bool GameInput__Demo__open(DemoRecording* demo, const DemoState mode);
void GameInput__Demo__read(DemoRecording* demo, InputRecord* record);
void GameInput__Demo__write(const DemoRecording* demo, InputRecord* record);
void GameInput__Demo__close(DemoRecording* demo);
