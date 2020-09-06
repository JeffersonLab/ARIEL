#ifndef canvas_Persistency_Provenance_EventID_h
#define canvas_Persistency_Provenance_EventID_h

// An EventID labels an unique readout of the data acquisition system,
// which we call an "event".

#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/SubRunID.h"
#include "canvas/Utilities/Exception.h"

#include <cstdint>
#include <iosfwd>

namespace art {
  class EventID;

  std::ostream& operator<<(std::ostream& os, EventID const& iID);
}

class art::EventID {
public:
  constexpr EventID() noexcept;
  EventID(RunNumber_t r, SubRunNumber_t sr, EventNumber_t e);

  // This needs to be done in enough places in the framework that this
  // constructor should be public.
  EventID(SubRunID const& sID, EventNumber_t e);

  RunID const& runID() const;
  RunNumber_t run() const;
  SubRunID const& subRunID() const;
  SubRunNumber_t subRun() const;
  EventNumber_t event() const;

  bool isValid() const;
  bool isFlush() const;

  EventID next() const;
  EventID nextSubRun(
    EventNumber_t first = IDNumber<Level::Event>::first()) const;
  EventID nextRun() const;
  EventID previous() const;
  EventID previousSubRun() const;
  EventID previousRun() const;

  static EventID maxEvent();
  static EventID firstEvent();
  static EventID firstEvent(SubRunID const& srID);
  static constexpr EventID invalidEvent() noexcept;
  static EventID invalidEvent(RunID rID);
  static EventID invalidEvent(SubRunID const& srID);
  static constexpr EventID flushEvent() noexcept;
  static EventID flushEvent(RunID rID);
  static EventID flushEvent(SubRunID srID);

  // Comparison operators.
  bool operator==(EventID const& other) const;
  bool operator!=(EventID const& other) const;
  bool operator<(EventID const& other) const;
  bool operator>(EventID const& other) const;
  bool operator<=(EventID const& other) const;
  bool operator>=(EventID const& other) const;

  friend std::ostream& operator<<(std::ostream& os, EventID const& iID);

private:
  struct FlushFlag {};

  explicit constexpr EventID(FlushFlag) noexcept;
  EventID(RunID rID, FlushFlag);
  EventID(SubRunID srID, FlushFlag);

  EventNumber_t inRangeOrInvalid(EventNumber_t e);

  SubRunID subRun_;
  EventNumber_t event_;
};

inline constexpr art::EventID::EventID() noexcept
  : subRun_(), event_(IDNumber<Level::Event>::invalid())
{}

inline art::EventID::EventID(RunNumber_t r, SubRunNumber_t sr, EventNumber_t e)
  : subRun_(r, sr), event_(inRangeOrInvalid(e))
{}

inline art::EventID::EventID(SubRunID const& sID, EventNumber_t e)
  : subRun_(sID), event_(inRangeOrInvalid(e))
{}

inline art::RunID const&
art::EventID::runID() const
{
  return subRun_.runID();
}

inline art::RunNumber_t
art::EventID::run() const
{
  return subRun_.run();
}

inline art::SubRunID const&
art::EventID::subRunID() const
{
  return subRun_;
}

inline art::SubRunNumber_t
art::EventID::subRun() const
{
  return subRun_.subRun();
}

inline art::EventNumber_t
art::EventID::event() const
{
  return event_;
}

inline bool
art::EventID::isValid() const
{
  return (event_ != IDNumber<Level::Event>::invalid()) && subRun_.isValid();
}

inline bool
art::EventID::isFlush() const
{
  return event_ == IDNumber<Level::Event>::flush_value();
}

inline art::EventID
art::EventID::next() const
{
  if (!isValid()) {
    throw art::Exception{art::errors::InvalidNumber}
      << "Cannot increment invalid event number.\n";
  } else if (event_ == IDNumber<Level::Event>::max_natural()) {
    return nextSubRun();
  } else {
    return EventID{subRun_, event_ + 1u};
  }
}

inline art::EventID
art::EventID::nextSubRun(EventNumber_t first) const
{
  return EventID{subRun_.next(), first};
}

