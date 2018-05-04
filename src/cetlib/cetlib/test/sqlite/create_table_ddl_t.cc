// Tests for assembling the create-table SQLite expression.  These
// tests DO NOT actually create the table.

#include "cetlib/sqlite/Exception.h"
#include "cetlib/sqlite/create_table.h"
#include "cetlib/sqlite/detail/normalize_statement.h"

#include <cassert>
#include <iostream>

using namespace cet::sqlite;
using namespace std;

namespace {
  void
  compare_statements(std::string test, std::string ref)
  {
    detail::normalize_statement(test);
    detail::normalize_statement(ref);
    if (test != ref) {
      throw Exception{errors::OtherError} << "Statements do not match:\n"
                                          << " Test statement: " << test << '\n'
                                          << " Ref. statement: " << ref << '\n';
    }
  }
}

int
main() try {
  // Test simple create_table() so we can use it later
  {
    auto const& test = detail::create_table("employees");
    std::string const ref{"CREATE TABLE employees"};
    compare_statements(test, ref);
  }

  // One column
  {
    auto const& test =
      detail::create_table_ddl("employees", column<string>{"name"});
    std::string const ref{detail::create_table("employees") + "(name text)"};
    compare_statements(test, ref);
  }

  // Two columns
  {
    auto const& test = detail::create_table_ddl(
      "employees", column<string>{"name"}, column<int>{"id"});
    std::string const ref{detail::create_table("employees") +
                          "(name text, id integer)"};
    compare_statements(test, ref);
  }

  // Two columns, the first with the PRIMARY KEY constraint
  {
    auto const& test = detail::create_table_ddl(
      "employees", column<int, primary_key>{"id"}, column<string>{"name"});
    std::string const ref{detail::create_table("employees") +
                          "(id integer PRIMARY KEY, name text)"};
    compare_statements(test, ref);
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
