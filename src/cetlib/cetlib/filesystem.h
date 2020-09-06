#ifndef cetlib_filesystem_h
#define cetlib_filesystem_h

// ======================================================================
//
// filesystem: A family of file-level utilities
//
// ======================================================================

#include <string>

namespace cet {
  bool file_exists(std::string const& qualified_filename);
  bool is_absolute_filepath(std::string const& qualified_filename);
  bool is_relative_filepath(std::string const& qualified_filename);
}

// ======================================================================

#endif /* cetlib_filesystem_h */

// Local Variables:
// mode: c++
// End:
