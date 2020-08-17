//
// Information about the MC truth for hits in the tracker.
//

#include "toyExperiment/MCDataProducts/Intersection.h"

tex::Intersection::Intersection() : _shell(-1), _genTrack(), _position() {}

tex::Intersection::Intersection(int shell,
                                art::Ptr<GenParticle> const& genTrack,
                                CLHEP::Hep3Vector const& position)
  : _shell(shell), _genTrack(genTrack), _position(position)
{}

std::ostream&
tex::operator<<(std::ostream& ost, const tex::Intersection& intersec)
{
  ost << "Intersection: shell number: " << intersec.shell()
      << " GenParticle: " << intersec.genTrack().id() << " "
      << intersec.genTrack().key() << " Point: " << intersec.position();
  return ost;
}
