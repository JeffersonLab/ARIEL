#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/LorentzVector.h"

namespace CLHEP {

  inline bool operator!=(CLHEP::HepSymMatrix const& l, CLHEP::HepSymMatrix const& r)
  {
    return !(l==r);
  }

}

