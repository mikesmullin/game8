#pragma once

#include "../../../../src/engine/Engine.h"

// Globals ---------------------------------------------

#define WINDOW_SIZE (640)
#define SCREEN_SIZE ((u32)(WINDOW_SIZE / 4))
#define LISTEN_ADDR "0.0.0.0"
#define LISTEN_PORT "9000"
#define CONNECT_ADDR "127.0.0.1"
#define MAX_CLIENTS (1)

// Components ----------------------------------------------

// Entities ----------------------------------------------

// Game ----------------------------------------------

typedef struct Logic__State {
} Logic__State;

HOT_RELOAD__EXPORT void logic_onbootstrap(Engine__State* engine);
