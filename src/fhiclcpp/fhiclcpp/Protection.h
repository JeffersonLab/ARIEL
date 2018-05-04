#ifndef fhiclcpp_Protection_h
#define fhiclcpp_Protection_h

#include <string>

namespace fhicl {
  enum class Protection : unsigned char { NONE, PROTECT_IGNORE, PROTECT_ERROR };

  std::string to_string(Protection p);
}

#endif /* fhiclcpp_Protection_h */

// Local Variables:
// mode: c++
// End:
