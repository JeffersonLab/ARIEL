#ifndef canvas_Persistency_Provenance_ParentageRegistry_h
#define canvas_Persistency_Provenance_ParentageRegistry_h

#include "canvas/Persistency/Provenance/Parentage.h"
#include "canvas/Persistency/Provenance/ParentageID.h"
#include "canvas/Persistency/Provenance/thread_safe_registry_via_id.h"

namespace art {
  typedef thread_safe_registry_via_id<ParentageID, Parentage> ParentageRegistry;
}

#endif /* canvas_Persistency_Provenance_ParentageRegistry_h */

// Local Variables:
// mode: c++
// End:
