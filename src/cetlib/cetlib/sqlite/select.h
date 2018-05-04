#ifndef cetlib_sqlite_select_h
#define cetlib_sqlite_select_h

// ====================================================================
// The facilities presented here provide two ways of querying an
// SQLite database without having to use the C API.  Both approach
// involve using a cet::sqlite::query_result template object.
//
// Type-safe interface
// -------------------
//
// The type-safe interface is the first approach to be used when
// needing to query the database.  Instead of explicitly typing a
// lengthy character string as the entire select statement, the string
// is assembled for you via function calls.  For example (e.g.):
//
//   auto stmt = select("id","name").from(db,"workers");
//
// results in the creation of a SelectStmt object (here 'stmt'), which
// can then be used to query the database.  The encouraged usage
// pattern is to create the query_result object into which the results
// of the query will be inserted.  Then, the query itself is executed:
//
//   query_result<int,string> res;
//   res << select("id","name").from(db,"workers");
//
// The query is NOT executed until, the operator<< function is called.
// If a user attempts to execute a query with an incorrectly formed select
// statement (e.g.):
//
//   res << select("id","name");
//
// a compile-time error will be triggered.  In this way, using the
// 'select' facility, and its associated subsequent function calls
// (e.g. 'from') ensure a safer, type-safe approach to making SQLite
// queries that avoid typographical errors.
//
// One can assemble more complicated statements (e.g.):
//
//   select().from().where().order_by().limit();
//
// It is incumbent on the user to know how select statements can be
// meaningfully formed in SQLite to inform which functions can be
// called after the initial 'select()' call.
//
// N.B. It is still possible to make typographical errors with the
//      above interface since the required arguments to some of the
//      functions are strings.  However, it is less likely that one
//      will introduce an error using this approach.
//
// String-based interface
// ----------------------
//
// For complicated querying statements that cannot be represented by
// the type-safe interface, the cet::sqlite::query can be used (e.g.):
//
//   auto r = query<double, int>(db, "SELECT ... "); // ==>
//   query_result<double,int>
//
// Although quite flexible, use of query is prone to typographical
// errors that are less likely when using the type-safe interface.
// ====================================================================

#include "cetlib/sqlite/detail/get_result.h"
#include "cetlib/sqlite/query_result.h"

#include <string>

#include "sqlite3.h"

namespace cet {
  namespace sqlite {

    template <typename... Args>
    query_result<Args...>
    query(sqlite3* db, std::string const& ddl)
    {
      query_result<Args...> res;
      char* errmsg{nullptr};
      if (sqlite3_exec(
            db, ddl.c_str(), detail::get_result<Args...>, &res, &errmsg) !=
          SQLITE_OK) {
        std::string msg{errmsg};
        sqlite3_free(errmsg);
        throw sqlite::Exception{sqlite::errors::SQLExecutionError} << msg;
      }
      return res;
    }

    struct SelectStmt {
      SelectStmt(std::string&& ddl, sqlite3* const db)
        : ddl_{std::move(ddl)}, db_{db}
      {}
      std::string ddl_;
      sqlite3* db_;

      auto
      where(std::string const& cond) &&
      {
        ddl_ += " where ";
        ddl_ += cond;
        return SelectStmt{std::move(ddl_), db_};
      }

      auto
      order_by(std::string const& column, std::string const& clause = {}) &&
      {
        ddl_ += " order by ";
        ddl_ += column;
        ddl_ += " " + clause;
        return SelectStmt{std::move(ddl_), db_};
      }

      auto
      limit(int const num) &&
      {
        ddl_ += " limit ";
        ddl_ += std::to_string(num);
        return SelectStmt{std::move(ddl_), db_};
      }
    };

    struct IncompleteSelectStmt {

      IncompleteSelectStmt(std::string&& ddl) : ddl_{std::move(ddl)} {}

      auto
      from(sqlite3* const db, std::string const& tablename) &&
      {
        ddl_ += " from ";
        ddl_ += tablename;
        return SelectStmt{std::move(ddl_), db};
      }
      std::string ddl_;
    };

    namespace detail {
      inline std::string
      concatenate()
      {
        return "";
      }

      template <typename H, typename... T>
      std::string
      concatenate(H const& h, T const&... t)
      {
        return sizeof...(t) != 0u ? std::string{h} + "," + concatenate(t...) :
                                    std::string{h};
      }
    }

    template <typename... T>
    auto
    select(T const&... t)
    {
      std::string result{"select " + detail::concatenate(t...)};
      return IncompleteSelectStmt{std::move(result)};
    }

    template <typename... T>
    auto
    select_distinct(T const&... t)
    {
      std::string result{"select distinct " + detail::concatenate(t...)};
      return IncompleteSelectStmt{std::move(result)};
    }

    template <typename... Args>
    void
    operator<<(query_result<Args...>& r, SelectStmt const& cq)
    {
      r = query<Args...>(cq.db_, cq.ddl_ + ";");
    }

  } // sqlite
} // cet

#endif /* cetlib_sqlite_select_h */

// Local variables:
// mode: c++
// End:
