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

  struct NestedConfig {

    using Default_t = typename Val_t::default_type;
    using DefaultElement_t = typename Val_t::default_type::value_type;

    explicit NestedConfig(Default_t defaultPars = {})
      : pars(Name("pars"), std::move(defaultPars))
    {}

    Val_t pars;
  };

  struct Config {
    Table<NestedConfig> nested{Name("nested")};
    Table<NestedConfig> nestedWithDef{
      Name("nestedWithDef"),
      NestedConfig::Default_t{
        NestedConfig::DefaultElement_t{"T", {"U", "V", "W"}}}};
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

BOOST_AUTO_TEST_CASE(GoodTuple1)
{
  string const good{"nested: { pars: [[A, [B]], [X, [Y, Z]]] }"s};
  std::vector<std::string> const ref1{"B"s};
  std::vector<std::string> const ref2{"Y"s, "Z"s};
  std::vector<std::string> const ref3{"U"s, "V"s, "W"s};
  auto validatedConfig = validateConfig(good);
  auto const& pars = validatedConfig().nested().pars();
  BOOST_TEST_REQUIRE(pars.size() == 2ull);
  BOOST_TEST_REQUIRE(std::get<0>(pars[0]) == "A"s);
  BOOST_TEST_REQUIRE(std::get<1>(pars[0]) == ref1);
  BOOST_TEST_REQUIRE(std::get<0>(pars[1]) == "X"s);
  BOOST_TEST_REQUIRE(std::get<1>(pars[1]) == ref2);
  auto const& defaultedPars = validatedConfig().nestedWithDef().pars();
  BOOST_TEST_REQUIRE(defaultedPars.size() == 1ull);
  BOOST_TEST_REQUIRE(std::get<0>(defaultedPars[0]) == "T"s);
  BOOST_TEST_REQUIRE(std::get<1>(defaultedPars[0]) == ref3);
}

BOOST_AUTO_TEST_CASE(GoodTuple2)
{
  string const good{"nested: { pars: [[A, [B]], [X, [Y, Z]]] }\n"s +
                    "nestedWithDef: { pars: [[M, [N, O, P, Q]], [R, [S]]] }"s};
  std::vector<std::string> const ref1{"B"s};
  std::vector<std::string> const ref2{"Y"s, "Z"s};
  std::vector<std::string> const ref3{"N"s, "O"s, "P"s, "Q"s};
  std::vector<std::string> const ref4{"S"s};
  auto validatedConfig = validateConfig(good);
  auto const& pars = validatedConfig().nested().pars();
  BOOST_TEST_REQUIRE(pars.size() == 2ull);
  BOOST_TEST_REQUIRE(std::get<0>(pars[0]) == "A"s);
  BOOST_TEST_REQUIRE(std::get<1>(pars[0]) == ref1);
  BOOST_TEST_REQUIRE(std::get<0>(pars[1]) == "X"s);
  BOOST_TEST_REQUIRE(std::get<1>(pars[1]) == ref2);
  auto const& defaultedPars = validatedConfig().nestedWithDef().pars();
  BOOST_TEST_REQUIRE(defaultedPars.size() == 2ull);
  BOOST_TEST_REQUIRE(std::get<0>(defaultedPars[0]) == "M"s);
  BOOST_TEST_REQUIRE(std::get<1>(defaultedPars[0]) == ref3);
  BOOST_TEST_REQUIRE(std::get<0>(defaultedPars[1]) == "R"s);
  BOOST_TEST_REQUIRE(std::get<1>(defaultedPars[1]) == ref4);
}

BOOST_AUTO_TEST_CASE(BadTuple1)
{
  string const bad{"nested: { pars: [[A], [X, [Y, Z]]] }"s};
  BOOST_REQUIRE_THROW(validateConfig(bad), detail::validationException);
}

BOOST_AUTO_TEST_CASE(BadTuple2)
{
  string const bad{"nested: { pars: [[A]] }"s};
  BOOST_REQUIRE_THROW(validateConfig(bad), detail::validationException);
}

BOOST_AUTO_TEST_CASE(BadTuple3)
{
  string const bad{"nested: { pars: [[A, 7]] }"s};
  BOOST_REQUIRE_THROW(validateConfig(bad), fhicl::exception);
}

BOOST_AUTO_TEST_CASE(BadTuple4)
{
  string const bad{"nested: { pars: [[A, [7]]] }\n"s +
                   "nestedWithDef: { pars: [[A]]}"s};
  BOOST_REQUIRE_THROW(validateConfig(bad), detail::validationException);
}

BOOST_AUTO_TEST_SUITE_END()
