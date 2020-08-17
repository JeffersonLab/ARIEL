//
// Input argument is an angle in radians, return value is
// the same angle, placed in the domain [ pi, -pi).
//

#include "toyExperiment/Utilities/phi_small.h"

#include <cmath>

double
tex::phi_small(double arg)
{
  static double twopi = 2.0 * M_PI;
  if (arg <= -M_PI) {
    arg += twopi;
  } else if (arg > M_PI) {
    arg -= twopi;
  }
  return arg;
}
