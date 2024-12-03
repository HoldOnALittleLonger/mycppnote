#include <iostream>
#include <boost/smart_ptr/intrusive_ptr.hpp>

struct A {
  int v;
  unsigned short ref_counter;

  A() { v = ref_counter = 0; }
  ~A() { std::cout<<"A Destructor : Ref : "<<ref_counter<<std::endl; }
};

void intrusive_ptr_add_ref(A *pA)
{
  std::cout<<__func__<<" was called."<<std::endl;
  ++pA->ref_counter;
}

void intrusive_ptr_release(A *pA)
{
  std::cout<<__func__<<" was called."<<std::endl;
  --pA->ref_counter;
  if (!pA->ref_counter) {
    delete pA;
    std::cout<<"Counter down to zero,deleted target."<<std::endl;
  }
}

void func(A *pA)
{
  boost::intrusive_ptr<A> i1(pA);
  std::cout<<"Ref : "<<i1->ref_counter<<std::endl;

  boost::intrusive_ptr<A> i2(pA);
  std::cout<<"Ref : "<<i2->ref_counter<<std::endl;

  std::cout<<"Ready exit func(A*)"<<std::endl;
}

int main(void)
{
  A *objectA = new A;
  func(objectA);

  std::cout<<"Ready exit main()"<<std::endl;

  return 0;
}
