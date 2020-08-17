#ifndef canvas_Utilities_ensurePointer_h
#define canvas_Utilities_ensurePointer_h

// Ensure we can get a desired pointer type from a compatible iterator.

// In order to allow new iterator value types (e.g. smart pointers) to
// be converted to pointers. specialize art::detail::EnsurePointer.

// See canvas/Persistency/Common/Ptr.h for an example of this. Also see
// test/Utilities/ensurePointer.h for testing coverage illustration.

#include "canvas/Utilities/Exception.h"
#include "cetlib_except/demangle.h"

#include <iterator>
#include <type_traits>

namespace art {

  template <typename WANTED_POINTER, typename InputIterator>
  WANTED_POINTER ensurePointer(InputIterator it);

  namespace detail {
    template <typename T1, typename T2>
    struct are_cv_compatible {
      using T1P = std::remove_cv_t<std::remove_pointer_t<T1>>;
      using T2P = std::remove_cv_t<std::remove_pointer_t<T2>>;
      static constexpr bool value{std::is_base_of_v<T1P, T2P> ||
                                  std::is_same_v<T1P, T2P>};
    };

    template <typename TO, typename FROM>
    constexpr std::enable_if_t<are_cv_compatible<TO, FROM>::value,
                               std::add_pointer_t<std::remove_pointer_t<TO>>>
    addr(FROM& from)
    {
      return &from;
    }

    template <typename TO, typename FROM>
    constexpr std::enable_if_t<!are_cv_compatible<TO, FROM>::value &&
                                 are_cv_compatible<FROM, TO>::value,
                               std::add_pointer_t<std::remove_pointer_t<TO>>>
    addr(FROM& from)
    {
      return &dynamic_cast<
        std::add_lvalue_reference_t<std::remove_pointer_t<TO>>>(from);
    }

    template <typename TO, typename FROM>
    struct EnsurePointer {
      constexpr TO
      operator()(FROM& from) const
      {
        return detail::addr<TO>(from);
      }
      constexpr TO
      operator()(FROM const& from) const
      {
        return detail::addr<TO>(from);
      }
    };

    template <typename TO, typename PFROM>
    struct EnsurePointer<TO, PFROM*> {
      constexpr TO
      operator()(PFROM* from) const
      {
        return detail::addr<TO>(*from);
      }
    };
  }
}

template <typename WANTED_POINTER, typename InputIterator>
inline WANTED_POINTER
art::ensurePointer(InputIterator it) try {
  static_assert(std::is_pointer_v<WANTED_POINTER>,
                "Supplied template argument is not a pointer.");
  return detail::EnsurePointer<
    WANTED_POINTER,
    typename std::iterator_traits<InputIterator>::value_type>{}(*it);
}
catch (std::bad_cast&) {
  throw Exception(errors::LogicError, "ensurePointer")
    << "Iterator value type "
    << cet::demangle_symbol(
         typeid(typename std::iterator_traits<InputIterator>::value_type)
           .name())
    << " and wanted pointer type "
    << cet::demangle_symbol(typeid(WANTED_POINTER).name())
    << " are incompatible.\n";
}
#endif /* canvas_Utilities_ensurePointer_h */

// Local Variables:
// mode: c++
// End:
