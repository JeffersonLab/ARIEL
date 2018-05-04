#ifndef canvas_Persistency_Provenance_Timestamp_h
#define canvas_Persistency_Provenance_Timestamp_h

#include <cstdint>

namespace art {
  using TimeValue_t = std::uint64_t;
  class Timestamp;
}

class art::Timestamp {
public:
  constexpr Timestamp(TimeValue_t const iValue)
    : timeLow_{static_cast<std::uint32_t>(lowMask() & iValue)}
    , timeHigh_{static_cast<std::uint32_t>(iValue >> 32)}
  {}

  constexpr Timestamp()
    : timeLow_{invalidTimestamp().timeLow_}
    , timeHigh_{invalidTimestamp().timeHigh_}
  {}

  constexpr TimeValue_t
  value() const
  {
    return (static_cast<std::uint64_t>(timeHigh_) << 32) | timeLow_;
  }

  constexpr std::uint32_t
  timeLow() const
  {
    return timeLow_;
  }
  constexpr std::uint32_t
  timeHigh() const
  {
    return timeHigh_;
  }

  // ---------- const member functions ---------------------
  constexpr bool
  operator==(Timestamp const& iRHS) const
  {
    return timeHigh_ == iRHS.timeHigh_ && timeLow_ == iRHS.timeLow_;
  }

  constexpr bool
  operator!=(Timestamp const& iRHS) const
  {
    return !(*this == iRHS);
  }

  constexpr bool
  operator<(Timestamp const& iRHS) const
  {
    return timeHigh_ == iRHS.timeHigh_ ? timeLow_ < iRHS.timeLow_ :
                                         timeHigh_ < iRHS.timeHigh_;
  }

  constexpr bool
  operator<=(Timestamp const& iRHS) const
  {
    return timeHigh_ == iRHS.timeHigh_ ? timeLow_ <= iRHS.timeLow_ :
                                         timeHigh_ <= iRHS.timeHigh_;
  }

  constexpr bool
  operator>(Timestamp const& iRHS) const
  {
    return timeHigh_ == iRHS.timeHigh_ ? timeLow_ > iRHS.timeLow_ :
                                         timeHigh_ > iRHS.timeHigh_;
  }

  constexpr bool
  operator>=(Timestamp const& iRHS) const
  {
    return timeHigh_ == iRHS.timeHigh_ ? timeLow_ >= iRHS.timeLow_ :
                                         timeHigh_ >= iRHS.timeHigh_;
  }

  // ---------- static member functions --------------------
  static constexpr Timestamp
  invalidTimestamp()
  {
    return Timestamp{0};
  }
  static constexpr Timestamp
  endOfTime()
  {
    return Timestamp(-1);
  }
  static constexpr Timestamp
  beginOfTime()
  {
    return Timestamp{1};
  }

private:
  std::uint32_t timeLow_;
  std::uint32_t timeHigh_;

  static constexpr TimeValue_t
  lowMask()
  {
    return 0xFFFFFFFF;
  }
};
#endif /* canvas_Persistency_Provenance_Timestamp_h */

// Local Variables:
// mode: c++
// End:
