//
//  Exercise 10:
//  Exercise art::Ptr
//

#include "art-workbook/FirstPtr/sumChildren.h"

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"
#include "toyExperiment/PDT/PDT.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "TH1D.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

namespace {

  // Print the per Event header.
  inline
  void printEventHeader( art::EventID const& evtId, art::ProductID const& prodId ){
    std::cout << "\nStarting event: " << evtId << std::endl;
    std::cout << "Product Id of GenParticleCollection: " << prodId << std::endl;
    std::cout << "      "
              << "Index"
              << "     Name"
              << " Status"
              << "     Parent"
              << "    Children"
              << std::endl;
  }

  // Print one line for one GenParticle
  inline
  void printGen( tex::GenParticle const& gen, size_t index, tex::PDT const& pdt  ){

    std::cout << "Gen: "
              << std::setw(6) << index << " "
              << std::setw(8) << pdt.getById( gen.pdgId()).name() << " "
              << std::setw(6) << "       ";

    if ( gen.hasParent() ){
      std::cout << std::setw(4) << gen.parent().id() << " "
                << std::setw(4) << gen.parent().key();
    } else {
      std::cout << "  ----  ---";
    }

    if ( gen.hasChildren() ){
      for ( auto const& child : gen.children() ){
        std::cout << std::setw(4) << child.id()
                  << std::setw(4) << child.key();
      }
    }

    std::cout << std::endl;
  }

}

namespace tex {

  class Ptr1 : public art::EDAnalyzer {

  public:

    explicit Ptr1(fhicl::ParameterSet const& );

    void beginJob() override;
    void analyze(art::Event const& event) override;

  private:

    art::InputTag gensTag_;

    int maxPrint_;
    int nPrint_;

    TH1D* hMass_;

  };

}

tex::Ptr1::Ptr1(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),

  gensTag_(pset.get<std::string>("genParticlesInputTag")),
  maxPrint_(pset.get<int>("maxPrint",0)),
  nPrint_(0),

  hMass_(nullptr){
}

void tex::Ptr1::beginJob(){

  art::ServiceHandle<art::TFileService> tfs;

  hMass_ = tfs->make<TH1D>( "hMass",  "Mass of the daughters", 50, -0.001, 0.001);

}

void tex::Ptr1::analyze(art::Event const& event ){

  art::ServiceHandle<PDT> pdt;

  auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  if ( nPrint_ < maxPrint_) printEventHeader( event.id(), gens.id() );

  for ( GenParticle const& gen : *gens ){

    if ( gen.hasChildren() ){
      CLHEP::HepLorentzVector sum = sumChildren(gen);
      hMass_->Fill(sum.mag()-gen.momentum().mag());
    }

    if ( nPrint_ < maxPrint_) printGen( gen, &gen-&gens->front(), *pdt);

  }

  ++nPrint_;

} // end tex::Ptr1::analyze




DEFINE_ART_MODULE(tex::Ptr1)
