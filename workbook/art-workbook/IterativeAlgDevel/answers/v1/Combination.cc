//
// A combination of two reconstructed tracks.
//

#include "art-workbook/IterativeAlgDevel/Combination.h"
#include "art-workbook/IterativeAlgDevel/RecoTrk.h"

italgdev::Combination::Combination():
  combo_(),
  child0_(),
  child1_(){
}

italgdev::Combination::Combination( <YOUR NAMESPACE HERE>::RecoTrk const& combo,
                                    art::Ptr<tex::FittedHelixData> const& child0,
                                    art::Ptr<tex::FittedHelixData> const& child1 ):
  combo_(combo),
  child0_(child0),
  child1_(child1){
  }

art::Ptr<tex::FittedHelixData> const& italgdev::Combination::child(size_t i) const{
  return i ? child1_ : child0_;
}
