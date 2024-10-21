#pragma once

#include <stdint.h>
typedef uint32_t u32;
typedef float f32;

static u32 BLACK = 0xff000000;
static u32 LIME = 0xff00ff00;
static u32 BLUE = 0xffff0000;
static u32 PINK = 0xffff00ff;
static u32 WHITE = 0xffffffff;
static u32 TRANSPARENT = 0x00000000;

u32 alpha_blend(u32 bottom, u32 top);