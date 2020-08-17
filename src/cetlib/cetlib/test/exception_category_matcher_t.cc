#include <catch2/catch.hpp>

#include "cetlib/exception_category_matcher.h"

using cet::exception_category_matcher;

TEST_CASE("Category matcher")
{
  exception_category_matcher matcher("junk");
  SECTION("Exception category matches")
  {
    cet::exception e("junk");
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, matcher);
    }
    SECTION("Not Thrown") { CHECK_THAT(e, matcher); }
  }
  SECTION("Exception category does not match")
  {
    cet::exception e("junky");
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, !matcher);
    }
    SECTION("Not Thrown") { CHECK_THAT(e, !matcher); }
  }
}
