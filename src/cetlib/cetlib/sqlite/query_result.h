#ifndef cetlib_sqlite_query_result_h
#define cetlib_sqlite_query_result_h

// ================================================================
// query_result<T...>
//
// The query_result template contains the result of an SQLite table
// query.  The supplied template arguments specify the C++ types that
// should be used to represent the entries of each row returned by the
// query.
//
// The query_result is not automatically populated upon construction,
// but it is populated to a call to operator<< (e.g.):
//
//   query_result<int, string> nums;
//   nums << select("id", "name").from(db, "workers");
//
// where 'nums' now contains the set of values from column "id" and
// column "name" of table "workers" in the database db.  After the
// query result has been filled, it can be interacted through a simple
// loop:
//
//   for (auto const& [id, name] : nums) {
//     // 'id' (type int) and 'name' (type string) now filled with the
//     // results of each row from the query.
//   }
//
// For cases where only one row and one column are expected for the
// query, an automatic conversion can be achieved by using
// 'unique_value' (e.g.):
//
//   query_result<unsigned> r;
//   r << select("age").from(db, "workers");
//   unsigned const highest_age {unique_value(r)};
//
// If 'r' contains more than one row and/or more than one column, an
// exception would be thrown upon a call to unique_value.
//
// ----------------------------------------------------------------
// Technical considerations:
//
//   Instead of filling a query_result via a call like:
//
//     query_result<T...> r;
//     r << select(...).from(...);
//
//   a query_result should probably take a SelectStmt as its c'tor
//   argument, whereupon construction, the query takes place and the
//   query_result can then be used without any operator<< equals
//   calls:
//
//     query_result<T...> const r {select(...).from(...)};
//
//   This was not implemented since the c'tor calls could get
//   very long--but that is arguably a poor reason for retaining the
//   above interface.
//
// =======================================================

#include <sstream>
#include <tuple>
#include <vector>

#include "cetlib/container_algorithms.h"
#include "cetlib/sqlite/Exception.h"

namespace cet::sqlite {

  template <typename... Args>
  struct query_result {
    bool
    empty() const
    {
      return data.empty();
    }
    auto
    begin() const
    {
      return data.begin();
    }
    auto
    end() const
    {
      return data.end();
    }
    explicit operator bool() const { return !empty(); }

    std::vector<std::string> columns;
    std::vector<std::tuple<Args...>> data;
  };

  template <typename T>
  inline T
  unique_value(query_result<T> const& r)
  {
    if (r.data.size() != 1ull) {
      throw sqlite::Exception{sqlite::errors::SQLExecutionError}
        << "unique_value expected of non-unique query.";
    }
    return std::get<T>(r.data[0]);
  }

  template <typename... Args>
  std::ostream&
  operator<<(std::ostream& os, query_result<Args...> const& res)
  {
    using size_t = decltype(res.columns.size());
    auto const ncolumns = res.columns.size();
    for (size_t i{}; i != ncolumns; ++i) {
      os << res.columns[i] << ' ';
    }
    os << "\n--------------------------------\n";
    for (auto const& row : res.data) {
      os << row.str() << '\n';
    }
    return os;
  }

} // cet::sqlite

#endif /* cetlib_sqlite_query_result_h */

// Local Variables:
// mode: c++
// End:
