// ======================================================================
//
// test return values from FHiCL files
//
//    The purpose of this test is to verify that all non-table
//    optional values are accurately retrieved after being validated.
//
// ======================================================================

#define BOOST_TEST_MODULE (return optional values from fcl)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/types/OptionalAtom.h"
#include "fhiclcpp/types/OptionalSequence.h"
#include "fhiclcpp/types/OptionalTable.h"
#include "fhiclcpp/types/OptionalTuple.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/Tuple.h"

#include "cetlib/test_macros.h"
#include "fhiclcpp/test/types/FixtureBase.h"

#include <iostream>
#include <string>

using namespace fhicl;
using namespace fhicl::detail;
using namespace std;

namespace {

  double const tolerance [[gnu::unused]] = std::numeric_limits<double>::epsilon();
  double const ftolerance [[gnu::unused]] = std::numeric_limits<float>::epsilon();

  struct Physics {
    Tuple<std::string, double> energyCutoff{Name("energyCutoff")};
    OptionalAtom<std::string> moniker{Name("moniker")};
  };

  struct Composer {
    Atom<std::string> composer{Name("composer")};
    OptionalTuple<int, std::string> aSymphonyMoniker{Name("aSymphonyMoniker")};
  };

  struct Configuration {
    OptionalAtom<int> atom{Name("atom")};
    OptionalAtom<std::string> name{Name("name")};
    OptionalTable<Physics> physics{Name("physics")};
    OptionalSequence<int> list1{Name("list1")};
    OptionalSequence<int, 4> list2{Name("list2")};
    OptionalSequence<Sequence<int, 2>> list3{Name("list3")};
    OptionalSequence<Table<Composer>, 4> list4{Name("list4")};
  };

  struct Fixture : fhiclcpp_types::FixtureBase<Configuration> {
    Fixture() : FixtureBase("return_optionalValues_from_fcl_t.fcl") {}
  };
}

// provide use of 'Table<Configuration> config'
BOOST_FIXTURE_TEST_SUITE(optionalValues_from_fcl, Fixture)

// [1] OptionalAtom<T>
BOOST_AUTO_TEST_CASE(optAtom_t)
{
  int i{};
  std::string n;
  BOOST_CHECK(config().atom(i));
  BOOST_CHECK(!config().name(n));
  BOOST_CHECK_EQUAL(i, 5);
  BOOST_CHECK(n.empty());
}

// [2] OptionalTable<T>
BOOST_AUTO_TEST_CASE(optTable_t)
{
  Physics phys;
  std::string name;
  BOOST_CHECK(config().physics(phys));
  BOOST_CHECK(phys.moniker(name));
  BOOST_CHECK_EQUAL(name, "Johnny");
  BOOST_CHECK_EQUAL(phys.energyCutoff.get<0>(), "QGSP");
  BOOST_CHECK_CLOSE_FRACTION(phys.energyCutoff.get<1>(), 14.6, tolerance);
}

// [3] OptionalSequence<T>
BOOST_AUTO_TEST_CASE(optSeqVector_t1)
{
  std::vector<int> intList;
  BOOST_CHECK(!config().list1(intList));
}

// [4] OptionalSequence<T,SIZE>
BOOST_AUTO_TEST_CASE(optSeqVector_t2)
{
  std::array<int, 4> intList;
  auto ref = {1, 2, 4, 8};
  BOOST_CHECK(config().list2(intList));
  CET_CHECK_EQUAL_COLLECTIONS(intList, ref);
}

// [5] OptionalSequence<T>
BOOST_AUTO_TEST_CASE(optSeqVector_t3)
{
  std::vector<std::array<int, 2>> intLists;
  BOOST_CHECK(config().list3(intLists));

  decltype(intLists) const ref{{{0, 1}}, {{1, 2}}, {{2, 4}}, {{3, 8}}};
  std::size_t i{};
  for (auto const& list : intLists) {
    CET_CHECK_EQUAL_COLLECTIONS(list, ref[i]);
    ++i;
  }
}

// [6] OptionalSequence<T>
BOOST_AUTO_TEST_CASE(optSeqVector_t4)
{
  std::array<Composer, 4> composers;
  BOOST_CHECK(config().list4(composers));

  std::array<std::string, 4> const ref{
    {"Mozart", "Beethoven", "Brahms", "Mahler"}};

  std::size_t i{};
  for (auto const& comp : composers) {
    BOOST_CHECK_EQUAL(comp.composer(), ref[i]);
    ++i;
  }
}

// [7] OptionalTuple<T>
BOOST_AUTO_TEST_CASE(optTuple_t)
{
  enum composer_t { Mozart, Beethoven, Brahms, Mahler };

  std::array<Composer, 4> composers;
  BOOST_CHECK(config().list4(composers));

  std::array<int, 4> const symphonyNumbers{{41, 3, 0, 8}};
  std::array<std::string, 4> const symphonyMonikers{
    {"Jupiter", "Eroica", "", "Symphony for a Thousand"}};
  std::size_t i{};
  for (auto const& comp : composers) {

    std::tuple<int, std::string> moniker;
    if (i != static_cast<composer_t>(Brahms)) {
      BOOST_CHECK(comp.aSymphonyMoniker(moniker));
    } else {
      BOOST_CHECK(!comp.aSymphonyMoniker(moniker));
    }

    BOOST_CHECK_EQUAL(std::get<int>(moniker), symphonyNumbers[i]);
    BOOST_CHECK_EQUAL(std::get<std::string>(moniker), symphonyMonikers[i]);
    ++i;
  }
}

BOOST_AUTO_TEST_SUITE_END()
