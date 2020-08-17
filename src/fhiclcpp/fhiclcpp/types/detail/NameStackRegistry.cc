#include "fhiclcpp/types/detail/NameStackRegistry.h"
#include "fhiclcpp/exception.h"

#include <numeric>
#include <regex>

namespace {
  std::regex const re_sequence_element{R"(\[\d+\])"};
}

namespace fhicl {
  std::string
  NameStackRegistry::full_key(std::string const& name)
  {
    if (name.empty()) {
      throw exception{error::other, "NameStackRegistry::full_key"}
        << "Cannot insert empty name into 'NameStackRegistry'.\n";
    }

    if (names_.empty() || std::regex_match(name, re_sequence_element)) {
      names_.emplace_back(name);
    } else {
      names_.emplace_back("." + name);
    }
    return std::accumulate(names_.begin(), names_.end(), std::string{});
  }
}
