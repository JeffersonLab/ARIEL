#define BOOST_TEST_MODULE (Bounded sequences with defaults)

#include "cetlib/quiet_unit_test.hpp"
#include "cetlib/test_macros.h"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Name.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/Tuple.h"

#include <string>

using namespace fhicl;
using namespace std;
using namespace string_literals;

namespace {

  struct ArrayConfig {
    Sequence<string, 2> composers{Name{"composers"}, {"Mahler", "Elgar"}};
  };

  struct TupleConfig {
    Tuple<string, unsigned> ages{Name{"ages"}, {"David"s, 9}};
  };

  template <typename T>
  Table<T>
  validateConfig(std::string const& cfg)
  {
    ParameterSet ps;
    make_ParameterSet(cfg, ps);
    Table<T> validatedConfig{Name("validatedConfig")};
    validatedConfig.validate_ParameterSet(ps);
    return validatedConfig;
  }
}

BOOST_AUTO_TEST_SUITE(bounded_sequence_with_defaults)

BOOST_AUTO_TEST_CASE(GoodArray)
{
  string const good{};
  auto const& validatedTable = validateConfig<ArrayConfig>(good);
  BOOST_CHECK_EQUAL(validatedTable().composers(0), "Mahler"s);
  BOOST_CHECK_EQUAL(validatedTable().composers(1), "Elgar"s);
}

BOOST_AUTO_TEST_CASE(GoodTuple)
{
  string const good{};
  auto const& validatedTable = validateConfig<TupleConfig>(good);
  BOOST_CHECK_EQUAL(validatedTable().ages.get<0>(), "David"s);
  BOOST_CHECK_EQUAL(validatedTable().ages.get<1>(), 9);
}

BOOST_AUTO_TEST_CASE(BadSequence)
{
  string const bad{"composers: [Beethoven]"};
  BOOST_REQUIRE_THROW(validateConfig<ArrayConfig>(bad),
                      detail::validationException);
}

BOOST_AUTO_TEST_CASE(BadTuple)
{
  string const bad{"ages: [Jenny]"};
  BOOST_REQUIRE_THROW(validateConfig<TupleConfig>(bad),
                      detail::validationException);
}

BOOST_AUTO_TEST_SUITE_END()
