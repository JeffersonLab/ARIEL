#define BOOST_TEST_MODULE (pow test)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/pow.h"

using cet::cube;
using cet::diff_of_squares;
using cet::fourth;
using cet::pow;
using cet::square;
using cet::sum_of_squares;

BOOST_AUTO_TEST_SUITE(pow_test)

BOOST_AUTO_TEST_CASE(zero)
{
  BOOST_CHECK(pow<1>(0) == 0);
  BOOST_CHECK(pow<2>(0u) == 0u);
  BOOST_CHECK(square(0.0) == 0.0);
  BOOST_CHECK(pow<3>(0.0F) == 0.0F);
  BOOST_CHECK(cube(0.0L) == 0.0L);
  BOOST_CHECK(pow<4>(0UL) == 0UL);
  BOOST_CHECK(fourth(0UL) == 0UL);
  BOOST_CHECK(pow<5>(0L) == 0L);
  BOOST_CHECK(pow<9>(0.0) == 0.0);
}

BOOST_AUTO_TEST_CASE(one)
{
  BOOST_CHECK(pow<0>(1) == 1);
  BOOST_CHECK(pow<1>(1) == 1);
  BOOST_CHECK(pow<2>(1u) == 1u);
  BOOST_CHECK(square(1.0) == 1.0);
  BOOST_CHECK(pow<3>(1.0F) == 1.0F);
  BOOST_CHECK(cube(1.0L) == 1.0L);
  BOOST_CHECK(pow<4>(1UL) == 1UL);
  BOOST_CHECK(fourth(1UL) == 1UL);
  BOOST_CHECK(pow<5>(1L) == 1L);
  BOOST_CHECK(pow<9>(1.0) == 1.0);
}

BOOST_AUTO_TEST_CASE(two)
{
  BOOST_CHECK(pow<0>(2) == 1);
  BOOST_CHECK(pow<1>(2) == 2);
  BOOST_CHECK(pow<2>(2u) == 4u);
  BOOST_CHECK(square(2.0) == 4.0);
  BOOST_CHECK(pow<3>(2.0F) == 8.0F);
  BOOST_CHECK(cube(2.0L) == 8.0L);
  BOOST_CHECK(pow<4>(2UL) == 16UL);
  BOOST_CHECK(fourth(2UL) == 16UL);
  BOOST_CHECK(pow<5>(2L) == 32L);
  BOOST_CHECK(pow<9>(2.0) == 512.0);
}

BOOST_AUTO_TEST_CASE(diffs)
{
  BOOST_CHECK(diff_of_squares(13, 12) == 25);
  BOOST_CHECK(diff_of_squares(13u, 12u) == 25u);
  BOOST_CHECK(diff_of_squares(13L, 12L) == 25L);
  BOOST_CHECK(diff_of_squares(13.0, 12.0) == 25.0);
  BOOST_CHECK(diff_of_squares(13.0F, 12.0F) == 25.0F);
  BOOST_CHECK(diff_of_squares(13.0L, 12.0L) == 25.0L);
}

BOOST_AUTO_TEST_CASE(sum2s)
{
  BOOST_CHECK(sum_of_squares(1, 2) == 5);
  BOOST_CHECK(sum_of_squares(1u, 2u) == 5u);
  BOOST_CHECK(sum_of_squares(1L, 2L) == 5L);
  BOOST_CHECK(sum_of_squares(1.0, 2.0) == 5.0);
  BOOST_CHECK(sum_of_squares(1.0F, 2.0F) == 5.0F);
  BOOST_CHECK(sum_of_squares(1.0L, 2.0L) == 5.0L);
}

BOOST_AUTO_TEST_CASE(sum3s)
{
  BOOST_CHECK(sum_of_squares(1, 2, 3) == 14);
  BOOST_CHECK(sum_of_squares(1u, 2u, 3u) == 14u);
  BOOST_CHECK(sum_of_squares(1L, 2L, 3L) == 14L);
  BOOST_CHECK(sum_of_squares(1.0, 2.0, 3.0) == 14.0);
  BOOST_CHECK(sum_of_squares(1.0F, 2.0F, 3.0F) == 14.0F);
  BOOST_CHECK(sum_of_squares(1.0L, 2.0L, 3.0L) == 14.0L);
}

BOOST_AUTO_TEST_SUITE_END()
