//
// Input:        two angles, in radians.
// Return value: difference between the two
//
// This funciton deals with the cut line and returns delta phi with a small
// absolute value for angles that are just on either side of the cut line.
//

#include "toyExperiment/Utilities/dphi.h"
#include "toyExperiment/Utilities/phi_norm.h"
#include "toyExperiment/Utilities/phi_small.h"

double
tex::dphi(double arg1, double arg2)
{
  double phi1 = phi_norm(arg1);
  double phi2 = phi_norm(arg2);
  return phi_small(phi1 - phi2);
}
