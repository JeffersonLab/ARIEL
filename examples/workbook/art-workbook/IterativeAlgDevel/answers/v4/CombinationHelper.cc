#include "art-workbook/IterativeAlgDevel/CombinationHelper.h"
#include "toyExperiment/Reconstruction/FittedHelix.h"

#include <algorithm>
#include <vector>

using namespace tex;

namespace helper {

  std::vector<CombinationData>
  makeCombinationsAlg(FittedHelixDataCollection const& fitdata,
                      double const mass,
                      double const bz)
  {
    std::vector<CombinationData> result;

    // Loop over all unique pairs of tracks.
    for ( size_t i=0; i+1<fitdata.size(); ++i ) {

      FittedHelix fit1 { fitdata[i], bz };
      RecoTrk trk1 { fit1.lorentzAtPoca(mass), fit1.lorentzAtPocaCov(mass) };

      for ( size_t j=i+1; j<fitdata.size(); ++j){

        FittedHelix fit2 { fitdata[j], bz };

        // Skip a pair of tracks if they have the same sign of electric charge.
        if ( std::abs(fit1.q()+fit2.q()) > 0.5 ) continue;

        RecoTrk trk2 { fit2.lorentzAtPoca(mass), fit2.lorentzAtPocaCov(mass) };

        result.emplace_back(trk1+trk2, i, j);

      } // end inner loop over tracks
    }   // end outer loop over tracks

    return result;
  }

}
