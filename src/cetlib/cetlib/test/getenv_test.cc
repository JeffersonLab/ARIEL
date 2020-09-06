#ifndef _POSIX_C_SOURCE
/* For setenv() */
#define _POSIX_C_SOURCE 200112L
#endif

#include "catch2/catch.hpp"

#include "cetlib/exception_category_matcher.h"
#include "cetlib/exception_message_matcher.h"
#include "cetlib/getenv.h"
#include "cetlib_except/exception.h"

#include <cstdlib>
#include <string>

namespace {
  auto const VAR = "GETENV_TEST";
}

TEST_CASE("getenv_value")
{
  std::string const val{"TEST"};
  setenv(VAR, val.c_str(), 1);
  SECTION("Throw") { REQUIRE(val == cet::getenv(VAR)); }
  SECTION("NoThrow") { REQUIRE(val == cet::getenv(VAR, std::nothrow)); }
}

TEST_CASE("getenv_empty")
{
  std::string const val; // Empty
  setenv(VAR, val.c_str(), 1);
  SECTION("Throw") { REQUIRE(val == cet::getenv(VAR)); }
  SECTION("NoThrow") { REQUIRE(val == cet::getenv(VAR, std::nothrow)); }
}

TEST_CASE("getenv_unset")
{
  unsetenv(VAR);

  SECTION("Throw")
  {
    using namespace std::string_literals;
    REQUIRE_THROWS_MATCHES(
      cet::getenv(VAR),
      cet::exception,
      cet::exception_category_matcher("getenv") &&
        cet::exception_message_matcher(Catch::Matchers::Contains(
          "Can't find an environment variable named \""s + VAR + "\"\n")));
  }

  SECTION("No Throw") { CHECK(cet::getenv(VAR, std::nothrow).empty()); }
}
