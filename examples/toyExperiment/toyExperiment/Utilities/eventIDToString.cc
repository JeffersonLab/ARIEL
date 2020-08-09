//
// Turn an art event id into a string.
//

#include "toyExperiment/Utilities/eventIDToString.h"

#include "canvas/Persistency/Provenance/EventID.h"

#include <sstream>

std::string
tex::eventIDToString( art::EventID const& id ){
  std::ostringstream os;
  os << id;
  return os.str();
}
