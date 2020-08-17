//
// Make the geometry description available to modules and to other services.
//

#include "toyExperiment/Geometry/Geometry.h"
#include "toyExperiment/Utilities/ParameterSetFromFile.h"
#include "toyExperiment/Utilities/ParameterSetHelpers.h"

#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

#include <iostream>

tex::Geometry::Geometry(fhicl::ParameterSet const& pset,
                        art::ActivityRegistry& iRegistry)
  : _verbosity(pset.get<int>("verbosity", 0))
  , _geometryFile(pset.get<std::string>("geometryFile"))
  , _tracker()
{
  iRegistry.sPreBeginRun.watch(this, &Geometry::preBeginRun);
}

void
tex::Geometry::preBeginRun(art::Run const& run)
{

  // In real life we might update geometry on every new run or new subRun.
  // In this simple example we create the geometry once and never update it.
  static bool doneOnce(false);
  if (doneOnce)
    return;
  doneOnce = true;

  if (_verbosity > 0) {
    std::cout << "Geometry::preBeginRun: "
              << " " << run.id() << " Geometry file: " << _geometryFile
              << std::endl;
  }

  // Parse the geometry .fcl file into a parameter set.
  ParameterSetFromFile pSetFile(_geometryFile);

  // Construct the tracker.
  fhicl::ParameterSet trackerPSet =
    pSetFile.pSet().get<fhicl::ParameterSet>("tracker");
  makeTracker(trackerPSet);

  if (_verbosity > 1) {
    std::cout << _tracker << std::endl;
  }

  // Construct the luminous region
  fhicl::ParameterSet luminousRegionPSet =
    pSetFile.pSet().get<fhicl::ParameterSet>("luminousRegion");
  makeLuminousRegion(luminousRegionPSet);

  if (_verbosity > 1) {
    std::cout << _luminousRegion << std::endl;
  }

  // Construct the magnetic field information.
  fhicl::ParameterSet bFieldPSet =
    pSetFile.pSet().get<fhicl::ParameterSet>("bfield");
  makeBField(bFieldPSet);
  if (_verbosity > 1) {
    std::cout << "B_z is: " << _bz << std::endl;
  }

  // Construct the bounding box information.
  fhicl::ParameterSet worldPSet =
    pSetFile.pSet().get<fhicl::ParameterSet>("world");
  makeWorldBox(worldPSet);
  if (_verbosity > 1) {
    std::cout << "Half lengths of the world box are: " << _worldHalfLengths[0]
              << " " << _worldHalfLengths[1] << " " << _worldHalfLengths[2]
              << std::endl;
  }

} // preBeginRun()

void
tex::Geometry::makeTracker(fhicl::ParameterSet const& pSet)
{

  int id(0);
  for (auto const& component : TrackerComponent::knownNames()) {

    fhicl::ParameterSet p = pSet.get<fhicl::ParameterSet>(component.second);

    size_t nShells = p.get<size_t>("nShells");
    double r0 = p.get<double>("r0");
    double dr = p.get<double>("dr");
    double hlen = p.get<double>("halfLength");
    double hthick = p.get<double>("halfThickness");
    double sigma = p.get<double>("sigma");

    _tracker.reserve(_tracker.nShells() + nShells);

    for (size_t i = 0; i < nShells; ++i) {
      double r = r0 + i * dr;
      _tracker.addShell(Shell(id, component.first, r, hlen, hthick, sigma));
      id++;
    }
  }

} // end makeTracker

void
tex::Geometry::makeLuminousRegion(fhicl::ParameterSet const& pSet)
{
  _luminousRegion = LuminousRegion(pSet.get<CLHEP::Hep3Vector>("center"),
                                   pSet.get<CLHEP::Hep3Vector>("sigma"));
} // end makeLuminousRegion

void
tex::Geometry::makeBField(fhicl::ParameterSet const& pSet)
{
  _bz = pSet.get<double>("bz");
}

void
tex::Geometry::makeWorldBox(fhicl::ParameterSet const& pSet)
{
  _worldHalfLengths = pSet.get<std::vector<double>>("halfLengths");
}

bool
tex::Geometry::insideWorld(CLHEP::Hep3Vector const& p)
{
  if (std::abs(p.x()) > _worldHalfLengths[0])
    return false;
  if (std::abs(p.y()) > _worldHalfLengths[1])
    return false;
  if (std::abs(p.z()) > _worldHalfLengths[2])
    return false;
  return true;
}

DEFINE_ART_SERVICE(tex::Geometry)
