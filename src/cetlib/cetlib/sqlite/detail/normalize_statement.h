#ifndef cetlib_sqlite_detail_normalize_statement_h
#define cetlib_sqlite_detail_normalize_statement_h

// =======================================================
//
// normalize_statement
//
// This facility is used to compare normalize the SQLite statements.
// It does the following:
// - Remove spaces just inside of parentheses
// - Replaces multiple spaces with 1 space
// - Ensures no spaces after commas
// =======================================================

#include <string>

namespace cet {
  namespace sqlite {
    namespace detail {
      void normalize_statement(std::string& to_replace);
      std::string normalized_statement(std::string to_replace);
    }
  }
}

#endif /* cetlib_sqlite_detail_normalize_statement_h */

// Local Variables:
// mode: c++
// End:
