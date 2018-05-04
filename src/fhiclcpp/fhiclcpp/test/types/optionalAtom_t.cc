// ======================================================================
//
// test optional atom
//
// ======================================================================

#define BOOST_TEST_MODULE (optionalAtom test)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/types/OptionalAtom.h"
#include "fhiclcpp/types/detail/ParameterSchemaRegistry.h"

#include <iostream>
#include <string>
#include <vector>

using namespace fhicl;
using namespace fhicl::detail;
using namespace std;

BOOST_AUTO_TEST_SUITE(optionalAtom_test)

// [1] Atom<T>
BOOST_AUTO_TEST_CASE(optionalAtom_t_01)
{
  ParameterSet p1;
  ParameterSet p2;
  p2.put("optAtom", 1);
  OptionalAtom<int> test{Name("optAtom")};
}

BOOST_AUTO_TEST_SUITE_END()
