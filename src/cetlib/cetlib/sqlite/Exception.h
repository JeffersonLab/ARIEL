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

namespace cet {
  namespace sqlite {
    namespace errors {

      enum ErrorCodes {
        LogicError = 0,
        SQLExecutionError,
        OtherError,
        Unknown
      };

    } // errors

    namespace ExceptionDetail {
      std::string translate(errors::ErrorCodes);
    }

    using Exception =
      cet::coded_exception<errors::ErrorCodes, ExceptionDetail::translate>;
  } // sqlite
} // cet

  // ======================================================================

#endif /* cetlib_sqlite_Exception_h */

// Local Variables:
// mode: c++
// End:
