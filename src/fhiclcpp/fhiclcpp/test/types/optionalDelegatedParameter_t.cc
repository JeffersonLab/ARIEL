#define BOOST_TEST_MODULE (optionalDelegatedParameter test)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/test/types/FixtureBase.h"
#include "fhiclcpp/types/OptionalDelegatedParameter.h"

using namespace fhicl;

namespace {
  struct S {
    OptionalDelegatedParameter nda{Name("nested_optional_delegated_atom")};
    OptionalDelegatedParameter ndt{Name("pset")};
  };

  struct Config {
    OptionalDelegatedParameter dt{Name("optional_delegated_table")};
    OptionalDelegatedParameter ds{Name("optional_delegated_sequence")};
    Table<S> nested{Name("s")};
  };

  struct Fixture : fhiclcpp_types::FixtureBase<Config> {
    Fixture() : FixtureBase("optionalDelegatedParameter_t.fcl") {}
  };
}

BOOST_FIXTURE_TEST_SUITE(optionalDelegatedParameter_t, Fixture)

BOOST_AUTO_TEST_CASE(optional_delegation_check)
{
  ParameterSet ps;
  BOOST_TEST(config().dt.get_if_present(ps));

  std::vector<int> nums;
  BOOST_TEST(config().ds.get_if_present(nums));

  std::string greeting;
  BOOST_TEST(config().nested().nda.get_if_present(greeting));
  BOOST_TEST(greeting == "Hello, Billy");

  ps = ParameterSet{};
  BOOST_TEST(not config().nested().ndt.get_if_present(ps));
  BOOST_TEST(ps.is_empty());
}

BOOST_AUTO_TEST_SUITE_END()
