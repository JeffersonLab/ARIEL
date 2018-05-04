#include "canvas/Persistency/Provenance/EventRange.h"
#include "canvas/Utilities/Exception.h"

using art::EventRange;

namespace {
  void
  require_ordering(art::EventNumber_t const b, art::EventNumber_t const e)
  {
    if (b > e)
      throw art::Exception{art::errors::LogicError}
        << "The 'begin' value for an EventRange must be less "
        << "than the 'end' value.\n"
        << "  begin: " << b << "  end: " << e << '\n';
  }
}

EventRange
EventRange::invalid()
{
  return EventRange{};
}

EventRange
EventRange::forSubRun(SubRunNumber_t const s)
{
  return EventRange{s, 0, IDNumber<Level::Event>::invalid()};
}

EventRange::EventRange(SubRunNumber_t const s,
                       EventNumber_t const b,
                       EventNumber_t const e)
  : subRun_{s}, begin_{b}, end_{e}
{
  require_ordering(begin_, end_);
}

bool
EventRange::merge(EventRange const& other)
{
  require_not_full_SubRun();
  if (!are_valid(*this, other))
    return false;

  bool const mergeable = is_adjacent(other);
  if (mergeable)
    end_ = other.end_;
  return mergeable;
}

bool
EventRange::operator<(EventRange const& other) const
{
  if (subRun_ == other.subRun_) {
    if (begin_ == other.begin_) {
      return end_ < other.end_;
    } else {
      return begin_ < other.begin_;
    }
  } else {
    return subRun_ < other.subRun_;
  }
}

bool
EventRange::operator==(EventRange const& other) const
{
  return subRun_ == other.subRun_ && begin_ == other.begin_ &&
         end_ == other.end_;
}

bool
EventRange::operator!=(EventRange const& other) const
{
  return !operator==(other);
}

void
EventRange::set_end(EventNumber_t const e)
{
  require_not_full_SubRun();
  require_ordering(begin_, e);
  end_ = e;
}

bool
EventRange::contains(SubRunNumber_t const s, EventNumber_t const e) const
{
  return subRun_ == s && e >= begin_ && e < end_;
}

bool
EventRange::are_valid(EventRange const& l, EventRange const& r)
{
  return l.is_valid() && r.is_valid();
}

bool
EventRange::is_valid() const
{
  return art::is_valid(subRun_);
}

bool
EventRange::is_full_subRun() const
{
  return art::is_valid(subRun_) && begin_ == 0 &&
         end_ == IDNumber<Level::Event>::invalid();
}

bool
EventRange::is_adjacent(EventRange const& other) const
{
  if (!are_valid(*this, other))
    return false;
  return subRun_ == other.subRun_ && end_ == other.begin_;
}

bool
EventRange::is_disjoint(EventRange const& other) const
{
  if (!are_valid(*this, other))
    return false;
  return (subRun_ == other.subRun_) ? end_ <= other.begin_ : true;
}

bool
EventRange::is_same(EventRange const& other) const
{
  if (!are_valid(*this, other))
    return false;
  return operator==(other);
}

bool
EventRange::is_subset(EventRange const& other) const
{
  if (!are_valid(*this, other))
    return false;
  return subRun_ == other.subRun_ && begin_ >= other.begin_ &&
         end_ <= other.end_;
}

bool
EventRange::is_superset(EventRange const& other) const
{
  if (!are_valid(*this, other))
    return false;
  return subRun_ == other.subRun_ && begin_ <= other.begin_ &&
         end_ >= other.end_;
}

bool
EventRange::is_overlapping(EventRange const& other) const
{
  if (!are_valid(*this, other))
    return false;
  return !is_disjoint(other) && !is_subset(other) && !is_superset(other);
}

std::ostream&
art::operator<<(std::ostream& os, EventRange const& r)
{
  os << "SubRun: " << r.subRun();
  if (r.is_full_subRun())
    os << " (full sub-run)";
  else
    os << " Event range: [" << r.begin() << ',' << r.end() << ')';
  return os;
}
