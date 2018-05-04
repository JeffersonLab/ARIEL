#include "cetlib/replace_all.h"

bool
cet::replace_all(std::string& in,
                 std::string const& from,
                 std::string const& to)
{
  bool result = false;
  auto const rep_length = from.size();
  auto const skip_length = to.size();
  std::string::size_type pos = 0;
  while ((pos = in.find(from, pos)) != std::string::npos) {
    result = true;
    in.replace(pos, rep_length, to);
    pos += skip_length;
  }
  return result;
}
