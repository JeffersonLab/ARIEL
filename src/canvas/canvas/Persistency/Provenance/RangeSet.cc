#include "canvas/Persistency/Provenance/RangeSet.h"
#include "canvas/Persistency/Provenance/EventID.h"
#include "cetlib/crc32.h"

using namespace art;

namespace {

  auto
  checksum(std::string const& compact_string)
  {
    return cet::crc32{compact_string}.digest();
  }

  bool
  both_valid(RangeSet const& l, RangeSet const& r)
  {
    return l.is_valid() && r.is_valid();
  }

  bool
  disjoint(std::vector<EventRange> const& ranges)
  {
    if (ranges.size() < 2ull)
      return true;

    auto it = ranges.cbegin();
    auto const end = ranges.cend();
    for (auto next = std::next(it); next != end; ++it, ++next) {
      if (!it->is_disjoint(*next))
        return false;
    }
    return true;
  }
}

EventRange
detail::full_run_event_range()
{
  static EventRange const range{
    IDNumber<Level::SubRun>::invalid(), 0, IDNumber<Level::Event>::invalid()};
  return range;
}

RangeSet
RangeSet::invalid()
{
  return RangeSet{};
}

RangeSet
RangeSet::forRun(RunID const rid)
{
  return RangeSet{rid.run(), {detail::full_run_event_range()}};
}

RangeSet
RangeSet::forSubRun(SubRunID const srid)
{
  return RangeSet{srid.run(), {EventRange::forSubRun(srid.subRun())}};
}

RangeSet::RangeSet(RunNumber_t const r) : RangeSet{r, {}} {}

RangeSet::RangeSet(RunNumber_t const r,
                   std::vector<EventRange> const& eventRanges)
  : run_{r}, ranges_{eventRanges}
{
  sort();
  collapse();
}

RangeSet&
RangeSet::collapse()
{
  if (isCollapsed_) {
    return *this;
  }

  if (ranges_.size() < 2) {
    isCollapsed_ = true;
    return *this;
  }

  if (!is_sorted())
    throw art::Exception(art::errors::LogicError, "RangeSet::collapse()")
      << "A range set must be sorted before it is collapsed.\n";

  auto processing = ranges_;
  decltype(ranges_) result;
  result.reserve(ranges_.size());
  result.push_back(ranges_.front());
  for (auto ir = ranges_.cbegin() + 1, e = ranges_.cend(); ir != e; ++ir) {
    auto const& r = *ir;
    auto& back = result.back();
    if (back.is_adjacent(r)) {
      back.merge(r);
    } else {
      throw_if_not_disjoint(run_, back, r);
      result.push_back(r);
    }
  }
  std::swap(ranges_, result);
  isCollapsed_ = true;
  return *this;
}

void
RangeSet::assign_ranges(RangeSet::const_iterator const b,
                        RangeSet::const_iterator const e)
{
  require_not_full_run();
  ranges_.assign(b, e);
}

void
art::RangeSet::update(EventID const& id)
{
  require_not_full_run();
  if (ranges_.empty()) {
    run_ = id.run();
    ranges_.emplace_back(id.subRun(), id.event(), id.next().event());
    return;
  }
  auto& back = ranges_.back();
  if (back.subRun() == id.subRun() && back.end() == id.event()) {
    back.set_end(id.next().event());
  } else {
    ranges_.emplace_back(id.subRun(), id.event(), id.next().event());
  }
}

RangeSet&
RangeSet::merge(RangeSet const& other)
{
  if (!other.is_valid())
    return *this;

  if (!is_valid())
    run_ = other.run();

  std::vector<EventRange> merged;
  std::merge(ranges_.begin(),
             ranges_.end(),
             other.ranges_.begin(),
             other.ranges_.end(),
             std::back_inserter(merged));
  std::unique(merged.begin(), merged.end());
  std::swap(ranges_, merged);
  isCollapsed_ = false;
  collapse();
  return *this;
}

std::pair<RangeSet::const_iterator, bool>
RangeSet::split_range(SubRunNumber_t const s, EventNumber_t const e)
{
  require_not_full_run();
  bool did_split{false};
  auto result = ranges_.end();
  auto foundRange =
    std::find_if(ranges_.cbegin(), ranges_.cend(), [s, e](auto const& r) {
      return r.contains(s, e);
    });

  // Split only if:
  // - the range is found (i.e. the event is contained by the found range)
  // - the range is valid
  // - the size of the range is greater than 1
  if (foundRange != ranges_.cend() && foundRange->is_valid() &&
      foundRange->size() > 1ull) {
    auto const begin = foundRange->begin();
    auto const end = foundRange->end();
    auto leftIt = ranges_.emplace(foundRange, s, begin, e);
    result = std::next(leftIt);
    EventRange right{s, e, end};
    std::swap(*result, right);
    did_split = true;
  }
  return std::make_pair(result, did_split);
}

unsigned
RangeSet::checksum() const
{
  // Could cache checksums to improve performance when necessary.
  return checksum_ = ::checksum(to_compact_string());
}

bool
RangeSet::has_disjoint_ranges() const
{
  if (isCollapsed_ || is_sorted()) {
    return ranges_.size() < 2ull ? true : disjoint(ranges_);
  }

  RangeSet tmp{*this};
  tmp.sort();
  tmp.collapse();
  return tmp.has_disjoint_ranges();
}

bool
RangeSet::empty() const
{
  for (auto const& range : ranges_) {
    if (!range.empty())
      return false;
  }
  return true;
}

bool
RangeSet::contains(RunNumber_t const r,
                   SubRunNumber_t const s,
                   EventNumber_t const e) const
{
  if (run_ != r)
    return false;

  for (auto const& range : ranges_) {
    if (range.contains(s, e))
      return true;
  }

  return false;
}

bool
RangeSet::is_valid() const
{
  if (run_ == IDNumber<Level::Run>::invalid()) {
    return false;
  }
  if (is_full_run()) {
    return true;
  }
  for (auto const& range : ranges_) {
    if (!range.is_valid())
      return false;
  }
  return true;
}

bool
RangeSet::is_full_run() const
{
  return ranges_.size() == 1ull &&
         ranges_.front() == detail::full_run_event_range();
}

bool
RangeSet::is_full_subRun() const
{
  return ranges_.size() == 1ull && ranges_.front().is_full_subRun();
}

bool
RangeSet::is_sorted() const
{
  return std::is_sorted(ranges_.cbegin(), ranges_.cend());
}

std::string
RangeSet::to_compact_string() const
{
  using namespace std;
  string s{to_string(run_)};
  if (!ranges_.empty())
    s += ":";
  for (auto const& r : ranges_) {
    s += to_string(r.subRun());
    s += "[";
    s += to_string(r.begin());
    s += ",";
    s += to_string(r.end());
    s += ")";
  }
  return s;
}

bool
art::operator==(RangeSet const& l, RangeSet const& r)
{
  if (!both_valid(l, r))
    return false;
  return l.run() == r.run() && l.ranges() == r.ranges();
}

bool
art::same_ranges(RangeSet const& l, RangeSet const& r)
{
  return l == r;
}

bool
art::disjoint_ranges(RangeSet const& l, RangeSet const& r)
{
  if (!both_valid(l, r))
    return false;

  if (!l.has_disjoint_ranges() || !r.has_disjoint_ranges())
    return false;

  if (l.run() != r.run())
    return true;

  // Empty RangeSets are disjoint wrt. other RangeSets.  Must handle
  // this case separately than l == r case.
  if (l.empty() || r.empty())
    return true;

  // If we get this far, then neither RangeSet is empty.
  if (l == r)
    return false;

  RangeSet ltmp{l};
  RangeSet rtmp{r};
  auto const& lranges = ltmp.collapse().ranges();
  auto const& rranges = rtmp.collapse().ranges();

  std::vector<EventRange> merged;
  std::merge(lranges.begin(),
             lranges.end(),
             rranges.begin(),
             rranges.end(),
             std::back_inserter(merged));

  return disjoint(merged);
}

void
art::throw_if_not_disjoint(RunNumber_t const rn,
                           EventRange const& left,
                           EventRange const& right) noexcept(false)
{
  if (left.is_disjoint(right))
    return;
  throw art::Exception(art::errors::EventRangeOverlap)
    << "Attempt to merge event ranges that both contain one or more of the "
       "same events\n"
    << " Run: " << rn << '\n'
    << "  " << left << "  vs.\n"
    << "  " << right;
}

bool
art::overlapping_ranges(RangeSet const& l, RangeSet const& r)
{
  if (!both_valid(l, r))
    return false;
  return !disjoint_ranges(l, r);
}

std::ostream&
art::operator<<(std::ostream& os, RangeSet const& rs)
{
  os << " Run: " << rs.run();
  if (rs.is_full_run()) {
    os << " (full run)";
    return os;
  }
  for (auto const& er : rs.ranges()) {
    os << "\n  " << er;
  }
  return os;
}
