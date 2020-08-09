//
//  Exercise 6:
//    Access a data product from the event, using the art::Event::getValidHandle<T>
//    member function template.  Introduce "auto".
//

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <string>

namespace tex {

  class ReadGens3 : public art::EDAnalyzer {

  public:

    explicit ReadGens3(fhicl::ParameterSet const& );

    void analyze(art::Event const& event) override;

  private:

    art::InputTag gensTag_;

  };

}

tex::ReadGens3::ReadGens3(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),
  gensTag_(pset.get<std::string>("genParticlesInputTag")){
}

void tex::ReadGens3::analyze(art::Event const& event ){

  auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  std::cout << "ReadGens3::analyze event: " << event.id().event()
            << " GenParticles: "            << gens->size()
            << std::endl;

}

DEFINE_ART_MODULE(tex::ReadGens3)
