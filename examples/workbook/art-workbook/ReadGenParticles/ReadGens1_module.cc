//
//  Exercise 6:
//    Introduce the art::Event::getByLabel<T> member function template.
//    Get the GenParticleCollection produced by the event generator.
//

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <string>

namespace tex {

  class ReadGens1 : public art::EDAnalyzer {

  public:

    explicit ReadGens1(fhicl::ParameterSet const& );

    void analyze(art::Event const& event) override;

  private:

    art::InputTag gensTag_;

  };

}

tex::ReadGens1::ReadGens1(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),
  gensTag_(pset.get<std::string>("genParticlesInputTag")){
}

void tex::ReadGens1::analyze(art::Event const& event ){

  art::Handle<GenParticleCollection> gens;
  event.getByLabel(gensTag_,gens);

  std::cout << "ReadGens1::analyze event: " << event.id().event()
            << " GenParticles: "            << gens->size()
            << std::endl;

}

DEFINE_ART_MODULE(tex::ReadGens1)
