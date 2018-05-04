// ======================================================================
//
// test parse_value_string()
//
// ======================================================================

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

void
ensure(int which, bool claim)
{
  if (not claim)
    std::exit(which);
}

bool
parse_as(string const& input, string const& wanted)
{
  extended_value result;
  string unparsed;
  if (!parse_value_string(input, result, unparsed)) {
    std::cerr << "Input:    \"" << input << "\"\n"
              << "Wanted:   \"" << wanted << "\"\n"
              << "Unparsed: \"" << unparsed << "\"\n";
    return false;
  }

  std::string got = result.to_string();
  if (got == wanted)
    return true;

  std::cerr << "Input:    \"" << input << "\"\n"
            << "Wanted:   \"" << wanted << "\"\n"
            << "Got:      \"" << got << "\"\n"
            << "Unparsed: \"" << unparsed << "\"\n";
  return false;
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

string
nil()
{
  return string(9, '\0');
}

int
main()
{
  ensure(1, parse_as("@nil", nil()));
  ensure(2, parse_as(" @nil", nil()));
  ensure(3, parse_as(" @nil ", nil()));
  ensure(4, !parse_as(" true or false ", "true"));
  ensure(5, !parse_as(" TRUE or FALSE ", dquoted("TRUE")));
  ensure(6, parse_as("infinity", "+infinity"));
  ensure(7, parse_as(" -infinity", "-infinity"));
  ensure(8, !parse_as("+ infinity", "+infinity"));
  ensure(9, parse_as("nil_", dquoted("nil_")));

  ensure(11, parse_as("0", "0"));
  ensure(12, parse_as("000", "0"));
  ensure(13, parse_as("123456", "123456"));
  ensure(14, parse_as("1234567", "1.234567e6"));
  ensure(15, parse_as("0.0", "0"));
  ensure(16, parse_as("0.", "0"));

  ensure(21, parse_as("+0", "0"));
  ensure(22, parse_as("+000", "0"));
  ensure(23, parse_as("+123456", "123456"));
  ensure(24, parse_as("+1234567", "1.234567e6"));
  ensure(25, parse_as("-0", "0"));
  ensure(26, parse_as("-000", "0"));
  ensure(27, parse_as("-123456", "-123456"));
  ensure(28, parse_as("-1234567", "-1.234567e6"));
  ensure(29, parse_as("1.23e-4", "1.23e-4"));

  ensure(31, parse_as("+0.0E0", "0"));
  ensure(32, parse_as("+00.", "0"));
  ensure(33, parse_as("-0e99", "0"));
  ensure(34, parse_as("-.00", "0"));
  ensure(35, parse_as("+123456e0", "123456"));
  ensure(36, parse_as("+1234567e-0", "1.234567e6"));

  ensure(41, parse_as("Hello", dquoted("Hello")));
  ensure(42, parse_as("_123", dquoted("_123")));
  ensure(43, parse_as("_123Aa_", dquoted("_123Aa_")));
  ensure(44, !parse_as("X...", dquoted("X")));
  ensure(45, parse_as("3a4B5c", dquoted("3a4B5c")));
  ensure(46, parse_as("0123Aa_", dquoted("0123Aa_")));
  ensure(47, !parse_as("9...", dquoted("9...")));
  ensure(48, parse_as("nil", dquoted("nil")));

  ensure(51, parse_as(" ( 1 , 2.3 ) ", "(1,2.3)"));
  ensure(52, parse_as(" ( infinity , -infinity ) ", "(+infinity,-infinity)"));
  ensure(53, parse_as(dquoted("Hi, there!"), dquoted("Hi, there!")));
  ensure(54, parse_as(dquoted("\\t\\n"), dquoted("\\t\\n")));
  ensure(55, parse_as(squoted("Hi, there!"), dquoted("Hi, there!")));
  ensure(56, parse_as(squoted("\\t\\n"), dquoted("\\\\t\\\\n")));
  ensure(57, parse_as("infinityx", dquoted("infinityx")));
  ensure(58, parse_as(dquoted("Hi, \\\"Tom\\\""), dquoted("Hi, \\\"Tom\\\"")));
  ensure(59, parse_as(dquoted("@Nil"), dquoted("@Nil")));

  ensure(61, parse_as(" [1,2.3] ", "[1,2.3]"));
  ensure(62, parse_as(" [ 1 , 2.3 ] ", "[1,2.3]"));
  ensure(63,
         parse_as(" [hello,there] ",
                  "[" + dquoted("hello") + "," + dquoted("there") + "]"));
  ensure(64,
         parse_as("[ hello,\nthere\t] ",
                  "[" + dquoted("hello") + "," + dquoted("there") + "]"));
  ensure(65, parse_as(" [ ] ", "[]"));

  ensure(71, parse_as("{ }", "{}"));
  ensure(72, parse_as("{a : 1.2 }", "{a:1.2}"));
  ensure(73,
         parse_as("{a : 1.2 b: hello}", "{a:1.2 b:" + dquoted("hello") + "}"));
  ensure(74, parse_as("{a : 3 b: 7 a: @erase}", "{b:7}"));
  return 0;

} // main()
