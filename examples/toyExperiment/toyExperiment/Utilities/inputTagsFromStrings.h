#ifndef Utilities_inputTagsFromStrings_h
#define Utilities_inputTagsFromStrings_h
//
// Return an std::vector of art::InputTags, created from
// an std::vector of std::strings
//

#include "canvas/Utilities/InputTag.h"

#include <string>
#include <vector>

namespace tex {

  std::vector<art::InputTag> inputTagsFromStrings(std::vector<std::string>);

}

#endif /* Utilities_inputTagsFromStrings_h */
