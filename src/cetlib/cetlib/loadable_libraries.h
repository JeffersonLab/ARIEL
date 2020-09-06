#ifndef cetlib_loadable_libraries_h
#define cetlib_loadable_libraries_h

#include <iosfwd>
#include <string>

namespace cet {
  void loadable_libraries(std::ostream& os,
                          std::string const& spec,
                          std::string const& suffix);
}

#endif /* cetlib_loadable_libraries_h */

// Local Variables:
// mode: c++
// End:
