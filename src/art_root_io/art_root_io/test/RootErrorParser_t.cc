#include "art_root_io/detail/RootErrorParser.h"

#include "TError.h"

#include <catch2/catch.hpp>
#include <string>

using art::detail::RootErrorParser;

SCENARIO("A simple RootError object")
{
  RootErrorParser const error{
    "TCling::AutoParsing", "Cannot find payload information for class Billy"};
  CHECK(error.has_message("payload information"));
  CHECK(error.has_message_in("Cannot find", "AutoP"));
  CHECK(error.in("TCling::"));
  CHECK(
    error.in_any_of_these_locations("A", "B", "TCling", "Don't ever get here"));
  CHECK(error.has_any_of_these_messages(
    "Where is your", "payload information", "?"));
}
