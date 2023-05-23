#include <stdint.h>
#include <stdio.h>

int32_t main(int32_t argument_count, char **arguments)
{
// printf("Hello, World\n");
#if defined(__AVX512F__)
    printf("Using AVX 512\n");
#elif defined(__ARM_NEON)
    printf("Using NEON\n");
#else
    printf("Using Standard\n");
#endif

    return 0;
}
