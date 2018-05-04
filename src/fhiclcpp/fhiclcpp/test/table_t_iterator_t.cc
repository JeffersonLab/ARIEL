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

BOOST_AUTO_TEST_SUITE(table_t_iterator_t)

BOOST_AUTO_TEST_CASE(iterator_comparisons_1)
{
  BOOST_CHECK(::shims::isSnippetMode(SNIPPET_MODE) == SNIPPET_MODE);

  using table_t = fhicl::extended_value::table_t;

  table_t table;
  table_t const& ctable = table;
  BOOST_CHECK(table.cbegin() == table.cend());
  BOOST_CHECK(table.cbegin() == table.end());
  BOOST_CHECK(table.begin() == table.end());
  BOOST_CHECK(table.begin() == table.cend());
  BOOST_CHECK(ctable.begin() == ctable.end());
  BOOST_CHECK(table.begin() == table.end());
  BOOST_CHECK(ctable.begin() == table.end());
  BOOST_CHECK(table.begin() == ctable.end());
  table.emplace("string1", extended_value{false, STRING, "string"});
  BOOST_CHECK(table.cbegin() != table.cend());
  BOOST_CHECK(table.cbegin() != table.end());
  BOOST_CHECK(table.begin() != table.end());
  BOOST_CHECK(table.begin() != table.cend());
  BOOST_CHECK(ctable.begin() != ctable.end());
  BOOST_CHECK(table.begin() != table.end());
  BOOST_CHECK(ctable.begin() != table.end());
  BOOST_CHECK(table.begin() != ctable.end());
}
BOOST_AUTO_TEST_SUITE_END()
