#ifndef cetlib_sqlite_column_h
#define cetlib_sqlite_column_h

// ===================================================================
// column<T, Constraints...>
//
// SQLite tables are composed of columns that are specified in SQLite
// as (e.g):
//
//  CREATE TABLE workers(FirstName TEXT, LastName TEXT);
//
// where the fields in parentheses denote two columns with the names
// 'FirstName' and 'LastName', both of which are of the type 'TEXT'.
//
// The column template allows a user to specify a column using native
// C++ types.  For example, the above SQLite statement can be achieved
// and executed in C++ using the create_table command:
//
//   cet::sqlite::create_table(db,
//                             "workers",
//                             column<string>{"FirstName"},
//                             column<string>{"LastName"});
//
// Column constraints are also allowed (e.g.):
//
//   column<int, primary_key, autoincrement>{"id"}
//
// See the notes in cetlib/sqlite/detail/column_constraint.h.
// ===================================================================

#include "cetlib/sqlite/detail/column_constraint.h"

#include <array>
#include <string>
#include <tuple>
#include <utility>

namespace cet {
  namespace sqlite {

    template <size_t N>
    using name_array = std::array<std::string, N>;

    class column_base {
    public:
      column_base(std::string const& n) : name_{n} {}
      auto const&
      name() const
      {
        return name_;
      }

    private:
      std::string name_;
    };

    // column<T> is a containing struct that knows its C++ type (T)
    // and the sqlite translation (sqlite_type()).  There is no
    // implementation for the general case; the template must be
    // specialized for each supported type.
    template <typename T, typename... Constraints>
    struct column;

    // IMPROVEMENT: The specializations can be improved by some
    // template metaprogramming and using the std::is_arithmetic and
    // std::is_floating_point type traits.
    template <typename... Constraints>
    struct column<double, Constraints...> : column_base {
      using column_base::column_base;
      using type = double;
      std::string
      sqlite_type() const
      {
        return " numeric";
      }
    };

    template <typename... Constraints>
    struct column<float, Constraints...> : column_base {
      using column_base::column_base;
      using type = float;
      std::string
      sqlite_type() const
      {
        return " numeric";
      }
    };

    template <typename... Constraints>
    struct column<int, Constraints...> : column_base {
      using column_base::column_base;
      using type = int;
      std::string
      sqlite_type() const
      {
        return " integer";
      }
    };

    template <typename... Constraints>
    struct column<long, Constraints...> : column_base {
      using column_base::column_base;
      using type = long;
      std::string
      sqlite_type() const
      {
        return " integer";
      }
    };

    template <typename... Constraints>
    struct column<long long, Constraints...> : column_base {
      using column_base::column_base;
      using type = long long;
      std::string
      sqlite_type() const
      {
        return " integer";
      }
    };

    template <typename... Constraints>
    struct column<unsigned int, Constraints...> : column_base {
      using column_base::column_base;
      using type = int;
      std::string
      sqlite_type() const
      {
        return " integer";
      }
    };

    template <typename... Constraints>
    struct column<unsigned long, Constraints...> : column_base {
      using column_base::column_base;
      using type = long;
      std::string
      sqlite_type() const
      {
        return " integer";
      }
    };

    template <typename... Constraints>
    struct column<unsigned long long, Constraints...> : column_base {
      using column_base::column_base;
      using type = long long;
      std::string
      sqlite_type() const
      {
        return " integer";
      }
    };

    template <typename... Constraints>
    struct column<std::string, Constraints...> : column_base {
      using column_base::column_base;
      using type = std::string;
      std::string
      sqlite_type() const
      {
        return " text";
      }
    };

    //=============================================================================
    // A permissive_column type is used in the context of an Ntuple so
    // that the following constructs are allowed:
    //
    //   Ntuple<int, double, string>   // has identical semantics to...
    //   Ntuple<column<int>, column<double>, column<string>>
    //
    // The benefit is that if a user wants to specify a constraint for
    // a given column, the way to do that in the context of the Ntuple
    // is for the user to specify (e.g.) column<int, primary_key> for
    // the relevant column, but not be required to use column<...> for
    // all others (e.g.):
    //
    //   Ntuple<column<int, primary_key>, double, string>;

    template <typename T, typename... Constraints>
    struct permissive_column : column<T, Constraints...> {
      using column<T, Constraints...>::column;
      using element_type = T;
    };

    template <typename T, typename... Constraints>
    struct permissive_column<column<T, Constraints...>>
      : permissive_column<T, Constraints...> {
      using permissive_column<T, Constraints...>::permissive_column;
    };

  } // sqlite
} // cet

#endif /* cetlib_sqlite_column_h */

// Local Variables:
// mode: c++
// End:
