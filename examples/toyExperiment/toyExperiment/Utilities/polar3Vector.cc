//
// Instantiate CLHEP::Hep3Vectors using polar coordinates.
// (Not provided in the the native class).
//

#include <cmath>

#include "toyExperiment/Utilities/polar3Vector.h"
#include "toyExperiment/Utilities/safeSqrt.h"
#include "toyExperiment/Utilities/sqrtOrThrow.h"

CLHEP::Hep3Vector tex::polar3Vector( double p0,
                                       double cz,
                                       double phi){

  double sz = safeSqrt( 1.-cz*cz);
  return CLHEP::Hep3Vector( p0*sz*cos(phi),
                            p0*sz*sin(phi),
                            p0*cz
                            );
}

CLHEP::Hep3Vector tex::polar3Vector( double p0,
                                       double cz,
                                       double phi,
                                       double eps
                                       ){

  double sz = sqrtOrThrow( 1.-cz*cz,eps);
  return CLHEP::Hep3Vector( p0*sz*cos(phi),
                            p0*sz*sin(phi),
                            p0*cz
                              );
}
