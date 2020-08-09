//
//  The "Hello World" module.
//

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>

namespace tex {

  class HelloWorld : public art::EDAnalyzer {

  public:

    explicit HelloWorld(fhicl::ParameterSet const& );

    void analyze(art::Event const& event) override;

  };

}

tex::HelloWorld::HelloWorld(fhicl::ParameterSet const& pset):
  art::EDAnalyzer(pset){
}

void tex::HelloWorld::analyze(art::Event const& event){
  std::cout << "Hello World! This event has the id: "
            << event.id()
            << std::endl;
}

DEFINE_ART_MODULE(tex::HelloWorld)
