#ifndef cetlib_sqlite_detail_DefaultDatabaseOpenPolicy_h
#define cetlib_sqlite_detail_DefaultDatabaseOpenPolicy_h

//=====================================================================
// The locking mechanisms for NFS systems are deficient and can thus
// wreak havoc with SQLite, which depends upon them.  In order to
// support an SQLite database on NFS, we use a URI, explicitly
// including the query parameter: 'nolock=1'.  The
// DefaultDatabaseOpenPolicy achieves this.
//=====================================================================

#include <string>

struct sqlite3;

namespace cet {
  namespace sqlite {
    namespace detail {
      class DefaultDatabaseOpenPolicy {
      public:
        sqlite3* open(std::string const& file_name);
      };
    }
  }
}

#endif /* cetlib_sqlite_detail_DefaultDatabaseOpenPolicy_h */

// Local variables:
// mode: c++
// End:
