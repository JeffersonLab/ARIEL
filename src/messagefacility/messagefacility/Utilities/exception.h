#ifndef messagefacility_Utilities_exception_h
#define messagefacility_Utilities_exception_h

#include "cetlib_except/coded_exception.h"
#include <string>

namespace mf {
  namespace errors {
    enum error { Configuration = 1, LogicError, OtherError };
  } // error

  namespace detail {
    std::string translate(errors::error);
  }

  typedef cet::coded_exception<errors::error, detail::translate> Exception;
} // mf

#endif /* messagefacility_Utilities_exception_h */

/// Local Variables:
/// mode: C++
/// c-basic-offset: 2
/// indent-tabs-mode: nil
/// End:
