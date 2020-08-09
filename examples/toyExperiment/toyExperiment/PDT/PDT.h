#ifndef PDT_PDT_h
#define PDT_PDT_h
//
// Serve the Particle Data Table information.
//

#include "toyExperiment/PDT/ParticleInfo.h"

#include "art/Framework/Services/Registry/ServiceMacros.h"

#include <map>
#include <string>
#include <ostream>

namespace art {
  class ActivityRegistry;
}

namespace fhicl {
  class ParameterSet;
}

namespace tex {

  class PDT {

public:
    PDT(const fhicl::ParameterSet&, art::ActivityRegistry&);

    // Accessors
    ParticleInfo const& getById( PDGCode::type ) const;
    void print( std::ostream& ) const;

private:

    std::string _pdtFile;
    int _verbosity;

    typedef std::map<PDGCode::type,ParticleInfo> map_type;
    map_type _pdt;

  };

  std::ostream& operator<<(std::ostream& ost,
                           const PDT& t );

}

DECLARE_ART_SERVICE( tex::PDT, LEGACY )

#endif /* PDT_PDT_h */
