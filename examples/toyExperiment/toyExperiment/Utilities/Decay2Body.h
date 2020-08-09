#ifndef Utilities_Decay2Body_h
#define Utilities_Decay2Body_h

//
// Compuate a 2 body decay: 0 -> 1 2
//

#include "toyExperiment/Utilities/TwoBodyKinematics.h"
#include "toyExperiment/Utilities/RandomUnitSphere.h"

#include "CLHEP/Vector/LorentzVector.h"

namespace tex {

  class Decay2Body{

  public:

    Decay2Body( CLHEP::HepLorentzVector const& p0,
                double m1,
                double m2,
                RandomUnitSphere& unitSphere );

    // Accessors.
    double m0() const { return _m0;}
    double m1() const { return _m1;}
    double m2() const { return _m2;}

    TwoBodyKinematics const& kinematics() const { return _kine; }

    CLHEP::HepLorentzVector const& p0() const { return _p0; }
    CLHEP::HepLorentzVector const& p1() const { return _p1; }
    CLHEP::HepLorentzVector const& p2() const { return _p2; }

    RandomUnitSphere& randomUnitSphere() { return _unitSphere; }

  private:

    CLHEP::HepLorentzVector _p0;
    double _m0, _m1, _m2;

    RandomUnitSphere& _unitSphere;

    TwoBodyKinematics       _kine;
    CLHEP::HepLorentzVector _p1;
    CLHEP::HepLorentzVector _p2;

  };

} // end namespace tex

#endif /* Utilities_Decay2Body_h */
