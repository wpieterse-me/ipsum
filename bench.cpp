#include "celero/Celero.h"

#if defined(__AVX2__) || defined(__AVX512F__)
#include <immintrin.h>
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

#if defined(__ARM_FEATURE_SVE)
#include <arm_sve.h>
#endif

CELERO_MAIN

struct Point2D
{
    int32_t x;
    int32_t y;
};

int32_t PointSide(const Point2D &a, const Point2D &b, const Point2D &c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

void DrawTriangle(size_t window_width, size_t window_height, uint32_t *framebuffer, const Point2D &v0, const Point2D &v1, const Point2D &v2)
{
    Point2D p;
    for (p.y = 0; p.y < window_height; p.y++)
    {
        for (p.x = 0; p.x < window_width; p.x++)
        {
            int32_t w0 = PointSide(v1, v2, p);
            int32_t w1 = PointSide(v2, v0, p);
            int32_t w2 = PointSide(v0, v1, p);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                framebuffer[p.x * window_width + p.y] = 0xffffffff;
            }
        }
    }
}

class DemoSimpleFixture : public celero::TestFixture
{
};

BASELINE_F(DemoSimple, Baseline, DemoSimpleFixture, 16, 4096)
{
    static constexpr int32_t window_width = 512;
    static constexpr int32_t window_height = 512;

    uint32_t framebuffer[512 * 512] = {0};
    Point2D a{window_width, window_height};
    Point2D b{0, window_height};
    Point2D c{0, 0};

    celero::DoNotOptimizeAway([&]()
                              { DrawTriangle(window_width, window_height, framebuffer, a, b, c); });
}

#if defined(__ARM_NEON)

uint32_t GetPaletteIndex_NEON(Color color)
{
    auto v = vcreate_u8(0x0102030405060708);

    return vget_lane_u8(v, 0);
}

BENCHMARK_F(DemoSimple, NEON, DemoSimpleFixture, 16, 4096)
{
    celero::DoNotOptimizeAway(GetPaletteIndex_NEON(Color::Blue));
}

#endif

#if defined(__ARM_FEATURE_SVE)

uint32_t GetPaletteIndex_SVE(Color color)
{
    return svcntd();
}

BENCHMARK_F(DemoSimple, SVE, DemoSimpleFixture, 16, 4096)
{
    celero::DoNotOptimizeAway(GetPaletteIndex_SVE(Color::Blue));
}

#endif

#if defined(__AVX2__)

struct Edge
{
    static constexpr int32_t StepXSize = 8;
    static constexpr int32_t StepYSize = 1;

    __m256i OneStepX;
    __m256i OneStepY;

    __m256i Initialize(const Point2D &v0, const Point2D &v1, const Point2D &origin);
};

__m256i Edge::Initialize(const Point2D &v0, const Point2D &v1, const Point2D &origin)
{
    // Edge setup
    const int32_t A = v0.y - v1.y, B = v1.x - v0.x;
    const int32_t C = v0.x * v1.y - v0.y * v1.x;

    // Step deltas
    const int32_t OneStepXInitial = A * StepXSize;
    const int32_t OneStepYInitial = B * StepYSize;
    OneStepX = _mm256_set1_epi32(OneStepXInitial);
    OneStepY = _mm256_set1_epi32(OneStepYInitial);

    // x/y values for initial pixel block
    const __m256i Steps = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
    const __m256i XInitial = _mm256_set1_epi32(origin.x);
    const __m256i X = _mm256_add_epi32(XInitial, Steps);
    const __m256i Y = _mm256_set1_epi32(origin.y);

    // Edge function values at origin
    const __m256i AResult = _mm256_set1_epi32(A);
    const __m256i AMultiply = _mm256_mul_epi32(AResult, X);
    const __m256i BResult = _mm256_set1_epi32(B);
    const __m256i BMultiply = _mm256_mul_epi32(BResult, Y);
    const __m256i CResult = _mm256_set1_epi32(C);
    const __m256i AddAB = _mm256_add_epi32(AMultiply, BMultiply);
    const __m256i AddABC = _mm256_add_epi32(AddAB, CResult);

    return AddABC;
}

void DrawTriangle_AVX2(size_t window_width, size_t window_height, uint32_t *framebuffer, const Point2D &v0, const Point2D &v1, const Point2D &v2)
{
    // Bounding box and clipping again as before

    // Triangle setup
    Point2D p = {0, 0};
    Edge e01, e12, e20;

    __m256i w0_row = e12.Initialize(v1, v2, p);
    __m256i w1_row = e20.Initialize(v2, v0, p);
    __m256i w2_row = e01.Initialize(v0, v1, p);

    // Rasterize
    for (p.y = 0; p.y < window_height; p.y += Edge::StepYSize)
    {
        // Barycentric coordinates at start of row
        __m256i w0 = w0_row;
        __m256i w1 = w1_row;
        __m256i w2 = w2_row;

        for (p.x = 0; p.x < window_width; p.x += Edge::StepXSize)
        {
            // If p is on or inside all edges for any pixels,
            // render those pixels.
            const __m256i first_mask = _mm256_or_si256(w0, w1);
            const __m256i second_mask = _mm256_or_si256(first_mask, w2);
            const __m256i constant_zero = _mm256_set1_epi32(0);
            const __m256i compare_eq = _mm256_cmpeq_epi32(second_mask, constant_zero);
            const __m256i compare_gt = _mm256_cmpgt_epi32(second_mask, constant_zero);
            const __m256i final_mask = _mm256_or_si256(compare_eq, compare_gt);
            int integer_mask = _mm256_movemask_epi8(final_mask);
            if (integer_mask > 0)
            {
                framebuffer[p.x * window_width + p.y] = 0xffffffff;
            }

            // One step to the right
            w0 = _mm256_add_epi32(w0, e12.OneStepX);
            w1 = _mm256_add_epi32(w1, e20.OneStepX);
            w2 = _mm256_add_epi32(w2, e01.OneStepX);
        }

        // One row step
        w0_row = _mm256_add_epi32(w0_row, e12.OneStepY);
        w1_row = _mm256_add_epi32(w1_row, e20.OneStepY);
        w2_row = _mm256_add_epi32(w2_row, e01.OneStepY);
    }
}

BENCHMARK_F(DemoSimple, AVX2, DemoSimpleFixture, 16, 4096)
{
    static constexpr int32_t window_width = 512;
    static constexpr int32_t window_height = 512;

    uint32_t framebuffer[512 * 512] = {0};
    Point2D a{window_width, window_height};
    Point2D b{0, window_height};
    Point2D c{0, 0};

    celero::DoNotOptimizeAway([&]()
                              { DrawTriangle_AVX2(window_width, window_height, framebuffer, a, b, c); });
}

#endif

#if defined(__AVX512F__)

int32_t GetPaletteIndex_AVX512(Color color)
{
    const __m512i vec = _mm512_set1_epi32((uint32_t)color);
    const __m512i lookup = _mm512_setr_epi32((uint32_t)Color::Red, (uint32_t)Color::Green, (uint32_t)Color::Blue, (uint32_t)Color::White, (uint32_t)Color::Gray, (uint32_t)Color::Black, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
    const __mmask16 mask = _mm512_cmpeq_epi32_mask(vec, lookup);
    const uint32_t mask_b = mask | 0x40;
    return __builtin_ctz(mask_b);
}

BENCHMARK_F(DemoSimple, AVX512, DemoSimpleFixture, 16, 4096)
{
    celero::DoNotOptimizeAway(GetPaletteIndex_AVX512(Color::Blue));
}

#endif
