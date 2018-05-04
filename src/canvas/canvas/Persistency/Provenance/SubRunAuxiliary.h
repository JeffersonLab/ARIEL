#ifndef canvas_Persistency_Provenance_SubRunAuxiliary_h
#define canvas_Persistency_Provenance_SubRunAuxiliary_h

#include <iosfwd>

#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/ProcessHistoryID.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/SubRunID.h"
#include "canvas/Persistency/Provenance/Timestamp.h"

// Auxiliary subRun data that is persistent

namespace art {
  class SubRunAuxiliary;
}

class art::SubRunAuxiliary {
public:
  static constexpr BranchType branch_type = InSubRun;

  SubRunAuxiliary() = default;

  SubRunAuxiliary(SubRunID const& theId,
                  Timestamp const& theTime,
                  Timestamp const& theEndTime)
    : id_{theId}, beginTime_{theTime}, endTime_{theEndTime}
  {}

  SubRunAuxiliary(RunNumber_t const theRun,
                  SubRunNumber_t const theSubRun,
                  Timestamp const& theTime,
                  Timestamp const& theEndTime)
    : id_{theRun, theSubRun}, beginTime_{theTime}, endTime_{theEndTime}
  {}

  void write(std::ostream& os) const;

  ProcessHistoryID const&
  processHistoryID() const
  {
    return processHistoryID_;
  }

  void
  setProcessHistoryID(ProcessHistoryID const& phid) const
  {
    processHistoryID_ = phid;
  }

  SubRunID const&
  id() const
  {
    return id_;
  }
  RunID const&
  runID() const
  {
    return id_.runID();
  }
  RunNumber_t
  run() const
  {
    return id_.run();
  }
  SubRunNumber_t
  subRun() const
  {
    return id_.subRun();
  }

  Timestamp const&
  beginTime() const
  {
    return beginTime_;
  }

  Timestamp const&
  endTime() const
  {
    return endTime_;
  }

  void
  setEndTime(Timestamp const& time)
  {
    if (endTime_ == Timestamp::invalidTimestamp())
      endTime_ = time;
  }

  void
  setRangeSetID(unsigned const id) const
  {
    rangeSetID_ = id;
  }
  auto
  rangeSetID() const
  {
    return rangeSetID_;
  }

  bool mergeAuxiliary(SubRunAuxiliary const& newAux);

  // most recent process that processed this subRun
  // is the last on the list, this defines what "latest" is
  mutable ProcessHistoryID processHistoryID_{};
  mutable unsigned rangeSetID_{-1u};

  SubRunID id_{};
  // Times from DAQ
  Timestamp beginTime_{};
  Timestamp endTime_{};
};

inline std::ostream&
operator<<(std::ostream& os, art::SubRunAuxiliary const& p)
{
  p.write(os);
  return os;
}

#endif /* canvas_Persistency_Provenance_SubRunAuxiliary_h */

// Local Variables:
// mode: c++
// End:
