//
// A combination of two reconstructed tracks.
//

#include "art-workbook/ExDataProducts/Combination.h"

tex::Combination::Combination():
  combo_(),
  child0_(),
  child1_(){
}

tex::Combination::Combination( RecoTrk const& combo,
                                 art::Ptr<FittedHelixData> const& child0,
                                 art::Ptr<FittedHelixData> const& child1 ):
  combo_(combo),
  child0_(child0),
  child1_(child1){
}

art::Ptr<tex::FittedHelixData> const& tex::Combination::child(size_t i) const{
  if ( i == 0 ) return child0_;
  return child1_;
}


std::ostream& tex::operator<<(std::ostream& ost,
                              const tex::Combination& combo ){
  ost << "( "
      << combo.recoTrk()
      << "  contributing FittedHelices: ("
      << "  product: " << combo.child(0).id()
      << " offsets: " << combo.child(0).key() << " " << combo.child(1).key();
  return ost;
}
