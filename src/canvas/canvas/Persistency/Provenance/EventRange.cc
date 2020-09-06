#include "canvas/Persistency/Provenance/EventRange.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/IDNumber.h"
#include "canvas/Utilities/Exception.h"

#include <ostream>

using namespace std;

namespace art {

  namespace {

    void
    require_ordering(EventNumber_t const b, EventNumber_t const e)
    {
      if (b > e) {
        throw Exception{errors::LogicError}
          << "The 'begin' value for an EventRange must be less "
          << "than the 'end' value.\n"
          << "  begin: " << b << "  end: " << e << '\n';
      }
    }

  } // unnamed namespace

  // Note: static.
  EventRange
  EventRange::invalid() noexcept
  {
    return EventRange{};
  }

  // Note: static.
  EventRange
  EventRange::forSubRun(SubRunNumber_t const s) noexcept
  {
    return EventRange{s, 0, IDNumber<Level::Event>::invalid()};
  }

  // Note: static.
  bool
  EventRange::are_valid(EventRange const& l, EventRange const& r) noexcept
  {
    return l.is_valid() && r.is_valid();
  }

  EventRange::~EventRange() noexcept = default;
  EventRange::EventRange() noexcept = default;

  EventRange::EventRange(SubRunNumber_t const s,
                         EventNumber_t const b,
                         EventNumber_t const e)
    : subRun_{s}, begin_{b}, end_{e}
  {
    require_ordering(begin_, end_);
  }

  EventRange::EventRange(EventRange const& rhs) noexcept = default;
  EventRange::EventRange(EventRange&& rhs) noexcept = default;

  EventRange& EventRange::operator=(EventRange const& rhs) noexcept = default;

  EventRange& EventRange::operator=(EventRange&& rhs) noexcept = default;

  bool
  EventRange::operator<(EventRange const& other) const noexcept
  {
    if (subRun_ == other.subRun_) {
      if (begin_ == other.begin_) {
        return end_ < other.end_;
      }
      return begin_ < other.begin_;
    }
    return subRun_ < other.subRun_;
  }

  bool
  EventRange::operator==(EventRange const& other) const noexcept
  {
    return (subRun_ == other.subRun_) && (begin_ == other.begin_) &&
           (end_ == other.end_);
  }

  bool
  EventRange::operator!=(EventRange const& other) const noexcept
  {
    return !operator==(other);
  }

  SubRunNumber_t
  EventRange::subRun() const noexcept
  {
    return subRun_;
  }

  bool
  EventRange::empty() const noexcept
  {
    return begin_ == end_;
  }

  EventNumber_t
  EventRange::begin() const noexcept
  {
    return begin_;
  }

  EventNumber_t
  EventRange::end() const noexcept
  {
    return end_;
  }

  bool
  EventRange::is_valid() const noexcept
  {
    return ::art::is_valid(subRun_);
  }

  unsigned long long
  EventRange::size() const noexcept
  {
    return is_valid() ? (end_ - begin_) : -1ull;
  }

  bool
  EventRange::is_full_subRun() const noexcept
  {
    return ::art::is_valid(subRun_) && (begin_ == 0) &&
           (end_ == IDNumber<Level::Event>::invalid());
  }

  bool
  EventRange::contains(SubRunNumber_t const s, EventNumber_t const e) const
    noexcept
  {
    return (subRun_ == s) && (e >= begin_) && (e < end_);
  }

  bool
  EventRange::is_same(EventRange const& other) const noexcept
  {
    if (!are_valid(*this, other)) {
      return false;
    }
    return operator==(other);
  }

  bool
  EventRange::is_adjacent(EventRange const& other) const noexcept
  {
    if (!are_valid(*this, other)) {
      return false;
    }
    return (subRun_ == other.subRun_) && (end_ == other.begin_);
  }

  bool
  EventRange::is_disjoint(EventRange const& other) const noexcept
  {
    if (!are_valid(*this, other)) {
      return false;
    }
    return (subRun_ == other.subRun_) ? (end_ <= other.begin_) : true;
  }

  bool
  EventRange::is_subset(EventRange const& other) const noexcept
  {
    if (!are_valid(*this, other)) {
      return false;
    }
    return (subRun_ == other.subRun_) && (begin_ >= other.begin_) &&
           (end_ <= other.end_);
  }

  bool
  EventRange::is_superset(EventRange const& other) const noexcept
  {
    if (!are_valid(*this, other)) {
      return false;
    }
    return (subRun_ == other.subRun_) && (begin_ <= other.begin_) &&
           (end_ >= other.end_);
  }

  bool
  EventRange::is_overlapping(EventRange const& other) const noexcept
  {
    if (!are_valid(*this, other)) {
      return false;
    }
    return !is_disjoint(other) && !is_subset(other) && !is_superset(other);
  }

  bool
  EventRange::merge(EventRange const& other)
  {
    require_not_full_SubRun();
    if (!are_valid(*this, other)) {
      return false;
    }
    bool const mergeable = is_adjacent(other);
    if (mergeable) {
      end_ = other.end_;
    }
    return mergeable;
  }

  void
  EventRange::set_end(EventNumber_t const e)
  {
    require_not_full_SubRun();
    require_ordering(begin_, e);
    end_ = e;
  }

  void
  EventRange::require_not_full_SubRun() const
  {
    if (is_full_subRun()) {
      throw Exception{errors::LogicError} << "\nAn EventRange created using "
                                             "EventRange::forSubRun cannot be "
                                             "modified.\n";
    }
  }

  ostream&
  operator<<(ostream& os, EventRange const& r)
  {
    os << "SubRun: " << r.subRun();
    if (r.is_full_subRun()) {
      os << " (full sub-run)";
    } else {
      os << " Event range: [" << r.begin() << ',' << r.end() << ')';
    }
    return os;
  }

} // namespace art
