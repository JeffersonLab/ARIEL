#ifndef Utilities_eventIDToString_h
#define Utilities_eventIDToString_h

//
// Turn an art event id into a string.
//

#include <string>

namespace art {
  class EventID;
}

namespace tex {

  std::string eventIDToString(art::EventID const& id);

}
#endif /* Utilities_eventIDToString_h */
