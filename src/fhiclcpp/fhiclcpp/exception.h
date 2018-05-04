#ifndef fhiclcpp_exception_h
#define fhiclcpp_exception_h

// ======================================================================
//
// exception
//
// ======================================================================

#include "cetlib_except/coded_exception.h"

#include <string>

namespace fhicl {
  enum error {
    cant_find,
    cant_happen,
    cant_insert,
    number_is_too_large,
    parse_error,
    type_mismatch,
    protection_violation,
    cant_open_db,
    sql_error,
    unimplemented,
    other
  };

  namespace detail {
    std::string translate(error);
  }

  using exception = cet::coded_exception<error, detail::translate>;
}

  // ======================================================================

#endif /* fhiclcpp_exception_h */

// Local Variables:
// mode: c++
// End:
