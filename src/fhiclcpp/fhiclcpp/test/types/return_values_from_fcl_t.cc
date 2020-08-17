// ======================================================================
//
// test return values from FHiCL files
//
//    The purpose of this test is to verify that all non-table values
//    are accurately retrieved after being validated.
//
// ======================================================================

#define BOOST_TEST_MODULE (return values from fcl)

#include "cetlib/quiet_unit_test.hpp"
#include "fhiclcpp/test/types/FixtureBase.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Tuple.h"

#include <string>
#include <vector>

using namespace fhicl;
using namespace fhicl::detail;
using namespace std;

namespace {

  constexpr auto tolerance = std::numeric_limits<double>::epsilon();
  constexpr auto ftolerance = std::numeric_limits<float>::epsilon();

  struct Configuration {
    Atom<int> atom{Name("atom")};
    Sequence<int> vec{Name("vec")};
    Sequence<int, 2> arr{Name("arr")};

    Tuple<int, double, bool> tuple{Name("tuple")};
    Tuple<Sequence<int>, double, bool> tupWithVec{Name("tupWithVec")};
    Tuple<Sequence<int, 2>, double, bool> tupWithArr{Name("tupWithArr")};
    Tuple<Tuple<int, float>, double, bool> tupWithTup{Name("tupWithTup")};

    Sequence<Tuple<int, string>> vecWithTup{Name("vecWithTup")};
    Sequence<Tuple<string, int, string>, 2> arrWithTup{Name("arrWithTup")};

    Sequence<Sequence<int>> vecOfVec{Name("vecOfVec")};
    Sequence<Sequence<int, 2>> vecOfArr{Name("vecOfArr")};
    Sequence<Sequence<int>, 2> arrOfVec{Name("arrOfVec")};
    Sequence<Sequence<int, 2>, 2> arrOfArr{Name("arrOfArr")};
  };

  struct Fixture : fhiclcpp_types::FixtureBase<Configuration> {
    Fixture() : FixtureBase("return_values_from_fcl_t.fcl") {}
  };
}

// provide use of 'Table<Configuration> config'
BOOST_FIXTURE_TEST_SUITE(values_from_fcl, Fixture)

// [1] Atom<T>
BOOST_AUTO_TEST_CASE(atom)
{
  BOOST_TEST(config().atom() == 5);
}

// [2] Sequence<T>
BOOST_AUTO_TEST_CASE(seq_vector)
{
  auto ref = {3, 5, 8};
  auto val = config().vec();
  BOOST_TEST(val == ref, boost::test_tools::per_element{});
}

// [3] Sequence<T,SZ>
BOOST_AUTO_TEST_CASE(seq_array)
{
  auto ref = {4, 9};
  auto val = config().arr();
  BOOST_TEST(val == ref, boost::test_tools::per_element{});
}

// [4] Tuple<T...>
BOOST_AUTO_TEST_CASE(tuple)
{
  BOOST_TEST(config().tuple.get<0>() == 5);
  BOOST_TEST(config().tuple.get<1>() == 4.3, tolerance);
  BOOST_TEST(config().tuple.get<2>() == true);
}

// [5] Tuple< Sequence<T>, U...>
BOOST_AUTO_TEST_CASE(tuple_with_vector)
{
  auto ref = {4, 1, 4, 67, 89};
  auto val = config().tupWithVec.get<0>();
  BOOST_TEST(val == ref, boost::test_tools::per_element{});
  BOOST_TEST(config().tupWithVec.get<1>() == 4.56789, tolerance);
  BOOST_TEST(config().tupWithVec.get<2>() == false);
}

// [6] Tuple< Sequence<T,SZ>, U...>
BOOST_AUTO_TEST_CASE(tuple_with_array)
{
  auto ref = {5, 16};
  auto val = config().tupWithArr.get<0>();
  BOOST_TEST(val == ref, boost::test_tools::per_element{});
  BOOST_TEST(config().tupWithArr.get<1>() == 46.9, tolerance);
  BOOST_TEST(config().tupWithArr.get<2>() == true);
}

// [7] Tuple< Tuple<T...>, U...>
BOOST_AUTO_TEST_CASE(tuple_with_tuple)
{
  auto tuple0 = config().tupWithTup.get<0>();
  BOOST_TEST(std::get<0>(tuple0) == 4);
  BOOST_TEST(std::get<1>(tuple0) == 175.218f, ftolerance);
  BOOST_TEST(config().tupWithTup.get<1>() == 87.03, tolerance);
  BOOST_TEST(config().tupWithTup.get<2>() == false);
}

// [8] Sequence< Tuple<T...> >
BOOST_AUTO_TEST_CASE(vec_of_tups)
{
  vector<int> const nums{11, 22};
  vector<string> const strs{"Decay in orbit", "Radiative pion capture"};

  size_t i{};
  for (auto const& elem : config().vecWithTup()) {
    BOOST_TEST(std::get<0>(elem) == nums.at(i));
    BOOST_TEST(std::get<1>(elem) == strs.at(i++));
  }
}

// [9] Sequence< Tuple<T...>, SZ >
BOOST_AUTO_TEST_CASE(arr_of_tups)
{

  vector<string> const prefixes = {"Tchaikovsky wrote ", "Mahler wrote "};
  vector<int> const nums{6, 9};
  string const str = " symphonies";

  size_t i{};
  for (auto const& elem : config().arrWithTup()) {
    BOOST_TEST(std::get<0>(elem) == prefixes.at(i));
    BOOST_TEST(std::get<1>(elem) == nums.at(i++));
    BOOST_TEST(std::get<2>(elem) == str);
  }
}

// [10] Sequence< Sequence<T> >
BOOST_AUTO_TEST_CASE(vec_of_vecs)
{
  using vec_t = vector<int>;
  auto ref_vec = {vec_t{1, 4, 6, 8}, vec_t{1, 3}, vec_t{6}};
  auto it = ref_vec.begin();

  for (auto const& val : config().vecOfVec()) {
    auto ref = *it++;
    BOOST_TEST(val == ref, boost::test_tools::per_element{});
  }

  size_t i{};
  for (auto const& ref : ref_vec) {
    auto val = config().vecOfVec(i++);
    BOOST_TEST(val == ref, boost::test_tools::per_element{});
  }
}

// [11] Sequence< Sequence<T,SZ> >
BOOST_AUTO_TEST_CASE(vec_of_arrs)
{
  using array_t = array<int, 2>;
  auto ref_vec = {array_t{{1, 6}}, array_t{{2, 12}}, array_t{{3, 18}}};
  auto it = ref_vec.begin();

  for (auto const& val : config().vecOfArr()) {
    auto ref = *it++;
    BOOST_TEST(val == ref, boost::test_tools::per_element{});
  }
}

// [12] Sequence< Sequence<T>,SZ >
BOOST_AUTO_TEST_CASE(arr_of_vecs)
{
  auto ref_vec = {vector<int>{1, 2, 3, 4, 5, 6, 7}, vector<int>{8, 9, 10}};
  auto it = ref_vec.begin();

  for (auto const& val : config().arrOfVec()) {
    auto ref = *it++;
    BOOST_TEST(val == ref, boost::test_tools::per_element{});
  }
}

// [13] Sequence< Sequence<T,SZ>, SZ >
BOOST_AUTO_TEST_CASE(arr_of_arrs)
{
  auto ref_vec = {array<int, 2>{{1, 0}}, array<int, 2>{{0, 1}}};
  auto it = ref_vec.begin();

  for (auto const& val : config().arrOfArr()) {
    auto ref = *it++;
    BOOST_TEST(val == ref, boost::test_tools::per_element{});
  }
}

BOOST_AUTO_TEST_SUITE_END()
