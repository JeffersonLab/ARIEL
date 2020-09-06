// vim: set sw=2 expandtab :
#include "cetlib/container_algorithms.h"
#include "cetlib/sqlite/ConnectionFactory.h"
#include "cetlib/sqlite/column.h"
#include "cetlib/sqlite/create_table.h"
#include "cetlib/sqlite/insert.h"
#include "cetlib/sqlite/query_result.h"
#include "cetlib/sqlite/select.h"

#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace cet::sqlite;

int
main()
{
  ConnectionFactory cf;
  unique_ptr<Connection> c{cf.make_connection(":memory:")};
  string const name{"numbers"};
  vector<pair<string, int>> const pairs{{"one", 1}, {"five", 5}, {"ten", 10}};
  create_table(*c, name, column<string>("key"), column<int>("value"));
  for (auto const& pr : pairs) {
    insert_into(*c, name).values(pr.first, pr.second);
  }
  query_result<string, int> allEntries;
  allEntries << select("*").from(*c, name);
  cet::for_all_with_index(allEntries,
                          [&pairs](size_t const i, auto const& row) {
                            string key;
                            int value{0};
                            tie(key, value) = row;
                            assert(key == pairs[i].first);
                            assert(value == pairs[i].second);
                          });
  query_result<int> values;
  create_table_as("onlyValues", select("value").from(*c, name));
  cet::for_all_with_index(values, [&pairs](size_t const i, auto const& row) {
    auto const& v = get<0>(row);
    assert(v == pairs[i].second);
  });
}
