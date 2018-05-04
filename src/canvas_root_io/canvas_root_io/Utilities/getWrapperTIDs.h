#ifndef canvas_root_io_Utilities_getWrapperTIDs_h
#define canvas_root_io_Utilities_getWrapperTIDs_h

#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "canvas/Utilities/TypeID.h"

#include <vector>

namespace art {
  namespace root { // Internal use only.
    std::vector<TypeID> getWrapperTIDs(BranchDescription const& bd);
    std::vector<TypeID> getWrapperTIDs(std::string const& productClassName);
  }
}

inline auto
art::root::getWrapperTIDs(BranchDescription const& bd) -> std::vector<TypeID>
{
  return getWrapperTIDs(bd.producedClassName());
}

#endif /* canvas_root_io_Utilities_getWrapperTIDs_h */

// Local Variables:
// mode: c++
// End:
