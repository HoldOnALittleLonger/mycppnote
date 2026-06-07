#include <type_traits>

#include <cstddef>

// metafunctions to check whether @N is a good binary format.
template<unsigned int N>
struct is_good_binary_format : public std::true_type {
  static constexpr bool value =
    std::true_type::value && N % 10 <= 1 && is_good_binary_format<N / 10>::value;
};

template<>
struct is_good_binary_format<1> : public std::true_type {
  using std::true_type::value;
};

template<>
struct is_good_binary_format<0> : public std::true_type {
  using std::true_type::value;
};

template<unsigned int _number, bool good_format = is_good_binary_format<_number>::value>
struct binary { };

// only defined when @_number is a good binary format.
template<unsigned int _number>
struct binary<_number, true> {
  static constexpr unsigned int value = binary<_number / 10>::value << 1 | _number %10;
};

template<>
struct binary<0, true> {
  static constexpr unsigned int value = 0;
};

// use static_assert(false) to print value.
template<unsigned int X>
struct PrintX {
  PrintX()
  {
    static_assert(X != X, "<= @x value");
  }
};

int main(void)
{
  // auto a = binary<202>::value;
  constexpr auto x = binary<11111111>::value;
  PrintX<x> print_it;
  return 0;
}

  
