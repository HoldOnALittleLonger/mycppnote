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
}

int main(void)
{
  A objectA;
  boost::intrusive_ptr<A> i1(&objectA);
  std::cout<<"Ref : "<<i1->ref_counter<<std::endl;

  boost::intrusive_ptr<A> i2(&objectA);
  std::cout<<"Ref : "<<i2->ref_counter<<std::endl;

  return 0;
}
