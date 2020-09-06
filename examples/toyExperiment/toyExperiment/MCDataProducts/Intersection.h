#ifndef MCDataProducts_Intersection_h
#define MCDataProducts_Intersection_h

//
// Information about the MC truth for hits in the tracker.
//

#include "toyExperiment/MCDataProducts/GenParticle.h"

#include "canvas/Persistency/Common/Ptr.h"

#include "CLHEP/Vector/ThreeVector.h"

#include <ostream>

namespace tex {

  class Intersection {

  public:
    Intersection();

    Intersection(int shell,
                 art::Ptr<GenParticle> const& genTrack,
                 CLHEP::Hep3Vector const& position);

    int
    shell() const
    {
      return _shell;
    }
    art::Ptr<GenParticle> const&
    genTrack() const
    {
      return _genTrack;
    }
    CLHEP::Hep3Vector const&
    position() const
    {
      return _position;
    }

  private:
    int _shell;
    art::Ptr<GenParticle> _genTrack;
    CLHEP::Hep3Vector _position;
  };

  std::ostream& operator<<(std::ostream& ost, const Intersection& hit);

}

#endif /* MCDataProducts_Intersection_h */
