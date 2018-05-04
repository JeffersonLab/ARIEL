#ifndef cetlib_sqlite_detail_bind_parameters_h
#define cetlib_sqlite_detail_bind_parameters_h

//=======================================================================
// The bind_one_* functions are intended to be for internal use only.
// They provide a means of binding a C++ type to the corresponding
// SQLite type through the sqlite3_bind_* API.
//=======================================================================

#include "sqlite3.h"

#include <string>
#include <tuple>

namespace cet {
  namespace sqlite {
    namespace detail {

      void bind_one_parameter(sqlite3_stmt* s,
                              std::size_t const idx,
                              double const v);
      void bind_one_parameter(sqlite3_stmt* s,
                              std::size_t const idx,
                              int const v);
      void bind_one_parameter(sqlite3_stmt* s,
                              std::size_t const idx,
                              std::uint32_t const v);
      void bind_one_parameter(sqlite3_stmt* s,
                              std::size_t const idx,
                              sqlite_int64 const v);
      void bind_one_parameter(sqlite3_stmt* s,
                              std::size_t const idx,
                              std::string const& v);
      void bind_one_null(sqlite3_stmt* s, std::size_t const idx);

      template <class TUP, size_t N>
      struct bind_parameters {
        static void
        bind(sqlite3_stmt* s, TUP const& t)
        {
          bind_parameters<TUP, N - 1>::bind(s, t);
          if (auto& param = std::get<N - 1>(t))
            bind_one_parameter(s, N, *param);
          else
            bind_one_null(s, N);
        }
      };

      template <class TUP>
      struct bind_parameters<TUP, 1> {
        static void
        bind(sqlite3_stmt* s, TUP const& t)
        {
          if (auto& param = std::get<0>(t))
            bind_one_parameter(s, 1, *param);
          else
            bind_one_null(s, 1);
        }
      };

    } // detail
  }   // sqlite
} // cet

#endif /* cetlib_sqlite_detail_bind_parameters_h */

// Local Variables:
// mode: c++
// End:
