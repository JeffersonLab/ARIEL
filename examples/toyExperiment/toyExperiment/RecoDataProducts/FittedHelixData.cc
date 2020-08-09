//
// The persistent data for a fitted helix.
//

#include "toyExperiment/RecoDataProducts/FittedHelixData.h"
#include "CLHEP/Matrix/Vector.h"

tex::FittedHelixData::FittedHelixData():
  helix_(),
  cov_(Helix::dim,0),
  hits_(){
}

tex::FittedHelixData::FittedHelixData( Helix               const& helix,
                                         CLHEP::HepSymMatrix const& cov,
                                         hits_type           const& hits ):
  helix_(helix), cov_(cov), hits_(hits){
}

std::ostream& tex::operator<<(std::ostream& ost,
                              const tex::FittedHelixData& fit ){
  ost << "( cu: "  << fit.helix().cu()
      << " phi0: " << fit.helix().phi0()
      << " d0: "   << fit.helix().d0()
      << " ct: "   << fit.helix().ct()
      << " z0: "   << fit.helix().z0()
      << " )" << std::endl;
  for ( int i=Helix::icu; i<=Helix::iz0; ++i ) {
    for ( int j=i; j<=Helix::iz0; ++j ){
      std::cout << fit.cov()[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "nHits: " << fit.hits().size() << std::endl;

  return ost;
}
