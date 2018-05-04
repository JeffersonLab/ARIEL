#ifndef messagefacility_plugins_FileConfig_h
#define messagefacility_plugins_FileConfig_h
// vim: set sw=2 expandtab :

#include "fhiclcpp/types/Atom.h"

#include <string>

namespace mfplugins {

  struct FileConfig {

    fhicl::Atom<std::string> filename{fhicl::Name{"filename"}};
    fhicl::Atom<bool> append{
      fhicl::Name{"append"},
      fhicl::Comment{"If 'append' is set to 'true', then the contents\n"
                     "of an already-existing file are retained."},
      false};
  };

} // namespace mfplugins

#endif /* messagefacility_plugins_FileConfig_h */

// Local variables:
// mode: c++
// End:
