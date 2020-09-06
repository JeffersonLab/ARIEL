#ifndef canvas_Persistency_Provenance_RunID_h
#define canvas_Persistency_Provenance_RunID_h

//
// A RunID represents a unique period of operation of the data
// acquisition system, which we call a "run".
//
// Each RunID contains a fixed-size unsigned integer, the run number.
//

#include "canvas/Persistency/Provenance/IDNumber.h"
#include <cstdint>
#include <iosfwd>

namespace art {
  class RunID;

  std::ostream& operator<<(std::ostream& os, art::RunID const& iID);
}

class art::RunID {
public:
  constexpr RunID() noexcept;
  explicit RunID(RunNumber_t i);

  RunNumber_t run() const;

  bool isValid() const;
  bool isFlush() const;

  RunID next() const;
  RunID previous() const;

  static RunID maxRun();
  static RunID firstRun();
  static constexpr RunID flushRun() noexcept;

  // Comparison operators.
  bool operator==(RunID const& other) const;
  bool operator!=(RunID const& other) const;
  bool operator<(RunID const& other) const;
  bool operator<=(RunID const& other) const;
  bool operator>(RunID const& other) const;
  bool operator>=(RunID const& other) const;

  friend std::ostream& operator<<(std::ostream& os, RunID const& iID);

private:
  struct FlushFlag {};

  explicit constexpr RunID(FlushFlag) noexcept;

  RunNumber_t inRangeOrInvalid(RunNumber_t r);
  RunNumber_t run_;
};

inline constexpr art::RunID::RunID() noexcept
  : run_(IDNumber<Level::Run>::invalid())
{}

inline art::RunID::RunID(RunNumber_t i) : run_(inRangeOrInvalid(i)) {}

inline art::RunNumber_t
art::RunID::run() const
{
  return run_;
}

inline bool
art::RunID::isValid() const
{
  return (run_ != IDNumber<Level::Run>::invalid());
}

inline bool
art::RunID::isFlush() const
{
  return (run_ == IDNumber<Level::Run>::flush_value());
}

#include "canvas/Utilities/Exception.h"

inline art::RunID
art::RunID::next() const
{
  if (!isValid()) {
    throw Exception(errors::InvalidNumber)
      << "cannot increment invalid run number.";
  } else if (run_ == IDNumber<Level::Run>::max_natural()) {
    throw Exception(errors::InvalidNumber)
      << "cannot increment maximum run number.";
  }
  return RunID(run_ + 1);
}

inline art::RunID
art::RunID::previous() const
{
  if (!isValid()) {
    throw Exception(errors::InvalidNumber)
      << "cannot decrement minimum run number.";
  } else if (run_ == IDNumber<Level::Run>::max_natural()) {
    throw Exception(errors::InvalidNumber)
      << "cannot increment maximum run number.";
  }
  return RunID(run_ - 1);
}

inline art::RunID
art::RunID::maxRun()
{
  return RunID(IDNumber<Level::Run>::max_natural());
}

inline art::RunID
art::RunID::firstRun()
{
  return RunID(IDNumber<Level::Run>::first());
}

inline constexpr art::RunID
art::RunID::flushRun() noexcept
{
  return RunID(FlushFlag());
}

// Comparison operators.
inline bool
art::RunID::operator==(RunID const& other) const
{
  return other.run_ == run_;
}

inline bool
art::RunID::operator!=(RunID const& other) const
{
  return !(*this == other);
}

#include "canvas/Persistency/Provenance/SortInvalidFirst.h"

inline bool
art::RunID::operator<(RunID const& other) const
{
  static SortInvalidFirst<RunNumber_t> op(IDNumber<Level::Run>::invalid());
  return op(run_, other.run_);
}

inline bool
art::RunID::operator<=(RunID const& other) const
{
  return (*this < other) || (*this == other);
}

inline bool
art::RunID::operator>(RunID const& other) const
{
  return (other < *this);
}

inline bool
art::RunID::operator>=(RunID const& other) const
{
  return !(*this < other);
}

inline art::RunNumber_t
art::RunID::inRangeOrInvalid(RunNumber_t r)
{
  if (r == IDNumber<Level::Run>::invalid() ||
      (r >= IDNumber<Level::Run>::first() &&
       r <= IDNumber<Level::Run>::max_natural())) {
    return r;
  } else {
    throw Exception(errors::InvalidNumber)
      << "Attempt to construct RunID with an invalid number.\n"
      << "Maybe you want RunID::flushRun()?\n";
  }
}

inline constexpr art::RunID::RunID(FlushFlag) noexcept
  : run_{IDNumber<Level::Run>::flush_value()}
{}

#endif /* canvas_Persistency_Provenance_RunID_h */

// Local Variables:
// mode: c++
// End:
