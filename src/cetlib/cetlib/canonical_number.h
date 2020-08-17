#ifndef cetlib_canonical_number_h
#define cetlib_canonical_number_h

// ======================================================================
//
// canonical_number: Transform a number string into a canonical form
//
// "Number string" is defined by the following regular expression:
//      ^[-+]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][-+]?[0-9]+)?$
//
// ======================================================================

#include <string>

namespace cet {

  bool canonical_number(std::string const& value, std::string& result);
}

// ======================================================================

#endif /* cetlib_canonical_number_h */

// Local Variables:
// mode: c++
// End:
