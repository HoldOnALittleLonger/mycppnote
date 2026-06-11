#include <boost/mpl/int.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/multiplies.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/type_traits.hpp>

#include <cstddef>

template<typename _Tp>
struct cond_add_refp
  : boost::mpl::if_<
    boost::mpl::or_<
      boost::is_same<_Tp, int>,
      boost::is_same<_Tp, long>>,
    boost::add_pointer<_Tp>,
    boost::add_reference<_Tp>>
{ };

int main(void)
{
  BOOST_STATIC_ASSERT((boost::is_same<cond_add_refp<int>::type, int *>::value,
                       "should be integer pointer"));
  BOOST_STATIC_ASSERT((boost::is_same<cond_add_refp<float>::type, float&>::value,
                       "should be floating reference"));

  BOOST_STATIC_ASSERT((boost::mpl::int_<15>::next::value == 16, "should be 16"));
  BOOST_STATIC_ASSERT((boost::is_same<boost::mpl::plus<boost::mpl::int_<4>, boost::mpl::int_<4>>,
                       boost::mpl::int_<8>>::value, "should be 8"));

  typedef boost::mpl::int_<4> value4;
  typedef boost::mpl::int_<8> value8;
  typedef typename boost::mpl::multiplies<value4, value8>::type value32;
  BOOST_STATIC_ASSERT((boost::mpl::equal_to<value32, boost::mpl::int_<32>>::value,
                       "should equal to 32"));

  static_assert(value32::value != 32, "<= @value32");

  return 0;
}
  


