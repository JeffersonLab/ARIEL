#ifndef Conditions_Conditions_h
#define Conditions_Conditions_h
//
// Make the conditions information available to modules and to other services.
//

#include "toyExperiment/Conditions/ShellConditions.h"
#include "toyExperiment/Geometry/Geometry.h"

#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "fhiclcpp/ParameterSet.h"

#include <string>

namespace art{
  class ActivityRegistry;
}

namespace tex {

  class Conditions {

public:
    Conditions(const fhicl::ParameterSet&, art::ActivityRegistry&);

    // Accessors
    ShellConditions const& shellConditions( size_t id) { return _shellConditions.at(id); }

    // Called by art.
    void preBeginRun( art::Run const &run);

private:

    std::string _conditionsFile;
    int _verbosity;

    art::ServiceHandle<Geometry> _geom;

    std::vector<ShellConditions> _shellConditions;

    void makeShellConditions ( fhicl::ParameterSet const& );
    void addOneShell         ( int id, fhicl::ParameterSet const& );

  };

}

DECLARE_ART_SERVICE( tex::Conditions, LEGACY )

#endif /* Conditions_Conditions_h */
