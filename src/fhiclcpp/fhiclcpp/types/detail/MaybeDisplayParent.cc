#include "fhiclcpp/types/detail/MaybeDisplayParent.h"
#include "cetlib/exempt_ptr.h"
#include "cetlib/split_by_regex.h"
#include "fhiclcpp/types/detail/ParameterBase.h"

#include <cctype>
#include <regex>
#include <sstream>
#include <vector>

//===================================================================================
namespace fhicl {
  namespace detail {

    std::vector<std::string>
    MaybeDisplayParent::get_parents(std::string const& k)
    {
      std::regex const r_index{"\\[(\\d+)\\]"};
      std::vector<std::string> parents;
      for (auto const& key : cet::split_by_regex(k, "\\."))
        for (auto const& token :
             cet::split_by_regex(std::regex_replace(key, r_index, ",$1"), ","))
          parents.push_back(token);

      // Skip first key, which is something like "<some_label>" in the context
      // whenever this function is called.
      return std::vector<std::string>(parents.begin() + 1, parents.end());
    }
  }
}

// Local variables:
// mode: c++
// End:
