#ifndef Geometry_Geometry_h
#define Geometry_Geometry_h

//
// Hold the geometry description and make it available to modules and to other
// services.
//

#include "toyExperiment/Geometry/LuminousRegion.h"
#include "toyExperiment/Geometry/Tracker.h"

#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "fhiclcpp/ParameterSet.h"

#include "CLHEP/Vector/ThreeVector.h"

#include <string>

namespace fhicl {
  class ParameterSet;
}

namespace tex {

  class Geometry {

  public:
    Geometry(const fhicl::ParameterSet&, art::ActivityRegistry&);

    // Accesors
    Tracker const&
    tracker() const
    {
      return _tracker;
    }
    double
    bz() const
    {
      return _bz;
    }
    std::vector<double> const&
    worldHalfLengths() const
    {
      return _worldHalfLengths;
    }
    LuminousRegion const&
    luminousRegion() const
    {
      return _luminousRegion;
    }

    // true if the given point is inside the world box.
    bool insideWorld(CLHEP::Hep3Vector const&);

    // Called by art.
    void preBeginRun(art::Run const& run);

  private:
    int _verbosity;
    std::string _geometryFile;
    Tracker _tracker;
    std::vector<double> _worldHalfLengths;
    LuminousRegion _luminousRegion;

    // The magnetic field is uniform in the z direction; this is the B_z
    // component of that field.
    double _bz;

    // Helper functions to construct the objects held within the service.
    void makeTracker(fhicl::ParameterSet const& pSet);
    void makeBField(fhicl::ParameterSet const& pSet);
    void makeWorldBox(fhicl::ParameterSet const& pSet);
    void makeLuminousRegion(fhicl::ParameterSet const& pSet);
  };

}

DECLARE_ART_SERVICE(tex::Geometry, LEGACY)

#endif /* Geometry_Geometry_h */
