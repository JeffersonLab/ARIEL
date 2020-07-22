// ======================================================================
//
// no_delete test
//
// ======================================================================

#define BOOST_TEST_MODULE (no_delete test)
#include "cetlib/no_delete.h"
#include "cetlib/quiet_unit_test.hpp"

#include <iostream>
#include <memory>

BOOST_AUTO_TEST_SUITE(no_delete_test)

BOOST_AUTO_TEST_CASE(no_delete_t)
{
  std::ostringstream oss1, oss2;
  auto* ossp1 = &oss1;
  auto* ossp2 = &oss2;
  {
    std::shared_ptr<std::ostream>{&oss1, cet::no_delete()};
    std::unique_ptr<std::ostream, cet::no_delete>{&oss2};
  }
  BOOST_CHECK_EQUAL(ossp1, &oss1);
  BOOST_CHECK_EQUAL(ossp2, &oss2);
}

BOOST_AUTO_TEST_SUITE_END()
