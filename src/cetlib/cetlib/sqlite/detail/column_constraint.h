#ifndef cetlib_sqlite_detail_column_constraint_h
#define cetlib_sqlite_detail_column_constraint_h

//===================================================================
// Column constraints are specified in SQLite by appending additional
// keywords to the column definition.  These can be specified by
// (e.g.):
//                                  translates
//   column<int, primary_key>{"id"} ==========> id INTEGER PRIMARY KEY
//                                      to
//
// -------------------------------------------------------------------
//
// Technical considerations:
//
//   For now, the C++ representation does not allow constraints that
//   take additional arguments, since there is no easy way to have a
//   homogeneous interface in the context of bare columns<...>
//   statements vs. Ntuple<...> usage.  For example, suppose we
//   support the 'DEFAULT 1.56' constraint.  Whereas one could
//   create the translation:
//
//     columns<int, default_value>{"id", 1.56} ==> id INTEGER DEFAULT 1.56
//
//   There would be no easy to do it with Ntuple, without changing the
//   Ntuple c'tor (e.g.).
//
//     Ntuple<column<int,default_value>> n {db, "someTable", {{"id", 1.56}}};
//
//   and this is not supported right now.
//=======================================================================

#include <string>

namespace cet {
  namespace sqlite {

    struct primary_key {
      static std::string
      snippet()
      {
        return " PRIMARY KEY";
      }
    };
  }
}

#endif /* cetlib_sqlite_detail_column_constraint_h */

// Local Variables:
// mode: c++
// End:
