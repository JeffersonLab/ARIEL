#define BOOST_TEST_MODULE (lpad test)

#include "cetlib/lpad.h"
#include "cetlib/quiet_unit_test.hpp"
#include <string>

BOOST_AUTO_TEST_SUITE(lpad_test)

BOOST_AUTO_TEST_CASE(dont_pad_test)
{
  std::string const s0("abc");

  std::string s1 = cet::lpad(s0, 0, '*');
  BOOST_CHECK(s1 == s0);

  std::string s2 = cet::lpad(s0, 1, '*');
  BOOST_CHECK(s2 == s0);

  std::string s3 = cet::lpad(s0, 2, '*');
  BOOST_CHECK(s3 == s0);

  std::string s4 = cet::lpad(s0, 3, '*');
  BOOST_CHECK(s4 == s0);
}

BOOST_AUTO_TEST_CASE(do_pad_test)
{
  std::string const s0("abc");

  std::string s1 = cet::lpad(s0, 4);
  BOOST_CHECK(s1 == ' ' + s0);

  std::string s2 = cet::lpad(s0, 5, '*');
  BOOST_CHECK(s2 == std::string(2, '*') + s0);
}

BOOST_AUTO_TEST_SUITE_END()
