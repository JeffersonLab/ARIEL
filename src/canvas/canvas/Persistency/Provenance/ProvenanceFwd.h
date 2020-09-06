#ifndef canvas_Persistency_Provenance_ProvenanceFwd_h
#define canvas_Persistency_Provenance_ProvenanceFwd_h

// ======================================================================
//
// ProvenanceFwd
//
// ======================================================================

namespace art {

  class BranchDescription;
  class BranchID;
  struct BranchKey;
  class EventAuxiliary;
  class EventID;
  class ModuleDescription;
  class Parentage;
  class ProcessConfiguration;
  class ProcessHistory;
  class ProductID;
  class ProductProvenance;
  class RunAuxiliary;
  class RunID;
  class SubRunAuxiliary;
  class SubRunID;
  class Timestamp;

} // art

// ----------------------------------------------------------------------

#include "canvas/Persistency/Provenance/Compatibility/BranchIDList.h"
#include "canvas/Persistency/Provenance/ParentageID.h"
#include "canvas/Persistency/Provenance/ProcessConfigurationID.h"
#include "canvas/Persistency/Provenance/ProcessHistoryID.h"
#include "canvas/Persistency/Provenance/ProductStatus.h"
#include "canvas/Persistency/Provenance/ReleaseVersion.h"

// ======================================================================

#endif /* canvas_Persistency_Provenance_ProvenanceFwd_h */

// Local Variables:
// mode: c++
// End:
