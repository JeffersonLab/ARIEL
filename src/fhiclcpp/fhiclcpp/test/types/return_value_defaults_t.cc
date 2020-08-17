// ======================================================================
//
// test types return values with defaults:
//
//   verify that types 1-13 below return the correct values
//
// ======================================================================

#define BOOST_TEST_MODULE (return value defaults)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Tuple.h"

#include <string>
#include <vector>

using namespace fhicl;
using namespace fhicl::detail;
using namespace std;

BOOST_AUTO_TEST_SUITE(types_return_value_defaults_test)

// [1] Atom<T>
BOOST_AUTO_TEST_CASE(one_atom_t)
{
  Atom<int> test{Name("atom"), 4};
  BOOST_TEST(test() == 4);
}

// [2] Sequence<T>
BOOST_AUTO_TEST_CASE(one_sequence_t)
{
  auto ref = {1, 2, 4};
  Sequence<int> test{Name("sequence"), ref};
  BOOST_TEST(test() == ref, boost::test_tools::per_element{});
  std::size_t i{};
  for (auto const& elem : ref)
    BOOST_TEST(elem == test(i++));
}

// [3] Sequence<T,SZ>
BOOST_AUTO_TEST_CASE(one_sequence_2_t)
{
  auto ref = {5, 7};
  Sequence<int, 2> test{Name("sequence"), ref};
  BOOST_TEST(test() == ref, boost::test_tools::per_element{});

  std::size_t i{};
  for (auto const& elem : ref)
    BOOST_TEST(elem == test(i++));
}

// [4] Tuple<T...>
BOOST_AUTO_TEST_CASE(one_tuple_t)
{
  Tuple<int, double, bool> test{Name("tuple"), {4, 1.5, false}};
  BOOST_TEST(test.get<0>() == 4);
  BOOST_TEST(test.get<1>() == 1.5);
  BOOST_TEST(test.get<2>() == false);
}

// [5] Tuple< Sequence<T>, U...>
BOOST_AUTO_TEST_CASE(seq_in_tuple_t)
{
  auto ref = {1, 3, 5};
  Tuple<Sequence<int>, double, bool> test{Name("tuple"), {ref, 4.6, true}};
  BOOST_TEST(test.get<0>() == ref, boost::test_tools::per_element{});
  BOOST_TEST(test.get<1>() == 4.6);
  BOOST_TEST(test.get<2>() == true);
}

// [6] Tuple< Sequence<T,SZ>, U...>
BOOST_AUTO_TEST_CASE(bounded_seq_in_tuple_t)
{
  auto ref = {9, 15};
  Tuple<Sequence<int, 2>, double, bool> test{Name("tuple"), {ref, 0.2, false}};
  BOOST_TEST(test.get<0>() == ref, boost::test_tools::per_element{});
  BOOST_TEST(test.get<1>() == 0.2);
  BOOST_TEST(test.get<2>() == false);
}

// [7] Tuple< Tuple<T...>, U...>
BOOST_AUTO_TEST_CASE(tuple_in_tuple_t)
{
  Tuple<Tuple<int, float>, double, bool> test{Name("tuple"),
                                              {{4, 3.7f}, 8.1, true}};
  auto const tuple0 = test.get<0>();
  BOOST_TEST(std::get<0>(tuple0) == 4);
  BOOST_TEST(std::get<1>(tuple0) == 3.7f);
  BOOST_TEST(test.get<1>() == 8.1);
  BOOST_TEST(test.get<2>() == true);
}

// [8] Sequence< Tuple<T...> >
BOOST_AUTO_TEST_CASE(tuple_in_seq_t)
{
  std::vector<std::tuple<int, float>> ref_vec{std::make_tuple(2, 5.4f),
                                              std::make_tuple(4, 104.5f),
                                              std::make_tuple(8, 15.3f)};
  Sequence<Tuple<int, float>> test{Name("seqtuple"),
                                   {{2, 5.4f}, {4, 104.5f}, {8, 15.3f}}};
  std::size_t i{};
  for (auto const& elem : test()) {
    BOOST_TEST(std::get<0>(elem) == std::get<0>(ref_vec.at(i)));
    BOOST_TEST(std::get<1>(elem) == std::get<1>(ref_vec.at(i++)));
  }
}

// [9] Sequence< Tuple<T...>, SZ >
BOOST_AUTO_TEST_CASE(tuple_in_seq_2_t)
{
  std::array<std::tuple<int, float>, 2> ref_vec{
    {std::make_tuple(1, 2.3f), std::make_tuple(9, 3.2f)}};

  Sequence<Tuple<int, float>, 2> test{Name("seqtuple"), {{1, 2.3f}, {9, 3.2f}}};
  std::size_t i{};
  for (auto const& elem : test()) {
    BOOST_TEST(std::get<0>(elem) == std::get<0>(ref_vec.at(i)));
    BOOST_TEST(std::get<1>(elem) == std::get<1>(ref_vec.at(i++)));
  }
}

// [10] Sequence< Sequence<T> >
BOOST_AUTO_TEST_CASE(seq_in_seq_t)
{
  auto ref_vec = std::vector<std::vector<int>>{{1, 5, 7}, {2}};
  Sequence<Sequence<int>> test{Name("seqseq"), {{1, 5, 7}, {2}}};
  std::size_t i{};
  for (auto const& val : test()) {
    auto ref = ref_vec.at(i++);
    BOOST_TEST(val == ref);
  }

  i = 0ul;
  for (auto const& ref : ref_vec) {
    auto val = test(i++);
    BOOST_TEST(val == ref);
  }
}

// [11] Sequence< Sequence<T,SZ> >
BOOST_AUTO_TEST_CASE(seq_2_in_seq_t)
{
  auto ref_vec = std::vector<std::array<int, 2>>{{{1, 2}}};
  Sequence<Sequence<int, 2>> test{Name("seqseq"), {{1, 2}}};
  std::size_t i{};
  for (auto const& val : test()) {
    auto ref = ref_vec.at(i++);
    BOOST_TEST(val == ref);
  }
}

// [12] Sequence< Sequence<T>,SZ >
BOOST_AUTO_TEST_CASE(seq_in_seq_2_t)
{
  std::array<std::vector<int>, 2> ref_vec{
    {std::vector<int>{4}, std::vector<int>{{1, 4, 9, 1}}}};

  Sequence<Sequence<int>, 2> test{Name("seqseq"), {{4}, {1, 4, 9, 1}}};
  std::size_t i{};
  for (auto const& val : test()) {
    auto ref = ref_vec.at(i++);
    BOOST_TEST(val == ref);
  }
}

// [13] Sequence< Sequence<T,SZ>, SZ >
BOOST_AUTO_TEST_CASE(seq_2_in_seq_2_t)
{
  std::array<std::array<int, 2>, 2> ref_vec{
    {std::array<int, 2>{{6, 7}}, std::array<int, 2>{{2, 1}}}};

  Sequence<Sequence<int, 2>, 2> test{Name("seqseq"), {{6, 7}, {2, 1}}};
  std::size_t i{};
  for (auto const& val : test()) {
    auto ref = ref_vec.at(i++);
    BOOST_TEST(val == ref);
  }
}
BOOST_AUTO_TEST_SUITE_END()
