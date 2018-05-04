#ifndef canvas_Persistency_Provenance_type_aliases_h
#define canvas_Persistency_Provenance_type_aliases_h

#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/ProductID.h"

#include <array>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

namespace art {
  // The key is the process name
  using ProcessLookup = std::map<std::string, std::vector<ProductID>>;

  using ViewLookup_t = ProcessLookup;

  // The key is the friendly class name
  using ProductLookup_t = std::map<std::string, ProcessLookup>;
  using AvailableProducts_t = std::unordered_set<ProductID, ProductID::Hash>;
}

#endif /* canvas_Persistency_Provenance_type_aliases_h */

// Local variables:
// mode: c++
// End:
