#ifndef canvas_Persistency_Provenance_IDNumber_h
#define canvas_Persistency_Provenance_IDNumber_h
// vim: set sw=2 expandtab :

#include "canvas/Utilities/Level.h"
#include <cstdint>

namespace art {

  template <Level>
  struct IDNumber;

  template <>
  struct IDNumber<Level::Event> {
    using type = std::uint32_t;
    static constexpr type
    invalid() noexcept
    {
      return -1u;
    }
    static constexpr type
    max_valid() noexcept
    {
      return invalid() - 1u;
    }
    static constexpr type
    flush_value() noexcept
    {
      return max_valid();
    }
    static constexpr type
    max_natural() noexcept
    {
      return flush_value() - 1u;
    }
    static constexpr type
    first() noexcept
    {
      return 1u;
    }
    static type
    next(type const n) noexcept
    {
      return n + 1u;
    }
  };

  template <>
  struct IDNumber<Level::SubRun> {
    using type = std::uint32_t;
    static constexpr type
    invalid() noexcept
    {
      return -1u;
    }
    static constexpr type
    max_valid() noexcept
    {
      return invalid() - 1u;
    }
    static constexpr type
    flush_value() noexcept
    {
      return max_valid();
    }
    static constexpr type
    max_natural() noexcept
    {
      return flush_value() - 1u;
    }
    static constexpr type
    first() noexcept
    {
      return 0u;
    }
  };

  template <>
  struct IDNumber<Level::Run> {
    using type = std::uint32_t;
    static constexpr type
    invalid() noexcept
    {
      return -1u;
    }
    static constexpr type
    max_valid() noexcept
    {
      return invalid() - 1u;
    }
    static constexpr type
    flush_value() noexcept
    {
      return max_valid();
    }
    static constexpr type
    max_natural() noexcept
    {
      return flush_value() - 1u;
    }
    static constexpr type
    first() noexcept
    {
      return 1u;
    }
  };

  template <Level L>
  using IDNumber_t = typename IDNumber<L>::type;

  template <Level L = Level::Event>
  constexpr bool
  is_valid(IDNumber_t<L> const id) noexcept
  {
    return id != IDNumber<L>::invalid();
  }

  using EventNumber_t = IDNumber_t<Level::Event>;
  using SubRunNumber_t = IDNumber_t<Level::SubRun>;
  using RunNumber_t = IDNumber_t<Level::Run>;

} // namespace art

#endif /* canvas_Persistency_Provenance_IDNumber_h */

// Local variables:
// mode: c++
// End:
