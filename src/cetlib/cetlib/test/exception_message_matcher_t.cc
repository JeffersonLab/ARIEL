#include <catch2/catch.hpp>

#include "cetlib/exception_message_matcher.h"

using cet::exception_message_matcher;

TEST_CASE("Contains Match")
{
  exception_message_matcher matcher(Catch::Matchers::Contains("x"));
  cet::exception e("junk");
  SECTION("Exception message matches")
  {
    e << "yxz";
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, matcher);
    }
    SECTION("Not Thrown") { CHECK_THAT(e, matcher); }
  }
  SECTION("Exception message does not match")
  {
    e << "yyz";
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, !matcher);
    }
    SECTION("Not Thrown") { CHECK_THAT(e, !matcher); }
  }
}

TEST_CASE("Equals Match")
{
  exception_message_matcher matcher(
    Catch::Matchers::Equals("---- junk BEGIN\n  x\n---- junk END\n"));
  cet::exception e("junk");
  SECTION("Exception message matches")
  {
    e << "x";
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, matcher);
    }
    SECTION("Not Thrown") { CHECK_THAT(e, matcher); }
  }
  SECTION("Exception message does not match")
  {
    e << "y";
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, !matcher);
    }
    SECTION("Not Thrown") { CHECK_THAT(e, !matcher); }
  }
}

TEST_CASE("StartsWith Match")
{
  exception_message_matcher matcher(
    Catch::Matchers::StartsWith("---- junk BEGIN\n"));
  SECTION("Exception message matches")
  {
    cet::exception e("junk");
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, matcher);
    }
    SECTION("Not Thrown") { CHECK_THAT(e, matcher); }
  }
  SECTION("Exception message does not match")
  {
    cet::exception e("junky");
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, !matcher);
    }
    SECTION("Not Thrown") { CHECK_THAT(e, !matcher); }
  }
}

TEST_CASE("EndsWith Match")
{
  exception_message_matcher matcher(
    Catch::Matchers::EndsWith("---- junk END\n"));
  SECTION("Exception message matches")
  {
    cet::exception e("junk");
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, matcher);
    }
    SECTION("Not Thrown") { CHECK_THAT(e, matcher); }
  }
  SECTION("Exception message does not match")
  {
    cet::exception e("junky");
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, !matcher);
    }
    SECTION("Not Thrown") { CHECK_THAT(e, !matcher); }
  }
}

TEST_CASE("Regex Match")
{
  exception_message_matcher matcher(
    Catch::Matchers::Matches("---- jun.*?\\b BEGIN\n.*\n.*\n"));
  SECTION("Exception message matches")
  {
    cet::exception e("junk");
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, matcher);
    }
    SECTION("Not Thrown") { CHECK_THAT(e, matcher); }
  }
  SECTION("Exception message does not match")
  {
    cet::exception e("jill");
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, !matcher);
    }
    SECTION("Not Thrown") { CHECK_THAT(e, !matcher); }
  }
}

TEST_CASE("Composed Matchers")
{
  exception_message_matcher xmatcher(Catch::Matchers::Contains("x")),
    ymatcher(Catch::Matchers::Contains("y\n"));
  cet::exception e("junk");
  SECTION("Exception message matches")
  {
    e << "xy";
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, xmatcher && ymatcher);
    }
    SECTION("Not Thrown") { CHECK_THAT(e, xmatcher && ymatcher); }
  }
  SECTION("Exception message does not match")
  {
    e << "yx";
    SECTION("Thrown")
    {
      CHECK_THROWS_MATCHES(throw e, cet::exception, !(xmatcher && ymatcher));
    }
    SECTION("Not Thrown") { CHECK_THAT(e, !(xmatcher && ymatcher)); }
  }
}
