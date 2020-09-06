#ifndef cetlib_sqlite_insert_h
#define cetlib_sqlite_insert_h

// ====================================================================
//
// The insert_into facility provides a means of ensuring, in a
// type-safe manner, the insertion of values into an already-existing
// database table.
//
// The encouraged usage pattern is:
//
//   using namespace cet::sqlite;
//   create_table(db, "workers", column<int>{"id"}, column<string>{"name"});
//   insert_into(db, "workers").values(24, "Billy");
//   insert_into(db, "workers").values(17, "Jenny");
//
// Extensive usage of insert_into(...) may be inefficient as each call
// prepares a statement, executes it, and then finalizes it.  To
// support more efficient insertion, consider using the Ntuple
// facility.
//
// --------------------------------------------------------------------
//
// Technical considerations:
//
//  - It is not difficult to expand the functionality of insert_into
//    to support inserting into specific columns (as is supported with
//    a bare SQLite insert statement), should it be requested.
//  - It should be determined whether a simple-enough syntax can be
//    developed to support insert statements with placeholders.
//
// ====================================================================

#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "cetlib/sqlite/exec.h"
#include "sqlite3.h"

namespace cet::sqlite {
  namespace detail {
    inline std::string
    maybe_quote(std::string const& s)
    {
      return "\"" + s + "\"";
    }

    inline std::string
    maybe_quote(char const* s)
    {
      return maybe_quote(std::string{s});
    }

    template <typename T>
    T
    maybe_quote(T const& t)
    {
      return t;
    }

    inline void
    values_str_impl(std::ostream&)
    {}

    template <typename H, typename... T>
    void
    values_str_impl(std::ostream& os, H const& h, T const&... t)
    {
      if (sizeof...(T) != 0u) {
        os << maybe_quote(h) << ',';
        values_str_impl(os, t...);
      } else
        os << maybe_quote(h);
    }

    template <typename... Args>
    std::string
    values_str(Args const&... args)
    {
      std::ostringstream oss;
      values_str_impl(oss, args...);
      return oss.str();
    }
  }

  struct IncompleteInsert {
    IncompleteInsert(sqlite3* const db, std::string&& ddl)
      : db_{db}, ddl_{std::move(ddl)}
    {}

    template <typename... T>
    void
    values(T const&... t) &&
    {
      ddl_ += " values (";
      ddl_ += detail::values_str(t...);
      ddl_ += ");";
      exec(db_, ddl_);
    }

    sqlite3* const db_;
    std::string ddl_;
  };

  inline auto
  insert_into(sqlite3* const db, std::string const& tablename)
  {
    std::string result{"insert into " + tablename};
    return IncompleteInsert{db, std::move(result)};
  }

} // cet::sqlite

#endif /* cetlib_sqlite_insert_h */

// Local variables:
// mode: c++
// End:
