#ifndef Geometry_LuminousRegion_h
#define Geometry_LuminousRegion_h

//
// Describe the luminous region, the region from which the tracks originate.
//

#include "CLHEP/Vector/ThreeVector.h"

#include <iosfwd>

namespace tex {

  class LuminousRegion{

  public:

    LuminousRegion();

    LuminousRegion( CLHEP::Hep3Vector const& center,
                    CLHEP::Hep3Vector const& sigma );

    CLHEP::Hep3Vector const& center() const { return center_; }
    CLHEP::Hep3Vector const& sigma()  const { return sigma_;  }

  private:

    CLHEP::Hep3Vector center_;
    CLHEP::Hep3Vector sigma_;

  };

  std::ostream& operator<<(std::ostream& ost,
                           const LuminousRegion& s );

}  //namespace mu2e

#endif /* Geometry_LuminousRegion_h */
