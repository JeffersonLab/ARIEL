//
// The peristent data members of a helix.
// Plus some simple methods that operate on those data members.
//

#include "toyExperiment/RecoDataProducts/Helix.h"

#include <iostream>
#include <stdexcept>

tex::Helix::Helix() : par_(dim, 0) {}

tex::Helix::Helix(CLHEP::HepVector const& v) : par_(v) {}

tex::Helix::Helix(double cu, double phi0, double d0, double ct, double z0)
  : par_(dim, 0)
{
  par_[icu] = cu;
  par_[iphi0] = phi0;
  par_[id0] = d0;
  par_[ict] = ct;
  par_[iz0] = z0;
}

tex::Helix::Helix(CLHEP::Hep3Vector const& pos,
                  CLHEP::Hep3Vector const& p,
                  double q,
                  double bz)
  : par_(dim, 0)
{

  double pt(p.perp());

  if (pt == 0.) {
    throw std::range_error("Cannot compute helix representation if pt is 0.\n");
  }

  // Conversion factor from momentum to radius of curvature.
  double a = k() * bz * q;

  // Reciprocal radius of curvature, signed with the geometric charge.
  double rInv = a / pt;

  double cu = 0.5 * a / pt;
  double phi0 = p.phi();

  double cross = (pos.x() * p.y() - pos.y() * p.x()) / pt;
  double dot = (pos.x() * p.x() + pos.y() * p.y()) / pt;

  // Answer for a track that is a straight line.
  double d0(-cross);
  double s(dot);

  // Answer for a track that is curved.
  if (rInv != 0.) {
    double posPerp = pos.perp2();
    double disc = sqrt(1. - 2. * rInv * cross + rInv * rInv * posPerp);
    d0 = (-2. * cross + rInv * posPerp) / (disc + 1.);
    s = std::atan2(rInv * dot, 1 - rInv * cross) / rInv;
  }

  double ct = p.z() / pt;
  double z0 = pos.z() - ct * s;

  par_[icu] = cu;
  par_[iphi0] = phi0;
  par_[id0] = d0;
  par_[ict] = ct;
  par_[iz0] = z0;
}

// Track position at specified 3d arc length relative to POCA.
CLHEP::Hep3Vector
tex::Helix::position(double s) const
{

  // Turning angle from POCA to the postion at the requested arc length.
  double phi = 2. * s * cu() * sinTheta();

  // Signed radius from origin to center of curvature.
  double rc = d0() - 0.5 / cu();

  // Center of curvature.
  double xc = -rc * sin(phi0());
  double yc = rc * cos(phi0());

  double x = xc + 0.5 / cu() * sin(phi - phi0());
  double y = yc + 0.5 / cu() * cos(phi - phi0());
  double z = z0() + s * cosTheta();

  return CLHEP::Hep3Vector(x, y, z);
}

CLHEP::Hep3Vector
tex::Helix::momentum(double s, double bz) const
{

  // Turning angle from POCA to the postion at the requested arc length.
  double phi = 2. * s * cu() * sinTheta();

  double p_t = pt(bz);
  double px = p_t * cos(phi0() - phi);
  double py = p_t * sin(phi0() - phi);
  double pz = p_t * ct();

  return CLHEP::Hep3Vector(px, py, pz);
}

CLHEP::HepLorentzVector
tex::Helix::fourMomentum(double s, double m, double bz) const
{
  double e = sqrt(p(bz) * p(bz) + m * m);
  return CLHEP::HepLorentzVector(momentum(s, bz), e);
}

double
tex::Helix::pt(double bz) const
{
  return std::abs(0.5 * Helix::k() * bz / cu());
}

double
tex::Helix::p(double bz) const
{
  return pt(bz) / sinTheta();
}

double
tex::Helix::sinTheta() const
{
  return 1. / sqrt(1. + ct() * ct());
}

double
tex::Helix::cosTheta() const
{
  return ct() / sqrt(1. + ct() * ct());
}

std::ostream&
tex::operator<<(std::ostream& ost, tex::Helix const& helix)
{

  ost << "( cu: " << helix.cu() << " d0: " << helix.d0()
      << " phi0: " << helix.phi0() << " ct: " << helix.ct()
      << " z0: " << helix.z0() << " )";

  return ost;
}
