#ifndef canvas_Persistency_Provenance_EventRange_h
#define canvas_Persistency_Provenance_EventRange_h

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
    explicit EventRange() = default;

    static EventRange invalid();
    static EventRange forSubRun(SubRunNumber_t s);

    explicit EventRange(SubRunNumber_t s,
                        EventNumber_t begin,
                        EventNumber_t end);

    bool operator<(EventRange const& other) const;
    bool operator==(EventRange const& other) const;
    bool operator!=(EventRange const& other) const;

    auto
    subRun() const
    {
      return subRun_;
    }
    bool
    empty() const
    {
      return begin_ == end_;
    }
    auto
    begin() const
    {
      return begin_;
    }
    auto
    end() const
    {
      return end_;
    }
    auto
    size() const
    {
      return is_valid() ? end_ - begin_ : -1ull;
    }

    static bool are_valid(EventRange const& l, EventRange const& r);

    bool is_valid() const;
    bool is_full_subRun() const;
    bool contains(SubRunNumber_t s, EventNumber_t e) const;

    // is_same(other) == true:
    //     implies is_subset(other) == true
    //     implies is_superset(other) == true
    bool is_same(EventRange const& other) const;
    bool is_adjacent(EventRange const& other) const;
    bool is_disjoint(EventRange const& other) const;
    bool is_subset(EventRange const& other) const;
    bool is_superset(EventRange const& other) const;
    bool is_overlapping(EventRange const& other) const;

    bool merge(EventRange const& other);
    void set_end(EventNumber_t const e);

  private:
    void
    require_not_full_SubRun()
    {
      if (is_full_subRun())
        throw art::Exception{art::errors::LogicError}
          << "\nAn EventRange created using EventRange::forSubRun cannot be "
             "modified.\n";
    }

    SubRunNumber_t subRun_{IDNumber<Level::SubRun>::invalid()};
    EventNumber_t begin_{IDNumber<Level::Event>::invalid()};
    EventNumber_t end_{IDNumber<Level::Event>::invalid()};
  };

  std::ostream& operator<<(std::ostream& os, EventRange const& r);
}

#endif /* canvas_Persistency_Provenance_EventRange_h */

// Local variables:
// mode: c++
// End:
