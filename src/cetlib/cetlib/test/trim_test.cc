#define BOOST_TEST_MODULE (trim test)

#include "cetlib/quiet_unit_test.hpp"
#include "cetlib/trim.h"

using cet::trim;
using cet::trim_left;
using cet::trim_right;

using cet::trim_copy;
using cet::trim_left_copy;
using cet::trim_right_copy;

using std::string;

BOOST_AUTO_TEST_SUITE(trim_test)

BOOST_AUTO_TEST_CASE(trimmed_test)
{
  string s1("Hello.");

  {
    string s2(s1);
    BOOST_TEST(trim_left(s2) == s1);
  }
  {
    string s2(s1);
    BOOST_TEST(trim_right(s2) == s1);
  }
  {
    string s2(s1);
    BOOST_TEST(trim(s2) == s1);
  }

  {
    string s2(s1);
    BOOST_TEST(trim_left_copy(s2) == s1);
  }
  {
    string s2(s1);
    BOOST_TEST(trim_right_copy(s2) == s1);
  }
  {
    string s2(s1);
    BOOST_TEST(trim_copy(s2) == s1);
  }
}

BOOST_AUTO_TEST_SUITE_END()
