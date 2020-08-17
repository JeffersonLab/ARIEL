#include "fhiclcpp/detail/ParameterSetImplHelpers.h"
#include "boost/algorithm/string.hpp"
#include "cetlib/container_algorithms.h"
#include "cetlib/split_by_regex.h"
#include "fhiclcpp/exception.h"

#include <regex>

namespace {
  std::regex const reBrackets{R"((\]\[|\[|\]))"};
}

namespace fhicl {
  namespace detail {

    Keys
    get_names(std::string const& key)
    {
      std::vector<std::string> keys;
      boost::algorithm::split(keys, key, boost::algorithm::is_any_of("."));

      // Remove empty keys
      keys.erase(std::remove(keys.begin(), keys.end(), ""), keys.end());

      if (keys.empty())
        throw fhicl::exception(fhicl::cant_find, "vacuous key");

      std::string const last = keys.back();
      keys.pop_back();

      return Keys{keys, last};
    }

    SequenceKey
    get_sequence_indices(std::string const& key)
    {
      // Split "name[0][5][1]" according to delimiters "][", "[", and "]"
      // to give {"name","0","5","1"};
      auto tokens = cet::split_by_regex(key, reBrackets);

      auto const name = tokens.front();
      tokens.erase(tokens.begin());

      std::vector<std::size_t> indices;
      cet::transform_all(
        tokens, std::back_inserter(indices), [](std::string const& index) {
          return std::stoul(index);
        });
      return SequenceKey{name, indices};
    }
  }
}
