#include "fhiclcpp/types/detail/ostream_helpers.h"

#include <regex>
#include <string>

namespace {
  std::regex const re_std_prefix{"std::"};
}

namespace fhicl::detail::no_defaults {
  std::string
  stripped_typename(std::string const& fullName)
  {
    // Remove 'std::' from fullName
    return std::regex_replace(fullName, re_std_prefix, "");
  }
}
