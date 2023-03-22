#include<iostream>
#include<cstddef>

class A {
public:
  template<typename T>
  void printThis(T t)
  {
    std::cout<<static_cast<void *>(this)<<std::endl;
    [this](void) -> void
      {
	std::cout<<static_cast<void *>(this)<<std::endl;
      }();
  }
};

int main(void)
{
  A a;
  a.printThis(3);
  return 0;
}
