// ======================================================================
//
// lpad: Left-pad a string to the wanted size, unless already longer
//
// ======================================================================

#include "cetlib/lpad.h"

std::string
cet::lpad(std::string const& s,
          std::string::size_type wanted_size,
          char char_to_pad_with)
{
  if (wanted_size <= s.size())
    return s; // already long enough

  std::string result(wanted_size - s.size(), char_to_pad_with);
  return result.append(s);

} // lpad()

// ======================================================================
