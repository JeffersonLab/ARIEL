#define BOOST_TEST_MODULE (replace_all_test)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/replace_all.h"

#include <string>

using cet::replace_all;
using std::string;

namespace {
  void
  testit(std::string start,
         std::string const find,
         std::string const replace,
         std::string const ref)
  {
    replace_all(start, find, replace);
    BOOST_CHECK_EQUAL(start, ref);
  }
}

BOOST_AUTO_TEST_SUITE(replace_all_test)

BOOST_AUTO_TEST_CASE(replace_simple)
{
  testit("Once upon a time", "a", "some", "Once upon some time");
  testit("Once upon a time", " ", "_", "Once_upon_a_time");
  testit("Once upon a time", "time", "song", "Once upon a song");
}

BOOST_AUTO_TEST_CASE(no_replace)
{
  testit("Once upon a time", "z", "y", "Once upon a time");
}

BOOST_AUTO_TEST_CASE(edge_replace)
{
  testit("Once upon a time", "O", "o", "once upon a time");
  testit("Once upon a timE", "E", "e", "Once upon a time");
  testit("Once upuponon a time", "upon", "", "Once upon a time");
}

BOOST_AUTO_TEST_CASE(repeat_replace)
{
  testit("Once uponuponupon a time", "upon", "-", "Once --- a time");
  testit("Once --- a time", "-", "z", "Once zzz a time");
  testit("Once --- a time", "-", "upon", "Once uponuponupon a time");
  testit("Once -------- a time", "---", "upon", "Once uponupon-- a time");
}

BOOST_AUTO_TEST_CASE(substring_replace)
{
  testit("Once upon a time", "upon", "uponst", "Once uponst a time");
  testit("Once upon a time", "upon", "stupon", "Once stupon a time");
}

BOOST_AUTO_TEST_SUITE_END()
