#ifndef cetlib_sqlite_create_table_h
#define cetlib_sqlite_create_table_h

// =======================================================
//
// create_table
//
// The create_table(_as) interface allows users to create tables for a
// given SQLite database.  Two free functions exist:
//
//   (1) 'create_table' which takes a database handle object (of type
//        sqlite3*) or a cet::sqlite::Connection object, a string
//        corresponding to the desired table name, and a pack of
//        column<> arguments that correspond to the desired SQLite
//        columns.
//
//   (2) 'create_table_as' which takes a string corresponding to the
//       desired table name, and a const reference to a SelectStmt
//       object.  This signature does not receive a database handle
//       object, since the handle was already supplied when
//       constructing the SelectStmt object.
//
// See below for examples of usage.  Note that calling
// 'create_table(_as)' does NOT create an explicit SQLite transaction.
// If a transaction is desired, the create_table* calls should be
// surrounded by the appropriate transaction calls.
//
// Example of usage:
//
//    using namespace cet::sqlite;
//
//    Connection c {"a.db"};
//    create_table(c, "workers", column<string>{"name"}, column<int>{"id"});
//
//    // insert entries into table...
//
//    Transaction txn {c}; // Can work with transactions.
//    create_table_as("workersNamedDavid",
//                    select("*").from(c,"workers").where("name='David'"));
//    txn.commit();
//
// =======================================================

#include "cetlib/sqlite/column.h"
#include "cetlib/sqlite/exec.h"
#include "cetlib/sqlite/select.h"

#include "sqlite3.h"

#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>

using namespace std::string_literals;

namespace cet::sqlite {
  template <typename... Cols>
  void create_table(sqlite3* const db,
                    std::string const& tablename,
                    Cols const&... cols);

  void create_table_as(std::string const& tablename, SelectStmt const& stmt);
}

// ========================================================
// Implementation below

namespace cet::sqlite {
  namespace detail {

    template <typename T, typename... Constraints>
    std::string
    column_info(column<T, Constraints...> const& col)
    {
      std::string info{col.name() + col.sqlite_type()};
      info += (Constraints::snippet() + ... + ""s);
      return info;
    }

    template <typename H, typename... T>
    inline std::string
    columns(H const& h, T const&... t)
    {
      return (column_info(h) + ... + (", " + column_info(t)));
    }

    inline std::string
    create_table(std::string const& tablename)
    {
      return "CREATE TABLE "s + tablename;
    }

    template <typename... Cols>
    std::string
    create_table_ddl(std::string const& tablename, Cols const&... cols)
    {
      std::string ddl{create_table(tablename)};
      ddl += "( ";
      ddl += columns(cols...);
      ddl += " )";
      return ddl;
    }

    inline std::string
    create_table_as_ddl(std::string const& tablename, SelectStmt const& stmt)
    {
      std::string ddl{create_table(tablename)};
      ddl += " AS ";
      ddl += stmt.ddl_;
      return ddl;
    }

  } // detail

  template <typename... Cols>
  void
  create_table(sqlite3* const db,
               std::string const& tablename,
               Cols const&... cols)
  {
    auto const& ddl = detail::create_table_ddl(tablename, cols...);
    sqlite::exec(db, ddl);
  }

  inline void
  create_table_as(std::string const& tablename, SelectStmt const& stmt)
  {
    std::string ddl{detail::create_table_as_ddl(tablename, stmt)};
    sqlite::exec(stmt.db_, ddl);
  }

} // cet::sqlite

#endif /* cetlib_sqlite_create_table_h */

// Local Variables:
// mode: c++
// End:
