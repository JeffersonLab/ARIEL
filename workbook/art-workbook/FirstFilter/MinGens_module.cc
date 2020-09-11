//
// Pass events that have a minimum number of generated particles.
//

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"

#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"

#include <iostream>
#include <string>

namespace tex {

  class MinGens : public art::EDFilter {

  public:

    explicit MinGens(fhicl::ParameterSet const& );

    bool filter(art::Event& event) override;

  private:

    art::InputTag gensTag_;
    size_t        minGens_;

  };

}

tex::MinGens::MinGens(fhicl::ParameterSet const& pset ):
  art::EDFilter{pset},
  gensTag_(pset.get<std::string>("genParticlesInputTag")),
  minGens_(pset.get<int>("minimumGenParticles")){
}

bool tex::MinGens::filter(art::Event& event ){

  auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  return ( gens->size() >= minGens_ );
}

DEFINE_ART_MODULE(tex::MinGens)
