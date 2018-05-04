#ifndef cetlib_split_path_h
#define cetlib_split_path_h

// ======================================================================
//
// split_path: Split the string 'path' into components delimited by a
//             single colon. Adjacent colons result in an empty string.
//
// ======================================================================

#include <string>
#include <vector>

namespace cet {
  void split_path(std::string const& path,
                  std::vector<std::string>& components);
}

  // ======================================================================

#endif /* cetlib_split_path_h */

// Local Variables:
// mode: c++
// End:
