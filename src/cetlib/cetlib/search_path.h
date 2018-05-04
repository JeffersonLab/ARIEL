#ifndef cetlib_search_path_h
#define cetlib_search_path_h

// ======================================================================
//
// search_path: Seek filename or pattern in a given list of pathnames
//
// ======================================================================

#include "cetlib/container_algorithms.h"
#include "cetlib/split.h"
#include <algorithm>
#include <cstdlib>
#include <ostream>
#include <string>
#include <vector>

namespace cet {
  // A search_path is used to manage the use of a PATH-like
  // environment variable. It is created from the name of an
  // environment variable; the value associated with this variable is
  // expected to be a colon-separated list of directories. The
  // search_path provides facilities for finding files in these
  // directories.
  class search_path;

  std::ostream& operator<<(std::ostream& os, search_path const& p);
}

// ----------------------------------------------------------------------

class cet::search_path {
public:
  explicit search_path(std::string const& name_or_path);

  // If an environment variable was used to create the search_path
  // object, return it.  Otherwise, it will be empty.
  std::string const&
  showenv() const
  {
    return env_;
  }

  // Return true if there are no directories in the path.
  bool empty() const;

  // Return the number of directories in the path.
  std::size_t size() const;

  // Return the k'th entry in the path.
  std::string const& operator[](std::size_t k) const;

  // Return the full pathname of the file named filename, found
  // somewhere along the path. If no such file is found, an exception
  // is thrown.
  std::string find_file(std::string const& filename) const;

  // Look for a file named 'filename' in the path. If filename is
  // empty, or if no file is found, return false, without modifying
  // result. If one is found, return true and fill result with the
  // full pathname for the file.
  bool find_file(std::string const& filename, std::string& result) const;

  // Find all the files with names maching 'filename_pattern' in the
  // search path. filename_pattern is used to construct a std::regex
  // that is used for the matching. The path to each matching file is
  // appended to out, and the total number of matching paths is the
  // return value of the function.
  std::size_t find_files(std::string const& filename_pattern,
                         std::vector<std::string>& result) const;

  // Find all the files with names maching 'filename_pattern' in the
  // search path. filename_pattern is used to construct a std::regex
  // that is used for the matching. The path to each matching file is
  // written to 'dest', and the total number of matching paths is the
  // return value of the function.
  template <class OutIter>
  std::size_t find_files(std::string const& filename_pattern,
                         OutIter dest) const;

  // Return the string format (colon-delimited) of the search path.
  std::string to_string() const;

private:
  std::string const env_;
  std::vector<std::string> dirs_{};
}; // search_path

template <class OutIter>
std::size_t
cet::search_path::find_files(std::string const& pattern, OutIter dest) const
{
  std::vector<std::string> results;
  size_t const nfound{find_files(pattern, results)};
  cet::copy_all(results, dest);
  return nfound;
}

#endif /* cetlib_search_path_h */

// Local Variables:
// mode: c++
// End:
