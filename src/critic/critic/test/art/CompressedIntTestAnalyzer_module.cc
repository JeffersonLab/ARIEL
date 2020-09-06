#include "art/Framework/Core/ModuleMacros.h"
#include "art/test/TestObjects/ToyProducts.h"
#include "critic/test/art/GenericOneSimpleProductAnalyzer.h"

namespace arttest {
  using CompressedIntTestAnalyzer =
    GenericOneSimpleProductAnalyzer<int, CompressedIntProduct>;
}

DEFINE_ART_MODULE(arttest::CompressedIntTestAnalyzer)
