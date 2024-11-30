#pragma once

#include "GLMShim.h"
#include "Types.h"

f32 Math__random(f32 a, f32 b);
s32 Math__srandom(s32 a, s32 b);
u32 Math__urandom();
u32 Math__urandom2(u32 a, u32 b);

// --- HandmadeMath.h ---

#include "../../../vendor/HandmadeMath/HandmadeMath.h"