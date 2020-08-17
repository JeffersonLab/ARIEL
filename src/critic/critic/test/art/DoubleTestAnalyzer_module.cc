#include "art/Framework/Core/ModuleMacros.h"
#include "art/test/TestObjects/ToyProducts.h"
#include "critic/test/art/GenericOneSimpleProductAnalyzer.h"

namespace arttest {
  typedef GenericOneSimpleProductAnalyzer<double, DoubleProduct>
    DoubleTestAnalyzer;
}

DEFINE_ART_MODULE(arttest::DoubleTestAnalyzer)
