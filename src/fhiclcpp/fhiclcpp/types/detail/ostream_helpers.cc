
#include "cetlib_except/demangle.h"

#include <ostream>
#include <regex>
#include <string>

namespace fhicl {
  namespace detail {
    namespace no_defaults {

      std::string
      stripped_typename(std::string const& fullName)
      {
        // Remove 'std::' from fullName
        std::string const strippedName =
          std::regex_replace(fullName, std::regex("std::"), "");
        return strippedName;
      }
    }
  }
} // fhicl::detail::no_defaults

// Local variables:
// mode : c++
// End:
