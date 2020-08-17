#include "canvas/Persistency/Provenance/History.h"
// vim: set sw=2 expandtab :

namespace art {

  History::size_type
  History::size() const noexcept
  {
    return eventSelections_.size();
  }

  void
  History::addEventSelectionEntry(EventSelectionID const& eventSelection)
  {
    eventSelections_.push_back(eventSelection);
  }

  EventSelectionID const&
  History::getEventSelectionID(History::size_type const i) const noexcept
  {
    return eventSelections_[i];
  }

  EventSelectionIDVector const&
  History::eventSelectionIDs() const noexcept
  {
    return eventSelections_;
  }

  ProcessHistoryID const&
  History::processHistoryID() const noexcept
  {
    return processHistoryID_;
  }

  void
  History::setProcessHistoryID(ProcessHistoryID const& phid)
  {
    processHistoryID_ = phid;
  }

} // namespace art
