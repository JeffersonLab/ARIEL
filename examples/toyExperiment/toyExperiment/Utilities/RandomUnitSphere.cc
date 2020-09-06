//
// Return CLHEP::Hep3Vector objects that are unit vectors uniformly
// distributed over a specified section of the unit sphere.
//

#include "toyExperiment/Utilities/RandomUnitSphere.h"
#include "toyExperiment/Utilities/polar3Vector.h"

#include "art/Framework/Services/Optional/RandomNumberGenerator.h"

tex::RandomUnitSphere::RandomUnitSphere(CLHEP::HepRandomEngine& engine,
                                        double czmin,
                                        double czmax,
                                        double phimin,
                                        double phimax)
  : _czmin(czmin)
  , _czmax(czmax)
  , _phimin(phimin)
  , _phimax(phimax)
  , _randFlat(engine)
{}

tex::RandomUnitSphere::RandomUnitSphere(CLHEP::HepRandomEngine& engine,
                                        const RandomUnitSphereParams& pars)
  : _czmin(pars.czmin)
  , _czmax(pars.czmax)
  , _phimin(pars.phimin)
  , _phimax(pars.phimax)
  , _randFlat(engine)
{}

CLHEP::Hep3Vector
tex::RandomUnitSphere::fire()
{
  double cz = _czmin + (_czmax - _czmin) * _randFlat.fire();
  double phi = _phimin + (_phimax - _phimin) * _randFlat.fire();
  return polar3Vector(1., cz, phi);
}
