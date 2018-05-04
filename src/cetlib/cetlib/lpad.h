#ifndef cetlib_lpad_h
#define cetlib_lpad_h

// ======================================================================
//
// lpad: Left-pad a string to the wanted size, unless already longer
//
// ======================================================================

#include <string>

namespace cet {
  std::string lpad(std::string const& pad_me,
                   std::string::size_type wanted_size,
                   char char_to_pad_with = ' ');
}

#endif /* cetlib_lpad_h */

// Local Variables:
// mode: c++
// End:
