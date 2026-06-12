#include <boost/mpl/int.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/minus.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/iterator_tags.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/type_traits.hpp>

using namespace boost::mpl;

template<typename _SeqType>
struct ud_int_forward_iterator {
  typedef forward_iterator_tag category;
};

namespace boost { namespace mpl {
    template<typename _SeqType>
    struct deref<ud_int_forward_iterator<_SeqType>> {
      typedef typename _SeqType::type type;
    };
    
    template<typename _SeqType>
    struct next<ud_int_forward_iterator<_SeqType>> {
      typedef ud_int_forward_iterator<typename next<_SeqType>::type> type;
    };

  }}


int main(void)
{
  typedef ud_int_forward_iterator<int_<0>> int0_fiter;
  BOOST_STATIC_ASSERT(
                      equal_to<deref<int0_fiter>::type, int_<0>>::value
                      );
  typedef next<int0_fiter>::type int1_fiter;
  BOOST_STATIC_ASSERT(
                      equal_to<deref<int1_fiter>::type, int_<1>>::value
                      );

  BOOST_STATIC_ASSERT(
                      boost::is_same<
                      plus<deref<int0_fiter>::type, int_<1>>::type,
                      integral_c<int, 1>
                      >::value
                      );
  BOOST_STATIC_ASSERT(
                      boost::is_same<
                      minus<deref<int1_fiter>::type, int_<1>>::type,
                      integral_c<int, 0>
                      >::value
                      );
  
  static_assert(deref<int0_fiter>::type::value != int_<0>::value, "<= @value is ");

  
  return 0;
}
