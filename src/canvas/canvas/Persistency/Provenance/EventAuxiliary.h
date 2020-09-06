#ifndef canvas_Persistency_Provenance_EventAuxiliary_h
#define canvas_Persistency_Provenance_EventAuxiliary_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/EventID.h"
#include "canvas/Persistency/Provenance/ProcessHistoryID.h"
#include "canvas/Persistency/Provenance/Timestamp.h"

#include <atomic>
#include <iosfwd>

namespace art {
  class EventAuxiliary {
  public: // TYPES
    static constexpr BranchType branch_type = InEvent;
    enum ExperimentType {
      Any = 0,
      Align = 1,
      Calib = 2,
      Cosmic = 3,
      Data = 4,
      Mc = 5,
      Raw = 6,
      Test = 7
    };

  public: // MEMBER FUNCTIONS -- Special Member Functions
    ~EventAuxiliary();
    EventAuxiliary();
    EventAuxiliary(EventID const& theId,
                   Timestamp const& theTime,
                   bool isReal,
                   ExperimentType eType = Any);
    EventAuxiliary(EventAuxiliary const&);
    EventAuxiliary(EventAuxiliary&&);
    EventAuxiliary& operator=(EventAuxiliary const&);
    EventAuxiliary& operator=(EventAuxiliary&&);

  public: // MEMBER FUNCTIONS -- API for the user
    Timestamp const& time() const noexcept;
    EventID const& id() const noexcept;
    EventID const& eventID() const noexcept;
    RunID const& runID() const noexcept;
    SubRunID const& subRunID() const noexcept;
    RunNumber_t run() const noexcept;
    SubRunNumber_t subRun() const noexcept;
    EventNumber_t event() const noexcept;
    bool isRealData() const noexcept;
    EventAuxiliary::ExperimentType experimentType() const noexcept;
    bool operator==(EventAuxiliary const& other) const noexcept;
    void write(std::ostream& os) const;

  private: // MEMBER DATA
    EventID id_;
    Timestamp time_;
    bool isRealData_;
    ExperimentType experimentType_;
  };
  std::ostream& operator<<(std::ostream&, const EventAuxiliary&);
} // namespace art

#endif /* canvas_Persistency_Provenance_EventAuxiliary_h */

// Local Variables:
// mode: c++
// End:
