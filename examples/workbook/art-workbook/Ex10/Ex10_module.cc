//
// Exercise 10:
//  - First exercises with art::Ptr
//

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art_root_io/TFileService.h"

#include "TH1F.h"

#include <iostream>
#include <string>

namespace tex {

  class Ex10 : public art::EDAnalyzer {

  public:

    explicit Ex10(fhicl::ParameterSet const& );

    void beginJob() override;
    void analyze(art::Event const& event) override;

  private:

    // Intialized from parameter set.
    art::InputTag gensTag_;
    int           maxPrint_;

    int count_;

    art::ServiceHandle<art::TFileService> tfs_;

    TH1F* hn_;
    TH1F* hp_;

  };

}

tex::Ex10::Ex10(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),
  gensTag_(pset.get<std::string>("genParticlesInputTag")),
  maxPrint_(pset.get<int>("maxPrint",0)),
  count_(0),
  tfs_(),
  hn_(nullptr),
  hp_(nullptr){
}

void
tex::Ex10::beginJob(){

  hn_ = tfs_->make<TH1F>("n", "Number of generated particles per event",
                         10, 0., 10.);

  hp_ = tfs_->make<TH1F>("p", "Momentum of generated particles;[MeV]",
                         125, 0., 2500.);
}

void tex::Ex10::analyze(art::Event const& event ){

  auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  hn_->Fill(gens->size());

  bool doPrint = ++count_ < maxPrint_;

  int n(-1);
  for ( auto const& gen : *gens ){
    ++n;
    if ( gen.status() == GenParticle::decayed ){
      CLHEP::HepLorentzVector sum;
      for ( auto const& child : gen.children() ){
        sum += child->momentum();
      }
      if ( doPrint ){
        std::cout << "Decayed: "
                  << event.id().event()
                  << " index: "                << n
                  << " parent mass: "          << gen.momentum().mag()
                  << " number of children: "   << gen.children().size()
                  << " mass(sum of childen): " << sum.mag()
                  << std::endl;
      }
    }
  }

}

DEFINE_ART_MODULE(tex::Ex10)
