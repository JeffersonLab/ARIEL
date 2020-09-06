#ifndef canvas_Persistency_Common_detail_is_handle_h
#define canvas_Persistency_Common_detail_is_handle_h

#include "cetlib/metaprogramming.h"

#include <type_traits>

namespace art {
  namespace detail {
    template <class T, class Enable = void>
    struct is_handle : std::false_type {};

    template <class T>
    struct is_handle<T, std::void_t<typename T::HandleTag>> : std::true_type {};

    template <class T, class U>
    struct are_handles {
      static constexpr bool value{detail::is_handle<T>::value &&
                                  detail::is_handle<U>::value};
    };
  }
}
#endif /* canvas_Persistency_Common_detail_is_handle_h */

// Local Variables:
// mode: c++
// End:
