//
//  Exercise 6:
//    Access a data product from the event, using the art::Event::getValidHandle<T>
//    member function template.
//

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <string>

namespace tex {

  class ReadGens2 : public art::EDAnalyzer {

  public:

    explicit ReadGens2(fhicl::ParameterSet const& );

    void analyze(art::Event const& event) override;

  private:

    art::InputTag gensTag_;

  };

}

tex::ReadGens2::ReadGens2(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),
  gensTag_(pset.get<std::string>("genParticlesInputTag")){
}

void tex::ReadGens2::analyze(art::Event const& event ){

  art::ValidHandle<GenParticleCollection> gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  std::cout << "ReadGens2::analyze event: " << event.id().event()
            << " GenParticles: "            << gens->size()
            << std::endl;

}

DEFINE_ART_MODULE(tex::ReadGens2)
