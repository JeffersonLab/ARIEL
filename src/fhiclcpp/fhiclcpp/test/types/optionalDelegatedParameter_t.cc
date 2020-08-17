#define BOOST_TEST_MODULE (optionalDelegatedParameter test)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/test/types/FixtureBase.h"
#include "fhiclcpp/types/OptionalDelegatedParameter.h"

using namespace fhicl;

namespace {
  struct S {
    OptionalDelegatedParameter da{Name("optional_delegated_atom")};
  };

  struct Config {
    OptionalDelegatedParameter dt{Name("optional_delegated_table")};
    OptionalDelegatedParameter ds{Name("optional_delegated_sequence")};
    Table<S> nested{Name("s")};
  };

  struct Fixture : fhiclcpp_types::FixtureBase<Config> {
    Fixture() : FixtureBase("delegatedParameter_t.fcl") {}
  };
}

BOOST_FIXTURE_TEST_SUITE(optionalDelegatedParameter_t, Fixture)

BOOST_AUTO_TEST_CASE(optional_delegation_check)
{
  ParameterSet ps;
  config().dt.get_if_present(ps);
}

BOOST_AUTO_TEST_SUITE_END()
