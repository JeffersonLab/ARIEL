#ifndef canvas_Utilities_DebugMacros_h
#define canvas_Utilities_DebugMacros_h

#include "canvas/Utilities/fwd.h"

#include <iostream>

namespace art {
  struct debugvalue {

    debugvalue();

    int
    operator()()
    {
      return value_;
    }

    const char* cvalue_;
    int value_;
  };

  extern debugvalue debugit;
}

#define FDEBUG(lev)                                                            \
  if (lev <= art::debugit())                                                   \
  std::cerr

#endif /* canvas_Utilities_DebugMacros_h */

// Local Variables:
// mode: c++
// End:
