//
//  Exercise 1: print the event id.
//

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>

namespace tex {

  class First : public art::EDAnalyzer {

  public:

    explicit First(fhicl::ParameterSet const& );

    void analyze(art::Event const& event) override;

  };

}

tex::First::First(fhicl::ParameterSet const& pset):
  art::EDAnalyzer(pset){
  std::cout << "Hello from First::constructor." << std::endl;
}

void tex::First::analyze(art::Event const& event){
  std::cout << "Hello from First::analyze. Event id: "
            << event.id()
            << std::endl;
}

DEFINE_ART_MODULE(tex::First)
