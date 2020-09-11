//
//  Exercise 8: Homework solution
//  Loop over GenParticles and make some histograms of their properties
//

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art_root_io/TFileService.h"

#include "TH1D.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

namespace tex {

  class LoopGens4 : public art::EDAnalyzer {

  public:

    explicit LoopGens4(fhicl::ParameterSet const& );

    void beginJob() override;
    void analyze(art::Event const& event) override;

  private:

    art::InputTag gensTag_;

    // Control printed output.
    int maxPrint_;
    int nPrint_;

    TH1D *hNGens_;
    TH1D *hP_;
    TH1D *hcz_;
    TH1D *hphi_;
    TH1D *hmass_;
    TH1D *hx_;
    TH1D *hy_;
    TH1D *hz_;
    TH1D *hNChildren_;

  };

}

tex::LoopGens4::LoopGens4(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),

  gensTag_(pset.get<std::string>("genParticlesInputTag")),

  maxPrint_(pset.get<int>("maxPrint",0)),
  nPrint_(0),

  hNGens_(nullptr),
  hP_(nullptr),
  hcz_(nullptr),
  hphi_(nullptr),
  hmass_(nullptr),
  hx_(nullptr),
  hy_(nullptr),
  hz_(nullptr),
  hNChildren_(nullptr){
}

void tex::LoopGens4::beginJob(){

  art::ServiceHandle<art::TFileService> tfs;

  hNGens_  = tfs->make<TH1D>( "hNGens",  "Number of generated particles per event",    20,    0.,    20. );
  hP_      = tfs->make<TH1D>( "hP",      "Momentum of generated particles;[MeV/c]",   100,    0.,  2000. );
  hcz_     = tfs->make<TH1D>( "hcz",     "cos(Polar angle) of generated particles",   100,   -1.,     1. );
  hphi_    = tfs->make<TH1D>( "hphi",    "Azimuth of generated particles",            100, -M_PI,  M_PI  );
  hmass_   = tfs->make<TH1D>( "hmass",   "Rest Mass of Generated Particle;[MeV/c^2]", 110,    0.,  1100. );

  hx_      = tfs->make<TH1D>( "hx",      "X Coordinate of creation point;[mm]",       100,    -1,     1. );
  hy_      = tfs->make<TH1D>( "hy",      "Y Coordinate of creation point;[mm]",       100,    -1,     1. );
  hz_      = tfs->make<TH1D>( "hz",      "Z Coordinate of creation point;[mm]",       100,    -1,     1. );

  hNChildren_ = tfs->make<TH1D>( "hNChildren", "Number of children", 5, 0., 5.);
}

void tex::LoopGens4::analyze(art::Event const& event ){

  auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  hNGens_->Fill(gens->size());

  for ( auto const& gen : *gens ){

    CLHEP::HepLorentzVector const& p4 = gen.momentum();
    CLHEP::Hep3Vector const& p        = p4;
    CLHEP::Hep3Vector const& position = gen.position();

    hP_->   Fill( p.mag() );
    hcz_->  Fill( p.cosTheta() );
    hphi_-> Fill( p.phi()  );
    hmass_->Fill( p4.mag() );

    hx_->  Fill( position.x() );
    hy_->  Fill( position.y() );
    hz_->  Fill( position.z() );

    hNChildren_->Fill( gen.children().size() );

    if ( nPrint_ < maxPrint_ ){
      size_t index = &gen - &gens->front();

      std::cout << "Event: "        << std::setw(6) << event.id().event()
                << " GenParticle: " << std::setw(6) << index
                << " : "
                << gen
                << std::endl;
    }

  }

  ++nPrint_;

}
// end tex::LoopGens4::analyze

DEFINE_ART_MODULE(tex::LoopGens4)
