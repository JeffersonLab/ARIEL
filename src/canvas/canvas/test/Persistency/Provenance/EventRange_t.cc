#define BOOST_TEST_MODULE (EventRange_t)
#include "canvas/Persistency/Provenance/EventRange.h"
#include "cetlib/quiet_unit_test.hpp"

#include <iostream>
#include <sstream>
#include <string>

using art::EventRange;
using namespace std::string_literals;

// The EventRange class has member functions with names that are
// indicative of a collection.  To disable collection comparison, we
// specialize the following Boost template.  This suggests that the
// EventRange interface should be adjusted.

namespace boost::unit_test {
  template <>
  struct is_forward_iterable<EventRange> {};
}

BOOST_AUTO_TEST_SUITE(EventRange_t)

BOOST_AUTO_TEST_CASE(fullSubRun)
{
  auto er = EventRange::forSubRun(134);
  BOOST_TEST(er.is_valid());
  BOOST_TEST(er.is_full_subRun());
  BOOST_TEST(er.contains(er.subRun(), 17));
  std::ostringstream oss;
  oss << er;
  BOOST_TEST(oss.str() == "SubRun: 134 (full sub-run)"s);
}

BOOST_AUTO_TEST_CASE(merge1)
{
  EventRange er1{1, 2, 4};
  EventRange er2{1, 3, 6};
  BOOST_TEST(!er1.merge(er2));
  BOOST_TEST(!er1.is_disjoint(er2));
  BOOST_TEST(er1.is_valid());
  BOOST_TEST(!er1.is_full_subRun());
}

BOOST_AUTO_TEST_CASE(merge2)
{
  EventRange er1{1, 2, 4};
  EventRange const er2{1, 4, 6};
  BOOST_TEST(er1.is_adjacent(er2));
  BOOST_TEST(er1.is_disjoint(er2));
  BOOST_TEST(er1.merge(er2));

  EventRange const ref{1, 2, 6};
  BOOST_TEST(er1 == ref);
  std::ostringstream oss;
  oss << ref;
  BOOST_TEST(oss.str() == "SubRun: 1 Event range: [2,6)"s);

  BOOST_TEST(er1.contains(1, 2));
  BOOST_TEST(er1.contains(1, 4));
  BOOST_TEST(!er1.contains(1, 6));
  BOOST_TEST(!er1.contains(2, 4));
}

BOOST_AUTO_TEST_CASE(lessThan)
{
  EventRange const er0{1, 1, 6};
  EventRange const er1{1, 2, 4};
  EventRange const er2{1, 2, 5};
  EventRange const er3{2, 2, 5};
  EventRange const er4{2, 6, 9};
  BOOST_TEST(er0 < er1);
  BOOST_TEST(er1 < er2);
  BOOST_TEST(er2 < er3);
  BOOST_TEST(er0.is_disjoint(er3));
  BOOST_TEST(!er0.is_disjoint(er1));
  BOOST_TEST(er3.is_disjoint(er4));
}

BOOST_AUTO_TEST_CASE(traits1)
{
  EventRange const er0{1, 1, 6};
  EventRange const er1{1, 2, 4};
  BOOST_TEST(er1.is_subset(er0));
  BOOST_TEST(!er1.is_superset(er0));
  BOOST_TEST(!er0.is_subset(er1));
  BOOST_TEST(er0.is_superset(er1));
  BOOST_TEST(!er1.is_overlapping(er0));
  BOOST_TEST(!er0.is_overlapping(er1));
}

BOOST_AUTO_TEST_CASE(traits2)
{
  EventRange const er0{1, 1, 6};
  EventRange const er1{1, 2, 8};
  BOOST_TEST(!er1.is_subset(er0));
  BOOST_TEST(!er1.is_superset(er0));
  BOOST_TEST(!er0.is_subset(er1));
  BOOST_TEST(!er0.is_superset(er1));
  BOOST_TEST(er1.is_overlapping(er0));
  BOOST_TEST(er0.is_overlapping(er1));
}

BOOST_AUTO_TEST_SUITE_END()
