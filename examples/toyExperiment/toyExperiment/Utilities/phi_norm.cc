//
// Input argument is an angle in radians, return value is
// the same angle, in radians, placed in the domain [0, 2pi).
//

#include "toyExperiment/Utilities/phi_norm.h"

#include <cmath>

double
tex::phi_norm(double arg)
{
  static double twopi = 2.0 * M_PI;
  if (arg < 0.) {
    arg += twopi;
  } else if (arg >= twopi) {
    arg -= twopi;
  }
  return arg;
}
