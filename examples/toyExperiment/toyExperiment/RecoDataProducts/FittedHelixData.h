#ifndef RecoDataProducts_FittedHelixData_h
#define RecoDataProducts_FittedHelixData_h

//
// The persistent data for a fitted helix.
//


#include "toyExperiment/RecoDataProducts/Helix.h"
#include "toyExperiment/RecoDataProducts/TrkHit.h"

#include "canvas/Persistency/Common/Ptr.h"

#include "CLHEP/Matrix/SymMatrix.h"

#include <ostream>
#include <vector>

namespace tex {

  class FittedHelixData {

  public:

    // Genreflex still needs the space between > >.  C++11 does not.
    typedef std::vector<art::Ptr<TrkHit> >  hits_type;

    FittedHelixData();

    FittedHelixData( Helix               const& helix,
                     CLHEP::HepSymMatrix const& cov,
                     hits_type           const& hits );

    Helix               const& helix() const { return helix_; }
    CLHEP::HepSymMatrix const& cov()   const { return cov_;   }
    hits_type           const& hits()  const { return hits_;  }

    art::Ptr<TrkHit>    const& hit ( size_t i ) const { return hits_.at(i); }

  private:

    Helix               helix_;
    CLHEP::HepSymMatrix cov_;
    hits_type           hits_;

  };

  std::ostream& operator<<(std::ostream& ost,
                           const tex::FittedHelixData& fit );
}

#endif /* RecoDataProducts_FittedHelixData_h */
