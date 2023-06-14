#include <cstdint>
#include <iostream>

#include "container.hpp"
#include "vector.hpp"

int32_t main(int32_t argument_count, char** arguments) {
  dynamic_array_storage       storage{ sizeof(uint32_t), alignof(uint32_t) };
  dynamic_array_view<int32_t> view{ storage };

  view.push_back(int32_t{ 1 });
  view.push_back(int32_t{ 2 });
  view.push_back(int32_t{ 4 });
  view.push_back(int32_t{ 8 });
  view.push_back(int32_t{ 16 });
  view.push_back(int32_t{ 32 });

  for(auto item: view) {
    std::cout << item << std::endl;
  }

  return 0;
}
