#include <stdint.h>
#include <stdio.h>

#if defined(__AVX512F__)

#include <immintrin.h>

enum Color
{
    RED = 0x00ff0000,
    GREEN = 0x0000ff00,
    BLUE = 0x000000ff,
    WHITE = 0x00ffffff,
    GRAY0 = 0x00333333,
    GRAY1 = 0x00aaaaaa,
    GRAY2 = 0x00dddddd,
    BLACK = 0x00000000,
};

int32_t palette_avx512(int32_t color)
{
    const __m512i vec = _mm512_set1_epi32(color);
    const __m512i lookup = _mm512_setr_epi32(RED, GREEN, BLUE, WHITE, GRAY0, GRAY1, GRAY2, BLACK, -1, -1, -1, -1, -1, -1, -1, -1);
    const __mmask16 mask = _mm512_cmpeq_epi32_mask(vec, lookup);

    if (mask)
    {
        return __builtin_ctz(mask);
    }

    return -1;
}

#endif

int32_t main(int32_t argument_count, char **arguments)
{
// printf("Hello, World\n");
#if defined(__AVX512F__)
    printf("Using AVX 512 - %ld\n", palette_avx512(GREEN));
#elif defined(__ARM_NEON)
    printf("Using NEON\n");
#else
    printf("Using Standard\n");
#endif

    return 0;
}
