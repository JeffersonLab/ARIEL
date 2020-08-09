#ifndef MCDataProducts_GenParticle_h
#define MCDataProducts_GenParticle_h

//
// A minimal class to hold information about generated particles.
//

#include "toyExperiment/DataProducts/PDGCode.h"

#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"

#include <iosfwd>

namespace tex {

  class GenParticle {

  public:

    typedef art::PtrVector<GenParticle> children_type;

    GenParticle();

    GenParticle( PDGCode::type                  pdgId,
                 art::Ptr<GenParticle> const&   parent,
                 CLHEP::Hep3Vector const&       position,
                 CLHEP::HepLorentzVector const& momentum );

    PDGCode::type                  pdgId()           const { return  _pdgId;              }
    bool                           hasParent()       const { return  _parent.isNonnull(); }
    bool                           hasChildren()     const { return !_children.empty();   }
    art::Ptr<GenParticle> const&   parent()          const { return  _parent;             }
    children_type const&           children()        const { return  _children;           }
    art::Ptr<GenParticle> const&   child( size_t i ) const { return  _children.at(i);     }
    CLHEP::Hep3Vector const&       position()        const { return  _position;           }
    CLHEP::HepLorentzVector const& momentum()        const { return  _momentum;           }

    void addChild( art::Ptr<GenParticle> const& child );

  private:

    // PDG particle ID code.
    PDGCode::type _pdgId;

    // Ancestry navigation.
    art::Ptr<GenParticle> _parent;
    children_type         _children;

    // Position, momentum at creation.
    CLHEP::Hep3Vector       _position;
    CLHEP::HepLorentzVector _momentum;

  };

  std::ostream& operator<<(std::ostream& ost,
                           const tex::GenParticle& genp );
}

#endif /* MCDataProducts_GenParticle_h */
