#ifdef STANDALONE_TEST
#include <cassert>
#define BOOST_REQUIRE assert
#define BOOST_CHECK assert
#else
#define BOOST_TEST_MODULE (regex test)
#include "cetlib/LibraryManager.h"
#include "cetlib/quiet_unit_test.hpp"
#endif

#include <fstream>
#include <iostream>
#include <regex>

#ifdef STANDALONE_TEST
int
main()
{
#else
BOOST_AUTO_TEST_SUITE(RegexTests)

BOOST_AUTO_TEST_CASE(pluginRegex)
#endif
  {
    std::ifstream inFile("regex.txt");
    BOOST_REQUIRE(inFile);
#ifdef STANDALONE_TEST
    std::string const pattern_stem = {"(?:[A-Za-z0-9\\-]*_)*[A-Za-z0-9]+_"};
#else
  cet::LibraryManager dummy("noplugin");
  std::string const pattern_stem = dummy.patternStem();
#endif
    std::string pattern =
      std::string("lib(") + pattern_stem + ")([A-Za-z0-9]+)\\.so";
    for (std::string line; std::getline(inFile, line);) {
      static std::regex const r(pattern);
      std::smatch subs;
      bool result = std::regex_match(line, subs, r);
      std::cout << line << " match: " << std::boolalpha << result
                << (result ?
                      (std::string(" (stem ") +
                       subs[1].str().substr(0, subs[1].str().size() - 1) +
                       ", type " + subs[2].str() + ")") :
                      "")
                << std::endl;
      BOOST_CHECK(result);
    }
  }

#ifndef STANDALONE_TEST
  BOOST_AUTO_TEST_CASE(alternationCheck_subscript)
#endif
  {
    std::regex const pattern1{R"(.*\[(?:\d*[1-9]|\d+0)\])"};
    std::regex const pattern2{R"(.*\[(?:\d+0|\d*[1-9])\])"};

    std::vector<std::string> const tests{"test[0]", "test[1]", "test[10]"};

    auto verify_match = [&pattern1, &pattern2](std::string const& test) {
      return std::regex_match(test, pattern1) ==
             std::regex_match(test, pattern2);
    };

    for (auto const& test : tests)
      BOOST_CHECK(verify_match(test));
  }

#ifndef STANDALONE_TEST
  BOOST_AUTO_TEST_CASE(alternationCheck_dot)
#endif
  {
    std::regex const pattern1{R"(.*\.(?:\d*[1-9]|\d+0))"};
    std::regex const pattern2{R"(.*\.(?:\d+0|\d*[1-9]))"};

    std::vector<std::string> const tests{
      "test.0",
      "test.1",
      "test.10" // Using pattern1 vs. pattern2 gives different result w/
                // clang 3.6
    };

    auto verify_match = [&pattern1, &pattern2](std::string const& test) {
      return std::regex_match(test, pattern1) ==
             std::regex_match(test, pattern2);
    };

    for (auto const& test : tests)
      BOOST_CHECK(verify_match(test));
  }

#ifdef STANDALONE_TEST
}
#else
BOOST_AUTO_TEST_SUITE_END()
#endif
