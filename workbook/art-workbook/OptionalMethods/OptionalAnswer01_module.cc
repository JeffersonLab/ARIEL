//
//  Add the end* methods.
//

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"

#include <iostream>

namespace tex {

  class OptionalAnswer01 : public art::EDAnalyzer {

  public:

    explicit OptionalAnswer01(fhicl::ParameterSet const& );
    void beginJob   () override;
    void endJob     () override;
    void beginRun   ( art::Run const&    run    ) override;
    void endRun     ( art::Run const&    run    ) override;
    void beginSubRun( art::SubRun const& subRun ) override;
    void endSubRun  ( art::SubRun const& subRun ) override;
    void analyze    ( art::Event const&  event  ) override;

  };

}

tex::OptionalAnswer01::OptionalAnswer01(fhicl::ParameterSet const& pset):
  art::EDAnalyzer(pset){
  std::cout << "Hello from OptionalAnswer01::constructor." << std::endl;
}

void
tex::OptionalAnswer01::beginJob( ){
  std::cout << "Hello from OptionalAnswer01::beginJob." << std::endl;
}

void
tex::OptionalAnswer01::endJob( ){
  std::cout << "Hello from OptionalAnswer01::endJob." << std::endl;
}

void
tex::OptionalAnswer01::beginRun( art::Run const& run ){
  std::cout << "Hello from OptionalAnswer01::beginRun: "
            << run.id()
            << std::endl;
}

void
tex::OptionalAnswer01::endRun( art::Run const& run ){
  std::cout << "Hello from OptionalAnswer01::endRun: "
            << run.id()
            << std::endl;
}

void
tex::OptionalAnswer01::beginSubRun( art::SubRun const& subRun ){
  std::cout << "Hello from OptionalAnswer01::beginSubRun: "
            << subRun.id()
            << std::endl;
}

void
tex::OptionalAnswer01::endSubRun( art::SubRun const& subRun ){
  std::cout << "Hello from OptionalAnswer01::endSubRun: "
            << subRun.id()
            << std::endl;
}

void
tex::OptionalAnswer01::analyze(art::Event const& event){
  std::cout << "Hello from OptionalAnswer01::analyze. Event id: "
            << event.id()
            << std::endl;
}

DEFINE_ART_MODULE(tex::OptionalAnswer01)
