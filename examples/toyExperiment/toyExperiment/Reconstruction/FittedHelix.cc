
//
// Read facade class for FittedHelixData
//

#include "toyExperiment/Reconstruction/FittedHelix.h"
#include "toyExperiment/Geometry/Geometry.h"

#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "CLHEP/Matrix/Matrix.h"

#include "cetlib/pow.h"
#include "cetlib_except/exception.h"

tex::FittedHelix::FittedHelix(FittedHelixData const& fit)
  : fit_(fit), bz_(art::ServiceHandle<Geometry>()->bz())
{}

tex::FittedHelix::FittedHelix(FittedHelixData const& fit, double const bz)
  : fit_(fit), bz_(bz)
{}

CLHEP::HepLorentzVector
tex::FittedHelix::lorentzAtPoca(double m) const
{
  double e = sqrt(cet::square(fit_.helix().p(bz_)) + cet::square(m));
  return CLHEP::HepLorentzVector(momAtPoca(), e);
}

CLHEP::HepSymMatrix
tex::FittedHelix::lorentzAtPocaCov(double m) const
{

  CLHEP::HepLorentzVector p(lorentzAtPoca(m));

  // Dimension of a 4-vector.
  static const size_t dim4(4);

  if (cu() == 0.) {
    throw cet::exception("RECO")
      << "Cannot compute cartesian covariance matrix if curvature is 0. \n";
  }

  if (p.e() == 0.) {
    throw cet::exception("RECO")
      << "Cannot compute cartesian covariance matrix if energy is 0.\n";
  }

  double cuinv = 1. / cu();
  double pmagsq = p.vect().mag2();
  double einv = 1. / p.e();
  double ptrans = fit_.helix().pt(bz_);
  double ptsq = ptrans * ptrans;

  // Derivative of cartesian rep wrt helical rep.
  CLHEP::HepMatrix a(dim4, Helix::dim, 0);

  // d p_i / d cu()
  a[0][icu] = -p.x() * cuinv;
  a[1][icu] = -p.y() * cuinv;
  a[2][icu] = -p.z() * cuinv;
  a[3][icu] = -pmagsq * einv * cuinv;

  // d p_i / d phi0()
  a[0][iphi0] = -p.y();
  a[1][iphi0] = p.x();

  // d p_i / d d0() all are zero.

  // d p_i / d ct()
  a[2][ict] = ptrans;
  a[3][ict] = ct() * ptsq * einv;

  // d p_i / d z0() all are zero.

  // Perform the similarity transformation to the cartesian basis.
  CLHEP::HepSymMatrix c = cov().similarity(a);

  return c;
}
