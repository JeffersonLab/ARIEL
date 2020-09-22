#ifndef ExDataProducts_Combination_h
#define ExDataProducts_Combination_h

//
// A combination of two reconstructed tracks.
//
// Notes:
// 1) We would prefer an implementation with art::PtrVector but there is a bug in that class
//    template that we can't work around tonight.
//

#include "toyExperiment/RecoDataProducts/RecoTrk.h"
#include "toyExperiment/RecoDataProducts/FittedHelixData.h"
#include "canvas/Persistency/Common/Ptr.h"

#include <ostream>

namespace tex {

  class Combination {

  public:

    Combination();
    Combination( RecoTrk const& combo,
                 art::Ptr<FittedHelixData> const& child0,
                 art::Ptr<FittedHelixData> const& child1 );

    RecoTrk recoTrk() const { return combo_; }

    art::Ptr<FittedHelixData> const& child(size_t i) const;

  private:

    RecoTrk                   combo_;
    art::Ptr<FittedHelixData> child0_;
    art::Ptr<FittedHelixData> child1_;

  };

  std::ostream& operator<<(std::ostream& ost,
                           const tex::Combination& hit );

}

#endif /* ExDataProducts_Combination_h */
