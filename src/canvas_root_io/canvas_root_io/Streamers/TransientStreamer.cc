// vim: set sw=2 expandtab :
#include "canvas_root_io/Streamers/TransientStreamer.h"
#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "canvas/Persistency/Provenance/FileIndex.h"
#include "canvas/Persistency/Provenance/ProcessHistory.h"
#include "canvas/Persistency/Provenance/ProductProvenance.h"
#include "canvas/Persistency/Provenance/Transient.h"

namespace art {

  void
  setProvenanceTransientStreamers()
  {
    detail::SetTransientStreamer<Transient<BranchDescription::Transients>>();
    detail::SetTransientStreamer<Transient<FileIndex::Transients>>();
    detail::SetTransientStreamer<Transient<ProcessHistory::Transients>>();
    detail::SetTransientStreamer<Transient<ProductProvenance::Transients>>();
  }

} // namespace art
