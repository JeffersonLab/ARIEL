// ======================================================================
//
// test tableAs
//
// ======================================================================

#define BOOST_TEST_MODULE (tableAs test)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/test/types/FixtureBase.h"
#include "fhiclcpp/types/OptionalTableAs.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/TableAs.h"

#include <string>
#include <vector>

using namespace fhicl;
using namespace std;

namespace {

  // ===========================================================================
  struct Person {

    string name_;
    unsigned age_{};

    Person() = default;
    Person(string name, unsigned age) : name_{name}, age_{age} {}

    bool
    operator==(Person const& p) const
    {
      return (name_ == p.name_) && (age_ == p.age_);
    }

    bool
    operator!=(Person const& p) const
    {
      return !operator==(p);
    }
  };

  struct PersonConfig {
    Atom<string> name{Name{"name"}};
    Atom<unsigned> age{Name{"age"}};
  };

  Person
  convert(PersonConfig const& config)
  {
    return Person{config.name(), config.age()};
  }

  ostream&
  operator<<(ostream& os, Person const& p)
  {
    return os << "Name: " << p.name_ << "  Age: " << p.age_;
  }

  // ===========================================================================
  struct ContactInfo {
    string street_address;
    string phone_number;
  };

  bool
  operator==(ContactInfo const& a, ContactInfo const& b)
  {
    return a.street_address == b.street_address &&
           a.phone_number == b.phone_number;
  }

  namespace address {
    struct ContactInfoConfig {
      Atom<string> street_address{Name{"street_address"}};
      Atom<string> phone_number{Name{"phone_number"}};
    };

    ContactInfo
    convert(ContactInfoConfig const& config)
    {
      return {config.street_address(), config.phone_number()};
    }
  }

  ostream&
  operator<<(ostream& os, ContactInfo const& info)
  {
    return os << info.street_address << ", " << info.phone_number;
  }

  // ===========================================================================
  struct Household {
    TableAs<Person, PersonConfig> mother{Name{"mother"}};
    TableAs<Person, PersonConfig> father{Name{"father"}, Person{"John", 30}};
    Sequence<TableAs<Person, PersonConfig>> kids{Name("kids")};
    OptionalTableAs<ContactInfo, address::ContactInfoConfig> contact_info{
      Name("contact_info")};
  };

  struct Fixture : fhiclcpp_types::FixtureBase<Household> {
    Fixture() : FixtureBase("tableAs_t.fcl") {}
  };
}
// provide use of 'Table<Config> config'

BOOST_FIXTURE_TEST_SUITE(tableAs_test, Fixture)

BOOST_AUTO_TEST_CASE(tableAs_simple)
{
  auto const mother = config().mother();
  auto const ref = Person{"Angela", 32};
  BOOST_TEST(mother == ref);
}

BOOST_AUTO_TEST_CASE(tableAs_default_simple)
{
  auto const father = config().father();
  auto const ref = Person{"John", 30};
  BOOST_TEST(father == ref);
}

BOOST_AUTO_TEST_CASE(tableAs_inSequence)
{
  auto const kids = config().kids();
  auto const ref = {Person{"Alice", 1},
                    Person{"Bob", 2},
                    Person{"Charlie", 3},
                    Person{"Danielle", 4},
                    Person{"Edgar", 5}};
  BOOST_TEST(kids == ref, boost::test_tools::per_element{});
}

BOOST_AUTO_TEST_CASE(optionalTableAs)
{
  ContactInfo info;
  BOOST_TEST(config().contact_info(info));
  auto const ref = ContactInfo{"123 S. 45th Ave.", "(123) 456-7890"};
  BOOST_TEST(info == ref);
}

BOOST_AUTO_TEST_SUITE_END()
