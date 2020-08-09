#ifndef IterativeAlgDevel_RecoTrk_h
#define IterativeAlgDevel_RecoTrk_h

//
// A crude reconstructed track class in cartesian representation.
//

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/SymMatrix.h"

#include <ostream>

namespace italgdev {

  class RecoTrk {

  public:

    // Names of columns/rows if the covariance matrix.
    enum cartesian_index_type { ipx=0, ipy, ipz, ie };

    // Dimension of the covariance matrix.
    enum dimension_type { dimension = 4 };

    RecoTrk();

#ifndef __GCCXML__
    RecoTrk( CLHEP::HepLorentzVector const& momentum,
             CLHEP::HepSymMatrix     const& momCov );

    CLHEP::HepLorentzVector const& momentum()    const { return _momentum; }
    CLHEP::HepSymMatrix     const& momentumCov() const { return _momCov;   }

    double mass() const{
      return _momentum.mag();
    }

    double p() const{
      CLHEP::Hep3Vector const& pp = _momentum;
      return pp.mag();
    }

    double sigmaMass() const;
    double sigmaP() const;

    RecoTrk const& operator+=( RecoTrk const& rhs);
#endif  // __GCCXML__

  private:

    CLHEP::HepLorentzVector _momentum;
    CLHEP::HepSymMatrix     _momCov;

  };

#ifndef __GCCXML__
  inline
  RecoTrk operator+( RecoTrk const& r1, RecoTrk const& r2){
    RecoTrk r(r1);
    r += r2;
    return r;
  }

#endif  // __GCCXML__

}

#endif /* IterativeAlgDevel_RecoTrk_h */

// Local variables:
// mode: c++
// End:
