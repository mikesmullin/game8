#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef struct VirtualJoystick {
  f32 xAxis, yAxis, zAxis;
} VirtualJoystick;

typedef struct PointerState {
  s32 x, y;
  f32 wheely;
} PointerState;

#define KEYCODE_W 87
#define KEYCODE_A 65
#define KEYCODE_S 83
#define KEYCODE_D 68
#define KEYCODE_E 69
#define KEYCODE_Q 81
#define KEYCODE_TAB 258
#define KEYCODE_SPACE 32
#define KEYCODE_LCTRL 341
#define KEYCODE_R 82
#define KEYCODE_ESC 256

typedef struct InputState {
  bool fwd;
  bool back;
  bool left;
  bool right;
  bool use;
  bool up;
  bool down;
  bool reload;
  bool esc;
} InputState;

typedef struct GameInputComponent {
  bool mouseCaptured;
  VirtualJoystick joy;
  PointerState ptr;
  InputState key;
} GameInputComponent;