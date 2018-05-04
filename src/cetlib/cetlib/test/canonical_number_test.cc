#include "catch/catch.hpp"

#include "cetlib/canonical_number.h"

#include <cstdlib>
#include <iostream>
#include <string>

using cet::canonical_number;

namespace {
  bool
  becomes(std::string const& input, std::string const& wanted)
  {
    std::string result;
    return canonical_number(input, result) && result == wanted;
  }
}

TEST_CASE("Conversion of string representations of numbers to canonical "
          "representations.")
{
  CHECK(becomes("0", "0"));
  CHECK(becomes("+0", "0"));
  CHECK(becomes("-0", "0"));
  CHECK(becomes("000", "0"));
  CHECK(becomes("+000", "0"));
  CHECK(becomes("-000", "0"));
  CHECK(becomes("0.0", "0"));
  CHECK(becomes("+0.0", "0"));
  CHECK(becomes("-0.0e-5", "0"));

  CHECK(becomes("123456", "123456"));
  CHECK(becomes("12345678", "1.2345678e7"));
  CHECK(!becomes("", ""));
  CHECK(becomes(".12", "1.2e-1"));
  CHECK(becomes("12E4", "120000"));
  CHECK(becomes("12E5", "1.2e6"));
  CHECK(becomes("12.000E5", "1.2e6"));
  CHECK(becomes("12E-4", "1.2e-3"));
  CHECK(becomes("0.09", "9e-2"));

  CHECK(becomes("-0.0e-0", "0"));
  CHECK(becomes("+0.0E0", "0"));
  CHECK(becomes("-0e99", "0"));
  CHECK(becomes("-.00", "0"));

  CHECK(!becomes("1.2.3", ""));
  CHECK(!becomes("1.2, 3", ""));
  CHECK(!becomes("1.2X3", ""));
  CHECK(!becomes("0x123abx", ""));
  CHECK(!becomes("+0x123ab", ""));
  CHECK(!becomes("0x", ""));
  CHECK(!becomes("0b123", ""));
  CHECK(!becomes("+0b101", ""));
  CHECK(!becomes("0b", ""));

  CHECK(becomes("0X000", "0"));
  CHECK(becomes("0x123ab", "74667"));
  CHECK(becomes("0x123DE", "74718"));
  CHECK(becomes("0X123ab", "74667"));
  CHECK(becomes("0X00123De", "74718"));
  CHECK(becomes("0x1234567890", "7.818749352e10"));

  CHECK(becomes("0b000", "0"));
  CHECK(becomes("0b101", "5"));
  CHECK(becomes("0b01101", "13"));
  CHECK(becomes("0b1000110100101100011001", "2.312985e6"));
}
