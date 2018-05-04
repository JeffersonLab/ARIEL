#ifndef cetlib_column_width_h
#define cetlib_column_width_h

// ======================================================================
//
// column_width: Obtain the length of the longest in a vector of strings
//
// ======================================================================

#include <string>
#include <vector>

namespace cet {
  std::string::size_type column_width(std::vector<std::string> const&);
}

#endif /* cetlib_column_width_h */

// Local variables:
// mode: c++
// End:
