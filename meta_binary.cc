#include <cstddef>

template<unsigned int N>
constexpr bool is_greater_than_one(void)
{
  return N > 1;
}

template<unsigned int N>
constexpr bool check_if_bad_format(void)
{
  return is_greater_than_one<N % 10>() || check_if_bad_format<N / 10>();
}

template<>
constexpr bool check_if_bad_format<1>(void)
{
  return false;
}

template<unsigned int N>
constexpr unsigned int is_bad_format(void)
{
  /**
   * We intentionally let the result is "1 / 0",
   * because we dont want bad format can pass compilation.
   */
  return check_if_bad_format<N>() ? 1 / 0 : N;
}

template<unsigned int _number>
struct binary {
  static constexpr unsigned int value =
    binary<is_bad_format<_number>() / 10>::value << 1 | _number % 10;
};

template<>
struct binary<0> {
  static constexpr unsigned int value = 0;
};

template<unsigned int X>
struct PrintX {
  PrintX()
  {
    static_assert(X != X, "<= @x value");
  }
};


int main(void)
{
  //  auto x = binary<202>::value;
  constexpr auto x = binary<11111111>::value;
  PrintX<x> print_it;
  return 0;
}

  
