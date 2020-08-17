#include "canvas/Persistency/Provenance/RunAuxiliary.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/ProcessHistoryID.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/Timestamp.h"

#include <ostream>
#include <set>
#include <utility>

using namespace std;

namespace art {

  // Note: Cannot be noexcept because of processHistoryID_,
  // allEventsProcessHistories_, and id_.
  RunAuxiliary::~RunAuxiliary() {}

  // Note: Cannot be noexcept because of processHistoryID_,
  // allEventsProcessHistories_, and id_.
  RunAuxiliary::RunAuxiliary()
    : processHistoryID_{}
    , allEventsProcessHistories_{}
    , rangeSetID_{-1u}
    , id_{}
    , beginTime_{}
    , endTime_{}
  {}

  // Note: Cannot be noexcept because of processHistoryID_,
  // allEventsProcessHistories_, and id_.
  RunAuxiliary::RunAuxiliary(RunID const& theId,
                             Timestamp const& theTime,
                             Timestamp const& theEndTime)
    : processHistoryID_()
    , allEventsProcessHistories_()
    , rangeSetID_(-1u)
    , id_(theId)
    , beginTime_(theTime)
    , endTime_(theEndTime)
  {}

  // Note: Cannot be noexcept because of processHistoryID_,
  // allEventsProcessHistories_, and id_.
  RunAuxiliary::RunAuxiliary(RunNumber_t const& run,
                             Timestamp const& theTime,
                             Timestamp const& theEndTime)
    : processHistoryID_()
    , allEventsProcessHistories_()
    , rangeSetID_(-1u)
    , id_(run)
    , beginTime_(theTime)
    , endTime_(theEndTime)
  {}

  // Note: Cannot be noexcept because of processHistoryID_,
  // allEventsProcessHistories_, and id_.
  RunAuxiliary::RunAuxiliary(RunAuxiliary const& rhs)
    : processHistoryID_{rhs.processHistoryID_}
    , allEventsProcessHistories_{rhs.allEventsProcessHistories_}
    , rangeSetID_{rhs.rangeSetID_}
    , id_{rhs.id_}
    , beginTime_{rhs.beginTime_}
    , endTime_{rhs.endTime_}
  {}

  // Note: Cannot be noexcept because of processHistoryID_,
  // allEventsProcessHistories_, and id_.
  RunAuxiliary::RunAuxiliary(RunAuxiliary&& rhs)
    : processHistoryID_{move(rhs.processHistoryID_)}
    , allEventsProcessHistories_{move(rhs.allEventsProcessHistories_)}
    , rangeSetID_{move(rhs.rangeSetID_)}
    , id_{move(rhs.id_)}
    , beginTime_{move(rhs.beginTime_)}
    , endTime_{move(rhs.endTime_)}
  {}

  // Note: Cannot be noexcept because of processHistoryID_,
  // allEventsProcessHistories_, and id_.
  RunAuxiliary&
  RunAuxiliary::operator=(RunAuxiliary const& rhs)
  {
    if (this != &rhs) {
      processHistoryID_ = rhs.processHistoryID_;
      allEventsProcessHistories_ = rhs.allEventsProcessHistories_;
      rangeSetID_ = rhs.rangeSetID_;
      id_ = rhs.id_;
      beginTime_ = rhs.beginTime_;
      endTime_ = rhs.endTime_;
    }
    return *this;
  }

  // Note: Cannot be noexcept because of processHistoryID_,
  // allEventsProcessHistories_, and id_.
  RunAuxiliary&
  RunAuxiliary::operator=(RunAuxiliary&& rhs)
  {
    processHistoryID_ = move(rhs.processHistoryID_);
    allEventsProcessHistories_ = move(rhs.allEventsProcessHistories_);
    rangeSetID_ = move(rhs.rangeSetID_);
    id_ = move(rhs.id_);
    beginTime_ = move(rhs.beginTime_);
    endTime_ = move(rhs.endTime_);
    return *this;
  }

  ProcessHistoryID&
  RunAuxiliary::processHistoryID() const noexcept
  {
    return processHistoryID_;
  }

  unsigned
  RunAuxiliary::rangeSetID() const noexcept
  {
    return rangeSetID_;
  }

  RunID const&
  RunAuxiliary::id() const noexcept
  {
    return id_;
  }

  Timestamp const&
  RunAuxiliary::beginTime() const noexcept
  {
    return beginTime_;
  }

  Timestamp const&
  RunAuxiliary::endTime() const noexcept
  {
    return endTime_;
  }

  void
  RunAuxiliary::setProcessHistoryID(ProcessHistoryID const& phid) const
  {
    processHistoryID_ = phid;
  }

  RunID const&
  RunAuxiliary::runID() const noexcept
  {
    return id_;
  }

  void
  RunAuxiliary::runID(RunID const& id)
  {
    id_ = id;
  }

  RunNumber_t
  RunAuxiliary::run() const noexcept
  {
    return id_.run();
  }

  void
  RunAuxiliary::beginTime(Timestamp const& time)
  {
    beginTime_ = time;
  }

  void
  RunAuxiliary::endTime(Timestamp const& time)
  {
    endTime_ = time;
  }

  void
  RunAuxiliary::setRangeSetID(unsigned const id) const
  {
    rangeSetID_ = id;
  }

  void
  RunAuxiliary::write(ostream& os) const
  {
    os << "Process History ID = " << processHistoryID_ << endl;
    os << id_ << endl;
  }

  bool
  RunAuxiliary::mergeAuxiliary(RunAuxiliary const& newAux)
  {
    mergeNewTimestampsIntoThis_(newAux);
    mergeNewProcessHistoryIntoThis_(newAux);
    // Keep the process history ID that is in the preexisting principal
    // It may have been updated to include the current process.
    // There is one strange other case where the two ProcessHistoryIDs
    // could be different which should not be important and we just ignore.
    // There could have been previous processes which only dropped products.
    // These processes could have dropped the same branches but had different
    // process names ... Ignore this.
    return id_ == newAux.id();
  }

  void
  RunAuxiliary::mergeNewTimestampsIntoThis_(RunAuxiliary const& newAux)
  {
    if ((beginTime_ == Timestamp::invalidTimestamp()) ||
        (newAux.beginTime() == Timestamp::invalidTimestamp())) {
      beginTime_ = Timestamp::invalidTimestamp();
    } else if (newAux.beginTime() < beginTime_) {
      beginTime_ = newAux.beginTime();
    }
    if ((endTime_ == Timestamp::invalidTimestamp()) ||
        (newAux.endTime() == Timestamp::invalidTimestamp())) {
      endTime_ = Timestamp::invalidTimestamp();
    } else if (newAux.endTime() > endTime_) {
      endTime_ = newAux.endTime();
    }
  }

  void
  RunAuxiliary::mergeNewProcessHistoryIntoThis_(RunAuxiliary const& newAux)
  {
    allEventsProcessHistories_.insert(newAux.allEventsProcessHistories_.begin(),
                                      newAux.allEventsProcessHistories_.end());
  }

  ostream&
  operator<<(ostream& os, const RunAuxiliary& p)
  {
    p.write(os);
    return os;
  }

} // namspace art
