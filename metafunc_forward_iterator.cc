#include <boost/mpl/int.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/minus.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/type_traits.hpp>

using namespace boost::mpl;

struct ud_forward_iterator_tag { };

template<typename _SequenceTp>
struct ud_forward_iterator {
  typedef typename _SequenceTp::type type;
  typedef ud_forward_iterator<typename _SequenceTp::next::type> next;
  typedef ud_forward_iterator_tag category;
};

template<typename UD_Iterator>
struct ud_derefer {
  typedef typename UD_Iterator::type type;
};

template<typename UD_Iterator>
struct ud_category {
  typedef typename UD_Iterator::category category;
};

int main(void)
{

  /**
   * we use the int_<> sequence provided by boost's mpl.
   */
  typedef ud_forward_iterator<int_<0>::type> int0_iterator;
  typedef typename int0_iterator::next int1_iterator;
  constexpr auto v1(ud_derefer<int1_iterator>::type::value);

  BOOST_STATIC_ASSERT((boost::is_same<int_<1>::type, int0_iterator::next::type>::value,
                       "should be same type"));
  BOOST_STATIC_ASSERT((equal_to<int_<1>, ud_derefer<int1_iterator>::type>::value,
                       "should be same value"));
  BOOST_STATIC_ASSERT((boost::is_same<ud_forward_iterator_tag, ud_category<int0_iterator>>::value,
                       "should be same tag"));
  BOOST_STATIC_ASSERT((boost::is_same<ud_forward_iterator_tag, ud_category<int1_iterator>>::value,
                       "should be same tag"));

  static_assert(v1 != v1, "<= @v1");
  
  return 0;
}
