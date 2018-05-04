#include <algorithm>
#include <regex>

namespace cet {

  std::vector<std::string>
  split_by_regex(std::string const& str, std::string const& delimSet)
  {
    std::vector<std::string> tokens;
    std::regex tmpRegex(delimSet);
    std::copy(std::sregex_token_iterator(str.begin(), str.end(), tmpRegex, -1),
              std::sregex_token_iterator(),
              std::back_inserter(tokens));
    return tokens;
  }
}
