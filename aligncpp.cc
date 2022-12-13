#include<iostream>

struct alignas(1) X {
  int x = 3;
};

struct alignas(8) A { };
struct alignas(1) B { struct A a; };  //  ill-formed
                                      //  if alignas(1) doesnt appears before B,
				      //  the alignment of B would be 8,but 1 < 8

int main(void)
{
  alignas(8) int a(4);
  alignas(4) int b(8);
  struct X sx;
  struct A aa;
  struct B bb;
  std::cout<<"alignof a "<<alignof(a)<<std::endl;
  std::cout<<"alignof b "<<alignof(b)<<std::endl;
  std::cout<<"alignof sx.x "<<alignof(sx.x)<<std::endl;
  std::cout<<"alignof aa "<<alignof(aa)<<std::endl;
  std::cout<<"alignof bb "<<alignof(bb)<<std::endl;
  std::cout<<"alignof bb.a "<<alignof(bb.a)<<std::endl;
  return 0;
}
