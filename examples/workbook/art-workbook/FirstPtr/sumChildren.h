#ifndef FirstPtr_sumChildren_h
#define FirstPtr_sumChildren_h
//
// Count the number of times each PDG Id appears.
// Print a table of the results when requested.
//

#include "CLHEP/Vector/LorentzVector.h"

namespace tex {

  class GenParticle;

  CLHEP::HepLorentzVector sumChildren( tex::GenParticle const& gen );


} // end namespace tex

#endif /* FirstPtr_sumChildren_h */
