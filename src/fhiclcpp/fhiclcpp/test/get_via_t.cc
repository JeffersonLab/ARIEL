#define BOOST_TEST_MODULE (get via converter test)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/container_algorithms.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"

#include <string>
#include <vector>

using namespace fhicl;
using namespace std::string_literals;

namespace {
  struct SortedNames {
    std::vector<std::string> names;
  };

  SortedNames
  to_sorted_names(std::vector<std::string> const& names)
  {
    SortedNames result{names};
    cet::sort_all(result.names);
    return result;
  }
}

BOOST_AUTO_TEST_SUITE(via_test)

BOOST_AUTO_TEST_CASE(via_test)
{
  auto const config = "number: 5 "
                      "names: ['Esther', 'Julian', 'Bobby']"s;
  ParameterSet pset;
  make_ParameterSet(config, pset);

  BOOST_TEST(pset.get<int>("number") == 5);

  auto check_value = [](int const& i) { return i == 5; };
  auto const supplied_5 = pset.get<bool, int>("number", check_value);
  BOOST_TEST(supplied_5);

  auto const sorted_names = pset.get("names", to_sorted_names);
  std::vector const ref{"Bobby"s, "Esther"s, "Julian"s};
  BOOST_TEST(sorted_names.names == ref);
}

BOOST_AUTO_TEST_SUITE_END()
