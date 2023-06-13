#if defined(__AVX2__) || defined(__AVX512F__)
#include <immintrin.h>
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

#if defined(__ARM_FEATURE_SVE)
#include <arm_sve.h>
#endif

#include "celero/Celero.h"

#include "triangle/triangle.h"

CELERO_MAIN

static constexpr int32_t IMAGE_WIDTH = 512;
static constexpr int32_t IMAGE_HEIGHT = 512;
static uint32_t image[IMAGE_WIDTH * IMAGE_HEIGHT] = {0};

static point2d_t v0{IMAGE_WIDTH, IMAGE_HEIGHT, 1.0f, 0.0f, 0.0f};
static point2d_t v1{0, IMAGE_HEIGHT, 0.0f, 1.0f, 0.0f};
static point2d_t v2{0, 0, 0.0f, 0.0f, 1.0f};

class demo_simple_fixture : public celero::TestFixture
{
};

BASELINE_F(demo_simple, general, demo_simple_fixture, 100, 100)
{
    celero::DoNotOptimizeAway(image[0] == 128);
}

BENCHMARK_F(demo_simple, joshbeam, demo_simple_fixture, 100, 100)
{
    draw_triangle_joshbeam(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2);
    celero::DoNotOptimizeAway(image[0] == 128);
}

BENCHMARK_F(demo_simple, trinki2_p1, demo_simple_fixture, 100, 100)
{
    draw_triangle_trenki2_p1(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2);
    celero::DoNotOptimizeAway(image[0] == 128);
}

#if defined(__ARM_NEON)

#endif

#if defined(__ARM_FEATURE_SVE)

#endif

#if defined(__AVX2__)

#endif

#if defined(__AVX512F__)

#endif
