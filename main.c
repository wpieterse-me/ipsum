#include <stdint.h>
#include <stdio.h>

enum Color
{
    RED = 0x00ff0000,
    GREEN = 0x0000ff00,
    BLUE = 0x000000ff,
    WHITE = 0x00ffffff,
    GRAY = 0x00333333,
    BLACK = 0x00000000,
};

#if defined(__AVX512F__)

#include <immintrin.h>

int32_t palette_avx512(int32_t color)
{
    const __m512i vec = _mm512_set1_epi32(color);
    const __m512i lookup = _mm512_setr_epi32(RED, GREEN, BLUE, WHITE, GRAY, BLACK, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
    const __mmask16 mask = _mm512_cmpeq_epi32_mask(vec, lookup);
    const uint32_t mask_b = mask | 0x40;
    return __builtin_ctz(mask_b);
}

#endif

#if defined(__AVX2__)

#include <immintrin.h>

int32_t palette_avx2(int32_t color)
{
    const __m256i vec = _mm256_set1_epi32(color);
    const __m256i lookup = _mm256_setr_epi32(RED, GREEN, BLUE, WHITE, GRAY, BLACK, -1, -1);
    const __m256i mask = _mm256_cmpeq_epi32(vec, lookup);
    uint32_t bitmask = _mm256_movemask_ps((__m256)mask);
    bitmask = bitmask | 0x40;
    const int index = __builtin_ctz(bitmask);

    return index;
}

#endif

int32_t main(int32_t argument_count, char **arguments)
{
// printf("Hello, World\n");
#if defined(__AVX512F__)
    printf("Using AVX 512 :\n");
    printf(" - %d\n", palette_avx512(RED));
    printf(" - %d\n", palette_avx512(GREEN));
    printf(" - %d\n", palette_avx512(BLUE));
    printf(" - %d\n", palette_avx512(WHITE));
    printf(" - %d\n", palette_avx512(GRAY));
    printf(" - %d\n", palette_avx512(BLACK));
    printf(" - %d\n", palette_avx512(0xAFAFAFAF));
    printf(" - %d\n", palette_avx512(0xFAFAFAFA));
#elif defined(__AVX2__)
    printf("Using AVX 2 :\n");
    printf(" - %d\n", palette_avx2(RED));
    printf(" - %d\n", palette_avx2(GREEN));
    printf(" - %d\n", palette_avx2(BLUE));
    printf(" - %d\n", palette_avx2(WHITE));
    printf(" - %d\n", palette_avx2(GRAY));
    printf(" - %d\n", palette_avx2(BLACK));
    printf(" - %d\n", palette_avx2(0xAFAFAFAF));
    printf(" - %d\n", palette_avx2(0xFAFAFAFA));
#elif defined(__ARM_NEON)
    printf("Using NEON\n");
#else
    printf("Using Standard\n");
#endif

    return 0;
}
