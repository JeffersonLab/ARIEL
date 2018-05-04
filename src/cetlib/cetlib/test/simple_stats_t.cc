#define BOOST_TEST_MODULE (simple_stats test)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/simple_stats.h"

#include <cmath>

using cet::simple_stats;

BOOST_AUTO_TEST_SUITE(simple_stats_test)

BOOST_AUTO_TEST_CASE(default_ctor_test)
{
  simple_stats s;

  BOOST_CHECK_EQUAL(s.size(), 0u);
  BOOST_CHECK(std::isinf(s.max()) && s.max() < 0.0);
  BOOST_CHECK(std::isinf(s.min()) && s.min() > 0.0);
  BOOST_CHECK(std::isinf(s.small()));
  BOOST_CHECK_EQUAL(s.sum(), 0.0);
  BOOST_CHECK_EQUAL(s.sumsq(), 0.0);

  BOOST_CHECK(std::isnan(s.mean()));
  BOOST_CHECK(std::isnan(s.rms()));
  BOOST_CHECK(std::isnan(s.rms0()));
}

BOOST_AUTO_TEST_CASE(value_ctor_test)
{
  double pi = 3.14;
  simple_stats s(pi);

  BOOST_CHECK_EQUAL(s.size(), 1u);
  BOOST_CHECK_EQUAL(s.max(), pi);
  BOOST_CHECK_EQUAL(s.min(), pi);
  BOOST_CHECK_EQUAL(s.small(), pi);
  BOOST_CHECK_EQUAL(s.sum(), pi);
  BOOST_CHECK_EQUAL(s.sumsq(), pi * pi);

  BOOST_CHECK_EQUAL(s.mean(), pi);
  BOOST_CHECK(std::isnan(s.rms()));
  BOOST_CHECK(std::isnan(s.rms0(1u)));
}

BOOST_AUTO_TEST_CASE(sample_test)
{
  double pi = 3.14;
  simple_stats s(pi);
  s.sample(-pi);

  BOOST_CHECK_EQUAL(s.size(), 2u);
  BOOST_CHECK_EQUAL(s.max(), pi);
  BOOST_CHECK_EQUAL(s.min(), -pi);
  BOOST_CHECK_EQUAL(s.small(), pi);
  BOOST_CHECK_EQUAL(s.sum(), 0.0);
  BOOST_CHECK_EQUAL(s.sumsq(), 2 * pi * pi);

  BOOST_CHECK_EQUAL(s.mean(), 0.0);
}

BOOST_AUTO_TEST_SUITE_END()
