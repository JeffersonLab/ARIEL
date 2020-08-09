#ifndef IterativeAlgDevel_FittedHelixDataFactory_h
#define IterativeAlgDevel_FittedHelixDataFactory_h

#include "toyExperiment/RecoDataProducts/FittedHelixDataCollection.h"
#include "toyExperiment/RecoDataProducts/RecoTrk.h"

#include "art-workbook/IterativeAlgDevel/detail/ToyParticle.h"

#include <vector>

namespace helper {

  class FittedHelixDataFactory {
  public:

    static tex::FittedHelixDataCollection make(std::size_t npos, std::size_t nneg);

  private:

    static std::vector<detail::ToyParticle> generateParticles_(std::size_t npos, std::size_t nneg);
    static tex::FittedHelixDataCollection makeFitData_(std::vector<detail::ToyParticle> const&);

  };

}

#endif

// Local variables:
// mode: c++
// End:
