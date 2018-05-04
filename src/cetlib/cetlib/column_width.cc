#include "cetlib/column_width.h"

#include <algorithm>

std::string::size_type
cet::column_width(std::vector<std::string> const& v)
{
  if (v.empty())
    return 0ull;

  auto max_elem =
    std::max_element(v.begin(), v.end(), [](auto const& a, auto const& b) {
      return a.size() < b.size();
    });
  return max_elem->size();
}
