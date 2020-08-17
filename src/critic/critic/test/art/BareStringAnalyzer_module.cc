#include "art/Framework/Core/ModuleMacros.h"
#include "critic/test/art/GenericOneSimpleProductAnalyzer.h"

#include <string>

namespace arttest {
  using BareStringAnalyzer =
    GenericOneSimpleProductAnalyzer<std::string, std::string>;
}

DEFINE_ART_MODULE(arttest::BareStringAnalyzer)
