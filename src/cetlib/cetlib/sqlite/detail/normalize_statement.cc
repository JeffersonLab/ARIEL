#include "cetlib/sqlite/detail/normalize_statement.h"

#include <regex>

void
cet::sqlite::detail::normalize_statement(std::string& to_replace)
{
  // Remove spaces around commas
  {
    std::regex const r{"\\s*,\\s*"};
    to_replace = std::regex_replace(to_replace, r, ",");
  }
  // Remove spaces around parentheses
  {
    std::regex const r{R"(\s*([\(\)])\s*)"};
    to_replace = std::regex_replace(to_replace, r, "$1");
  }
  // Replace multiple spaces with 1 space.
  {
    std::regex const r{"\\s+"};
    to_replace = std::regex_replace(to_replace, r, " ");
  }
}

std::string cet::sqlite::detail::normalized_statement(
  std::string to_replace) // Argument is a copy!
{
  normalize_statement(to_replace);
  return to_replace;
}
