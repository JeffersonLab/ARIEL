#ifndef fhiclcpp_test_boost_test_print_pset_h
#define fhiclcpp_test_boost_test_print_pset_h

// ==================================================================
// The function here is provided so that Boost knows how to form a
// printout message should a test (e.g. BOOST_TEST(ps1 == ps2)) fail.
// ==================================================================

#include "fhiclcpp/ParameterSet.h"

#include <ostream>

namespace fhicl {
  inline std::ostream&
  boost_test_print_type(std::ostream& os, ParameterSet const& pset)
  {
    return os << pset.to_indented_string();
  }
}

#endif /* fhiclcpp_test_boost_test_print_pset_h */

// Local Variables:
// mode: c++
// End:
