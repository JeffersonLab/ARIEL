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
  using Val_t = Sequence<Tuple<string, Sequence<string>>>;
  struct Config {
    Val_t pars{Name("pars")};
  };

  Table<Config>
  validateConfig(std::string const& cfg)
  {
    intermediate_table table;
    ParameterSet ps;
    parse_document(cfg, table);
    make_ParameterSet(table, ps);
    Table<Config> validatedConfig{Name("validatedConfig")};
    validatedConfig.validate_ParameterSet(ps);
    return validatedConfig;
  }
}

BOOST_AUTO_TEST_SUITE(Nested_Validation)

BOOST_AUTO_TEST_CASE(GoodTuple1)
{
  string const good{"pars: [[A, [B]], [X, [Y, Z]]]"s};
  std::vector<std::string> const ref1{"B"s};
  std::vector<std::string> const ref2{"Y"s, "Z"s};
  auto validatedConfig = validateConfig(good);
  auto const& pars = validatedConfig().pars();
  BOOST_TEST_REQUIRE(pars.size() == 2);
  BOOST_TEST_REQUIRE(std::get<0>(pars[0]) == "A"s);
  BOOST_TEST_REQUIRE(std::get<1>(pars[0]) == ref1);
  BOOST_TEST_REQUIRE(std::get<0>(pars[1]) == "X"s);
  BOOST_TEST_REQUIRE(std::get<1>(pars[1]) == ref2);
}

BOOST_AUTO_TEST_CASE(BadTuple1)
{
  string const bad{"pars: [[A], [X, [Y, Z]]]"s};
  BOOST_REQUIRE_THROW(validateConfig(bad), detail::validationException);
}

BOOST_AUTO_TEST_CASE(BadTuple2)
{
  string const bad{"pars: [[A]]"s};
  BOOST_REQUIRE_THROW(validateConfig(bad), detail::validationException);
}

BOOST_AUTO_TEST_CASE(BadTuple3)
{
  string const bad{"pars: [[A, 7]]"s};
  BOOST_REQUIRE_THROW(validateConfig(bad), fhicl::exception);
}

BOOST_AUTO_TEST_SUITE_END()
