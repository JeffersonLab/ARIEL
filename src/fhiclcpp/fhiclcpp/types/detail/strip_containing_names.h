#ifndef fhiclcpp_types_detail_strip_containing_names_h
#define fhiclcpp_types_detail_strip_containing_names_h

#include <string>

namespace fhicl {
  namespace detail {
    std::string strip_first_containing_name(std::string const& key);
    std::string strip_all_containing_names(std::string const& key);
  }
}

#endif /* fhiclcpp_types_detail_strip_containing_names_h */

// Local variables:
// mode: c++
// End:
