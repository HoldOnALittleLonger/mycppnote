#include<iostream>
#include<exception>

auto func(int (&f)(int) noexcept(false), int x) noexcept
{
  return [f, x](void) -> int
    {
      std::cout<<"function name : "<<__func__<<std::endl;
      return f(x);
    };
}

int f1(int x) noexcept(false)
{
  throw x;
}

int main(void)
{
  auto pf = func(f1, 12);
  try {
    pf();
  } catch (const int &v) {
    std::cout<<"Throw : "<<v<<std::endl;
  }

  return 0;
}
