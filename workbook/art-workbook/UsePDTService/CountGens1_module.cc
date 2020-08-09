//
//  Exercise 9:
//  Introduce PDT service
//

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"
#include "toyExperiment/PDT/PDT.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include <iostream>
#include <string>

namespace tex {

  class CountGens1 : public art::EDAnalyzer {

  public:

    explicit CountGens1(fhicl::ParameterSet const& );

    void beginJob() override;
    void analyze(art::Event const& event) override;

  private:

    art::InputTag gensTag_;

  };

}

tex::CountGens1::CountGens1(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),

  gensTag_(pset.get<std::string>("genParticlesInputTag")){
}

void tex::CountGens1::beginJob(){

  art::ServiceHandle<PDT> pdt;

  std::cout << "\nThe content of the particle data table is: \n"
            << *pdt
            << "\n"
            << std::endl;

}

void tex::CountGens1::analyze(art::Event const& event ){

  art::ServiceHandle<PDT> pdt;

  auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  std::cout << "Particle ID information for generated tracks in event: "
            << event.id()
            << std::endl;

  for ( GenParticle const& gen : *gens ){
    std::size_t index = &gen-&gens->front();
    std::cout << "Track: "          << index
              << " Particle Info: " << pdt->getById( gen.pdgId())
              << std::endl;
  }

}
// end tex::CountGens1::analyze

DEFINE_ART_MODULE(tex::CountGens1)
