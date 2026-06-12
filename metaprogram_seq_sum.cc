#include <boost/mpl/transform.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/inserter.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

using namespace boost::mpl::placeholders;
using namespace boost;

int main(void)
{
  typedef typename mpl::vector<
    mpl::vector_c<int, 0, 1, 2>,
    mpl::vector_c<int, 3, 4, 5>,
    mpl::vector_c<int, 6, 7, 8>
    >::type numbers;

  typedef typename mpl::transform<
    numbers,
    mpl::front<_>,
    mpl::inserter<
      mpl::int_<0>,
      mpl::plus<_, _>
      >
    >::type number_sum;

  BOOST_STATIC_ASSERT(
                      boost::is_same<
                      number_sum,
                      mpl::integral_c<int, 9>
                      >::value
                      );

  BOOST_STATIC_ASSERT(
                      mpl::equal_to<
                      mpl::int_<9>,
                      number_sum
                      >::value
                      );

  return 0;
}
