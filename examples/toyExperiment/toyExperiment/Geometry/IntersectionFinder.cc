
#include "toyExperiment/Geometry/IntersectionFinder.h"

#include <cmath>
#include <iostream>

tex::IntersectionFinder::IntersectionFinder(double rs, Helix const& helix)
  : hasIntersection_(false), s_(0.), position_()
{

  double rt = helix.rho();
  double rc = helix.radiusToCenter();

  // Cosine of the 2D turning angle from POCA to the intersection.
  double a = 0.5 * (rc / rt + rt / rc - rs * rs / rt / rc);

  if (std::abs(a) < 1.) {
    hasIntersection_ = true;
    s_ = helix.rho() * acos(a) / helix.sinTheta();
    position_ = helix.position(s_);
  }
}

tex::IntersectionFinder::IntersectionFinder(Shell const& shell,
                                            Helix const& helix)
  : IntersectionFinder(shell.radius(), helix)
{

  if (!hasIntersection_)
    return;

  // Reject if the z position is outside of limits.
  if (std::abs(position_.z()) > shell.halfLength()) {
    hasIntersection_ = false;
    s_ = 0;
    position_ = CLHEP::Hep3Vector();
  }
}
