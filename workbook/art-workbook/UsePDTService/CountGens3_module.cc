//
//  Exercise 9:
//  Print a table that has, for each type of particle found in the
//  GenParticleCollection:
//    PDG Id, Name, Number seen in this job 
//

#include "art-workbook/UsePDTService/IdCounter.h"

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <fstream>
#include <string>

namespace tex {

  class CountGens3 : public art::EDAnalyzer {

  public:

    explicit CountGens3(fhicl::ParameterSet const& );

    void endJob() override;
    void analyze(art::Event const& event) override;

  private:

    art::InputTag gensTag_;
    std::string   outputFilename_;
    IdCounter     counter_;

  };

}

tex::CountGens3::CountGens3(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),
  gensTag_(pset.get<std::string>("genParticlesInputTag")),
  outputFilename_(pset.get<std::string>("outputFilename","")),
  counter_(){
}

void tex::CountGens3::analyze(art::Event const& event ){

  auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

  for ( GenParticle const& gen : *gens ){
    counter_.increment(gen.pdgId());
  }

}
// end tex::CountGens3::analyze

void tex::CountGens3::endJob(){

  // Print to log file
  counter_.print(std::cout);

  // Print to a secondary output file, if requested.
  if ( outputFilename_.empty() ) return;
  std::ofstream out( outputFilename_.c_str() );
  counter_.print(out, IdCounter::minimal);

}

DEFINE_ART_MODULE(tex::CountGens3)
