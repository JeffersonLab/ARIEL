//
// Return an std::vector of art::InputTags, created from
// an std::vector of std::strings
//

#include "toyExperiment/Utilities/inputTagsFromStrings.h"

std::vector<art::InputTag> tex::inputTagsFromStrings( std::vector<std::string> tagNames){

  std::vector<art::InputTag> tags;
  tags.reserve(tagNames.size());

  for ( auto const& name : tagNames ){
    tags.emplace_back(name);
  }

  return tags;
}
