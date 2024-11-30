#include "Easing.h"

#include "Math.h"

f32 easeInQuart(f32 t) {
  return t * t * t * t;
}

f32 easeOutQuart(f32 t) {
  return 1 - Math__pow4(1 - t);
}

f32 easeInOutQuart(f32 t) {  // in/out 0 .. 1
  return t < 0.5 ? 8 * t * t * t * t : 1 - Math__pow4(-2 * t + 2) / 2;
}
