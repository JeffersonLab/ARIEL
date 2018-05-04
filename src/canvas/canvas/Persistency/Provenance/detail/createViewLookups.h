#ifndef canvas_Persistency_Provenance_detail_createViewLookups_h
#define canvas_Persistency_Provenance_detail_createViewLookups_h

#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "canvas/Persistency/Provenance/type_aliases.h"

namespace art {
  namespace detail {
    ViewLookup_t createViewLookups(ProductDescriptions const& descriptions);
  }
}

#endif /* canvas_Persistency_Provenance_detail_createViewLookups_h */

// Local Variables:
// mode: c++
// End:
