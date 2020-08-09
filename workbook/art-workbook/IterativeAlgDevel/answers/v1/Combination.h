#ifndef IterativeAlgDevel_Combination_h
#define IterativeAlgDevel_Combination_h

#include "canvas/Persistency/Common/Ptr.h"
#include "art-workbook/IterativeAlgDevel/RecoTrk.h"
#include "toyExperiment/RecoDataProducts/FittedHelixData.h"

#include <ostream>

namespace italgdev {

  class Combination {

  public:

    Combination();
    Combination( <YOUR NAMESPACE HERE>::RecoTrk const& combo,
                 art::Ptr<tex::FittedHelixData> const& child0,
                 art::Ptr<tex::FittedHelixData> const& child1 );

    <YOUR NAMESPACE HERE>::RecoTrk recoTrk() const { return combo_; }

    art::Ptr<tex::FittedHelixData> const& child(size_t i) const;

  private:

    <YOUR NAMESPACE HERE>::RecoTrk              combo_;
    art::Ptr<tex::FittedHelixData> child0_;
    art::Ptr<tex::FittedHelixData> child1_;

  };

}

#endif

// Local variables:
// mode: c++
// End:
