#ifndef RecoProducts_FittedHelix_h
#define RecoProducts_FittedHelix_h

//
// Read facade class for FittedHelixData
//
// Notes:
//
// 1) Why is there a virtual destructor?  If we need a write facade,
//    that class will need to inherit from this one; so anticipate that
//    by making the d'tor virtual.

#include "toyExperiment/RecoDataProducts/FittedHelixData.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"

#include <ostream>

namespace tex {

  class FittedHelix {

  public:
    typedef FittedHelixData::hits_type hits_type;

    // Indices in HepSymMatrix and HepVector; see note 1.
    enum symMatrix_index_type {
      icu = Helix::icu,
      iphi0 = Helix::iphi0,
      id0 = Helix::id0,
      ict = Helix::ict,
      iz0 = Helix::iz0
    };

    explicit FittedHelix(FittedHelixData const& fit);
    explicit FittedHelix(FittedHelixData const& fit, double const bz);

    // See note 1.
    virtual ~FittedHelix() {}

    // Return the underlying data product.
    FittedHelixData const&
    data() const
    {
      return fit_;
    }

    // Forward the simple accessor functions of the underlying helix class
    Helix const&
    helix() const
    {
      return fit_.helix();
    }
    CLHEP::HepSymMatrix const&
    cov() const
    {
      return fit_.cov();
    }
    hits_type const&
    hits() const
    {
      return fit_.hits();
    }

    double
    cu() const
    {
      return fit_.helix().cu();
    }
    double
    phi0() const
    {
      return fit_.helix().phi0();
    }
    double
    d0() const
    {
      return fit_.helix().d0();
    }
    double
    ct() const
    {
      return fit_.helix().ct();
    }
    double
    z0() const
    {
      return fit_.helix().z0();
    }

    int
    q() const
    {
      return fit_.helix().q(bz_);
    }
    int
    qgeo() const
    {
      return fit_.helix().qgeo();
    }

    art::Ptr<TrkHit> const&
    hit(size_t i) const
    {
      return fit_.hit(i);
    }

    // Additional functions provided by this class.
    CLHEP::Hep3Vector
    poca() const
    {
      return fit_.helix().position(0.);
    }

    CLHEP::Hep3Vector
    momAtPoca() const
    {
      return fit_.helix().momentum(0., bz_);
    }
    CLHEP::HepLorentzVector lorentzAtPoca(double m) const;

    CLHEP::HepSymMatrix lorentzAtPocaCov(double m) const;

  private:
    FittedHelixData const& fit_;
    double bz_;
  };

  std::ostream& operator<<(std::ostream& ost, const FittedHelix& fit);

}

#endif /* RecoProductsfit_tedHelix_h */
