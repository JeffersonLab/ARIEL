//
//  Exercise 8:
//  Seven different ways to write the same loop.
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

  class LoopGens3 : public art::EDAnalyzer {

  public:

    explicit LoopGens3(fhicl::ParameterSet const& );

    void beginJob() override;
    void analyze(art::Event const& event) override;

  private:

    art::InputTag gensTag_;

    TH1D *hP1_;
    TH1D *hP2_;
    TH1D *hP3_;
    TH1D *hP4_;
    TH1D *hP5_;
    TH1D *hP6_;
    TH1D *hP7_;

  };

}

tex::LoopGens3::LoopGens3(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),

  gensTag_(pset.get<std::string>("genParticlesInputTag")),

  hP1_(nullptr),
  hP2_(nullptr),
  hP3_(nullptr),
  hP4_(nullptr),
  hP5_(nullptr),
  hP6_(nullptr),
  hP7_(nullptr){
}

void tex::LoopGens3::beginJob(){

  art::ServiceHandle<art::TFileService> tfs;

  hP1_ = tfs->make<TH1D>( "hP1", "Method 1: Momentum of generated particles;[MeV/c]",  100,    0.,  2000.);
  hP2_ = tfs->make<TH1D>( "hP2", "Method 2: Momentum of generated particles;[MeV/c]",  100,    0.,  2000.);
  hP3_ = tfs->make<TH1D>( "hP3", "Method 3: Momentum of generated particles;[MeV/c]",  100,    0.,  2000.);
  hP4_ = tfs->make<TH1D>( "hP4", "Method 4: Momentum of generated particles;[MeV/c]",  100,    0.,  2000.);
  hP5_ = tfs->make<TH1D>( "hP5", "Method 5: Momentum of generated particles;[MeV/c]",  100,    0.,  2000.);
  hP6_ = tfs->make<TH1D>( "hP6", "Method 6: Momentum of generated particles;[MeV/c]",  100,    0.,  2000.);
  hP7_ = tfs->make<TH1D>( "hP7", "Method 7: Momentum of generated particles;[MeV/c]",  100,    0.,  2000.);

}

void tex::LoopGens3::analyze(art::Event const& event ){

  auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  // Method 1
  for ( auto const& gen : *gens ){
    CLHEP::Hep3Vector const& p = gen.momentum();
    hP1_->Fill( p.mag() );
  }

  // Method 2
  for ( size_t i=0; i!=gens->size(); ++i ){
    CLHEP::Hep3Vector const& p = (*gens)[i].momentum();
    hP2_->Fill( p.mag() );
  }

  // Method 3
  for ( size_t i=0; i!=gens->size(); ++i ){
    CLHEP::Hep3Vector const& p = gens->at(i).momentum();
    hP3_->Fill( p.mag() );
  }

  // Method 4
  GenParticleCollection::const_iterator i=gens->begin();
  GenParticleCollection::const_iterator end=gens->end();
  for ( ; i!=end; ++i ){
    CLHEP::Hep3Vector const& p = i->momentum();
    hP4_->Fill( p.mag() );
  }

  // Method 5
  for ( auto j=gens->begin(); j!=gens->end(); ++j ){
    CLHEP::Hep3Vector const& p = j->momentum();
    hP5_->Fill( p.mag() );
  }

  // Method 6
  for ( auto j=gens->begin(), jend=gens->end(); j!=jend; ++j ){
    CLHEP::Hep3Vector const& p = j->momentum();
    hP6_->Fill( p.mag() );
  }

  // Method 7
  for ( auto j=std::begin(*gens), jend=std::end(*gens); j!=jend; ++j ){
    CLHEP::Hep3Vector const& p = j->momentum();
    hP7_->Fill( p.mag() );
  }

}
// end tex::LoopGens3::analyze

DEFINE_ART_MODULE(tex::LoopGens3)
