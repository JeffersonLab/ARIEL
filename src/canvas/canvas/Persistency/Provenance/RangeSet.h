#ifndef canvas_Persistency_Provenance_RangeSet_h
#define canvas_Persistency_Provenance_RangeSet_h

#include "canvas/Persistency/Provenance/EventRange.h"
#include "canvas/Persistency/Provenance/IDNumber.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/SubRunID.h"
#include "canvas/Utilities/Exception.h"
#include "cetlib/container_algorithms.h"

#include <algorithm>
#include <ostream>
#include <set>
#include <vector>

namespace art {

  class EventID;

  namespace detail {
    EventRange full_run_event_range();
  }

  class RangeSet {
  public:
    static RangeSet invalid();
    static RangeSet forRun(RunID);
    static RangeSet forSubRun(SubRunID);

    explicit RangeSet(RunNumber_t);
    explicit RangeSet(RunNumber_t, std::vector<EventRange> const& eventRanges);

    using const_iterator = std::vector<EventRange>::const_iterator;

    RunNumber_t
    run() const
    {
      return run_;
    }

    std::vector<EventRange> const&
    ranges() const
    {
      return ranges_;
    }

    bool contains(RunNumber_t, SubRunNumber_t, EventNumber_t) const;

    bool is_valid() const;

    bool is_full_run() const;
    bool is_full_subRun() const;
    bool is_sorted() const;

    bool
    is_collapsed() const
    {
      return isCollapsed_;
    }

    std::string to_compact_string() const;
    bool has_disjoint_ranges() const;

    // Empty means that no events are represented by this RangeSet.
    // It does not necessarily mean that the ranges_ data member is
    // empty.
    bool empty() const;

    auto
    begin() const
    {
      return ranges_.begin();
    }
    auto
    end() const
    {
      return ranges_.end();
    }

    unsigned checksum() const;

    decltype(auto)
    front()
    {
      return ranges_.front();
    }
    decltype(auto)
    back()
    {
      return ranges_.back();
    }

    void assign_ranges(const_iterator b, const_iterator e);

    void update(EventID const&);

    template <typename... ARGS>
    void emplace_range(ARGS&&...);

    RangeSet& collapse();
    RangeSet& merge(RangeSet const& other);

    // For a range [1,6) split into [1,3) and [3,6) the specified
    // event number is the new 'end' of the left range (3).
    std::pair<const_iterator, bool> split_range(SubRunNumber_t, EventNumber_t);

    void
    set_run(RunNumber_t const r)
    {
      run_ = r;
    }

    void
    sort()
    {
      cet::sort_all(ranges_);
    }

    void
    clear()
    {
      ranges_.clear();
    }

    static constexpr unsigned
    invalidChecksum()
    {
      return std::numeric_limits<unsigned>::max();
    }

  private:
    explicit RangeSet() = default;

    void
    require_not_full_run()
    {
      if (is_full_run())
        throw art::Exception(art::errors::LogicError,
                             "RangeSet::require_not_full_run")
          << "\nA RangeSet created using RangeSet::forRun cannot be "
             "modified.\n";
    }

    RunNumber_t run_{IDNumber<Level::Run>::invalid()};
    std::vector<EventRange> ranges_{};

    // Auxiliary info
    bool isCollapsed_{false};
    mutable unsigned checksum_{invalidChecksum()};
  };

  //==========================================================
  // Non-member functions

  bool operator==(RangeSet const& l, RangeSet const& r);
  bool same_ranges(RangeSet const& l, RangeSet const& r);
  bool disjoint_ranges(RangeSet const& l, RangeSet const& r);
  void throw_if_not_disjoint(RunNumber_t const rn,
                             EventRange const& left,
                             EventRange const& right) noexcept(false);

  // If one range-set is a superset of the other, the return value is
  // 'true'.  If two range-sets are the same, then they are also
  // overlapping.
  bool overlapping_ranges(RangeSet const& l, RangeSet const& r);
  std::ostream& operator<<(std::ostream& os, RangeSet const& rs);

  //==========================================================
  // RangeSet implementation details

  template <typename... ARGS>
  void
  RangeSet::emplace_range(ARGS&&... args)
  {
    require_not_full_run();
    ranges_.emplace_back(std::forward<ARGS>(args)...);
    isCollapsed_ = false;
  }
}

#endif /* canvas_Persistency_Provenance_RangeSet_h */

// Local variables:
// mode: c++
// End:
