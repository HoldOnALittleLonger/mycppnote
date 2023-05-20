#include<cstddef>

namespace A {
  int a;
}

namespace B {

  int b;

}

using func1 = void (*)(int);
using func2 = void (*)(int);

using A::a, B::b;

void function(int x)
{
}

int main(void)
{
  func1 pFunc1(function);
  func2 pFunc2(function);
  pFunc1(a);
  pFunc2(b);
  return 0;
}
