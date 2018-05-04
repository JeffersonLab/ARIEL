#ifndef cetlib_rpad_h
#define cetlib_rpad_h

// ======================================================================
//
// rpad: Right-pad a string to the wanted size, unless already longer
//
// ======================================================================

#include <string>

namespace cet {
  std::string rpad(std::string const& pad_me,
                   std::string::size_type wanted_size,
                   char char_to_pad_with = ' ');
}

#endif /* cetlib_rpad_h */

// Local Variables:
// mode: c++
// End:
