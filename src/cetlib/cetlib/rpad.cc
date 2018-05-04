// ======================================================================
//
// rpad: Right-pad a string to the wanted size, unless already longer
//
// ======================================================================

#include "cetlib/rpad.h"

std::string
cet::rpad(std::string const& s,
          std::string::size_type wanted_size,
          char char_to_pad_with)
{
  if (wanted_size <= s.size())
    return s; // already long enough

  std::string result(s);
  return result.append(wanted_size - s.size(), char_to_pad_with);

} // rpad()

// ======================================================================
