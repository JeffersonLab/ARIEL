// =======================================================
//
// sqlite statistics
//
// =======================================================

#include <cmath>

#include "cetlib/sqlite/Exception.h"
#include "cetlib/sqlite/statistics.h"

using namespace std::string_literals;

double
cet::sqlite::mean(sqlite3* const db,
                  std::string const& table_name,
                  std::string const& column_name)
{
  query_result<double> r;
  r << select("avg(" + column_name + ")").from(db, table_name);
  return unique_value(r);
}

double
cet::sqlite::median(sqlite3* const db,
                    std::string const& table_name,
                    std::string const& column_name)
{
  auto r = query<double>(
    db,
    "select avg("s + column_name + ")" + " from (select " + column_name +
      " from " + table_name + " order by " + column_name +
      " limit 2 - (select count(*) from " + table_name + ") % 2" +
      " offset (select (count(*) - 1) / 2" + " from " + table_name + "))");
  return unique_value(r);
}

double
cet::sqlite::rms(sqlite3* const db,
                 std::string const& table_name,
                 std::string const& column_name)
{
  auto r = query<double>(db,
                         "select sum("s + "(" + column_name + "-(select avg(" +
                           column_name + ") from " + table_name + "))" + "*" +
                           "(" + column_name + "-(select avg(" + column_name +
                           ") from " + table_name + "))" + " ) /" + "(count(" +
                           column_name + ")) from " + table_name);
  return std::sqrt(unique_value(r));
}
