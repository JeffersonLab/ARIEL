#define BOOST_TEST_MODULE(canonical_string_t)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/canonical_string.h"

using namespace cet;

BOOST_AUTO_TEST_SUITE(canonical_string_t)

BOOST_AUTO_TEST_CASE(is_single_quoted_string_t)
{
  BOOST_TEST(is_single_quoted_string("'3'"));    // '3'
  BOOST_TEST(!is_single_quoted_string("\"3\"")); // "3"
  BOOST_TEST(!is_single_quoted_string("'3',"));  // '3',
  BOOST_TEST(!is_single_quoted_string(",'3'"));  // ,'3'
  BOOST_TEST(is_single_quoted_string("'\\\'"));  // '\\\'
}

BOOST_AUTO_TEST_CASE(is_double_quoted_string_t)
{
  BOOST_TEST(is_double_quoted_string("\"3\""));   // "3"
  BOOST_TEST(!is_double_quoted_string("'3'"));    // '3'
  BOOST_TEST(!is_double_quoted_string("\"3\",")); // "3",
  BOOST_TEST(!is_double_quoted_string(",\"3\"")); // ,"3"
  // Controversial:
  BOOST_TEST(is_double_quoted_string("\"\\\"")); // "\"
}

void
becomes(std::string const& input, std::string const& wanted)
{
  std::string result;
  BOOST_TEST(cet::canonical_string(input, result),
             "canonical_result returns false on input: '" << input << "'");
  BOOST_TEST(result == wanted);
}

std::string
dquoted(std::string const& s)
{
  return '"' + s + '"';
}

std::string
squoted(std::string const& s)
{
  return '\'' + s + '\'';
}

BOOST_AUTO_TEST_CASE(canonical_string_t)
{
  becomes("a", dquoted("a"));
  becomes(squoted("a"), dquoted("a"));
  becomes(dquoted("a"), dquoted("a"));
  {
    std::string result;
    BOOST_TEST(!canonical_string("", result),
               "canonical_string() should return false on empty input.");
  }
  becomes(squoted("\n"), dquoted("\\n"));
  becomes(dquoted("\n"), dquoted("\\n"));
  becomes(squoted("\\"), dquoted("\\\\"));
  becomes(dquoted("\\"), dquoted("\\\\"));
  becomes(squoted("\\\\"), dquoted("\\\\\\\\"));
  becomes(dquoted("\\\\"), dquoted("\\\\"));

  becomes(squoted("\t\'\""), dquoted("\\t\\\'\\\""));
  becomes(dquoted("\t\'\""), dquoted("\\t\\\'\\\""));
}

BOOST_AUTO_TEST_SUITE_END()
