#ifndef IterativeAlgDevel_CombinationHelperImproved_h
#define IterativeAlgDevel_CombinationHelperImproved_h

#include "cetlib/container_algorithms.h"

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

  namespace detail {

    std::vector<tex::FittedHelix> lmakeFits(tex::FittedHelixDataCollection const&,
                                           double const bz,
                                           indices_t& pos_indices,
                                           indices_t& neg_indices);

    std::vector<IndexPair> lgetIndexPairs(indices_t const& pos_indices,
                                         indices_t const& neg_indices);

    std::vector<tex::RecoTrk> lmakeTracks(std::vector<tex::FittedHelix> const& fits,
                                         double const mass);
  }

  template <typename Lambda>
  std::vector<CombinationData> makeCombinationsAlgLambda(tex::FittedHelixDataCollection const& fitdata,
                                                         double const mass,
                                                         double const bz,
                                                         Lambda f)
  {
    using namespace tex;

    std::vector<std::size_t> posIndices, negIndices;
    auto const& fits       = detail::lmakeFits(fitdata, bz, posIndices, negIndices);
    auto const& indexPairs = detail::lgetIndexPairs( posIndices, negIndices );
    auto const& tracks     = detail::lmakeTracks(fits, mass);
    
    std::vector<CombinationData> result;
    auto makeCombination = [&tracks,&result,f](auto const& pair){
      RecoTrk const& trk1 = tracks[pair.first ];
      RecoTrk const& trk2 = tracks[pair.second];
      RecoTrk const& trk3 = trk1+trk2;
      result.emplace_back(trk3, pair.first, pair.second );
      f(trk3);
    };
    
    cet::for_all(indexPairs, makeCombination);
    
    return result;
  }


}

#endif

// Local variables:
// mode: c++
// End:
