#include <stdint.h>
#include <stdio.h>

int64_t maxofthree(int64_t, int64_t, int64_t);

int32_t main(int32_t argument_count, char** arguments) {
  printf("%ld\n", maxofthree(1, -4, -7));
  printf("%ld\n", maxofthree(2, -6, 1));
  printf("%ld\n", maxofthree(2, 3, 1));
  printf("%ld\n", maxofthree(-2, 4, 3));
  printf("%ld\n", maxofthree(2, -6, 5));
  printf("%ld\n", maxofthree(2, 4, 6));

  return 0;
}
