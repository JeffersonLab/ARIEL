#ifndef canvas_Persistency_Common_detail_throwPartnerException_h
#define canvas_Persistency_Common_detail_throwPartnerException_h

#include <typeinfo>

namespace art {
  namespace detail {
    [[noreturn]] void throwPartnerException(
      std::type_info const& generator,
      std::type_info const& wanted_wrapper_type);
  }
}
#endif /* canvas_Persistency_Common_detail_throwPartnerException_h */

// Local Variables:
// mode: c++
// End:
