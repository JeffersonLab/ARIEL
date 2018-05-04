#ifndef cetlib_h
#define cetlib_h

// ======================================================================
//
// include: File->string, interpolating #include'd files along the way
//
// ======================================================================

#include <iosfwd>
#include <string>

namespace cet {
  void include(std::istream& in, std::string& result);
  void include(std::istream& in,
               std::string const& search_path_arg,
               std::string& result);
}

  // ======================================================================

#endif /* cetlib_h */

// Local Variables:
// mode: c++
// End:
