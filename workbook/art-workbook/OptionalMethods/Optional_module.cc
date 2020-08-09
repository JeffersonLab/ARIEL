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

  class Optional : public art::EDAnalyzer {

  public:

    explicit Optional(fhicl::ParameterSet const& );
    void beginJob   () override;
    void beginRun   ( art::Run const&    run    ) override;
    void beginSubRun( art::SubRun const& subRun ) override;
    void analyze    ( art::Event const&  event  ) override;

  };

}

tex::Optional::Optional(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset){
  std::cout << "Hello from Optional::constructor." << std::endl;
}

void
tex::Optional::beginJob( ){
  std::cout << "Hello from Optional::beginJob." << std::endl;
}

void
tex::Optional::beginRun( art::Run const& run ){
  std::cout << "Hello from Optional::beginRun: "
            << run.id()
            << std::endl;
}

void
tex::Optional::beginSubRun( art::SubRun const& subRun ){
  std::cout << "Hello from Optional::beginSubRun: "
            << subRun.id()
            << std::endl;
}


void
tex::Optional::analyze(art::Event const& event){
  std::cout << "Hello from Optional::analyze. Event id: "
            << event.id()
            << std::endl;
}

DEFINE_ART_MODULE(tex::Optional)
