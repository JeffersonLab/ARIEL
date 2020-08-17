#define BOOST_TEST_MODULE (shlib_utils_t)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/shlib_utils.h"

#include <cassert>

BOOST_AUTO_TEST_SUITE(shlib_utils_t)

BOOST_AUTO_TEST_CASE(prefix)
{
  BOOST_TEST_REQUIRE(cet::shlib_prefix() == std::string("lib"));
}

BOOST_AUTO_TEST_CASE(suffix)
{
#if __APPLE__ && __MACH__
  BOOST_TEST_REQUIRE(cet::shlib_suffix() == std::string(".dylib"));
#elif __linux__
  BOOST_TEST_REQUIRE(cet::shlib_suffix() == std::string(".so"));
#else
#error "Unrecognized architecture!"
#endif
}

BOOST_AUTO_TEST_SUITE_END()
