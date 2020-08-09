//
// A data-like hit; contains no MCTruth information.
//

#include "toyExperiment/RecoDataProducts/TrkHit.h"

tex::TrkHit::TrkHit():
  _shell(0), _z(0), _phi(0), _sz(0), _sphi(0){
}

tex::TrkHit::TrkHit( int    shell,
                       double z,
                       double phi,
                       double sz,
                       double sphi):
  _shell(shell), _z(z), _phi(phi), _sz(sz), _sphi(sphi){
}

std::ostream& tex::operator<<(std::ostream& ost,
                              const tex::TrkHit& hit ){
  ost << "Hit( shell: " << hit.shell()
      << " z: "         << hit.z()
      << " phi: "       << hit.phi()
      << " sig(z): "    << hit.sigZ()
      << " sig(phi): "  << hit.sigPhi();
  return ost;
}
