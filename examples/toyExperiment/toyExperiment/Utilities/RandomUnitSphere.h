#ifndef Mu2eUtilities_RandomUnitSphere_h
#define Mu2eUtilities_RandomUnitSphere_h

//
// Return CLHEP::Hep3Vector objects that are unit vectors uniformly
// distributed over a specified section of the unit sphere.
//

#include "toyExperiment/Utilities/polar3Vector.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Vector/ThreeVector.h"

#include <cmath>

namespace tex {

  struct RandomUnitSphereParams {
    double czmin;
    double czmax;
    double phimin;
    double phimax;
    RandomUnitSphereParams(double ci, double ca, double pi, double pa)
      : czmin(ci), czmax(ca), phimin(pi), phimax(pa) {}
  };

  class RandomUnitSphere {

  public:

    explicit RandomUnitSphere( CLHEP::HepRandomEngine& engine,
                               double czmin=-1.,
                               double czmax=1.,
                               double phimin=0.,
                               double phimax=CLHEP::twopi);

    explicit RandomUnitSphere( CLHEP::HepRandomEngine& engine,
                               const RandomUnitSphereParams& par);

    CLHEP::Hep3Vector fire();

    // Alternate fire syntax which modifies the magnitude of the vector.
    CLHEP::Hep3Vector fire( double magnitude ){
      return magnitude*fire();
    }

    void setczmin(double czmin){
      _czmin=czmin;
    }

    void setczmax(double czmax){
      _czmax=czmax;
    }

    void setphimin(double phimin){
      _phimin=phimin;
    }

    void setphimax(double phimax){
      _phimax=phimax;
    }

    double czmin(){ return _czmin;}
    double czmax(){ return _czmax;}

    double phimin(){ return _phimin;}
    double phimax(){ return _phimax;}

    CLHEP::HepRandomEngine& engine(){
      return _randFlat.engine();
    }

  private:

    double _czmin;
    double _czmax;
    double _phimin;
    double _phimax;

    // The underlying uniform random number distribution.
    CLHEP::RandFlat _randFlat;

  };

}

#endif /* Mu2eUtilities_RandomUnitSphere_h */
