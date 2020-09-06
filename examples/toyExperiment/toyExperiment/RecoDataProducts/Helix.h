#ifndef RecoDataProducts_Helix_h
#define RecoDataProducts_Helix_h

//
// The representation of a helix.
//
// Notes:
//
// 1) The parameter vector has the following parameters, in order:
//    0 _cu;   // q/2R = diameter of curvature, signed with the geometric
//    charge. 1 _phi0; // phi at POCA to z axis 2 _d0;   // DOCA to z axis,
//    signed. 3 _ct;   // cot(theta) = tan(dip angle) 4 _z0;   // z at POCA to z
//    axis
//
// 2) The enum index_type defines 0-based indices to the above parameters.
//
// 3) The HepVector and HepSymMatrx classes have a subtlety in their public
// interface.
//    Consider:
//      HepVector v(dimension);
//      HepSymMatrix m(dimension);
//
//    These methods used 0-based indexing!
//    v[i], m[i][j] are 0-based.
//
//    But! These methods used 1-based indexing!
//    v(i), m(i,j)  are 1-based
//
//    The enum index_type is designed to be used with the [] operators.
//

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"

#include <iosfwd>

namespace tex {

  class Helix {

  public:
    // Indices in HepSymMatrix and HepVector; see notes 1 through 3.
    enum index_type { icu = 0, iphi0, id0, ict, iz0 };

    // Dimension of the parameter vector and of the covariance matrix.
    enum dimension_type { dim = 5 };

    Helix();

    Helix(CLHEP::HepVector const& v);

    Helix(double cu, double phi0, double d0, double ct, double z0);

    Helix(CLHEP::Hep3Vector const& position,
          CLHEP::Hep3Vector const& momentum,
          double q,
          double bz);

    /*
    Helix ( CLHEP::Hep3Vector const& momentum,
            double q,
            double bz):
    Helix ( momentum, CLHEP::Hep3Vector(), q, bz){
    }
    */

    CLHEP::HepVector
    par() const
    {
      return par_;
    }

    // The 5 helical track parameters; geometrical information only; see note 1.
    double
    cu() const
    {
      return par_[Helix::icu];
    }
    double
    phi0() const
    {
      return par_[Helix::iphi0];
    }
    double
    d0() const
    {
      return par_[Helix::id0];
    }
    double
    ct() const
    {
      return par_[Helix::ict];
    }
    double
    z0() const
    {
      return par_[Helix::iz0];
    }

    // Conversion factor between p_T and bend radius.
    // For p_T in MeV, radius in mm and B in Tesla.
    static double
    k()
    {
      return CLHEP::c_light * 1.e-3;
    }

    // Magnitude of the radius of curvature.
    double
    rho() const
    {
      return std::abs(0.5 / cu());
    }

    // Momentum magnitude and magnitude of the transverse momentum; both are
    // unsigned.
    double p(double bz) const;
    double pt(double bz) const;

    // Position and momentum, given a 3D path length from the POCA.
    CLHEP::Hep3Vector position(double s) const;
    CLHEP::Hep3Vector momentum(double s, double bz) const;
    CLHEP::HepLorentzVector fourMomentum(double s, double m, double bz) const;

    // Electric charge.
    int
    q(double bz) const
    {
      return (bz >= 0.) ? qgeo() : -qgeo();
    }

    // Geometric charge
    int
    qgeo() const
    {
      return (cu() > 0) ? 1 : -1;
    }

    // Derived information about the helix pitch: returns values on [0,1]
    double sinTheta() const;

    // Derived information about the helix pitch: returns values on [-1, 1.]
    double cosTheta() const;

    // A signed quantity.
    double
    radiusToCenter() const
    {
      return std::abs(d0() - 0.5 / cu());
    }

  private:
    CLHEP::HepVector par_;
  };

  std::ostream& operator<<(std::ostream& ost, const tex::Helix& helix);
}
#endif /* RecoDataProducts_Helix_h */
