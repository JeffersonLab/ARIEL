#ifndef fhiclcpp_detail_encode_extended_value_h
#define fhiclcpp_detail_encode_extended_value_h

// ======================================================================
//
// encode overload for extended_value
//
// ======================================================================

#include "boost/any.hpp"

namespace fhicl {

  class extended_value;

  namespace detail {

    boost::any encode(extended_value const& xval);
  }
}
#endif /* fhiclcpp_detail_encode_extended_value_h */

// Local Variables:
// mode: c++
// End:
