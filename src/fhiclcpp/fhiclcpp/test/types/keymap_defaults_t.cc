// ======================================================================
//
// test types keymap with defaults:
//
//   The purpose of this test is to verify that types 1-13 create
//   the correct key maps for ParameterSet validation whenever default
//   arguments are included during construction.
//
// ======================================================================

#define BOOST_TEST_MODULE (keymap test)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/test/types/KeyMap.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Tuple.h"

#include <iostream>
#include <string>
#include <vector>

using namespace fhicl;
using namespace fhicl::detail;
using namespace std;

BOOST_AUTO_TEST_SUITE(types_keymap_defaults_test)

// [1] Atom<T>
BOOST_AUTO_TEST_CASE(one_atom_t)
{
  KeyMap km;
  Atom<int> test{Name("atom"), 4};
  km.walk_over(test);

  auto map = km.result();
  auto ref = {"atom"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [2] Sequence<T>
BOOST_AUTO_TEST_CASE(one_sequence_t)
{
  KeyMap km;
  Sequence<int> test{Name("sequence"), {1, 2, 4}};
  km.walk_over(test);

  auto map = km.result();
  auto ref = {"sequence", "sequence[0]", "sequence[1]", "sequence[2]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [3] Sequence<T,SZ>
BOOST_AUTO_TEST_CASE(one_sequence_2_t)
{
  KeyMap km;
  Sequence<int, 2> test{Name("sequence"), {5, 7}};
  km.walk_over(test);

  auto map = km.result();
  auto ref = {"sequence", "sequence[0]", "sequence[1]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [4] Tuple<T...>
BOOST_AUTO_TEST_CASE(one_tuple_t)
{
  KeyMap km;
  Tuple<int, double, bool> test{Name("tuple"), {4, 1.5, false}};
  km.walk_over(test);

  auto map = km.result();
  auto ref = {"tuple", "tuple[0]", "tuple[1]", "tuple[2]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [5] Tuple< Sequence<T>, U...>
BOOST_AUTO_TEST_CASE(seq_in_tuple_t)
{
  KeyMap km;
  Tuple<Sequence<int>, double, bool> test{Name("tuple"),
                                          {{1, 3, 5}, 4.6, true}};
  km.walk_over(test);

  auto map = km.result(); // key_map(&test);
  auto ref = {"tuple",
              "tuple[0]",
              "tuple[0][0]",
              "tuple[0][1]",
              "tuple[0][2]",
              "tuple[1]",
              "tuple[2]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [6] Tuple< Sequence<T,SZ>, U...>
BOOST_AUTO_TEST_CASE(bounded_seq_in_tuple_t)
{
  KeyMap km;
  Tuple<Sequence<int, 2>, double, bool> test{Name("tuple"),
                                             {{9, 15}, 0.2, false}};
  km.walk_over(test);

  auto map = km.result();
  auto ref = {
    "tuple", "tuple[0]", "tuple[0][0]", "tuple[0][1]", "tuple[1]", "tuple[2]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [7] Tuple< Tuple<T...>, U...>
BOOST_AUTO_TEST_CASE(tuple_in_tuple_t)
{
  KeyMap km;
  Tuple<Tuple<int, float>, double, bool> test{Name("tuple"),
                                              {{4, 3.7f}, 8.1, true}};
  km.walk_over(test);

  auto map = km.result();
  auto ref = {
    "tuple", "tuple[0]", "tuple[0][0]", "tuple[0][1]", "tuple[1]", "tuple[2]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [8] Sequence< Tuple<T...> >
BOOST_AUTO_TEST_CASE(tuple_in_seq_t)
{
  KeyMap km;
  Sequence<Tuple<int, float>> test{Name("seqtuple"),
                                   {{2, 5.4f}, {4, 104.5f}, {8, 15.3f}}};
  km.walk_over(test);

  auto map = km.result();
  auto ref = {"seqtuple",
              "seqtuple[0]",
              "seqtuple[0][0]",
              "seqtuple[0][1]",
              "seqtuple[1]",
              "seqtuple[1][0]",
              "seqtuple[1][1]",
              "seqtuple[2]",
              "seqtuple[2][0]",
              "seqtuple[2][1]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [9] Sequence< Tuple<T...>, SZ >
BOOST_AUTO_TEST_CASE(tuple_in_seq_2_t)
{
  KeyMap km;
  Sequence<Tuple<int, float>, 2> test{Name("seqtuple"), {{1, 2.3f}, {9, 3.2f}}};
  km.walk_over(test);

  auto map = km.result();
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
  KeyMap km;
  Sequence<Sequence<int>> test{Name("seqseq"), {{1, 5, 7}, {2}}};
  km.walk_over(test);

  auto map = km.result();
  auto ref = {"seqseq",
              "seqseq[0]",
              "seqseq[0][0]",
              "seqseq[0][1]",
              "seqseq[0][2]",
              "seqseq[1]",
              "seqseq[1][0]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [11] Sequence< Sequence<T,SZ> >
BOOST_AUTO_TEST_CASE(seq_2_in_seq_t)
{
  KeyMap km;
  Sequence<Sequence<int, 2>> test{Name("seqseq"), {{1, 2}}};
  km.walk_over(test);

  auto map = km.result();
  auto ref = {"seqseq", "seqseq[0]", "seqseq[0][0]", "seqseq[0][1]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [12] Sequence< Sequence<T>, SZ >
BOOST_AUTO_TEST_CASE(seq_in_seq_2_t)
{
  KeyMap km;
  Sequence<Sequence<int>, 2> test{Name("seqseq"), {{4}, {1, 4, 9, 1}}};
  km.walk_over(test);

  auto map = km.result();
  auto ref = {"seqseq",
              "seqseq[0]",
              "seqseq[0][0]",
              "seqseq[1]",
              "seqseq[1][0]",
              "seqseq[1][1]",
              "seqseq[1][2]",
              "seqseq[1][3]"};
  BOOST_TEST(map == ref, boost::test_tools::per_element{});
}

// [13] Sequence< Sequence<T,SZ>, SZ >
BOOST_AUTO_TEST_CASE(seq_2_in_seq_2_t)
{
  KeyMap km;
  Sequence<Sequence<int, 2>, 2> test{Name("seqseq"), {{6, 7}, {2, 1}}};
  km.walk_over(test);

  auto map = km.result();
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
