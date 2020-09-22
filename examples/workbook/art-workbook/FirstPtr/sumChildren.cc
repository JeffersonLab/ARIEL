
#include "toyExperiment/MCDataProducts/GenParticleCollection.h"

#include "art-workbook/FirstPtr/sumChildren.h"

namespace tex {

  CLHEP::HepLorentzVector sumChildren( tex::GenParticle const& gen ){
    CLHEP::HepLorentzVector sum;
    for ( auto const& child : gen.children() ) {
      sum += child->momentum();
    }
    return sum;
  }

}
