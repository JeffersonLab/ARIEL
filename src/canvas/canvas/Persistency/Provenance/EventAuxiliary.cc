#include "canvas/Persistency/Provenance/EventAuxiliary.h"
// vim: set sw=2 expandtab :

#include <ostream>
#include <utility>

using namespace std;

namespace art {

  EventAuxiliary::~EventAuxiliary() {}

  EventAuxiliary::EventAuxiliary()
    : id_{}, time_{}, isRealData_{false}, experimentType_{Any}
  {}

  EventAuxiliary::EventAuxiliary(EventID const& theId,
                                 Timestamp const& theTime,
                                 bool isReal,
                                 ExperimentType eType /*= Any*/)
    : id_{theId}, time_{theTime}, isRealData_{isReal}, experimentType_{eType}
  {}

  EventAuxiliary::EventAuxiliary(EventAuxiliary const& rhs)
    : id_{rhs.id_}
    , time_{rhs.time_}
    , isRealData_{rhs.isRealData_}
    , experimentType_{rhs.experimentType_}
  {}

  EventAuxiliary::EventAuxiliary(EventAuxiliary&& rhs)
    : id_{move(rhs.id_)}
    , time_{move(rhs.time_)}
    , isRealData_{move(rhs.isRealData_)}
    , experimentType_{move(rhs.experimentType_)}
  {}

  EventAuxiliary&
  EventAuxiliary::operator=(EventAuxiliary const& rhs)
  {
    if (this != &rhs) {
      id_ = rhs.id_;
      time_ = rhs.time_;
      isRealData_ = rhs.isRealData_;
      experimentType_ = rhs.experimentType_;
    }
    return *this;
  }

  EventAuxiliary&
  EventAuxiliary::operator=(EventAuxiliary&& rhs)
  {
    id_ = move(rhs.id_);
    time_ = move(rhs.time_);
    isRealData_ = move(rhs.isRealData_);
    experimentType_ = move(rhs.experimentType_);
    return *this;
  }

  Timestamp const&
  EventAuxiliary::time() const noexcept
  {
    auto const& ret = time_;
    return ret;
  }

  EventID const&
  EventAuxiliary::id() const noexcept
  {
    auto const& ret = id_;
    return ret;
  }

  EventID const&
  EventAuxiliary::eventID() const noexcept
  {
    auto const& ret = id_;
    return ret;
  }

  RunID const&
  EventAuxiliary::runID() const noexcept
  {
    auto const& ret = id_.runID();
    return ret;
  }

  SubRunID const&
  EventAuxiliary::subRunID() const noexcept
  {
    auto const& ret = id_.subRunID();
    return ret;
  }

  RunNumber_t
  EventAuxiliary::run() const noexcept
  {
    auto ret = id_.run();
    return ret;
  }

  SubRunNumber_t
  EventAuxiliary::subRun() const noexcept
  {
    auto ret = id_.subRun();
    return ret;
  }

  EventNumber_t
  EventAuxiliary::event() const noexcept
  {
    auto ret = id_.event();
    return ret;
  }

  bool
  EventAuxiliary::isRealData() const noexcept
  {
    auto ret = isRealData_;
    return ret;
  }

  EventAuxiliary::ExperimentType
  EventAuxiliary::experimentType() const noexcept
  {
    auto ret = experimentType_;
    return ret;
  }

  bool
  EventAuxiliary::operator==(EventAuxiliary const& other) const noexcept
  {
    auto ret = (id_ == other.id_) && (time_ == other.time_) &&
               (isRealData_ == other.isRealData_) &&
               (experimentType_ == other.experimentType_);
    return ret;
  }

  void
  EventAuxiliary::write(ostream& os) const
  {
    os << id_ << endl;
  }

  ostream&
  operator<<(ostream& os, const EventAuxiliary& p)
  {
    p.write(os);
    return os;
  }

} // namespace art
