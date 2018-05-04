#ifndef cetlib_getenv_h
#define cetlib_getenv_h

// ======================================================================
//
// getenv: Obtain value of environment variable; throw if none such
//
// ======================================================================

#include <string>

namespace cet {
  std::string getenv(std::string const& name);
}

  // ======================================================================

#endif /* cetlib_getenv_h */

// Local variables:
// mode: c++
// End:
