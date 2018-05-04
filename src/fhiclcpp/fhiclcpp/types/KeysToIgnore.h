#ifndef fhiclcpp_types_KeysToIgnore_h
#define fhiclcpp_types_KeysToIgnore_h

#include "fhiclcpp/type_traits.h"

#include <set>
#include <string>

#define TEMPLATE_ARG "Template argument specified must be callable."

namespace fhicl {
  namespace detail {

    template <typename T>
    std::set<std::string>
    ensure_callable()
    {
      static_assert(tt::is_callable<T>::value, TEMPLATE_ARG);
      return T{}();
    }

    inline std::set<std::string>&
    concatenate_keys(std::set<std::string>& keys)
    {
      return keys;
    }

    template <typename H, typename... T>
    std::set<std::string>&
    concatenate_keys(std::set<std::string>& keys, H const& h, T const&... t)
    {
      keys.insert(begin(h), end(h));
      return concatenate_keys(keys, t...);
    }

  } // detail

  template <typename H, typename... T>
  struct KeysToIgnore {
    std::set<std::string>
    operator()()
    {
      std::set<std::string> keys_to_ignore{detail::ensure_callable<H>()};
      return detail::concatenate_keys(keys_to_ignore,
                                      detail::ensure_callable<T>()...);
    }
  };
}

#undef TEMPLATE_ARG
#endif /* fhiclcpp_types_KeysToIgnore_h */

// Local variables:
// mode: c++
// End:
