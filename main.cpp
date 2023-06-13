#include <cstdint>
#include <iostream>

#include "vector.hpp"

int32_t main(int32_t argument_count, char** arguments) {
  Vector_D32_4 lhs;

  lhs.standard_storage[0] = 1.0f;
  lhs.standard_storage[1] = 2.0f;
  lhs.standard_storage[2] = 3.0f;
  lhs.standard_storage[3] = 4.0f;

  Vector_D32_4 rhs;

  rhs.standard_storage[0] = 1.0f;
  rhs.standard_storage[1] = 2.0f;
  rhs.standard_storage[2] = 3.0f;
  rhs.standard_storage[3] = 4.0f;

  Vector_D32_4 result = lhs + rhs;

  for(auto index = 0; index < 4; index++) {
    std::cout << result.standard_storage[index] << std::endl;
  }

  return 0;
}
