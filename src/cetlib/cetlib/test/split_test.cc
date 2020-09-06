// ======================================================================
//
// split test
//
// ======================================================================

#define BOOST_TEST_MODULE (split test)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/split.h"
#include <iterator>
#include <string>
#include <vector>

using cet::split;
using string_vector = std::vector<std::string>;

BOOST_AUTO_TEST_SUITE(split_test)

BOOST_AUTO_TEST_CASE(empty_string_test)
{
  std::string s;
  std::vector<std::string> v;
  split(s, ':', std::back_inserter(v));
  BOOST_TEST(v.size() == 0ul);
}

BOOST_AUTO_TEST_CASE(a_string_test)
{
  std::string s("a");
  std::vector<std::string> v;
  split(s, ':', std::back_inserter(v));
  auto ref = {"a"};
  BOOST_TEST(v == ref, boost::test_tools::per_element{});
}

BOOST_AUTO_TEST_CASE(a1_string_test)
{
  std::string s("a:");
  std::vector<std::string> v;
  split(s, ':', std::back_inserter(v));
  auto ref = {"a"};
  BOOST_TEST(v == ref, boost::test_tools::per_element{});
}

BOOST_AUTO_TEST_CASE(boo_string_test)
{
  std::string s(":boo");
  std::vector<std::string> v;
  split(s, ':', std::back_inserter(v));
  auto ref = {"boo"};
  BOOST_TEST(v == ref, boost::test_tools::per_element{});
}

BOOST_AUTO_TEST_CASE(ab_string_test)
{
  std::string s("a:b");
  std::vector<std::string> v;
  split(s, ':', std::back_inserter(v));
  string_vector const expected{"a", "b"};
  BOOST_TEST(v == expected);
}

BOOST_AUTO_TEST_CASE(ab1_string_test)
{
  std::string s("a::b");
  std::vector<std::string> v;
  split(s, ':', std::back_inserter(v));
  string_vector const expected{"a", "b"};
  BOOST_TEST(v == expected);
}

BOOST_AUTO_TEST_CASE(split_test_1)
{
  std::string const s("abc:d::ef");
  std::vector<std::string> v;
  split(s, ':', std::back_inserter(v));
  string_vector const expected{"abc", "d", "ef"};
  BOOST_TEST(v == expected);
}

BOOST_AUTO_TEST_SUITE_END()
