#ifndef cetlib_sqlite_Exception_h
#define cetlib_sqlite_Exception_h

// ======================================================================
//
// Exception: sqlite-specific customizations for cetlib_except/coded_exception
//
// ======================================================================

#include "cetlib_except/coded_exception.h"
#include <string>

// ----------------------------------------------------------------------

namespace cet::sqlite {
  namespace errors {

    enum ErrorCodes { LogicError = 0, SQLExecutionError, OtherError, Unknown };

  } // errors

  namespace ExceptionDetail {
    std::string translate(errors::ErrorCodes);
  }

  using Exception =
    cet::coded_exception<errors::ErrorCodes, ExceptionDetail::translate>;
} // cet::sqlite

// ======================================================================

#endif /* cetlib_sqlite_Exception_h */

// Local Variables:
// mode: c++
// End:
