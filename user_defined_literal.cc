#include<iostream>
#include<string>

std::string operator""_S(const char *str, std::size_t n)
{
  return (n > 0) ? std::string{str} : std::string{nullptr};
}

int main(void)
{
  std::cout<<"hello world"_S<<std::endl;
  return 0;
}
