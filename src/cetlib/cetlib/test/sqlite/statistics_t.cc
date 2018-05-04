#include "cetlib/sqlite/ConnectionFactory.h"
#include "cetlib/sqlite/Ntuple.h"
#include "cetlib/sqlite/statistics.h"

#include <array>
#include <cassert>
#include <iostream>

namespace {

  enum quantity : std::size_t { min = 0, max, mean, median, rms, n };
  using quantities_t = std::array<unsigned, quantity::n>;

  auto
  test_statistics_quantities(sqlite3* const db,
                             std::string const& table_name,
                             std::string const& column_name,
                             quantities_t const& expected_values)
  {
    assert(cet::sqlite::min<unsigned>(db, table_name, column_name) ==
           expected_values[quantity::min]);
    assert(cet::sqlite::max<unsigned>(db, table_name, column_name) ==
           expected_values[quantity::max]);
    // Don't want to worry about floating-point tolerance now, so just
    // cast everything to unsigned.
    assert(static_cast<unsigned>(cet::sqlite::mean(
             db, table_name, column_name)) == expected_values[quantity::mean]);
    assert(
      static_cast<unsigned>(cet::sqlite::median(db, table_name, column_name)) ==
      expected_values[quantity::median]);
    assert(static_cast<unsigned>(cet::sqlite::rms(
             db, table_name, column_name)) == expected_values[quantity::rms]);
  }
}

using namespace std;
using namespace cet::sqlite;

int
main()
{
  ConnectionFactory cf;
  auto c = cf.make(":memory:");
  std::string const table_name{"workers"};
  {
    Ntuple<string, int, int> workers{
      c, table_name, {{"Name", "Age", "Experience"}}};
    workers.insert("Abby", 27, 5);
    workers.insert("Benny", 48, 10);
    workers.insert("Cassie", 52, 27);
    workers.insert("David", 65, 14);
    workers.insert("Emily", 38, 19);
  }
  test_statistics_quantities(
    c, table_name, "Age", {{27, 65, 46, 48, 12 /*.649...*/}});
  test_statistics_quantities(
    c, table_name, "Experience", {{5, 27, 15, 14, 7 /*.563...*/}});
}
