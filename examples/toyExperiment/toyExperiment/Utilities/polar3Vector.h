#ifndef Utilities_polar3Vector_h
#define Utilities_polar3Vector_h

//
// Construct a Hep3Vector from a polar coordinates representation.
//  ( magnitude, cos(polar angle), azimuth) = ( r, cz, phi)
//
// Notes
// 1) CLHEP::Hep3Vector itself provides setters for the representations:
//    ( r, theta, phi), ( r, phi, z), (r, eta, phi).
//    It does not provide ( r, cos(theta), phi).
//
// 2) CLHEP::Hep3Vector does not have a virtual destructor so we will
//    not inherit from it and will pay the copy penalty.
//

#include "CLHEP/Vector/ThreeVector.h"

namespace tex {

  // Uses safesqrt to compute sin(theta).
  CLHEP::Hep3Vector polar3Vector( double r,
                                  double cz,
                                  double phi );

  // Uses sqrtOrThrow to compute sin(theta).
  CLHEP::Hep3Vector polar3Vector( double r,
                                  double cz,
                                  double phi,
                                  double eps );
}

#endif /* Utilities_polar3Vector_h */
