#ifndef cetlib_except_demangle_h
#define cetlib_except_demangle_h

// ======================================================================
//
// demangle: Call the cross-vendor API to demangle symbols
//
// ======================================================================

#include <string>

namespace cet {
  std::string demangle_symbol(std::string const& symbol);
  std::string demangle_symbol(char const* symbol);

  std::string demangle_message(std::string mangled);
}

inline std::string
cet::demangle_symbol(std::string const& symbol)
{
  return demangle_symbol(symbol.c_str());
}

#endif /* cetlib_except_demangle_h */

// Local Variables:
// mode: c++
// End:
