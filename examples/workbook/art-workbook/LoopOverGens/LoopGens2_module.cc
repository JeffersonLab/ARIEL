//
//  Exercise 8:
//  Another way to write the same loop.
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

  class LoopGens2 : public art::EDAnalyzer {

  public:

    explicit LoopGens2(fhicl::ParameterSet const& );

    void beginJob() override;
    void analyze(art::Event const& event) override;

  private:

    art::InputTag gensTag_;

    TH1D *hNGens_;
    TH1D *hP_;
    TH1D *hcz_;

    void fillParticleHistograms ( GenParticle const& gen );

  };

}

tex::LoopGens2::LoopGens2(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),

  gensTag_(pset.get<std::string>("genParticlesInputTag")),

  hNGens_(nullptr),
  hP_(nullptr),
  hcz_(nullptr){
}

void tex::LoopGens2::beginJob(){

  art::ServiceHandle<art::TFileService> tfs;

  hNGens_  = tfs->make<TH1D>( "hNGens",  "Number of generated particles per event",  20,    0.,    20.);
  hP_      = tfs->make<TH1D>( "hP",      "Momentum of generated particles;[MeV/c]", 100,    0.,  2000.);
  hcz_     = tfs->make<TH1D>( "hcz",     "cos(Polar angle) of generated particles", 100,   -1.,     1.);

}

void tex::LoopGens2::analyze(art::Event const& event ){

  auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  hNGens_->Fill(gens->size());

  for ( auto const& gen : *gens ){
    fillParticleHistograms(gen);
  }

} // end tex::LoopGens2::analyze

void tex::LoopGens2::fillParticleHistograms( tex::GenParticle const& gen ){

    CLHEP::Hep3Vector const& p = gen.momentum();

    hP_->  Fill( p.mag() );
    hcz_-> Fill( p.cosTheta() );

}

DEFINE_ART_MODULE(tex::LoopGens2)
