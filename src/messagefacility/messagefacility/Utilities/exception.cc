#include "messagefacility/Utilities/exception.h"

std::string
mf::detail::translate(mf::errors::error code)
{
  switch (code) {
    case mf::errors::Configuration:
      return "Configuration error";
    case mf::errors::LogicError:
      return "Logic error";
    case mf::errors::OtherError:
      return "Other error";
    default:
      return "Unknown code";
  }
}

// ======================================================================

/// Local Variables:
/// mode: C++
/// c-basic-offset: 2
/// indent-tabs-mode: nil
/// End:
