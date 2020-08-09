//
//  Exercise 10:
//  Exercise art::Ptr
//

#include "art-workbook/FirstPtr/sumChildren.h"

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "TH1D.h"

#include <iostream>
#include <string>

namespace tex {

  class Ptr2 : public art::EDAnalyzer {

  public:

    explicit Ptr2(fhicl::ParameterSet const& );

    void beginJob() override;
    void analyze(art::Event const& event) override;

  private:

    art::InputTag gensTag_;

    std::array<TH1D*,4> hists_ ;

  };

}

tex::Ptr2::Ptr2(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),

  gensTag_(pset.get<std::string>("genParticlesInputTag")),
  hists_(){

}

void tex::Ptr2::beginJob(){

  art::ServiceHandle<art::TFileService> tfs;

  hists_[0] = tfs->make<TH1D>( "hpx",  "px: Parent-(Sum of Children)", 50, -0.001, 0.001);
  hists_[1] = tfs->make<TH1D>( "hpy",  "py: Parent-(Sum of Children)", 50, -0.001, 0.001);
  hists_[2] = tfs->make<TH1D>( "hpz",  "pz: Parent-(Sum of Children)", 50, -0.001, 0.001);
  hists_[3] = tfs->make<TH1D>( "he",   "e:  Parent-(Sum of Children)", 50, -0.001, 0.001);

}

void tex::Ptr2::analyze(art::Event const& event ){

  auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  for ( GenParticle const& gen : *gens ){

    if ( !gen.hasChildren() ) continue;

    CLHEP::HepLorentzVector sum = sumChildren(gen);
    for ( size_t i=0; i<hists_.size(); ++i ){
      hists_.at(i)->Fill( sum[i] );
    }

  }

} // end tex::Ptr2::analyze




DEFINE_ART_MODULE(tex::Ptr2)
