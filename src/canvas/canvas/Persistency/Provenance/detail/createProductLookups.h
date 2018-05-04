#ifndef canvas_Persistency_Provenance_detail_createProductLookups_h
#define canvas_Persistency_Provenance_detail_createProductLookups_h

#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "canvas/Persistency/Provenance/type_aliases.h"

namespace art {
  namespace detail {
    ProductLookup_t createProductLookups(
      ProductDescriptions const& descriptions);
  }
}

#endif /* canvas_Persistency_Provenance_detail_createProductLookups_h */

// Local Variables:
// mode: c++
// End:
