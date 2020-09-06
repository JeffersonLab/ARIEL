#ifndef fhiclcpp_types_detail_check_nargs_for_bounded_sequences_h
#define fhiclcpp_types_detail_check_nargs_for_bounded_sequences_h

#include <cstddef>
#include <string>

namespace fhicl::detail {
  void check_nargs_for_bounded_sequences(std::string const& key,
                                         std::size_t expected,
                                         std::size_t provided);
}

#endif /* fhiclcpp_types_detail_check_nargs_for_bounded_sequences_h */

// Local variables:
// mode: c++
// End:
