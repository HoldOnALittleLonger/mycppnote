#include <print>

#include <cstddef>

struct TheStruct;

template<>
struct std::formatter<TheStruct *> {
  constexpr auto parse(std::format_parse_context& __pc)
  {
    return __pc.begin();
  }

  auto format(TheStruct *__u, std::format_context& __fc) const
  {
    return std::format_to(__fc.out(), "{}", reinterpret_cast<void *>(__u));
  }
};

struct TheStruct {
  TheStruct() : a(2), b(4) { }

  void print_this(void)
  {
    std::println("this is {}", this);
  }

  int a;
  int b;
};

template<>
struct std::formatter<TheStruct> {
  constexpr auto parse(std::format_parse_context& __pc)
  {
    return __pc.begin();
  }

  auto format(TheStruct& __u, std::format_context& __fc) const
  {
    return std::format_to(__fc.out(), "[{}, {}]", __u.a, __u.b);
  }
};

int main(void)
{
  TheStruct u;
  std::println("{}", u);
  std::println("{}", &u);
  u.print_this();
  return 0;
}
