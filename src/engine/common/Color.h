#pragma once

#include "Types.h"

#define COLOR_BLACK 0xff000000
#define COLOR_LIME 0xff00ff00
#define COLOR_BLUE 0xffff0000
#define COLOR_PINK 0xffff00ff
#define COLOR_WHITE 0xffffffff
#define COLOR_TRANSPARENT 0x00000000

u32 alpha_blend(u32 bottom, u32 top);