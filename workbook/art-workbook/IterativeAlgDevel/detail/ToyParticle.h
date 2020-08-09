#ifndef IterativeAlgDevel_ToyParticle_h
#define IterativeAlgDevel_ToyParticle_h

#include "toyExperiment/DataProducts/PDGCode.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"

namespace helper {
  namespace detail {

    using PDGCode = tex::PDGCode;

    class ToyParticle {
    public:

      ToyParticle( PDGCode::type                  pdgId,
                   CLHEP::Hep3Vector const&       position,
                   CLHEP::HepLorentzVector const& momentum )
        : pdgId_(pdgId)
        , position_(position)
        , momentum_(momentum)
      {}

      PDGCode::type                  pdgId()    const { return  pdgId_;    }
      CLHEP::Hep3Vector const&       position() const { return  position_; }
      CLHEP::HepLorentzVector const& momentum() const { return  momentum_; }

    private:

      // PDG particle ID code.
      PDGCode::type pdgId_;

      // Position, momentum at creation.
      CLHEP::Hep3Vector       position_;
      CLHEP::HepLorentzVector momentum_;
    };

  }
}

#endif

// Local variables:
// mode: c++
// End:
