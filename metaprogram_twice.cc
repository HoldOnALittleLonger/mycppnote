#include <boost/type_traits.hpp>

#include <cstddef>

/**
 * Higher-order metafunction.
 */
template<typename _UnaryMetaFunc, typename _FuncArg>
struct twice {
  typedef typename _UnaryMetaFunc::template apply<_FuncArg>::type once;
  typedef typename _UnaryMetaFunc::template apply<once>::type type;
};

/**
 * Metafunction.
 */
struct add_pointer_f {
  /**
   * Metafunction forward.
   */
  template<typename _Arg>
  struct apply : boost::add_pointer<_Arg> { };
};

int main(void)
{
  BOOST_STATIC_ASSERT(boost::is_same<twice<add_pointer_f, int>::type, int **>::value);
  typename twice<add_pointer_f, int>::type ppint = (int **)0x12345678;
  return 0;
}
