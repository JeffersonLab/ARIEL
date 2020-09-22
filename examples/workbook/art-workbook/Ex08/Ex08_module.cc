//
// Exercise 8:
//  - Introduce a user supplied service, the Geometry service.
//

#include "toyExperiment/Geometry/Geometry.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"

#include <iostream>
#include <string>

namespace tex {

  class Ex08 : public art::EDAnalyzer {

  public:

    explicit Ex08(fhicl::ParameterSet const& );

    void beginRun( art::Run const& run ) override;
    void analyze(art::Event const& event) override;

  private:

    art::ServiceHandle<Geometry> geom_;

  };

}

tex::Ex08::Ex08(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),
  geom_(){
}

void tex::Ex08::beginRun( art::Run const& run ){

  std::cout << "\nGeometry at the start of run: " << run.id() << std::endl;

  auto const& world(geom_->worldHalfLengths());
  std::cout << "World half lengths: "
            << world.at(0) << " mm "
            << world.at(1) << " mm "
            << world.at(2) << " mm "
            << std::endl;

  double bz(geom_->bz());
  std::cout << "\nMagnetic field B_z is: " << bz << " Tesla" << std::endl;

  Tracker const& tracker(geom_->tracker());
  std::cout << "\nNumber of tracker shells: " << tracker.nShells() << std::endl;

  for ( auto const& shell : tracker.shells() ){
    std::cout << "    Shell: "      << std::setw(3) << shell.id()
              << "; radius: "       << std::setw(4) << shell.radius()
              << "mm; halfLength: " << std::setw(4) << shell.halfLength()
              << "mm"
              << std::endl;
  }

}

void tex::Ex08::analyze(art::Event const& ){
}

DEFINE_ART_MODULE(tex::Ex08)
