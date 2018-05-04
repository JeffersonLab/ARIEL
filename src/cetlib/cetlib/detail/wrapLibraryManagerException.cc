#include "cetlib/detail/wrapLibraryManagerException.h"

void
cet::detail::wrapLibraryManagerException(cet::exception const& e,
                                         std::string const& item_type,
                                         std::string const& libspec,
                                         std::string const& release)
{
  if (e.category() == "LogicError") {
    // Re-throw.
    throw;
  } else {
    // Wrap and throw.
    throw exception("Configuration", std::string("Unknown") + item_type, e)
      << item_type + " " << libspec << " with version " << release
      << " was not registered.\n"
      << "Perhaps your plugin type is misspelled "
      << "or is not a plugin recognized by this\n"
      << "system.\n";
  }
}
