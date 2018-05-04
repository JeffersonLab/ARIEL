#define BOOST_TEST_MODULE (maybe_ref test)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/column_width.h"
typedef std::vector<std::string> column;

BOOST_AUTO_TEST_SUITE(column_width)

BOOST_AUTO_TEST_CASE(default_behavior)
{
  column c;
  BOOST_CHECK(cet::column_width(c) == 0u);
  c.push_back("abc");
  c.push_back("abc");
  c.push_back("ab");
  c.push_back("a");
  c.push_back("ab");
  c.push_back("abc");
  BOOST_CHECK(cet::column_width(c) == 3u);
  c.insert(c.begin(), "wxyz");
  BOOST_CHECK(cet::column_width(c) == 4u);
  c.push_back("lmnop");
  BOOST_CHECK(cet::column_width(c) == 5u);
}

BOOST_AUTO_TEST_SUITE_END()
