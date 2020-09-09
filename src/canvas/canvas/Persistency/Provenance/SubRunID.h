#ifndef canvas_Persistency_Provenance_SubRunID_h
#define canvas_Persistency_Provenance_SubRunID_h

// A SubRunID represents a unique period within a run.

#include "canvas/Persistency/Provenance/RunID.h"

#include <cstdint>
#include <ostream>

namespace art {
  class SubRunID;

  std::ostream& operator<<(std::ostream& os, SubRunID const& iID);
}

class art::SubRunID {
public:
  constexpr SubRunID() noexcept;
  SubRunID(RunID rID, SubRunNumber_t srID);
  SubRunID(RunNumber_t rID, SubRunNumber_t srID);

  RunID const& runID() const;
  RunNumber_t run() const;
  SubRunNumber_t subRun() const;

  bool isValid() const;
  bool isFlush() const;

  SubRunID next() const;
  SubRunID nextRun() const;
  SubRunID previous() const;
  SubRunID previousRun() const;

  static SubRunID maxSubRun();
  static SubRunID firstSubRun();
  static SubRunID firstSubRun(RunID const& rID);
  static SubRunID invalidSubRun(RunID const& rID);
  static constexpr SubRunID flushSubRun() noexcept;
  static SubRunID flushSubRun(RunID const& rID);

  // Comparison operators.
  bool operator==(SubRunID const& other) const;
  bool operator!=(SubRunID const& other) const;
  bool operator<(SubRunID const& other) const;
  bool operator<=(SubRunID const& other) const;
  bool operator>(SubRunID const& other) const;
  bool operator>=(SubRunID const& other) const;

  friend std::ostream& operator<<(std::ostream& os, SubRunID const& iID);

private:
  struct FlushFlag {};

  explicit constexpr SubRunID(FlushFlag) noexcept;
  SubRunID(RunID rID, FlushFlag);

  SubRunNumber_t inRangeOrInvalid(SubRunNumber_t sr);

  void checkSane();

  RunID run_;
  SubRunNumber_t subRun_;
};

inline constexpr art::SubRunID::SubRunID() noexcept
  : run_(), subRun_(IDNumber<Level::SubRun>::invalid())
{}

inline art::SubRunID::SubRunID(RunNumber_t rID, SubRunNumber_t srID)
  : run_(rID), subRun_(inRangeOrInvalid(srID))
{
  checkSane();
}

inline art::RunID const&
art::SubRunID::runID() const
{
  return run_;
}

inline art::RunNumber_t
art::SubRunID::run() const
{
  return run_.run();
}

inline art::SubRunNumber_t
art::SubRunID::subRun() const
{
  return subRun_;
}

inline bool
art::SubRunID::isValid() const
{
  return (subRun_ != IDNumber<Level::SubRun>::invalid() && run_.isValid());
}

inline bool
art::SubRunID::isFlush() const
{
  return subRun_ == IDNumber<Level::SubRun>::flush_value();
}

#include "canvas/Utilities/Exception.h"

inline art::SubRunID
art::SubRunID::next() const
{
  if (!isValid()) {
    throw art::Exception(art::errors::InvalidNumber)
      << "cannot increment invalid subrun number.";
  } else if (subRun_ == IDNumber<Level::SubRun>::max_natural()) {
    return nextRun();
  }
  return SubRunID(run_, subRun_ + 1);
}

inline art::SubRunID
art::SubRunID::nextRun() const
{
  return SubRunID{run_.next(), IDNumber<Level::SubRun>::first()};
}

inline art::SubRunID
art::SubRunID::previous() const
{
  if (!isValid()) {
    throw art::Exception(art::errors::InvalidNumber)
      << "cannot decrement invalid subrun number.";
  } else if (subRun_ == IDNumber<Level::SubRun>::first()) {
    return previousRun();
  }
  return SubRunID(run_, subRun_ - 1);
}

inline art::SubRunID
art::SubRunID::previousRun() const
{
  return SubRunID{run_.previous(), IDNumber<Level::SubRun>::max_natural()};
}

inline art::SubRunID
art::SubRunID::maxSubRun()
{
  return SubRunID{RunID::maxRun(), IDNumber<Level::SubRun>::max_natural()};
}

inline art::SubRunID
art::SubRunID::firstSubRun()
{
  return SubRunID{RunID::firstRun(), IDNumber<Level::SubRun>::first()};
}

inline art::SubRunID
art::SubRunID::firstSubRun(RunID const& rID)
{
  return SubRunID{rID, IDNumber<Level::SubRun>::first()};
}

inline art::SubRunID
art::SubRunID::invalidSubRun(RunID const& rID)
{
  return SubRunID{rID, IDNumber<Level::SubRun>::invalid()};
}

inline constexpr art::SubRunID
art::SubRunID::flushSubRun() noexcept
{
  return SubRunID{FlushFlag()};
}

inline art::SubRunID
art::SubRunID::flushSubRun(RunID const& rID)
{
  return SubRunID{rID, FlushFlag()};
}

// Comparison operators.
inline bool
art::SubRunID::operator==(SubRunID const& other) const
{
  return other.run_ == run_ && other.subRun_ == subRun_;
}

inline bool
art::SubRunID::operator!=(SubRunID const& other) const
{
  return !(*this == other);
}

#include "canvas/Persistency/Provenance/SortInvalidFirst.h"

inline bool
art::SubRunID::operator<(SubRunID const& other) const
{
  constexpr SortInvalidFirst<SubRunNumber_t> op{
    IDNumber<Level::SubRun>::invalid()};
  if (run_ == other.run_) {
    return op(subRun_, other.subRun_);
  } else {
    return run_ < other.run_;
  }
}

inline bool
art::SubRunID::operator<=(SubRunID const& other) const
{
  return (*this < other) || (*this == other);
}

inline bool
art::SubRunID::operator>(SubRunID const& other) const
{
  return (other < *this);
}

inline bool
art::SubRunID::operator>=(SubRunID const& other) const
{
  return !(*this < other);
}

inline constexpr art::SubRunID::SubRunID(FlushFlag) noexcept
  : run_(RunID::flushRun()), subRun_(IDNumber<Level::SubRun>::flush_value())
{}

inline art::SubRunID::SubRunID(RunID rID, FlushFlag)
  : run_(std::move(rID)), subRun_(IDNumber<Level::SubRun>::flush_value())
{}

inline art::SubRunID::SubRunID(RunID rID, SubRunNumber_t srID)
  : run_(std::move(rID)), subRun_(inRangeOrInvalid(srID))
{
  checkSane();
}

inline art::SubRunNumber_t
art::SubRunID::inRangeOrInvalid(SubRunNumber_t sr)
{
  if (sr == IDNumber<Level::SubRun>::flush_value()) {
    throw Exception(errors::InvalidNumber)
      << "Attempt to construct SubRunID with an invalid number.\n"
      << "Maybe you want SubRunID::flushSubRun()?\n";
  }
  return sr;
}

inline void
art::SubRunID::checkSane()
{
  if (isValid() && !run_.isValid()) {
    throw art::Exception(art::errors::InvalidNumber)
      << "SubRunID is not meaningful with valid subRun and invalid Run.\n";
  }
}

#endif /* canvas_Persistency_Provenance_SubRunID_h */

// Local Variables:
// mode: c++
// End:
