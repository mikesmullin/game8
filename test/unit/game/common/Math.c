#define ENGINE__NO_MAIN
#include "../../../../src/engine/Engine.h"  // IWYU pragma: keep

//

#include "../../../../src/engine/common/Math.h"

// @describe Math
// @tag common
int main() {
  v3 a = {1.0f, 2.0f, 3.0f};
  v3 b = v3_cp(&a);
  a.x = 4;
  ASSERT(b.x == 1);

  f32 c = v3_dot(&b, &a);
  ASSERT(c == 17.0f);

  f32 d = v3_mag2(&b);
  ASSERT(d == 14.0f);

  f32 e = v3_mag(&b);
  // ASSERT(e == 3.74165750f); // math.h
  ASSERT(e == 3.74165750f);  // approx (7-digit accuracy)

  f32 f2;
  f2 = Math__sinf(1.23f);
  // ASSERT(f1 == 0.942488790f);  // math.h
  ASSERT(f2 == 0.942488849f);  // approx (7-digit accuracy)

  f2 = Math__cosf(1.23f);
  // ASSERT(f1 == 0.334237695f);  // math.h
  ASSERT(f2 == 0.334237754f);  // approx (7-digit accuracy)

  f2 = Math__tanf(1.23f);
  // ASSERT(f1 == 2.81981587f);  // math.h
  ASSERT(f2 == 2.81981587f);  // approx (7-digit accuracy)

  f2 = Math__atanf(1.23f);
  // ASSERT(f1 == 0.888173759f);  // math.h
  ASSERT(f2 == 0.888173759f);  // approx (6-digit accuracy)

  f2 = Math__atan2f(1.23f, 1.45f);
  // ASSERT(f1 == 0.703492224f);  // math.h
  ASSERT(f2 == 0.703492284f);  // approx (6-digit accuracy)

  u64 seed = 8008135ULL, prng1 = seed;
  f32 r = Math__randomf(0.0f, 10.0f, &prng1);
  ASSERT(r == 1.25978529f);
  ASSERT(prng1 == 11400714819331206620ULL);

  r = Math__randomf(0.0f, 10.0f, &prng1);
  ASSERT(r == 4.78351068f);
  ASSERT(prng1 == 4354685564944853489ULL);

  u32 u = Math__ceil(1.23f);
  ASSERT(u == 2);

  u = Math__floor(1.23f);
  ASSERT(u == 1);

  f32 f1 = acosf(0.23f);
  f2 = Math__acosf(0.23f);
  ASSERT(f1 == 1.33871865f);
  ASSERT(f2 == 1.33874631f);

  v3* a1 = &(v3){1, 2, 3};
  v3* a2 = &(v3){4, 5, 6};
  f2 = v3_dot(a1, a2);
  ASSERT(f2 == 32.0f);

  m4 p1;
  m4_persp(&p1, 45.0f, 1.0f, 0.1f, 100.0f);
  ASSERT(p1.ax == 1.79259121f);
  ASSERT(p1.ay == 0.0f);
  ASSERT(p1.az == 0.0f);
  ASSERT(p1.aw == 0.0f);

  return 0;
}
