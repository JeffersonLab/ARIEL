#ifndef Utilities_dphi_h
#define Utilities_dphi_h

//
// Input:        two angles, in radians.
// Return value: difference between the two
//
// This funciton deals with the cut line and returns delta phi with a small
// absolute value for angles that are just on either side of the cut line.
//

namespace tex {

  double dphi(double phi1, double phi2);

}

#endif /* Utilities_dphi_h */
