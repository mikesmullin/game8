#pragma once

#include <math.h>

typedef float f32;

f32 easeInQuart(f32 t) {
  return t * t * t * t;
}

f32 easeOutQuart(f32 t) {
  return 1 - pow(1 - t, 4);
}

f32 easeInOutQuart(f32 t) {  // in/out 0 .. 1
  return t < 0.5 ? 8 * t * t * t * t : 1 - pow(-2 * t + 2, 4) / 2;
}
