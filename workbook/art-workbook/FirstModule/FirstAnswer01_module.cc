//
//  Answer to the first activity in exercise 1.
//

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>

namespace tex {

  class FirstAnswer01 : public art::EDAnalyzer {

  public:

    explicit FirstAnswer01(fhicl::ParameterSet const& );

    void analyze(art::Event const& event) override;

  };

}

tex::FirstAnswer01::FirstAnswer01(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset){
  std::cout << "Hello from FirstAnswer01::constructor." << std::endl;
}

void tex::FirstAnswer01::analyze(art::Event const& event){
  std::cout << "Hello from FirstAnswer01::analyze. "
            << " run number: "     << event.id().run()
            << " sub run number: " << event.id().subRun()
            << " event number: "   << event.id().event()
            << std::endl;
}

DEFINE_ART_MODULE(tex::FirstAnswer01)
