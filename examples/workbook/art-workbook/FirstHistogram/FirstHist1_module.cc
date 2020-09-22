//
//  Exercise 7:
//    Make a histogram of the number of GenParticles/event.
//

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"

#include "TH1D.h"

#include <iostream>
#include <string>

namespace tex {

  class FirstHist1 : public art::EDAnalyzer {

  public:

    explicit FirstHist1(fhicl::ParameterSet const& );

    void beginJob() override;
    void analyze(art::Event const& event) override;

  private:

    art::InputTag gensTag_;

    TH1D* hNGens_;

  };

}

tex::FirstHist1::FirstHist1(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),
  gensTag_(pset.get<std::string>("genParticlesInputTag")),
  hNGens_(nullptr){
}

void tex::FirstHist1::beginJob(){

  art::ServiceHandle<art::TFileService> tfs;

  hNGens_ = tfs->make<TH1D>( "hNGens", "Number of generated particles per event", 20, 0., 20.);

}
void tex::FirstHist1::analyze(art::Event const& event ){

  auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  hNGens_->Fill(gens->size());

}

DEFINE_ART_MODULE(tex::FirstHist1)
