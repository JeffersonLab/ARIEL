#ifndef canvas_Utilities_Level_h
#define canvas_Utilities_Level_h
// vim: set sw=2 expandtab :

#include <cassert>
#include <cstdint>
#include <ostream>
#include <type_traits>

namespace art {

  // The outer-most level must be 0.
  enum class Level {
    Job = 0,
    InputFile,
    Run,
    SubRun,
    Event,
    NumNestingLevels,
    ReadyToAdvance
  };

  // The following facility is to translate from the scoped enumerator
  // to the value represented by the underlying type.
  constexpr auto
  underlying_value(Level const l) noexcept
  {
    return static_cast<std::underlying_type_t<Level>>(l);
  }

  constexpr auto
  highest_level() noexcept
  {
    return Level{0};
  }

  constexpr auto
  level_up(Level const l) noexcept
  {
    return Level{underlying_value(l) - 1};
  }

  constexpr auto
  most_deeply_nested_level() noexcept
  {
    return level_up(Level::NumNestingLevels);
  }

  constexpr auto
  level_down(Level const l) noexcept
  {
    return Level{underlying_value(l) + 1};
  }

  constexpr bool
  is_above_most_deeply_nested_level(Level const l) noexcept
  {
    return underlying_value(l) < underlying_value(most_deeply_nested_level());
  }

  constexpr bool
  is_most_deeply_nested_level(Level const l) noexcept
  {
    return l == most_deeply_nested_level();
  }

  constexpr bool
  is_highest_level(Level const l) noexcept
  {
    return l == highest_level();
  }

  constexpr bool
  is_level_contained_by(Level const l1, Level const l2) noexcept
  {
    return underlying_value(l1) > underlying_value(l2);
  }

  inline std::ostream&
  operator<<(std::ostream& os, Level const l)
  {
    switch (l) {
      case Level::Job:
        os << "Job";
        break;
      case Level::InputFile:
        os << "InputFile";
        break;
      case Level::Run:
        os << "Run";
        break;
      case Level::SubRun:
        os << "SubRun";
        break;
      case Level::Event:
        os << "Event";
        break;
      case Level::NumNestingLevels:
        os << underlying_value(Level::NumNestingLevels);
        break;
      case Level::ReadyToAdvance:
        os << "ReadyToAdvance";
        break;
    }
    return os;
  }

} // namespace art

#endif /* canvas_Utilities_Level_h */

// Local variables:
// mode: c++
// End:
