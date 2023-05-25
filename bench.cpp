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

enum class Color : uint32_t
{
    Red = 0x00ff0000,
    Green = 0x0000ff00,
    Blue = 0x000000ff,
    White = 0x00ffffff,
    Gray = 0x00333333,
    Black = 0x00000000,
};

uint32_t GetPaletteIndex(Color color)
{
    switch (color)
    {
    case Color::Red:
        return 0;
    case Color::Green:
        return 1;
    case Color::Blue:
        return 2;
    case Color::White:
        return 3;
    case Color::Gray:
        return 4;
    case Color::Black:
        return 5;
    }

    return 6;
}

class DemoSimpleFixture : public celero::TestFixture
{
public:
    virtual std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override
    {
        std::vector<celero::TestFixture::ExperimentValue> result;

        result.push_back({(int64_t)Color::Red});
        result.push_back({(int64_t)Color::Black});

        return result;
    }

    virtual void setUp(const celero::TestFixture::ExperimentValue &experiment_value)
    {
        this->color = (Color)experiment_value.Value;
    }

private:
    Color color;
};

BASELINE_F(DemoSimple, Baseline, DemoSimpleFixture, 10, 1000000)
{
    celero::DoNotOptimizeAway(GetPaletteIndex(Color::Blue));
}

#if defined(__ARM_NEON)

uint32_t GetPaletteIndex_NEON(Color color)
{
    auto v = vcreate_u8(0x0102030405060708);

    return vget_lane_u8(v, 0);
}

BENCHMARK_F(DemoSimple, NEON, DemoSimpleFixture, 10, 1000000)
{
    celero::DoNotOptimizeAway(GetPaletteIndex_NEON(Color::Blue));
}

#endif

#if defined(__ARM_FEATURE_SVE)

uint32_t GetPaletteIndex_SVE(Color color)
{
    return svcntd();
}

BENCHMARK_F(DemoSimple, SVE, DemoSimpleFixture, 10, 1000000)
{
    celero::DoNotOptimizeAway(GetPaletteIndex_SVE(Color::Blue));
}

#endif

#if defined(__AVX2__)

uint32_t GetPaletteIndex_AVX2(Color color)
{
    const __m256i vec = _mm256_set1_epi32((uint32_t)color);
    const __m256i lookup = _mm256_setr_epi32((uint32_t)Color::Red, (uint32_t)Color::Green, (uint32_t)Color::Blue, (uint32_t)Color::White, (uint32_t)Color::Gray, (uint32_t)Color::Black, -1, -1);
    const __m256i mask = _mm256_cmpeq_epi32(vec, lookup);
    uint32_t bitmask = _mm256_movemask_ps((__m256)mask);
    bitmask = bitmask | 0x40;
    const int index = __builtin_ctz(bitmask);

    return index;
}

BENCHMARK_F(DemoSimple, AVX2, DemoSimpleFixture, 10, 1000000)
{
    celero::DoNotOptimizeAway(GetPaletteIndex_AVX2(Color::Blue));
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

BENCHMARK_F(DemoSimple, AVX512, DemoSimpleFixture, 10, 1000000)
{
    celero::DoNotOptimizeAway(GetPaletteIndex_AVX512(Color::Blue));
}

#endif
