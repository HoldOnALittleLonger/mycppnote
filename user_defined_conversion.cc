#include<iostream>

struct X {
  int x;
  char c;

  operator int()
  {
    return this->x;
  }

  operator char()
  {
    return this->c;
  }

  operator int*()
  {
    return nullptr;
  }

  X() : x(0), c('\0') { }
};


int main(void)
{
  struct X x;

  std::cout<<static_cast<int>(x)<<std::endl;
  std::cout<<static_cast<int *>(x)<<std::endl;
  std::cout<<static_cast<char>(x)<<std::endl;
  return 0;
}
