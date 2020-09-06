// ======================================================================
//
// test parse_value_string()
//
// ======================================================================

#define BOOST_TEST_MODULE (document test)
#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/parse.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

using fhicl::extended_value;
using fhicl::parse_value_string;
using std::pair;
using std::string;

namespace {
  string const nil(9, '\0');
  string const failed(8, '\0');

  std::string
  parse_as(string const& input)
  {
    extended_value result;
    string unparsed;
    return parse_value_string(input, result, unparsed) ? result.to_string() :
                                                         failed;
  }

  string
  dquoted(string s)
  {
    return '"' + s + '"';
  }

  string
  squoted(string s)
  {
    return '\'' + s + '\'';
  }
}

BOOST_AUTO_TEST_SUITE(document_test)

BOOST_AUTO_TEST_CASE(symbols)
{
  BOOST_CHECK(parse_as("@nil") == nil);
  BOOST_CHECK(parse_as(" @nil") == nil);
  BOOST_CHECK(parse_as(" @nil ") == nil);
  BOOST_CHECK(parse_as(" true or false ") == failed);
  BOOST_CHECK(parse_as(" TRUE or FALSE ") == failed);
  BOOST_CHECK(parse_as("infinity") == "+infinity");
  BOOST_CHECK(parse_as(" -infinity") == "-infinity");
  BOOST_CHECK(parse_as("+ infinity") == failed);
  BOOST_CHECK(parse_as("nil_") == dquoted("nil_"));
}

BOOST_AUTO_TEST_CASE(numbers)
{
  BOOST_CHECK(parse_as("0") == "0");
  BOOST_CHECK(parse_as("000") == "0");
  BOOST_CHECK(parse_as("123456") == "123456");
  BOOST_CHECK(parse_as("1234567") == "1.234567e6");
  BOOST_CHECK(parse_as("0.0") == "0");
  BOOST_CHECK(parse_as("0.") == "0");

  BOOST_CHECK(parse_as("+0") == "0");
  BOOST_CHECK(parse_as("+000") == "0");
  BOOST_CHECK(parse_as("+123456") == "123456");
  BOOST_CHECK(parse_as("+1234567") == "1.234567e6");
  BOOST_CHECK(parse_as("-0") == "0");
  BOOST_CHECK(parse_as("-000") == "0");
  BOOST_CHECK(parse_as("-123456") == "-123456");
  BOOST_CHECK(parse_as("-1234567") == "-1.234567e6");
  BOOST_CHECK(parse_as("1.23e-4") == "1.23e-4");

  BOOST_CHECK(parse_as("+0.0E0") == "0");
  BOOST_CHECK(parse_as("+00.") == "0");
  BOOST_CHECK(parse_as("-0e99") == "0");
  BOOST_CHECK(parse_as("-.00") == "0");
  BOOST_CHECK(parse_as("+123456e0") == "123456");
  BOOST_CHECK(parse_as("+1234567e-0") == "1.234567e6");
}

BOOST_AUTO_TEST_CASE(everything_else)
{
  BOOST_CHECK(parse_as("Hello") == dquoted("Hello"));
  BOOST_CHECK(parse_as("_123") == dquoted("_123"));
  BOOST_CHECK(parse_as("_123Aa_") == dquoted("_123Aa_"));
  BOOST_CHECK(parse_as("X...") == failed);
  BOOST_CHECK(parse_as("3a4B5c") == dquoted("3a4B5c"));
  BOOST_CHECK(parse_as("0123Aa_") == dquoted("0123Aa_"));
  BOOST_CHECK(parse_as("9...") == failed);
  BOOST_CHECK(parse_as("nil") == dquoted("nil"));
  BOOST_CHECK(parse_as(R"($\d+^)") == failed);

  BOOST_CHECK(parse_as(" ( 1 , 2.3 ) ") == "(1,2.3)");
  BOOST_CHECK(parse_as(" ( infinity , -infinity ) ") ==
              "(+infinity,-infinity)");
  BOOST_CHECK(parse_as(dquoted("Hi, there!")) == dquoted("Hi, there!"));
  BOOST_CHECK(parse_as(dquoted("\\t\\n")) == dquoted("\\t\\n"));
  BOOST_CHECK(parse_as(squoted("Hi, there!")) == dquoted("Hi, there!"));
  BOOST_CHECK(parse_as(squoted("\\t\\n")) == dquoted("\\\\t\\\\n"));
  BOOST_CHECK(parse_as("infinityx") == dquoted("infinityx"));
  BOOST_CHECK(parse_as(dquoted("Hi, \\\"Tom\\\"")) ==
              dquoted("Hi, \\\"Tom\\\""));
  BOOST_CHECK(parse_as(dquoted("@Nil")) == dquoted("@Nil"));

  BOOST_CHECK(parse_as(" [1,2.3] ") == "[1,2.3]");
  BOOST_CHECK(parse_as(" [ 1 , 2.3 ] ") == "[1,2.3]");
  BOOST_CHECK(parse_as(" [hello,there] ") ==
              "[" + dquoted("hello") + "," + dquoted("there") + "]");
  BOOST_CHECK(parse_as("[ hello,\nthere\t] ") ==
              "[" + dquoted("hello") + "," + dquoted("there") + "]");
  BOOST_CHECK(parse_as(" [ ] ") == "[]");

  BOOST_CHECK(parse_as("{ }") == "{}");
  BOOST_CHECK(parse_as("{a : 1.2 }") == "{a:1.2}");
  BOOST_CHECK(parse_as("{a : 1.2 b: hello}") ==
              "{a:1.2 b:" + dquoted("hello") + "}");
  BOOST_CHECK(parse_as("{a : 3 b: 7 a: @erase}") == "{b:7}");
}

BOOST_AUTO_TEST_SUITE_END()
