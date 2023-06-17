#include <cstdint>
#include <iostream>

class IFactory {
  public:
    virtual ~IFactory() {
    }
};

class Factory : public IFactory {
  public:
};

int32_t main(int32_t argument_count, char** arguments) {
  std::cout << std::endl;
  std::cout << "DONE!!!" << std::endl;
  std::cout << std::endl;

  return 0;
}
