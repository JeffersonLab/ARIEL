#include "art-workbook/IterativeAlgDevel/CombinationHelperImproved.h"
#include "cetlib/container_algorithms.h"
#include "toyExperiment/Reconstruction/FittedHelix.h"

#include <algorithm>
#include <vector>
#include <utility>

using namespace tex;
using namespace helper;

std::vector<CombinationData>
helper::makeCombinationsAlgImproved(FittedHelixDataCollection const& fitdata,
                                    double const mass,
                                    double const bz)
{
  std::vector<std::size_t> posIndices, negIndices;
  auto const& fits       = detail::makeFits(fitdata, bz, posIndices, negIndices);
  auto const& indexPairs = detail::getIndexPairs( posIndices, negIndices );
  auto const& tracks     = detail::makeTracks(fits, mass);

  std::vector<CombinationData> result;
  auto makeCombination = [&tracks,&result](auto const& pair){
    RecoTrk const& trk1 = tracks[pair.first ];
    RecoTrk const& trk2 = tracks[pair.second];
    result.emplace_back(trk1+trk2, pair.first, pair.second );
  };

  cet::for_all(indexPairs, makeCombination);

  return result;
}

//======================================================================
// detail functions

std::vector<FittedHelix>
helper::detail::makeFits(FittedHelixDataCollection const& fitdata,
                         const double bz,
                         indices_t& pos_fit_indices,
                         indices_t& neg_fit_indices)
{
  std::vector<FittedHelix> result;
  std::transform( fitdata.begin(), fitdata.end(), std::back_inserter(result),
                  [bz](auto const& f){ return FittedHelix{f,bz};} );

  for ( std::size_t i = 0 ; i != result.size() ; ++i ) {
    auto& indices = (result[i].q() > 0) ? pos_fit_indices : neg_fit_indices;
    indices.push_back(i);
  }

  return result;
}

std::vector<IndexPair>
helper::detail::getIndexPairs(std::vector<std::size_t> const& pos,
                              std::vector<std::size_t> const& neg)
{
  std::vector<IndexPair> result;
  for (std::size_t const i : pos)
    for (std::size_t const j : neg)
      result.emplace_back(i,j);
  return result;
}

std::vector<RecoTrk>
helper::detail::makeTracks(std::vector<FittedHelix> const& fits,
                           double const mass)
{
  std::vector<RecoTrk> result;
  std::transform( fits.begin(), fits.end(), std::back_inserter(result),
                  [mass](auto const& fit)
                  {
                    return RecoTrk{fit.lorentzAtPoca(mass), fit.lorentzAtPocaCov(mass)};
                  }
                  );
  return result;
}
