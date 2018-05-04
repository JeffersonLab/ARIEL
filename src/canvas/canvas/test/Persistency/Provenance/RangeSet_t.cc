#define BOOST_TEST_MODULE (RangeSet_t)
#include "canvas/Persistency/Provenance/RangeSet.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "cetlib/quiet_unit_test.hpp"

#include <iostream>
#include <string>

using namespace std::string_literals;
using art::EventRange;
using art::RangeSet;
using art::RunID;

BOOST_AUTO_TEST_SUITE(RangeSet_t)

BOOST_AUTO_TEST_CASE(empty1)
{
  RangeSet const rs{1};
  BOOST_CHECK(rs.empty());
  BOOST_CHECK(rs.has_disjoint_ranges());
  BOOST_CHECK(rs.is_sorted());
}

BOOST_AUTO_TEST_CASE(empty2)
{
  RangeSet const rs1{1};
  RangeSet const rs2{1};
  BOOST_CHECK(art::disjoint_ranges(rs1, rs2));
}

BOOST_AUTO_TEST_CASE(empty3)
{
  RangeSet rs{1};
  rs.emplace_range(1, 2, 2);
  rs.emplace_range(1, 7, 7);
  rs.emplace_range(1, 7, 7);
  BOOST_CHECK(rs.empty());
}

BOOST_AUTO_TEST_CASE(fullRun1)
{
  auto rs = RangeSet::forRun(RunID{72});
  BOOST_CHECK(!rs.empty());
  BOOST_CHECK(rs.has_disjoint_ranges());
  BOOST_CHECK(rs.is_valid());
  BOOST_CHECK(rs.is_sorted());
  BOOST_CHECK(rs.is_collapsed());
  BOOST_CHECK(rs.is_full_run());
  std::ostringstream oss;
  oss << rs;
  BOOST_CHECK_EQUAL(oss.str(), " Run: 72 (full run)"s);
}

BOOST_AUTO_TEST_CASE(fullRun2)
{
  auto const rs1 = RangeSet::forRun(RunID{1});
  auto const rs2 = RangeSet::forRun(RunID{2});

  BOOST_CHECK(rs1.is_full_run());
  BOOST_CHECK(rs2.is_full_run());
  BOOST_CHECK(rs1.has_disjoint_ranges());
  BOOST_CHECK(rs2.has_disjoint_ranges());
  BOOST_CHECK(art::disjoint_ranges(rs1, rs2));
}

BOOST_AUTO_TEST_CASE(fullRun3)
{
  RangeSet rs1{1};
  rs1.emplace_range(1, 1, 2);
  auto const rs2 = RangeSet::forRun(RunID{1});

  BOOST_CHECK(!rs1.is_full_run());
  BOOST_CHECK(rs2.is_full_run());
  BOOST_CHECK(rs1.has_disjoint_ranges());
  BOOST_CHECK(rs2.has_disjoint_ranges());
  BOOST_CHECK(!art::disjoint_ranges(rs1, rs2));
}

BOOST_AUTO_TEST_CASE(fullRun4)
{
  // Mimic constructing a full-run range set based on its internal
  // representation (this is how a full-run range set would be
  // constructed from reading from a file).
  RangeSet const rs1{1, {art::detail::full_run_event_range()}};
  auto const rs2 = RangeSet::forRun(RunID{1});
  BOOST_CHECK_EQUAL(rs1, rs2);
}

BOOST_AUTO_TEST_CASE(disjoint1)
{
  RangeSet rs{1};
  rs.emplace_range(1, 2, 7);
  rs.emplace_range(1, 9, 14);
  BOOST_CHECK(rs.has_disjoint_ranges());
  BOOST_CHECK(rs.is_sorted());
  BOOST_CHECK(!rs.is_full_run());
}

BOOST_AUTO_TEST_CASE(disjoint2)
{
  RangeSet rs1{1};
  rs1.emplace_range(1, 1, 8);
  rs1.emplace_range(1, 9, 14);
  rs1.emplace_range(3, 1, 8);
  rs1.emplace_range(4, 5, 8);
  BOOST_CHECK(rs1.has_disjoint_ranges());
  BOOST_CHECK(rs1.is_sorted());

  RangeSet rs2{1};
  rs2.emplace_range(1, 8, 9);
  rs2.emplace_range(1, 14, 101);
  rs2.emplace_range(2, 14, 101);
  rs2.emplace_range(4, 1, 5);
  BOOST_CHECK(rs2.has_disjoint_ranges());
  BOOST_CHECK(rs2.is_sorted());

  BOOST_CHECK(art::disjoint_ranges(rs1, rs2));
}

BOOST_AUTO_TEST_CASE(collapsing1)
{
  RangeSet rs{1};
  rs.emplace_range(1, 1, 4);
  rs.emplace_range(1, 4, 6);
  rs.emplace_range(1, 6, 11);
  rs.emplace_range(1, 11, 101);
  BOOST_CHECK_EQUAL(rs.ranges().size(), 4u);
  rs.collapse();
  BOOST_REQUIRE_EQUAL(rs.ranges().size(), 1u);
  BOOST_CHECK_EQUAL(rs.front().begin(), 1u);
  BOOST_CHECK_EQUAL(rs.front().end(), 101u);
  BOOST_CHECK(rs.has_disjoint_ranges());
}

BOOST_AUTO_TEST_CASE(collapsing2)
{
  RangeSet rs{1};
  rs.emplace_range(1, 1, 4);
  rs.emplace_range(1, 1, 11);
  rs.emplace_range(1, 4, 11);
  BOOST_CHECK(rs.is_sorted());
  BOOST_CHECK_EXCEPTION(
    rs.collapse(), art::Exception, [](art::Exception const& e) {
      return e.categoryCode() == art::errors::EventRangeOverlap;
    });
}

BOOST_AUTO_TEST_CASE(splitting1)
{
  RangeSet rs{1};
  rs.emplace_range(1, 2, 7);
  rs.emplace_range(1, 9, 14);
  rs.split_range(1, 4);
  RangeSet ref{1};
  ref.emplace_range(1, 2, 4);
  ref.emplace_range(1, 4, 7);
  ref.emplace_range(1, 9, 14);
  BOOST_CHECK_EQUAL(rs, ref);
}

BOOST_AUTO_TEST_CASE(splitting2)
{
  RangeSet rs{1};
  rs.emplace_range(1, 2, 7);
  rs.emplace_range(1, 9, 14);
  auto split_range1 =
    rs.split_range(1, 7); // SubRun 1, Event 7 not contained in range
  auto split_range2 =
    rs.split_range(2, 7); // SubRun 2, Event 7 not contained " "
  BOOST_CHECK(split_range1.first == rs.end());
  BOOST_CHECK(split_range2.first == rs.end());
  BOOST_CHECK(!split_range1.second);
  BOOST_CHECK(!split_range2.second);
}

BOOST_AUTO_TEST_CASE(splitting3)
{
  RangeSet rs{1};
  rs.emplace_range(1, 2, 3);
  auto split_range1 = rs.split_range(1, 2);
  auto split_range2 = rs.split_range(1, 3);
  BOOST_CHECK(split_range1.first == rs.end()); // Range too small to split
  BOOST_CHECK(split_range2.first ==
              rs.end()); // SubRun 1, Event 3 not contained in range
  BOOST_CHECK(!split_range1.second);
  BOOST_CHECK(!split_range2.second);
}

BOOST_AUTO_TEST_CASE(assigning)
{
  RangeSet rs{1};
  rs.emplace_range(1, 2, 7);
  rs.emplace_range(1, 9, 14);
  rs.split_range(1, 4);
  RangeSet ref{1};
  ref.assign_ranges(rs.begin(), rs.end());
  BOOST_CHECK_EQUAL(rs, ref);
}

BOOST_AUTO_TEST_CASE(merging1)
{
  // Ranges: [1,3) & [4,8)
  RangeSet rs1{2};
  rs1.emplace_range(1, 1, 3);
  rs1.emplace_range(1, 4, 8);
  rs1.collapse();
  BOOST_REQUIRE(rs1.has_disjoint_ranges());

  // Ranges: [3,4) & [8,11)
  RangeSet rs2{2};
  rs2.emplace_range(1, 3, 4);
  rs2.emplace_range(1, 8, 11);
  rs2.collapse();
  BOOST_REQUIRE(rs2.has_disjoint_ranges());

  rs1.merge(rs2); // Ranges: [1,3), [3,4), [4,8) & [8,11)
                  // collapse to: [1,11)
  BOOST_CHECK_EQUAL(rs1.ranges().size(), 1u);
  BOOST_CHECK_EQUAL(rs1.front().begin(), 1u);
  BOOST_CHECK_EQUAL(rs1.front().end(), 11u);
}

BOOST_AUTO_TEST_CASE(merging2)
{
  // Ranges: [1,3) & [4,8)
  RangeSet rs1{2};
  rs1.emplace_range(1, 1, 3);
  rs1.emplace_range(1, 4, 8);
  rs1.collapse();
  BOOST_REQUIRE(rs1.has_disjoint_ranges());

  // Ranges: [1,7)
  RangeSet rs2{2};
  rs2.emplace_range(1, 1, 7);
  rs2.collapse();
  BOOST_REQUIRE(rs2.has_disjoint_ranges());

  BOOST_CHECK(!art::disjoint_ranges(rs1, rs2));
  BOOST_CHECK_EXCEPTION(
    rs1.merge(rs2), art::Exception, [](art::Exception const& e) {
      return e.categoryCode() == art::errors::EventRangeOverlap;
    });
}

BOOST_AUTO_TEST_CASE(merging3)
{
  // Ranges: [1,3) & [4,8)
  RangeSet rs1{2};
  rs1.emplace_range(1, 1, 3);
  rs1.emplace_range(1, 4, 8);
  rs1.collapse();
  BOOST_REQUIRE(rs1.has_disjoint_ranges());

  // Ranges: [3,4)
  RangeSet rs2{2};
  rs2.emplace_range(1, 3, 4);
  rs2.collapse();
  BOOST_REQUIRE(rs2.has_disjoint_ranges());

  BOOST_REQUIRE(art::disjoint_ranges(rs1, rs2));
  rs1.merge(rs2);

  std::vector<EventRange> const ref_ranges{
    EventRange{1, 1, 8},
  };

  RangeSet const ref{2, ref_ranges};
  BOOST_CHECK_EQUAL(rs1, ref);
}

BOOST_AUTO_TEST_CASE(merging4)
{
  // Range: [1,3)
  RangeSet rs1{2};
  rs1.emplace_range(1, 1, 3);
  rs1.collapse();
  auto const ref = rs1;

  // Empty range
  RangeSet rs2{2};
  rs2.collapse();

  BOOST_REQUIRE(art::disjoint_ranges(rs1, rs2));
  rs1.merge(rs2);

  BOOST_CHECK_EQUAL(rs1, ref);
}

BOOST_AUTO_TEST_CASE(merging5)
{
  // Range for full run
  auto rs1 = RangeSet::forRun(RunID{2});
  auto const ref = rs1;

  // Empty range
  RangeSet rs2{2};
  rs2.collapse();

  BOOST_REQUIRE(art::disjoint_ranges(rs1, rs2));
  rs1.merge(rs2);

  BOOST_CHECK_EQUAL(rs1, ref);
}

BOOST_AUTO_TEST_CASE(merging6)
{
  // Test merging of empty event ranges
  RangeSet rs1{1};
  rs1.emplace_range(1, 2, 2);
  rs1.emplace_range(1, 2, 2);
  rs1.emplace_range(1, 2, 2);
  rs1.collapse(); // Should all collapse to (1, 2, 2).

  RangeSet const rs2{1, {EventRange{1, 2, 2}}};
  BOOST_CHECK_EQUAL(rs1, rs2);

  // Now merge the RangeSets and confirm that it equals rs2.
  rs1.merge(rs2);
  BOOST_CHECK_EQUAL(rs1, rs2);
}

BOOST_AUTO_TEST_CASE(overlapping_ranges)
{
  RangeSet rs1{2};
  rs1.emplace_range(1, 1, 3);
  rs1.emplace_range(1, 4, 8);
  rs1.emplace_range(2, 3, 17);
  rs1.collapse();

  RangeSet rs2{2};
  rs2.emplace_range(1, 4, 8);
  rs2.emplace_range(2, 7, 9);
  rs2.collapse();

  BOOST_CHECK(art::overlapping_ranges(rs1, rs2));
  BOOST_CHECK(art::overlapping_ranges(rs2, rs1));
}

BOOST_AUTO_TEST_CASE(invalid)
{
  auto const rs1 = RangeSet::invalid();
  auto const rs2 = RangeSet::invalid();
  BOOST_CHECK(!art::overlapping_ranges(rs1, rs2));
  BOOST_CHECK(!art::same_ranges(rs1, rs2));
  BOOST_CHECK(!art::disjoint_ranges(rs1, rs2));
}

BOOST_AUTO_TEST_SUITE_END()
