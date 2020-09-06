#define BOOST_TEST_MODULE (table_t_iterator_t)
#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/make_ParameterSet.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace fhicl;
using table_t = fhicl::extended_value::table_t;

BOOST_TEST_DONT_PRINT_LOG_VALUE(table_t::const_iterator)
BOOST_TEST_DONT_PRINT_LOG_VALUE(table_t::iterator)

BOOST_AUTO_TEST_SUITE(table_t_iterator_t)

BOOST_AUTO_TEST_CASE(iterator_comparisons_1)
{
  BOOST_TEST(::shims::isSnippetMode(SNIPPET_MODE) == SNIPPET_MODE);

  table_t table;
  table_t const& ctable = table;
  BOOST_TEST(table.cbegin() == table.cend());
  BOOST_TEST(table.cbegin() == table.end());
  BOOST_TEST(table.begin() == table.end());
  BOOST_TEST(table.begin() == table.cend());
  BOOST_TEST(ctable.begin() == ctable.end());
  BOOST_TEST(table.begin() == table.end());
  BOOST_TEST(ctable.begin() == table.end());
  BOOST_TEST(table.begin() == ctable.end());
  table.emplace("string1", extended_value{false, STRING, "string"});
  BOOST_TEST(table.cbegin() != table.cend());
  BOOST_TEST(table.cbegin() != table.end());
  BOOST_TEST(table.begin() != table.end());
  BOOST_TEST(table.begin() != table.cend());
  BOOST_TEST(ctable.begin() != ctable.end());
  BOOST_TEST(table.begin() != table.end());
  BOOST_TEST(ctable.begin() != table.end());
  BOOST_TEST(table.begin() != ctable.end());
}
BOOST_AUTO_TEST_SUITE_END()
