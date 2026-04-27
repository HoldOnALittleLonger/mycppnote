#include <cstddef>
#include <array>
#include <print>

struct TempObj {
  TempObj(int x)
  {
    std::println("constructor called.");
  }

  ~TempObj()
  {
    std::println("destructor called.");
  }
};

int main(void)
{
  std::array<int, 10> a{1, 2, 3, 4, 5, 6};

  for (auto e : static_cast<TempObj>(4), a) {
    std::println("element is {}", e);
  }
  
  std::println("range for end.");

  return 0;
}
