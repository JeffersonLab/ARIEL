// ======================================================================
//
// split_by_regex test
//
// ======================================================================

#define BOOST_TEST_MODULE (split_by_regex test)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/split_by_regex.h"
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <vector>

using cet::split_by_regex;
using string_vector = std::vector<std::string>;

BOOST_AUTO_TEST_SUITE(split_by_regex_test)

BOOST_AUTO_TEST_CASE(empty_string_test)
{
  string_vector const v = split_by_regex("", ":");
  BOOST_TEST(v.size() == 1ul);
}

BOOST_AUTO_TEST_CASE(a_string_test)
{
  string_vector const v = split_by_regex("a", ":");
  BOOST_TEST(v.size() == 1ul);
  BOOST_TEST(v[0] == "a");
}

BOOST_AUTO_TEST_CASE(a1_string_test)
{
  string_vector const v = split_by_regex("a:", ":");
  BOOST_TEST(v.size() == 1ul);
  BOOST_TEST(v[0] == "a");
}

BOOST_AUTO_TEST_CASE(boo_string_test)
{
  string_vector const v = split_by_regex(":boo", ":");
  string_vector const expected{"", "boo"};
  BOOST_TEST(v == expected);
}

BOOST_AUTO_TEST_CASE(ab_string_test)
{
  string_vector const v = split_by_regex("a:b", ":");
  string_vector const expected{"a", "b"};
  BOOST_TEST(v == expected);
}

BOOST_AUTO_TEST_CASE(ab1_string_test)
{
  string_vector const v = split_by_regex("a::b", ":");
  string_vector const expected{"a", "", "b"};
  BOOST_TEST(v == expected);
}

BOOST_AUTO_TEST_CASE(split_test_1)
{
  string_vector const v = split_by_regex("abc:d:ef:", ":");
  string_vector const expected{"abc", "d", "ef"};
  BOOST_TEST(v == expected);
}

BOOST_AUTO_TEST_CASE(split_test_2)
{
  string_vector const v = split_by_regex(
    "namespace::class::static_function().value", R"((::|\(\)\.))");
  string_vector const expected{
    "namespace", "class", "static_function", "value"};
  BOOST_TEST(v == expected);
}

BOOST_AUTO_TEST_CASE(variable_subscript)
{
  string_vector const v = split_by_regex("v[0][1][2]", R"((\]\[|\[|\]))");
  string_vector const expected{"v", "0", "1", "2"};
  BOOST_TEST(v == expected);
}

BOOST_AUTO_TEST_SUITE_END()
