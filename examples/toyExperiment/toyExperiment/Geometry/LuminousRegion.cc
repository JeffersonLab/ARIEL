#include "toyExperiment/Geometry/LuminousRegion.h"

#include <ostream>

tex::LuminousRegion::LuminousRegion():
  center_(0.,0.,0.),
  sigma_ (0.,0.,0.){
}

tex::LuminousRegion::LuminousRegion( CLHEP::Hep3Vector const& center,
                                     CLHEP::Hep3Vector const& sigma ):
  center_(center),
  sigma_(sigma){
}

namespace tex {
  std::ostream& operator<<(std::ostream& ost,
                           const LuminousRegion& lr ){
    ost << "Center: " << lr.center() << "  Sigma: " << lr.sigma();
    return ost;
  }
}
