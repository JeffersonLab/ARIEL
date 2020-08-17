#include "fhiclcpp/types/Name.h"

#include <regex>

namespace {
  std::regex const re{"\\[|\\]"};
}

namespace fhicl {
  std::string
  get_regex_replace_str(std::string const& str)
  {
    return std::regex_replace(str, re, "\\$&");
  }
}
