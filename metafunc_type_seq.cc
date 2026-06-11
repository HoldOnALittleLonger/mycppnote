#include <boost/mpl/int.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/iterator_tags.hpp>
#include <boost/type_traits.hpp>

struct type_seq_void { };

struct type_seq_tag { };

template<
  typename T0 = type_seq_void, typename T1 = type_seq_void,
  typename T2 = type_seq_void, typename T3 = type_seq_void>
struct type_seq {
  typedef type_seq_tag tag;
  typedef T0 t0;
  typedef T1 t1;
  typedef T2 t2;
  typedef T3 t3;
};

// size impl provides O(1) complexity
template<typename T0, typename T1, typename T2, typename T3>
struct type_seq_size_impl
  : boost::mpl::int_<4>
{ };

template<>
struct type_seq_size_impl<
  type_seq_void,
  type_seq_void,
  type_seq_void,
  type_seq_void>
  : boost::mpl::int_<0>
{ };

template<typename T0>
struct type_seq_size_impl<
  T0,
  type_seq_void,
  type_seq_void,
  type_seq_void>
  : boost::mpl::int_<1>
{ };

template<typename T0, typename T1>
struct type_seq_size_impl<
  T0,
  T1,
  type_seq_void,
  type_seq_void>
  : boost::mpl::int_<2>
{ };

template<typename T0, typename T1, typename T2>
struct type_seq_size_impl<
  T0,
  T1,
  T2,
  type_seq_void>
  : boost::mpl::int_<3>
{ };

// Boost size specialization
namespace boost { namespace mpl {
    template<>
    struct size_impl<type_seq_tag> {
      template<typename TSeq>
      struct apply
        : type_seq_size_impl<
          typename TSeq::t0,
          typename TSeq::t1,
          typename TSeq::t2,
          typename TSeq::t3>
      { };
    };
  }}


// at
template<typename TSeq, int N> struct type_seq_at;

template<typename TSeq>
struct type_seq_at<TSeq, 0> {
  typedef typename TSeq::t0 type;
};

template<typename TSeq>
struct type_seq_at<TSeq, 1> {
  typedef typename TSeq::t1 type;
};

template<typename TSeq>
struct type_seq_at<TSeq, 2> {
  typedef typename TSeq::t2 type;
};

template<typename TSeq>
struct type_seq_at<TSeq, 3> {
  typedef typename TSeq::t3 type;
};

namespace boost { namespace mpl {
    template<typename T0 ,typename T1, typename T2, typename T3, class Pos>
    struct at<type_seq<T0, T1, T2, T3>, Pos>
      : type_seq_at<type_seq<T0, T1, T2, T3>, Pos::value>
    { };
  }}


template<typename TSeq, class TSeqPos>
struct type_seq_iterator {
  typedef boost::mpl::forward_iterator_tag category;
};

namespace boost { namespace mpl {
    template<typename TSeq, typename Pos>
    struct next<type_seq_iterator<TSeq, Pos>> {
      typedef type_seq_iterator<TSeq, typename boost::mpl::next<Pos>::type> type;
    };
  }}

namespace boost { namespace mpl {
    template<typename Seq, typename Pos>
    struct deref<type_seq_iterator<Seq, Pos>>
      : at<Seq, Pos>
    { };
  }}

int main(void)
{
  typedef type_seq<int, char, float, long> type_seq_icfl;
  typedef type_seq_iterator<type_seq_icfl, boost::mpl::int_<0>> iter0_icfl;
  BOOST_STATIC_ASSERT((boost::mpl::equal_to<boost::mpl::size<type_seq_icfl>, boost::mpl::int_<4>>::value,
                       "should size 4"));
  BOOST_STATIC_ASSERT((boost::is_same<int, boost::mpl::deref<iter0_icfl>::type>::value,
                       "should be int type"));
  BOOST_STATIC_ASSERT((boost::is_same<
                       char,
                       boost::mpl::deref<boost::mpl::next<iter0_icfl>::type>::type>::value,
                       "should be char type"));
  BOOST_STATIC_ASSERT((boost::is_same<
                       long,
                       boost::mpl::at<type_seq_icfl, boost::mpl::int_<3>>::type>::value,
                       "should be long type"));
  return 0;
}
  
