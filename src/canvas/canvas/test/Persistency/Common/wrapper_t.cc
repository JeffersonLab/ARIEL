#include "canvas/Persistency/Common/Sampled.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "canvas/Utilities/InputTag.h"

using namespace art;

int
main()
{
  InputTag const invalid{};

  Wrapper<int> w1;
  auto edp = w1.createEmptySampledProduct(invalid);
  assert(edp);
  assert(edp->isPresent());

  auto const& sampled = dynamic_cast<Wrapper<Sampled<int>> const&>(*edp);
  assert(sampled->empty());

  Wrapper<Sampled<int>> w2;
  try {
    w2.createEmptySampledProduct(invalid);
    assert(false);
  }
  catch (Exception const& e) {
    assert(e.categoryCode() == errors::LogicError);
    std::cerr << e.what();
  }
}
