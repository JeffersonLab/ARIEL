#ifndef art_root_io_detail_getObjectRequireDict_h
#define art_root_io_detail_getObjectRequireDict_h

#include "canvas/Utilities/TypeID.h"

namespace art {
  namespace root {

    namespace detail {
      void require_dictionary(TypeID const&) noexcept(false);
    }

    template <typename T>
    T
    getObjectRequireDict() noexcept(false)
    {
      detail::require_dictionary(TypeID{typeid(T)});
      return T{};
    }

  } // namespace root
} // namespace art

#endif /* art_root_io_detail_getObjectRequireDict_h */

// Local Variables:
// mode: c++
// End:
