//
//  Introduce the begin* and end* methods.
//

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"

#include <iostream>

namespace tex {

  class OptionalBug01 : public art::EDAnalyzer {

  public:

    explicit OptionalBug01(fhicl::ParameterSet const& );

    void beginJob   (                           ) override;
    void beginSubRun( art::SubRun const& subRun ) override;
    void analyze    ( art::Event const&  event  ) override;
    void beginrun   ( art::Run&          run    );

  };

}

tex::OptionalBug01::OptionalBug01(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset){
  std::cout << "Hello from OptionalBug01::constructor." << std::endl;
}

void
tex::OptionalBug01::beginJob( ){
  std::cout << "Hello from OptionalBug01::beginJob." << std::endl;
}

void
tex::OptionalBug01::beginrun( art::Run& run ){
  std::cout << "Hello from OptionalBug01::beginRun: "
            << run.id()
            << std::endl;
}

void
tex::OptionalBug01::beginSubRun( art::SubRun const& subRun ){
  std::cout << "Hello from OptionalBug01::beginSubRun: "
            << subRun.id()
            << std::endl;
}


void
tex::OptionalBug01::analyze(art::Event const& event){
  std::cout << "Hello from OptionalBug01::analyze. Event id: "
            << event.id()
            << std::endl;
}

DEFINE_ART_MODULE(tex::OptionalBug01)
