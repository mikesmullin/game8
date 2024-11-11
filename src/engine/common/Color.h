#pragma once

#include "Types.h"

#define BLACK 0xff000000
#define LIME 0xff00ff00
#define BLUE 0xffff0000
#define PINK 0xffff00ff
#define WHITE 0xffffffff
#define TRANSPARENT 0x00000000

u32 alpha_blend(u32 bottom, u32 top);