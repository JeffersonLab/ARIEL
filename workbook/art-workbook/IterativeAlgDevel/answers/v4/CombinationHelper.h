#ifndef IterativeAlgDevel_CombinationHelper_h
#define IterativeAlgDevel_CombinationHelper_h

#include "toyExperiment/RecoDataProducts/FittedHelixDataCollection.h"
#include "toyExperiment/RecoDataProducts/RecoTrk.h"

#include <tuple>
#include <utility>
#include <vector>

namespace helper {

  using CombinationData = std::tuple<tex::RecoTrk,std::size_t,std::size_t>;

  std::vector<CombinationData> makeCombinationsAlg(tex::FittedHelixDataCollection const&,
                                                   double const mass,
                                                   double const bz);

}

#endif

// Local variables:
// mode: c++
// End:
