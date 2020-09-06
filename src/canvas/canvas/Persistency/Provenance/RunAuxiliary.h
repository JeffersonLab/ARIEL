#ifndef canvas_Persistency_Provenance_RunAuxiliary_h
#define canvas_Persistency_Provenance_RunAuxiliary_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/ProcessHistoryID.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/Timestamp.h"

#include <iosfwd>
#include <set>
#include <utility>

// Auxiliary run data that is persistent

namespace art {

  class RunAuxiliary {

  public: // TYPES
    static constexpr BranchType branch_type = InRun;

  public: // MEMBER FUNCTIONS -- Special Member Functions
    ~RunAuxiliary();

    RunAuxiliary();

    RunAuxiliary(RunID const& theId,
                 Timestamp const& theTime,
                 Timestamp const& theEndTime);

    RunAuxiliary(RunNumber_t const& run,
                 Timestamp const& theTime,
                 Timestamp const& theEndTime);

    RunAuxiliary(RunAuxiliary const&);

    RunAuxiliary(RunAuxiliary&&);

    RunAuxiliary& operator=(RunAuxiliary const&);

    RunAuxiliary& operator=(RunAuxiliary&&);

  public:
    void write(std::ostream&) const;

    ProcessHistoryID& processHistoryID() const noexcept;

    void setProcessHistoryID(ProcessHistoryID const&) const;

    unsigned rangeSetID() const noexcept;

    void setRangeSetID(unsigned const id) const;

    RunID const& id() const noexcept;

    RunID const& runID() const noexcept;

    void runID(RunID const&);

    RunNumber_t run() const noexcept;

    Timestamp const& beginTime() const noexcept;

    void beginTime(Timestamp const&);

    Timestamp const& endTime() const noexcept;

    void endTime(Timestamp const&);

    bool mergeAuxiliary(RunAuxiliary const&);

  private:
    void mergeNewTimestampsIntoThis_(RunAuxiliary const&);
    void mergeNewProcessHistoryIntoThis_(RunAuxiliary const&);

    // most recent process that put a RunProduct into this run
    // is the last on the list, this defines what "latest" is
    mutable ProcessHistoryID processHistoryID_{};

    // allEventsProcessHistories_ contains all the ProcessHistoryIDs for all
    // events in this run seen so far.
    // Note: The default ctor for set is not noexcept.
    std::set<ProcessHistoryID> allEventsProcessHistories_{};

    mutable unsigned rangeSetID_{-1u};

    RunID id_{};

    Timestamp beginTime_{};

    Timestamp endTime_{};
  };

  std::ostream& operator<<(std::ostream&, const RunAuxiliary&);

} // namespace art

#endif /* canvas_Persistency_Provenance_RunAuxiliary_h */

// Local Variables:
// mode: c++
// End:
