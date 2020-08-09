//
// Compuate a 2 body decay: 0 -> 1 2
//

#include "toyExperiment/Utilities/Decay2Body.h"

#include "CLHEP/Vector/Boost.h"
#include "CLHEP/Vector/ThreeVector.h"

tex::Decay2Body::Decay2Body( CLHEP::HepLorentzVector const& ap0,
                               double am1,
                               double am2,
                               RandomUnitSphere& aunitSphere ):
  _p0(ap0),
  _m0(_p0.mag()),
  _m1(am1),
  _m2(am2),
  _unitSphere(aunitSphere),
  _kine(_m0, _m1, _m2){

  // Construct daughters in the rest frame of particle 0.
  CLHEP::Hep3Vector p = _unitSphere.fire( _kine.p() );
  CLHEP::HepLorentzVector q1(  p, _kine.e1() );
  CLHEP::HepLorentzVector q2( -p, _kine.e2() );

  /*
  std::cout << "Kine Check: "
            << _m0 << " "
            <<(q1+q2).mag() << " "
            << p
            << std::endl;
  */

  // Boost daughters to the lab frame.
  CLHEP::Hep3Vector beta = _p0.vect()/_p0.e();
  CLHEP::HepBoost boost(beta);
  _p1 = boost(q1);
  _p2 = boost(q2);

  /*
  std::cout << "Daughters: "
            << _kine.p() << " "
            << _p1 << " "
            << _p2 << " "
            << std::endl;
  */

  CLHEP::HepLorentzVector sum=_p1+_p2;
  /*
  std::cout << "Zero check: "
            << _p0  << " "
            << sum << " "
            << _p0-sum
            << std::endl;
  */

}
