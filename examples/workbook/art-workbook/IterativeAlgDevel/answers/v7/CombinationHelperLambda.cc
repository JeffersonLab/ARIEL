#include "art-workbook/IterativeAlgDevel/CombinationHelperLambda.h"
#include "toyExperiment/Reconstruction/FittedHelix.h"

#include <algorithm>
#include <vector>
#include <utility>

using namespace tex;
using namespace helper;

//======================================================================
// detail functions

std::vector<FittedHelix>
helper::detail::lmakeFits(FittedHelixDataCollection const& fitdata,
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
helper::detail::lgetIndexPairs(std::vector<std::size_t> const& pos,
                               std::vector<std::size_t> const& neg)
{
  std::vector<IndexPair> result;
  for (std::size_t const i : pos)
    for (std::size_t const j : neg)
      result.emplace_back(i,j);
  return result;
}

std::vector<RecoTrk>
helper::detail::lmakeTracks(std::vector<FittedHelix> const& fits,
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
