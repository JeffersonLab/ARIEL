// Test the normalize_statement functionality, which is used for comparing
// schemas.

#include "cetlib/sqlite/Exception.h"
#include "cetlib/sqlite/detail/normalize_statement.h"

#include <cassert>
#include <iostream>

using namespace cet::sqlite;
using namespace std;

namespace {
  void
  normalize_then_compare(std::string test, std::string const& ref)
  {
    detail::normalize_statement(test);
    if (test != ref) {
      throw Exception{errors::OtherError} << "Statements do not match:\n"
                                          << " Test statement: " << test << '\n'
                                          << " Ref. statement: " << ref << '\n';
    }
  }
}

int
main() try {
  // Test spaces around parentheses and commas
  {
    std::string const test{"CREATE TABLE TimeEvent (   Run  integer,   SubRun "
                           "integer,Event integer  , Time numeric)  "};
    std::string const ref{"CREATE TABLE TimeEvent(Run integer,SubRun "
                          "integer,Event integer,Time numeric)"};
    normalize_then_compare(test, ref);
  }
}
catch (std::exception const& e) {
  std::cerr << e.what() << '\n';
  return 1;
}
catch (...) {
  std::cerr << "Caught unknown exception.\n";
  return 2;
}
