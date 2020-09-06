#ifndef fhiclcpp_types_MaybeUseFunction_h
#define fhiclcpp_types_MaybeUseFunction_h

#include <functional>

namespace fhicl {
  struct MaybeUseFunction {
    explicit MaybeUseFunction(std::function<bool()> func) : value{func} {}
    operator std::function<bool()>() const { return value; }
    std::function<bool()> value;
  };
}

#endif /* fhiclcpp_types_MaybeUseFunction_h */

// Local Variables:
// mode: c++
// End:
