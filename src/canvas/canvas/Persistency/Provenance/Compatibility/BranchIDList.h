#ifndef canvas_Persistency_Provenance_Compatibility_BranchIDList_h
#define canvas_Persistency_Provenance_Compatibility_BranchIDList_h

// =======================================================================
// For art::ProductID versions 1-9, the schema included a (process
// index, product index) doublet, which served as indices into the
// BranchIDLists object.  This was a means of providing persistent
// art::Ptrs.  The schema of art::ProductID has changed such that the
// BranchIDList(s) types are no longer needed.  These types must be
// retained for converting old on-disk ProductID schemas to new ones.
// =======================================================================

#include "canvas/Persistency/Provenance/BranchID.h"
#include <vector>

namespace art {
  typedef std::vector<BranchID::value_type> BranchIDList;
  typedef std::vector<BranchIDList> BranchIDLists;
}
#endif /* canvas_Persistency_Provenance_Compatibility_BranchIDList_h */

// Local Variables:
// mode: c++
// End:
