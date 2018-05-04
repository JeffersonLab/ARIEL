#ifndef canvas_Persistency_Provenance_canonicalProductName_h
#define canvas_Persistency_Provenance_canonicalProductName_h

#include <string>

namespace art {
  // Could consider having a function template that receives the type
  // of the product as a template argument instead of requiring the
  // user to provide the friendly class name.

  std::string canonicalProductName(std::string const& friendlyClassName,
                                   std::string const& moduleLabel,
                                   std::string const& productInstanceName,
                                   std::string const& processName);
}

#endif /* canvas_Persistency_Provenance_canonicalProductName_h */

// Local variables:
// mode: c++
// End:
