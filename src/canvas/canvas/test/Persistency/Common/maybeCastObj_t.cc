#define BOOST_TEST_MODULE (Tests of maybeCastObj interface)
#include "cetlib/quiet_unit_test.hpp"

#include <string>

#include "canvas/Persistency/Common/detail/maybeCastObj.h"
namespace {
  // Helper function
  template <typename T, typename U>
  bool
  runUpcastAllowed()
  {
    return art::detail::upcastAllowed(typeid(T), typeid(U));
  }

  // Testing hierarchy
  // Non virtual
  class MCBase {};

  class MCConcrete : public MCBase {};

  // Virtual
  class MCBaseVirtual {
  public:
    virtual ~MCBaseVirtual() {}
    virtual int
    foo()
    {
      return 0;
    }
  };

  class MCConcreteVirtual_A : public MCBaseVirtual {
  public:
    virtual ~MCConcreteVirtual_A() {}
    virtual int
    foo()
    {
      return 1;
    }
  };

  class MCConcreteVirtual_B : public MCBaseVirtual {
  public:
    virtual ~MCConcreteVirtual_B() {}
    virtual int
    foo()
    {
      return 2;
    }
  };

  class MCConcreteVirtual_C : public MCConcreteVirtual_B, public MCConcrete {
  public:
    virtual ~MCConcreteVirtual_C() {}
    virtual int
    foo()
    {
      return 3;
    }
  };

  // Other tree of inheritance
  class MCOtherBase {};

  // Multiple
  class MCConcreteMultiple : public MCBase, public MCOtherBase {};

  // Ambiguous?
}

BOOST_AUTO_TEST_SUITE(maybeCastObj_t)

BOOST_AUTO_TEST_CASE(identical_types)
{
  // Primitive types
  BOOST_TEST_REQUIRE((runUpcastAllowed<int, int>()));
  BOOST_TEST_REQUIRE((runUpcastAllowed<double, double>()));

  // Std class type
  BOOST_TEST_REQUIRE((runUpcastAllowed<std::string, std::string>()));

  // User class types
  BOOST_TEST_REQUIRE((runUpcastAllowed<MCBase, MCBase>()));
  BOOST_TEST_REQUIRE((runUpcastAllowed<MCConcrete, MCConcrete>()));
}

BOOST_AUTO_TEST_CASE(mismatched_types)
{
  // Different non-class types are never castable
  BOOST_TEST_REQUIRE(!(runUpcastAllowed<int, double>()));

  // Can never cast primitive->class or class->primitive
  BOOST_TEST_REQUIRE((!runUpcastAllowed<double, std::string>()));
  BOOST_TEST_REQUIRE((!runUpcastAllowed<std::string, char>()));

  // Should not be able to cast from one user hierarchy to another
  BOOST_TEST_REQUIRE((!runUpcastAllowed<MCConcrete, MCOtherBase>()));
}

BOOST_AUTO_TEST_CASE(user_types)
{
  // NB: In all of the below, tests of valid upcasts fail on
  // Root implementation. *Possibly* due to lack of dictionary?
  // Cast from base <-> concrete (non-virtual)
  BOOST_TEST_REQUIRE((!runUpcastAllowed<MCBase, MCConcrete>()));
  BOOST_TEST_REQUIRE((runUpcastAllowed<MCConcrete, MCBase>()));

  // Cast from raw base <-> concrete (virtual)
  BOOST_TEST_REQUIRE((!runUpcastAllowed<MCBaseVirtual, MCConcreteVirtual_A>()));
  BOOST_TEST_REQUIRE((runUpcastAllowed<MCConcreteVirtual_A, MCBaseVirtual>()));

  // Across hierarchy
  BOOST_TEST_REQUIRE(
    (!runUpcastAllowed<MCConcreteVirtual_B, MCConcreteVirtual_A>()));

  // Between levels
  BOOST_TEST_REQUIRE(
    (runUpcastAllowed<MCConcreteVirtual_C, MCConcreteVirtual_B>()));
  BOOST_TEST_REQUIRE((runUpcastAllowed<MCConcreteVirtual_C, MCConcrete>()));

  // Multiple
  BOOST_TEST_REQUIRE((runUpcastAllowed<MCConcreteMultiple, MCBase>()));
  BOOST_TEST_REQUIRE((runUpcastAllowed<MCConcreteMultiple, MCOtherBase>()));
}

BOOST_AUTO_TEST_CASE(user_type_variables)
{
  // Non Virtual
  MCConcrete actualConcrete{};
  MCBase& refToBase = actualConcrete;
  BOOST_TEST_REQUIRE(
    (!art::detail::upcastAllowed(typeid(refToBase), typeid(MCConcrete))));

  // Virtual
  MCConcreteVirtual_A actualConcreteVirtual{};
  MCBaseVirtual& refToBaseVirtual = actualConcreteVirtual;
  BOOST_TEST_REQUIRE((art::detail::upcastAllowed(typeid(refToBaseVirtual),
                                                 typeid(MCConcreteVirtual_A))));
  BOOST_TEST_REQUIRE((!art::detail::upcastAllowed(
    typeid(refToBaseVirtual), typeid(MCConcreteVirtual_B))));
}

BOOST_AUTO_TEST_SUITE_END()
