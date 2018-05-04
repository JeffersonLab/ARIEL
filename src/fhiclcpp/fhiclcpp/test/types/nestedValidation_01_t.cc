#define BOOST_TEST_MODULE (Nested Validation)

#include "cetlib/quiet_unit_test.hpp"

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
  using Val_t = Tuple<string, Sequence<string>>;
  struct Config {
    Val_t pars{Name("pars")};
  };

  Table<Config>
  validateConfig(std::string const& cfg)
  {
    ParameterSet ps;
    make_ParameterSet(cfg, ps);
    Table<Config> validatedConfig{Name("validatedConfig")};
    validatedConfig.validate_ParameterSet(ps);
    return validatedConfig;
  }
}

BOOST_AUTO_TEST_SUITE(Nested_Validation)

BOOST_AUTO_TEST_CASE(GoodTuple)
{
  string const good{"pars: [X, [Y, Z]]"s};
  std::vector<std::string> const ref{"Y"s, "Z"s};
  auto validatedConfig = validateConfig(good);
  auto const& pars = validatedConfig().pars();
  BOOST_TEST_REQUIRE(std::get<0>(pars) == "X"s);
  BOOST_TEST_REQUIRE(std::get<1>(pars) == ref);
}

BOOST_AUTO_TEST_CASE(BadTuple1)
{
  string const bad{"pars: [ X ]"s};
  BOOST_REQUIRE_THROW(validateConfig(bad), detail::validationException);
}

BOOST_AUTO_TEST_CASE(BadTuple2)
{
  string const bad{"pars: [ X, 1 ]"s};
  BOOST_CHECK_THROW(validateConfig(bad), fhicl::exception);
}

BOOST_AUTO_TEST_SUITE_END()
