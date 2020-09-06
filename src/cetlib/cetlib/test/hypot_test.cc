#include "catch2/catch.hpp"

#include <cmath>
#include <limits>

#include "cetlib/hypot.h"

using cet::hypot;

TEST_CASE("Basic test")
{
  CHECK(hypot(5, 12) == 13);
  CHECK(hypot(.5, 1.2) == Approx(1.3).epsilon(0.0001));
  CHECK(hypot(.05L, .12L) == Approx(.13L).epsilon(0.0001L));
}

TEST_CASE("NaN test")
{
  CHECK(std::isnan(hypot(std::numeric_limits<double>::quiet_NaN(), 1.2)));
  CHECK(std::isnan(hypot(3.1415926, std::numeric_limits<double>::quiet_NaN())));
}

TEST_CASE("Inf test")
{
  CHECK(std::isinf(hypot(std::numeric_limits<double>::infinity(), 1.2)));
  CHECK(std::isinf(hypot(3.1415926, std::numeric_limits<double>::infinity())));

  CHECK(std::isinf(hypot(-std::numeric_limits<double>::infinity(), 1.2)));
  CHECK(std::isinf(hypot(3.1415926, -std::numeric_limits<double>::infinity())));
}
