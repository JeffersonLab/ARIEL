#include "cetlib/split_path.h"
#include <cassert>

using cet::split_path;
using std::string;
using std::vector;

void
test_empty_path()
{
  vector<string> results;
  split_path("", results);
  assert(results.empty());
}

void
test_single_entry_path()
{
  vector<string> results;
  string sole_entry("ABC_D/EFG/H");
  split_path(sole_entry, results);
  assert(results.size() == 1);
  assert(results[0] == sole_entry);
}

void
test_adjacent_colons()
{
  vector<string> results;
  string path("A::B");
  split_path(path, results);
  assert(results.size() == 3);
  assert(results[0] == "A");
  assert(results[1] == "");
  assert(results[2] == "B");
}

void
test_leading_colon()
{
  vector<string> results;
  string path(":BOO");
  split_path(path, results);
  assert(results.size() == 2);
  assert(results[0].empty());
  assert(results[1] == "BOO");
}

void
test_trailing_colon()
{
  vector<string> results;
  string path("A:");
  split_path(path, results);
  assert(results.size() == 2);
  assert(results[0] == "A");
  assert(results[1].empty());
}

void
test_typical_use()
{
  vector<string> results;
  vector<string> path_elements;
  path_elements.push_back("/p/gcc/v4_5_1/Linux64/lib64");
  path_elements.push_back("/p/gcc/v4_5_1/Linux64/lib");
  path_elements.push_back("/p/root/v5_26_00d/lib");
  path_elements.push_back("/p/cmake/v2_6_4/.");
  path_elements.push_back("/p/cppunit/v1_12_1/slf5.x86_64.a1/lib");
  string path = path_elements[0];
  for (size_t i = 1; i < path_elements.size(); ++i) {
    path += ":";
    path += path_elements[i];
  }

  // Now do the test
  split_path(path, results);
  assert(results == path_elements);
}

int
main()
{
  test_empty_path();
  test_single_entry_path();
  test_adjacent_colons();
  test_leading_colon();
  test_trailing_colon();
  test_typical_use();
}
