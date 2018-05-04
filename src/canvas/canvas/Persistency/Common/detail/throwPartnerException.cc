#include "canvas/Persistency/Common/detail/throwPartnerException.h"
#include "canvas/Utilities/Exception.h"
#include "cetlib_except/demangle.h"

void
art::detail::throwPartnerException(std::type_info const& generator,
                                   std::type_info const& wanted_wrapper_type)
{
  throw Exception(errors::LogicError, "makePartner")
    << "Attempted to make type "
    << cet::demangle_symbol(wanted_wrapper_type.name())
    << " which is not a valid wrapped partner of "
    << cet::demangle_symbol(generator.name())
    << "\nPlease report to the ART framework developers.\n";
}
