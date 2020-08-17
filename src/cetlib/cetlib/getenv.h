#ifndef cetlib_getenv_h
#define cetlib_getenv_h

// ======================================================================
//
// getenv: Obtain value of environment variable; throw if none such
//
// ======================================================================

#include <new>
#include <string>

namespace cet {
  std::string getenv(std::string const& name);
  std::string getenv(std::string const& name, std::nothrow_t);
}

// ======================================================================

#endif /* cetlib_getenv_h */

// Local variables:
// mode: c++
// End:
