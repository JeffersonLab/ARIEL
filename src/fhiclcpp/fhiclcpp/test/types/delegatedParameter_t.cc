#define BOOST_TEST_MODULE (delegatedParameter test)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/test/types/FixtureBase.h"
#include "fhiclcpp/types/DelegatedParameter.h"
#include "fhiclcpp/types/OptionalDelegatedParameter.h"

#include <string>

using namespace fhicl;
using namespace std::string_literals;

namespace {
  struct S {
    DelegatedParameter da{Name("delegated_atom")};
    OptionalDelegatedParameter oda{Name("optional_delegated_atom")};
  };

  struct Config {
    DelegatedParameter dt{Name("delegated_table")};
    DelegatedParameter ds{Name("delegated_sequence")};
    Table<S> nested{Name("s")};
  };

  struct Fixture : fhiclcpp_types::FixtureBase<Config> {
    Fixture() : FixtureBase("delegatedParameter_t.fcl") {}
  };

  void
  receive_PSet(ParameterSet const& pset)
  {
    std::set<std::string> const ref{
      "everything", "everything[0]", "you", "imagine"};
    auto const& test = pset.get_all_keys();
    BOOST_TEST(test == ref, boost::test_tools::per_element{});
  }

  void
  receive_vector(std::vector<int> const& v)
  {
    std::vector const ref{1, 2, 3};
    BOOST_TEST(v == ref);
  }

  void
  receive_int(int const i)
  {
    BOOST_TEST(i == 3);
  }
}

BOOST_FIXTURE_TEST_SUITE(delegatedParameter_t, Fixture)

BOOST_AUTO_TEST_CASE(delegation_check)
{
  receive_PSet(config().dt.get<ParameterSet>());
  receive_vector(config().ds.get<std::vector<int>>());
  receive_int(config().nested().da.get<int>());
  std::string maybe_string;
  config().nested().oda.get_if_present(maybe_string);
  BOOST_TEST("Hello, Billy"s == maybe_string);
}

BOOST_AUTO_TEST_SUITE_END()
