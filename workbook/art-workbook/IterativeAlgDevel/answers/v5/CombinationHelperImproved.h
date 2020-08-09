#ifndef IterativeAlgDevel_CombinationHelperImproved_h
#define IterativeAlgDevel_CombinationHelperImproved_h

#include "toyExperiment/RecoDataProducts/FittedHelixDataCollection.h"
#include "toyExperiment/RecoDataProducts/RecoTrk.h"
#include "toyExperiment/Reconstruction/FittedHelix.h"

#include <tuple>
#include <utility>
#include <vector>

namespace helper {

  using CombinationData = std::tuple<tex::RecoTrk,std::size_t,std::size_t>;
  using IndexPair       = std::pair<std::size_t,std::size_t>;
  using indices_t       = std::vector<std::size_t>;

  std::vector<CombinationData> makeCombinationsAlgImproved(tex::FittedHelixDataCollection const&,
                                                           double const mass,
                                                           double const bz);

  namespace detail {

    std::vector<tex::FittedHelix> makeFits(tex::FittedHelixDataCollection const&,
                                           double const bz,
                                           indices_t& pos_indices,
                                           indices_t& neg_indices);

    std::vector<IndexPair> getIndexPairs(indices_t const& pos_indices,
                                         indices_t const& neg_indices);

    std::vector<tex::RecoTrk> makeTracks(std::vector<tex::FittedHelix> const& fits,
                                         double const mass);
  }

}

#endif

// Local variables:
// mode: c++
// End:
