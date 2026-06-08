#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/type_traits.hpp>

#include <cstddef>

using namespace boost::mpl::placeholders;

template<typename _Tp>
struct add_reference {
  typedef _Tp& type;
};

template<typename _Func, typename _Tp>
struct ret_type {
  typedef typename _Func::type:: template apply<_Tp>::type type;
  //                      #^ @result_
};

int main(void)
{
  int a = 0;
  typename boost::mpl::apply<boost::mpl::lambda<add_reference<_1>>, int>::type
    int_ref = a;
  
  //  we can get compilation error msg that @int_ref is a lvalue reference to
  //  int type.
  //  typename boost::mpl::apply<boost::mpl::lambda<add_reference<_1>>, int>::type
  //    int_ref = 0;
  
  //  bad usage
  //  typename boost::mpl::lambda<add_reference<_1>, int>::type
  //    int_ref2 = a;

  typename ret_type<boost::mpl::lambda<boost::add_pointer<_1>>, int>::type pint = &a;
  
  return 0;
}
