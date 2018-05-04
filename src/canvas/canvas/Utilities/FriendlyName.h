#ifndef canvas_Utilities_FriendlyName_h
#define canvas_Utilities_FriendlyName_h

////////////////////////////////////////////////////////////////////////
// friendlyName(): a free function to generate a friendly name for a
// type.
////////////////////////////////////////////////////////////////////////

#include <string>

namespace art {
  namespace friendlyname {
    std::string friendlyName(std::string const& iFullName);
  }
}
#endif /* canvas_Utilities_FriendlyName_h */

// Local Variables:
// mode: c++
// End:
