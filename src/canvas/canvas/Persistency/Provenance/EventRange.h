#ifndef canvas_Persistency_Provenance_EventRange_h
#define canvas_Persistency_Provenance_EventRange_h
// vim: set sw=2 expandtab :

// An EventRange has a SubRun number, and two event numbers
// corresponding to the half-open range:
//
//   [beginEvent, endEvent)

#include "canvas/Persistency/Provenance/IDNumber.h"
#include "canvas/Utilities/Exception.h"

#include <ostream>

namespace art {

  class EventRange {
  public:
    static EventRange invalid() noexcept;
    static EventRange forSubRun(SubRunNumber_t s) noexcept;
    static bool are_valid(EventRange const& l, EventRange const& r) noexcept;

    ~EventRange() noexcept;
    explicit EventRange() noexcept;

    // Note: Throws LogicError if begin > end.
    explicit EventRange(SubRunNumber_t s,
                        EventNumber_t begin,
                        EventNumber_t end);

    EventRange(EventRange const&) noexcept;
    EventRange(EventRange&&) noexcept;
    EventRange& operator=(EventRange const&) noexcept;
    EventRange& operator=(EventRange&&) noexcept;

    bool operator<(EventRange const& other) const noexcept;
    bool operator==(EventRange const& other) const noexcept;
    bool operator!=(EventRange const& other) const noexcept;

    SubRunNumber_t subRun() const noexcept;
    EventNumber_t begin() const noexcept;
    EventNumber_t end() const noexcept;

    unsigned long long size() const noexcept;
    bool empty() const noexcept;
    bool is_valid() const noexcept;
    bool is_full_subRun() const noexcept;
    bool contains(SubRunNumber_t s, EventNumber_t e) const noexcept;

    // is_same(other) == true:
    //     implies is_subset(other) == true
    //     implies is_superset(other) == true
    bool is_same(EventRange const& other) const noexcept;
    bool is_adjacent(EventRange const& other) const noexcept;
    bool is_disjoint(EventRange const& other) const noexcept;
    bool is_subset(EventRange const& other) const noexcept;
    bool is_superset(EventRange const& other) const noexcept;
    bool is_overlapping(EventRange const& other) const noexcept;

    // Throws LogicError if we are a full SubRun range.
    bool merge(EventRange const& other);

    // Throws LogicError if we are a full SubRun range.
    // Throws LogicError if our begin_ > e.
    void set_end(EventNumber_t const e);

  private:
    // Throws LogicError if we are a full SubRun range.
    void require_not_full_SubRun() const;

    SubRunNumber_t subRun_{IDNumber<Level::SubRun>::invalid()};
    EventNumber_t begin_{IDNumber<Level::Event>::invalid()};
    EventNumber_t end_{IDNumber<Level::Event>::invalid()};
  };

  std::ostream& operator<<(std::ostream& os, EventRange const& r);

} // namespace art

#endif /* canvas_Persistency_Provenance_EventRange_h */

// Local variables:
// mode: c++
// End:
