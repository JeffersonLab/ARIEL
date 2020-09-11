//
//  Exercise 9:
//  Solution to the homework.
//

#include "art-workbook/UsePDTService/IdCounter.h"

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"
#include "toyExperiment/PDT/PDT.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "TH1D.h"

#include <iostream>
#include <string>

namespace tex {

  class CountGens2 : public art::EDAnalyzer {

  public:

    explicit CountGens2(fhicl::ParameterSet const& );

    void beginJob() override;
    void endJob() override;
    void analyze(art::Event const& event) override;

  private:

    art::InputTag gensTag_;
    IdCounter     counter_;

    TH1D* hQ_;
    TH1D* hQSum_;

  };

}

tex::CountGens2::CountGens2(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),

  gensTag_(pset.get<std::string>("genParticlesInputTag")),

  hQ_(nullptr),
  hQSum_(nullptr){
}

void tex::CountGens2::beginJob(){
  art::ServiceHandle<art::TFileService> tfs;

  hQ_    = tfs->make<TH1D>( "hQ",    "Charge of each GenParticle", 5,  -2.,  2.);
  hQSum_ = tfs->make<TH1D>( "hQSum", "Charge of each GenParticle", 5,  -2.,  2.);

}

void tex::CountGens2::analyze(art::Event const& event ){

  art::ServiceHandle<PDT> pdt;

  auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  double qsum(0.);
  for ( GenParticle const& gen : *gens ){
    counter_.increment(gen.pdgId());

    double q = pdt->getById( gen.pdgId()).charge();
    hQ_->Fill(q);
    qsum += q;
  }

  hQSum_->Fill(qsum);

}
// end tex::CountGens2::analyze

void tex::CountGens2::endJob(){
  counter_.print(std::cout);
}

DEFINE_ART_MODULE(tex::CountGens2)
