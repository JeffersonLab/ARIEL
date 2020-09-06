//
// Make the geometry description available to modules and to other services.
//

#include "toyExperiment/Conditions/Conditions.h"
#include "toyExperiment/Geometry/Tracker.h"
#include "toyExperiment/Utilities/ParameterSetFromFile.h"

#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

#include <iomanip>
#include <iostream>
#include <sstream>

tex::Conditions::Conditions(fhicl::ParameterSet const& pset,
                            art::ActivityRegistry& iRegistry)
  : _conditionsFile(pset.get<std::string>("conditionsFile"))
  , _verbosity(pset.get<int>("verbosity", 0))
  , _geom(art::ServiceHandle<Geometry>())
{

  iRegistry.sPreBeginRun.watch(this, &Conditions::preBeginRun);

  if (_verbosity > 0) {
    std::cout << "Conditions: "
              << " Conditions file: " << _conditionsFile
              << " verbosity: " << _verbosity << std::endl;
  }
}

void
tex::Conditions::preBeginRun(art::Run const&)
{

  // In real life we might update conditions on every new run or new subRun.
  // In this simple example we create the conditions information once and never
  // update it.
  static bool doneOnce(false);
  if (doneOnce)
    return;
  doneOnce = true;

  // Parse the geometry .fcl file into a parameter set.
  ParameterSetFromFile pSetFile(_conditionsFile);

  // Construct the information for the tracker.
  fhicl::ParameterSet trackerPSet =
    pSetFile.pSet().get<fhicl::ParameterSet>("tracker");
  makeShellConditions(trackerPSet);

  if (_verbosity > 2) {
    std::cout << "Conditons data for each shell in the tracker: " << std::endl;
    for (auto const& cond : _shellConditions) {
      std::cout << "   " << cond << std::endl;
    }
  }
}

void
tex::Conditions::makeShellConditions(fhicl::ParameterSet const& pset)
{

  // Get number of shells from the geometry service.
  size_t nShells = _geom->tracker().nShells();

  // Fill the conditions information for all shells.
  _shellConditions.reserve(nShells);
  for (size_t i = 0; i < nShells; ++i) {

    // Format the name of the ParameterSet for each shell.
    std::ostringstream os;
    os << "shell" << std::setfill('0') << std::setw(2) << i;

    addOneShell(i, pset.get<fhicl::ParameterSet>(os.str()));
  }
}

void
tex::Conditions::addOneShell(int id, fhicl::ParameterSet const& pset)
{

  double eff = pset.get<double>("eff");
  double sigZ = pset.get<double>("sigZ");
  double sigPhi = pset.get<double>("sigPhi");
  _shellConditions.emplace_back(id, eff, sigZ, sigPhi);
}

DEFINE_ART_SERVICE(tex::Conditions)