inline art::EventID
art::EventID::nextRun() const
{
  return EventID{subRun_.nextRun(), IDNumber<Level::Event>::first()};
}

inline art::EventID
art::EventID::previous() const
{
  if (!isValid()) {
    throw art::Exception(art::errors::InvalidNumber)
      << "cannot decrement invalid event number.";
  } else if (event_ == IDNumber<Level::Event>::first()) {
    return previousSubRun();
  } else {
    return EventID{subRun_, event_ - 1u};
  }
}

inline art::EventID
art::EventID::previousSubRun() const
{
  return EventID{subRun_.previous(), IDNumber<Level::Event>::max_natural()};
}

inline art::EventID
art::EventID::previousRun() const
{
  return EventID{subRun_.previousRun(), IDNumber<Level::Event>::max_natural()};
}

inline art::EventID
art::EventID::maxEvent()
{
  return EventID{SubRunID::maxSubRun(), IDNumber<Level::Event>::max_natural()};
}

inline art::EventID
art::EventID::firstEvent()
{
  return EventID{SubRunID::firstSubRun(), IDNumber<Level::Event>::first()};
}

inline art::EventID
art::EventID::firstEvent(SubRunID const& srID)
{
  return EventID{srID, IDNumber<Level::Event>::first()};
}

inline constexpr art::EventID
art::EventID::invalidEvent() noexcept
{
  return EventID{};
}

inline art::EventID
art::EventID::invalidEvent(RunID rID)
{
  return EventID{SubRunID::invalidSubRun(rID),
                 IDNumber<Level::Event>::invalid()};
}

inline art::EventID
art::EventID::invalidEvent(SubRunID const& srID)
{
  return EventID{srID, IDNumber<Level::Event>::invalid()};
}

inline constexpr art::EventID
art::EventID::flushEvent() noexcept
{
  return EventID{FlushFlag()};
}

inline art::EventID
art::EventID::flushEvent(RunID rID)
{
  return EventID{rID, FlushFlag()};
}

inline art::EventID
art::EventID::flushEvent(SubRunID srID)
{
  return EventID{srID, FlushFlag()};
}

// Comparison operators.
inline bool
art::EventID::operator==(EventID const& other) const
{
  return other.subRun_ == subRun_ && other.event_ == event_;
}

inline bool
art::EventID::operator!=(EventID const& other) const
{
  return !(*this == other);
}

#include "canvas/Persistency/Provenance/SortInvalidFirst.h"

inline bool
art::EventID::operator<(EventID const& other) const
{
  static SortInvalidFirst<EventNumber_t> op(IDNumber<Level::Event>::invalid());
  if (subRun_ == other.subRun_) {
    return op(event_, other.event_);
  } else {
    return subRun_ < other.subRun_;
  }
}

inline bool
art::EventID::operator>(EventID const& other) const
{
  return (other < *this);
}

inline bool
art::EventID::operator<=(EventID const& other) const
{
  return (*this < other) || (*this == other);
}

inline bool
art::EventID::operator>=(EventID const& other) const
{
  return !(*this < other);
}

inline constexpr art::EventID::EventID(FlushFlag) noexcept
  : subRun_(SubRunID::flushSubRun())
  , event_(IDNumber<Level::Event>::flush_value())
{}

inline art::EventID::EventID(RunID rID, FlushFlag)
  : subRun_(SubRunID::flushSubRun(rID))
  , event_(IDNumber<Level::Event>::flush_value())
{}

inline art::EventID::EventID(SubRunID srID, FlushFlag)
  : subRun_(std::move(srID)), event_(IDNumber<Level::Event>::flush_value())
{}

inline art::EventNumber_t
art::EventID::inRangeOrInvalid(EventNumber_t e)
{
  if (e == IDNumber<Level::Event>::invalid() ||
      e <= IDNumber<Level::Event>::max_natural()) {
    return e;
  } else {
    throw Exception(errors::InvalidNumber)
      << "Attempt to construct an EventID with an invalid number.\n"
      << "Maybe you want EventID::flushEvent()?\n";
  }
}

#endif /* canvas_Persistency_Provenance_EventID_h */

// Local Variables:
// mode: c++
// End:
