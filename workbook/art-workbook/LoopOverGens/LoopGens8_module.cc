//
//  Exercise 8:
//  Variant of LoopGens1 with a deliberate error.
//

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art_root_io/TFileService.h"

#include "TH1D.h"

#include <iostream>
#include <string>

namespace tex {

  class LoopGens8 : public art::EDAnalyzer {

  public:

    explicit LoopGens8(fhicl::ParameterSet const& );

    void beginJob() override;
    void analyze(art::Event const& event) override;

  private:

    art::InputTag gensTag_;

    TH1D *hNGens_;
    TH1D *hP_;
    TH1D *hcz_;

  };

}

tex::LoopGens8::LoopGens8(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),

  gensTag_(pset.get<std::string>("genParticlesInputTag")),

  hNGens_(nullptr),
  hP_(nullptr),
  hcz_(nullptr){
}

void tex::LoopGens8::beginJob(){

  art::ServiceHandle<art::TFileService> tfs;

  hNGens_  = tfs->make<TH1D>( "hNGens",  "Number of generated particles per event", 20,    0.,    20.);
  hP_      = tfs->make<TH1D>( "hP",      "Momentum of generated particles;[MeV/c]", 100,    0.,  2000. );
  hcz_     = tfs->make<TH1D>( "hcz",     "cos(Polar angle) of generated particles", 100,   -1.,     1. );

}

void tex::LoopGens8::analyze(art::Event const& event ){

  auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  hNGens_->Fill(gens->size());

  for ( GenParticle const& gen : *gens ){

    hP_->  Fill( gen.momentum().mag() );
    hcz_-> Fill( gen.momentum().cosTheta() );

  }

}
// end tex::LoopGens8::analyze

DEFINE_ART_MODULE(tex::LoopGens8 )
