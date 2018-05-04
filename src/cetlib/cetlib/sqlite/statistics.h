#ifndef cetlib_sqlite_statistics_h
#define cetlib_sqlite_statistics_h

// ===================================================================
// The statistics utilities provided below allow a user to specify a
// database handle, the table name, and the column name from which
// the statistics quantity will be calculated.
//
// For more complicated statistics calculations, explicit querying
// should be made, either through using the cet::sqlite::select
// facility, or by using the more general std::sqlite::query facility.
//
// N.B. The calculated rms is actually the biased (division over N
//      instead of N-1) standard deviation of a sample.  The name of
//      this function was chosen to comport with the parlance used in
//      high-energy physics.
// ===================================================================

#include "cetlib/sqlite/query_result.h"
#include "cetlib/sqlite/select.h"

#include "sqlite3.h"

#include <string>

namespace cet {
  namespace sqlite {

    template <typename T = double>
    T min(sqlite3* const db,
          std::string const& table_name,
          std::string const& column_name);

    template <typename T = double>
    T max(sqlite3* const db,
          std::string const& table_name,
          std::string const& column_name);

    double mean(sqlite3* db,
                std::string const& table_name,
                std::string const& column_name);
    double median(sqlite3* db,
                  std::string const& table_name,
                  std::string const& column_name);
    double rms(sqlite3* db,
               std::string const& table_name,
               std::string const& column_name);

  } // sqlite
} // cet

//==================================================================
// Implementation below

template <typename T>
T
cet::sqlite::min(sqlite3* const db,
                 std::string const& table_name,
                 std::string const& column_name)
{
  query_result<T> r;
  r << select("min(" + column_name + ")").from(db, table_name);
  return unique_value(r);
}

template <typename T>
T
cet::sqlite::max(sqlite3* const db,
                 std::string const& table_name,
                 std::string const& column_name)
{
  query_result<T> r;
  r << select("max(" + column_name + ")").from(db, table_name);
  return unique_value(r);
}

#endif /* cetlib_sqlite_statistics_h */

// Local Variables:
// mode: c++
// End:
