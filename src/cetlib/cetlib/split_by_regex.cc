#include <algorithm>
#include <regex>

namespace cet {
  std::vector<std::string>
  split_by_regex(std::string const& str, std::regex const& reDelimSet)
  {
    std::vector<std::string> tokens;
    std::copy(
      std::sregex_token_iterator(str.begin(), str.end(), reDelimSet, -1),
      std::sregex_token_iterator{},
      std::back_inserter(tokens));
    return tokens;
  }

  std::vector<std::string>
  split_by_regex(std::string const& str, std::string const& delimSet)
  {
    std::regex const tmpRegex{delimSet};
    return split_by_regex(str, tmpRegex);
  }
}
