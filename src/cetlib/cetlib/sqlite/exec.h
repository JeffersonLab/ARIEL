#ifndef cetlib_sqlite_exec_h
#define cetlib_sqlite_exec_h

// ======================================================================
// 'exec' is the utility used to encapsulate sqlite3_exec calls when
// no results are expected to be retrieved from the call.  If results
// from an SQLite query are desired, then the cet::sqlite::select(...)
// functionality should be used.
// ======================================================================

#include "sqlite3.h"
#include <string>

namespace cet {
  namespace sqlite {
    void exec(sqlite3* db, std::string const& ddl);
  }
}

#endif /* cetlib_sqlite_exec_h */

// Local Variables:
// mode: c++
// End:
