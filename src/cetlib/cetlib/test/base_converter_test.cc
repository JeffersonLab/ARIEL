#include "cetlib/base_converter.h"
#include <cstdlib>
#include <iostream>
#include <string>

using cet::base_converter;

void
ensure(int which, bool claim)
{
  if (not claim)
    std::exit(which);
}

bool
hex2dec(std::string const& input, std::string const& wanted)
{
  std::string result;

  try {
    result = base_converter::hex_to_dec(input);
  }
  catch (...) {
    return false;
  }

  if (result == wanted)
    return true;

  std::cerr << "Input:  " << input << '\n'
            << "Wanted: " << wanted << '\n'
            << "Result: " << result << '\n';
  return false;
}

bool
dec2hex(std::string const& input, std::string const& wanted)
{
  std::string result;

  try {
    result = base_converter::dec_to_hex(input);
  }
  catch (...) {
    return false;
  }

  if (result == wanted)
    return true;

  std::cerr << "Input:  " << input << '\n'
            << "Wanted: " << wanted << '\n'
            << "Result: " << result << '\n';
  return false;
}

bool
bin2dec(std::string const& input, std::string const& wanted)
{
  std::string result;

  try {
    result = base_converter::bin_to_dec(input);
  }
  catch (...) {
    return false;
  }

  if (result == wanted)
    return true;

  std::cerr << "Input:  " << input << '\n'
            << "Wanted: " << wanted << '\n'
            << "Result: " << result << '\n';
  return false;
}

bool
dec2bin(std::string const& input, std::string const& wanted)
{
  std::string result;

  try {
    result = base_converter::dec_to_bin(input);
  }
  catch (...) {
    return false;
  }

  if (result == wanted)
    return true;

  std::cerr << "Input:  " << input << '\n'
            << "Wanted: " << wanted << '\n'
            << "Result: " << result << '\n';
  return false;
}

bool
any2any(std::string const& source_set,
        std::string const& target_set,
        std::string const& input,
        std::string const& wanted)
{
  std::string result;

  try {
    base_converter converter(source_set, target_set);
    result = converter.convert(input);
  }
  catch (...) {
    return false;
  }

  if (result == wanted)
    return true;

  std::cerr << "SourceBaseSet:  " << source_set << '\n'
            << "TargetBaseSet:  " << target_set << '\n'
            << "Input:  " << input << '\n'
            << "Wanted: " << wanted << '\n'
            << "Result: " << result << '\n';
  return false;
}

int
main()
{
  ensure(1, hex2dec("0", "0"));
  ensure(2, hex2dec("A", "10"));
  ensure(3, hex2dec("0F", "15"));
  ensure(4, hex2dec("FF", "255"));
  ensure(5, hex2dec("ABCD", "43981"));
  ensure(6, hex2dec("ABCDEFABCDEFABCD", "12379814471884843981"));

  ensure(11, dec2hex("0", "0"));
  ensure(12, dec2hex("010", "A"));
  ensure(13, dec2hex("15", "F"));
  ensure(14, dec2hex("255", "FF"));
  ensure(15, dec2hex("43981", "ABCD"));
  ensure(16, dec2hex("12379814471884843981", "ABCDEFABCDEFABCD"));

  ensure(21, bin2dec("0", "0"));
  ensure(22, bin2dec("1", "1"));
  ensure(23, bin2dec("0101", "5"));
  ensure(24, bin2dec("100000010000101001011110110011", "541235123"));

  ensure(31, dec2bin("0", "0"));
  ensure(32, dec2bin("1", "1"));
  ensure(33, dec2bin("05", "101"));
  ensure(34, dec2bin("541235123", "100000010000101001011110110011"));

  ensure(41, any2any("0123456789", "abcd", "0", "a"));
  ensure(42, any2any("0123456789", "abcd", "4", "ba"));
  ensure(43, any2any("0123456789", "abcd", "9", "cb"));

  ensure(51, !hex2dec("abc", ""));
  ensure(52, !hex2dec("12XY", ""));
  ensure(53, !bin2dec("102", ""));

  return 0;

} // main()
