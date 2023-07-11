#include <math.h>

#include "celero/Celero.h"

CELERO_MAIN

float Q_rsqrt(float number) {
  long        i;
  float       x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y  = number;
  i  = *(long*)&y;
  i  = 0x5f3759df - (i >> 1);
  y  = *(float*)&i;
  y  = y * (threehalfs - (x2 * y * y));

  return y;
}

class demo_simple_fixture : public celero::TestFixture { };

BASELINE_F(isqrt, native, demo_simple_fixture, 100, 10000000) {
  auto result = 1.0f / sqrtf(2.0f);
  celero::DoNotOptimizeAway(result != 0);
}

BENCHMARK_F(isqrt, carmack, demo_simple_fixture, 100, 10000000) {
  auto result = Q_rsqrt(2.0f);
  celero::DoNotOptimizeAway(result != 0);
}
