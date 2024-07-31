#include<iostream>
#include<utility>

struct A {
  int x;
  char y;

  A() : x(0), y('c') { std::cout<<"Calls default constructor."<<std::endl; }
  A(int i) : x(i), y('c') { std::cout<<"Calls conversion constructor."<<std::endl; }
  explicit A(const A &a)
  {
    x = a.x;
    y = a.y;

    std::cout<<"Calls copy-constructor."<<std::endl;
  }

  A &operator=(const A &a)
  {
    x = a.x;
    y = a.y;

    std::cout<<"Calls assignment."<<std::endl;
    return *this;
  }

  template<class _Type>
  void forward_func(_Type &&a)
  {
    std::cout<<"Calls forwarding."<<std::endl;
    internal_func(std::forward<_Type>(a));
  }
  
  //  typedef TR T&
  //  TR& == T&
  //  typedef TR T&&
  //  TR&& == TR

  //  forward reference combined T& and T&&,TR is T&&

  //  class _Type means typdef _Type T
  //  P1 == lvalue reference,_Type == lvalue reference
  //  _Type&& == _Type == lvalue reference
  //  P2 == rvalue reference,_Type == rvalue reference
  //  _Type&& == _Type == rvalue reference


  void internal_func(const A &a)
  {
    std::cout<<"Calls internal_func lv."<<std::endl;
  }

  void internal_func(const A &&a)
  {
    std::cout<<"Calls internal_func rv."<<std::endl;
  }

  /*  void internal_func(A a)
  {
    std::cout<<"Calls internal_func v."<<std::endl;
  }
  */
};
/*
A func(A arg)
{
  std::cout<<"Calls func."<<std::endl;
  return arg;
}
*/

//  if copy-constructor is explicit,then cant pass object as parameter of a
//  function,and cant return from a function by value.

int main(void)
{
  A a;
  a.x = 3;
  a.y = 'k';

  A b = A(2);
  A c(b);
  A d;
  b = c;

  A &lA(b);
  A &&rA = std::move(d);

  a.forward_func(lA);
  a.forward_func(std::move(c));
  a.forward_func(rA);
  a.forward_func(std::forward<A&&>(rA));
  a.forward_func(a);


  return 0;
}
