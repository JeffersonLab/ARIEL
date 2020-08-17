// ======================================================================
//
// test types keymap: verify that types 1-13 create the correct key
//                    maps for ParameterSet validation.
//
// ======================================================================

#define BOOST_TEST_MODULE (keymap test)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/test/types/KeyMap.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Tuple.h"

#include <string>
#include <vector>

using namespace fhicl;
using namespace fhicl::detail;
using namespace std;

BOOST_AUTO_TEST_SUITE(types_keymap_test)

// [1] Atom<T>
BOOST_AUTO_TEST_CASE(one_atom_t)
{
  auto map = KeyMap::get<Atom<int>>("atom");
  auto ref = {"atom"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [2] Sequence<T>
BOOST_AUTO_TEST_CASE(one_sequence_t)
{
  auto map = KeyMap::get<Sequence<int>>("sequence");
  auto ref = {"sequence", "sequence[0]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [3] Sequence<T,SZ>
BOOST_AUTO_TEST_CASE(one_sequence_2_t)
{
  auto map = KeyMap::get<Sequence<int, 2>>("sequence");
  auto ref = {"sequence", "sequence[0]", "sequence[1]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [4] Tuple<T...>
BOOST_AUTO_TEST_CASE(one_tuple_t)
{
  auto map = KeyMap::get<Tuple<int, double, bool>>("tuple");
  auto ref = {"tuple", "tuple[0]", "tuple[1]", "tuple[2]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [5] Tuple< Sequence<T>, U...>
BOOST_AUTO_TEST_CASE(seq_in_tuple_t)
{
  auto map = KeyMap::get<Tuple<Sequence<int>, double, bool>>("tuple");
  auto ref = {"tuple", "tuple[0]", "tuple[0][0]", "tuple[1]", "tuple[2]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [6] Tuple< Sequence<T,SZ>, U...>
BOOST_AUTO_TEST_CASE(bounded_seq_in_tuple_t)
{
  auto map = KeyMap::get<Tuple<Sequence<int, 2>, double, bool>>("tuple");
  auto ref = {
    "tuple", "tuple[0]", "tuple[0][0]", "tuple[0][1]", "tuple[1]", "tuple[2]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [7] Tuple< Tuple<T...>, U...>
BOOST_AUTO_TEST_CASE(tuple_in_tuple_t)
{
  auto map = KeyMap::get<Tuple<Tuple<int, float>, double, bool>>("tuple");
  auto ref = {
    "tuple", "tuple[0]", "tuple[0][0]", "tuple[0][1]", "tuple[1]", "tuple[2]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [8] Sequence< Tuple<T...> >
BOOST_AUTO_TEST_CASE(tuple_in_seq_t)
{
  auto map = KeyMap::get<Sequence<Tuple<int, float>>>("seqtuple");
  auto ref = {"seqtuple", "seqtuple[0]", "seqtuple[0][0]", "seqtuple[0][1]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [9] Sequence< Tuple<T...>, SZ >
BOOST_AUTO_TEST_CASE(tuple_in_seq_2_t)
{
  auto map = KeyMap::get<Sequence<Tuple<int, float>, 2>>("seqtuple");
  auto ref = {"seqtuple",
              "seqtuple[0]",
              "seqtuple[0][0]",
              "seqtuple[0][1]",
              "seqtuple[1]",
              "seqtuple[1][0]",
              "seqtuple[1][1]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [10] Sequence< Sequence<T> >
BOOST_AUTO_TEST_CASE(seq_in_seq_t)
{
  auto map = KeyMap::get<Sequence<Sequence<int>>>("seqseq");
  auto ref = {"seqseq", "seqseq[0]", "seqseq[0][0]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [11] Sequence< Sequence<T,SZ> >
BOOST_AUTO_TEST_CASE(seq_2_in_seq_t)
{
  auto map = KeyMap::get<Sequence<Sequence<int, 2>>>("seqseq");
  auto ref = {"seqseq", "seqseq[0]", "seqseq[0][0]", "seqseq[0][1]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [12] Sequence< Sequence<T,SZ> >
BOOST_AUTO_TEST_CASE(seq_in_seq_2_t)
{
  auto map = KeyMap::get<Sequence<Sequence<int>, 2>>("seqseq");
  auto ref = {
    "seqseq", "seqseq[0]", "seqseq[0][0]", "seqseq[1]", "seqseq[1][0]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [13] Sequence< Sequence<T,SZ>, SZ >
BOOST_AUTO_TEST_CASE(seq_2_in_seq_2_t)
{
  auto map = KeyMap::get<Sequence<Sequence<int, 2>, 2>>("seqseq");
  auto ref = {"seqseq",
              "seqseq[0]",
              "seqseq[0][0]",
              "seqseq[0][1]",
              "seqseq[1]",
              "seqseq[1][0]",
              "seqseq[1][1]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

BOOST_AUTO_TEST_SUITE_END()
