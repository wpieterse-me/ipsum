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

#include "triangle.h"

CELERO_MAIN

static constexpr int32_t IMAGE_WIDTH = 2048;
static constexpr int32_t IMAGE_HEIGHT = 2048;
static uint32_t image[IMAGE_WIDTH * IMAGE_HEIGHT] = {0};

static point2d_t v0{IMAGE_WIDTH, IMAGE_HEIGHT};
static point2d_t v1{0, IMAGE_HEIGHT};
static point2d_t v2{0, 0};

static uint32_t color = 0xFF0000FF;

class demo_simple_fixture : public celero::TestFixture
{
};

BASELINE_F(demo_simple, span, demo_simple_fixture, 100, 10000)
{
    draw_triangle_span(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
    celero::DoNotOptimizeAway(image[0] == color);
}

BENCHMARK_F(demo_simple, general, demo_simple_fixture, 100, 10000)
{
    draw_triangle_general(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
    celero::DoNotOptimizeAway(image[0] == color);
}

BENCHMARK_F(demo_simple, optimized_1, demo_simple_fixture, 100, 10000)
{
    draw_triangle_optimized_1(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
    celero::DoNotOptimizeAway(image[0] == color);
}

BENCHMARK_F(demo_simple, optimized_2, demo_simple_fixture, 100, 10000)
{
    draw_triangle_optimized_2(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
    celero::DoNotOptimizeAway(image[0] == color);
}

#if defined(__ARM_NEON)

BENCHMARK_F(demo_simple, NEON, demo_simple_fixture, 32, 16384)
{
}

#endif

#if defined(__ARM_FEATURE_SVE)

BENCHMARK_F(demo_simple, SVE, demo_simple_fixture, 32, 16384)
{
}

#endif

#if defined(__AVX2__)

BENCHMARK_F(demo_simple, avx2, demo_simple_fixture, 100, 10000)
{
    draw_triangle_avx2(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
    celero::DoNotOptimizeAway(image[0] == color);
}

#endif

#if defined(__AVX512F__)

BENCHMARK_F(demo_simple, AVX512, demo_simple_fixture, 32, 16384)
{
}

#endif
