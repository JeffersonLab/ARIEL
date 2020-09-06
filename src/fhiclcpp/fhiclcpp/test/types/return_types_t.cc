// ======================================================================
//
// test types keymap: verify that types 1-13 return the correct types.
//
// ======================================================================

#define BOOST_TEST_MODULE (return types test)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/TableFragment.h"
#include "fhiclcpp/types/Tuple.h"

#include <array>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

using namespace fhicl;
using namespace fhicl::detail;
using namespace std;

namespace {

  template <typename R, typename T>
  void
  require_type_as(T& t)
  {
    auto rt = t();
    BOOST_TEST((std::is_same_v<R, decltype(rt)>));
  }
}

BOOST_AUTO_TEST_SUITE(types_return_types_test)

// [1] Atom<T>
BOOST_AUTO_TEST_CASE(one_atom_t)
{
  Atom<int> test{Name("atom")};
  require_type_as<int>(test);
}

// [2] Sequence<T>
BOOST_AUTO_TEST_CASE(one_sequence_t)
{
  Sequence<int> test{Name("sequence")};
  require_type_as<std::vector<int>>(test);
}

// [3] Sequence<T,SZ>
BOOST_AUTO_TEST_CASE(one_sequence_2_t)
{
  Sequence<int, 2> test{Name("sequence")};
  require_type_as<std::array<int, 2>>(test);
}

// [4] Tuple<T...>
BOOST_AUTO_TEST_CASE(one_tuple_t)
{
  Tuple<int, double, bool> test{Name("tuple")};
  require_type_as<std::tuple<int, double, bool>>(test);
}

// [5] Tuple< Sequence<T>, U...>
BOOST_AUTO_TEST_CASE(seq_in_tuple_t)
{
  Tuple<Sequence<int>, double, bool> test{Name("tuple")};
  require_type_as<std::tuple<std::vector<int>, double, bool>>(test);
}

// [6] Tuple< Sequence<T,SZ>, U...>
BOOST_AUTO_TEST_CASE(bounded_seq_in_tuple_t)
{
  Tuple<Sequence<int, 2>, double, bool> test{Name("tuple")};
  require_type_as<std::tuple<std::array<int, 2>, double, bool>>(test);
}

// [7] Tuple< Tuple<T...>, U...>
BOOST_AUTO_TEST_CASE(tuple_in_tuple_t)
{
  Tuple<Tuple<int, float>, double, bool> test{Name("tuple")};
  require_type_as<std::tuple<std::tuple<int, float>, double, bool>>(test);
}

// [8] Sequence< Tuple<T...> >
BOOST_AUTO_TEST_CASE(tuple_in_seq_t)
{
  Sequence<Tuple<int, float>> test{Name("seqtuple")};
  require_type_as<std::vector<std::tuple<int, float>>>(test);
}

// [9] Sequence< Tuple<T...>, SZ >
BOOST_AUTO_TEST_CASE(tuple_in_seq_2_t)
{
  Sequence<Tuple<int, float>, 2> test{Name("seqtuple")};
  require_type_as<std::array<std::tuple<int, float>, 2>>(test);
}

// [10] Sequence< Sequence<T> >
BOOST_AUTO_TEST_CASE(seq_in_seq_t)
{
  Sequence<Sequence<int>> test{Name("seqseq")};
  require_type_as<std::vector<std::vector<int>>>(test);
}

// [11] Sequence< Sequence<T,SZ> >
BOOST_AUTO_TEST_CASE(seq_2_in_seq_t)
{
  Sequence<Sequence<int, 2>> test{Name("seqseq")};
  require_type_as<std::vector<std::array<int, 2>>>(test);
}

// [12] Sequence< Sequence<T,SZ> >
BOOST_AUTO_TEST_CASE(seq_in_seq_2_t)
{
  Sequence<Sequence<int>, 2> test{Name("seqseq")};
  require_type_as<std::array<std::vector<int>, 2>>(test);
}

// [13] Sequence< Sequence<T,SZ>, SZ >
BOOST_AUTO_TEST_CASE(seq_2_in_seq_2_t)
{
  Sequence<Sequence<int, 2>, 2> test{Name("seqseq")};
  require_type_as<std::array<std::array<int, 2>, 2>>(test);
}
BOOST_AUTO_TEST_SUITE_END()
