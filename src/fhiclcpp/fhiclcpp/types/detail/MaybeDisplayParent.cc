#include "fhiclcpp/types/detail/MaybeDisplayParent.h"
#include "cetlib/split_by_regex.h"

#include <regex>

namespace {
  std::regex const re_dot{"\\."};
  std::regex const re_index{"\\[(\\d+)\\]"};
}

namespace fhicl::detail {

  std::vector<std::string>
  MaybeDisplayParent::get_parents(std::string const& k)
  {
    std::vector<std::string> parents;
    for (auto const& key : cet::split_by_regex(k, re_dot))
      for (auto const& token :
           cet::split_by_regex(std::regex_replace(key, re_index, ",$1"), ","))
        parents.push_back(token);

    // Skip first key, which is something like "<some_label>" in the context
    // whenever this function is called.
    return std::vector<std::string>(parents.begin() + 1, parents.end());
  }
}
